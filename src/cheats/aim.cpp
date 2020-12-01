#include "aim.hpp"
#include "config/options.hpp"
#include "utils/utils.hpp"
#include "math/solver.hpp"
#include "entities.hpp"
#include "offsets.hpp"

#include <thread>

using apex::cheats::aim;
using namespace std::chrono_literals;

int get_class_id_aim( std::uintptr_t ent )
{
    std::uintptr_t client_nttable = 0u;
    if ( apex::utils::process::get().read<std::uintptr_t>( ent + 24, client_nttable ); !client_nttable ) {
        return -1;
    }

    std::uintptr_t get_client_class_fn = 0u;
    if ( apex::utils::process::get().read<std::uintptr_t>( client_nttable + 24, get_client_class_fn ); !( get_client_class_fn ) ) {
        return -1;
    }

    std::uint32_t relative_table_offs = 0u;
    if ( apex::utils::process::get().read<std::uint32_t>( get_client_class_fn + 3, relative_table_offs ); !( relative_table_offs ) ) {
        return -1;
    }

    auto id = 0;
    apex::utils::process::get().read<std::int32_t>( get_client_class_fn + relative_table_offs + 7 + 0x28, id );

    return id;
}

void aim::run()
{
    while ( true ) {
        std::this_thread::sleep_for( 1ms );
        if ( !this->should_iterate() ) {
            this->reset_state();
            continue;
        }

        auto local_player = entity_list::get().get_local_player();
        if ( !local_player || !local_player->is_alive() ) {
            this->reset_state();
            continue; // invalid local player
        }

        if ( !this->get_bullet_info() ) {
            this->reset_state();
            continue; // invalid weapon somehow
        }

        auto target = this->get_best_entity();
        if ( !target ) {
            this->reset_state();
            continue; // no target to aim
        }

        this->aim_at( target );
    }
}

bool aim::should_iterate() const noexcept
{
    if ( !utils::process::get().good() ) {
        return false;
    }

    return options->aimbot.enabled && utils::is_key_down( static_cast<apex::utils::keycode_t>( options->aimbot.key ) );
}

std::optional<std::pair<float, float>> aim::get_bullet_info()
{
    auto local_player = entity_list::get().get_local_player();

    auto weapon_handle = local_player->get_primary_weapon_handle();
    if ( !weapon_handle || ( weapon_handle == static_cast<std::uint32_t>( -1 ) ) ) {
        return std::nullopt;
    }

    auto weapon = static_cast<sdk::weapon_t *>( entity_list::get().at( weapon_handle & 0xffff ) );
    if ( !weapon || !weapon->is_weapon() ) {
        return std::make_pair( 1.f, 1.f );
    }

    return std::make_pair( weapon->get_bullet_velocity(), weapon->get_bullet_gravity_scale() );
}
apex::sdk::entity_t *aim::get_best_entity()
{
    // 'true' if user wants target selection to use distance instead of fov
    const auto use_distance = options->aimbot.target_selection_switch_on_key && utils::is_key_down( static_cast<utils::keycode_t>( options->aimbot.target_selection_key ) );

    auto local_player = entity_list::get().get_local_player();
    auto smallest_unit = use_distance ? std::numeric_limits<float>::max() : options->aimbot.fov;
    auto ans = static_cast<sdk::entity_t *>( nullptr );

    if ( options->aimbot.target_lock && !use_distance && ( this->m_target_index != 0 ) ) {
        auto target = entity_list::get().at( this->m_target_index );

        // as long as the same target is valid keep at it
        if ( validate_entity( target ) ) {
            return target;
        }
    }

    for ( auto &e : entity_list::get() ) {
        if ( !validate_entity( e ) ) {
            continue;
        }

        auto entity_position = this->select_pos( e );

        float current_unit = 0.f;
        if ( use_distance ) {
            current_unit = entity_position.distance_from( local_player->get_pos() ) * 0.01905f;
        } else {
            current_unit = math::get_fov(
                local_player->get_angles(),
                math::vector_angles( local_player->get_pos(), entity_position ) );
        }

        if ( current_unit >= smallest_unit ) {
            continue;
        }

        ans = e;
        smallest_unit = current_unit;
    }

    return ans;
}
void aim::aim_at( sdk::entity_t *entity )
{
    auto entity_pos = this->select_pos( entity );
    if ( entity_pos.is_zero() ) {
        this->reset_state();
        return;
    }

    auto local_player = entity_list::get().get_local_player();
    auto local_pos = local_player->get_camera_pos();

    auto angles = math::vector_angles( local_pos, entity_pos );
    if ( !this->compensate_pos( entity->as<sdk::player_t>(), entity_pos, angles ) ) {
        this->reset_state();
        return;
    }

    if ( options->aimbot.rcs_enabled && !this->compensate_recoil( angles ) ) {
        this->reset_state();
        return;
    }

    if ( options->aimbot.smooth_enabled && !this->smooth( angles ) ) {
        this->reset_state();
        return;
    }

    angles.clamp();
    options->aimbot.current_target = entity->is_player() ? entity->as<sdk::player_t>()->get_base() : entity->as<sdk::item_t>()->get_base();
    this->m_target_index = entity->index;
    utils::process::get().write( local_player->get_base() + offsets_t::get().local_angles, angles, sizeof( float ) * 2 );
}
bool aim::validate_entity( sdk::entity_t *entity ) const noexcept
{
    if ( !entity ) {
        return false;
    }

    auto local_player = entity_list::get().get_local_player();
    if ( entity->is_player() ) {
        auto player = entity->as<sdk::player_t>();
        if ( player->get_base() == local_player->get_base() ) {
            return false;
        }

        if ( player->get_pos().is_zero() ) {
            return false;
        }

        if ( ( player->get_pos().distance_from( local_player->get_pos() ) * 0.01905f ) > options->aimbot.maximum_distance ) {
            return false;
        }

        if ( !player->is_alive() ||
            ( player->get_pos().z() > 11000.f /* in dropship */ ) ) {
            return false;
        }

        auto is_friend = local_player->get_team() == player->get_team();
        if ( is_friend && !options->aimbot.aim_on_friends ) {
            return false;
        }

        // marked as 'friend'
        if ( player->priority == -1 ) {
            return false;
        }

        return true;
    }

    // for debugging
    else if ( entity->is_dummy() && options->aimbot.aim_on_dummies ) {
        auto dummy = entity->as<sdk::player_t>();
        if ( dummy->get_pos().is_zero() ) {
            return false;
        }

        if ( ( dummy->get_pos().distance_from( local_player->get_pos() ) * 0.01905f ) > options->aimbot.maximum_distance ) {
            return false;
        }

        return true;
    }

    return false;
}
apex::math::vector3 aim::select_pos( sdk::entity_t *entity )
{
    if ( entity->is_dummy() ) {
        auto pos = entity->as<sdk::player_t>()->get_pos();
        pos.z() += 64.f;
        return pos;
    }

    auto player = entity->as<sdk::player_t>();
    auto bone_matrix = player->get_bone_matrix();

    auto bone_index = options->aimbot.primary_hitbox;
    if ( utils::is_key_down( static_cast<utils::keycode_t>( options->aimbot.secondary_hitbox_key ) ) ) {
        bone_index = options->aimbot.secondary_hitbox;
    }

    float x = 0.f, y = 0.f, z = 0.f;
    utils::process::get().read( bone_matrix + 0xcc + static_cast<uintptr_t>( bone_index ) * 0x30, x );
    utils::process::get().read( bone_matrix + 0xdc + static_cast<uintptr_t>( bone_index ) * 0x30, y );
    utils::process::get().read( bone_matrix + 0xec + static_cast<uintptr_t>( bone_index ) * 0x30, z );

    return math::vector3 { x, y, z } + player->get_pos();
}
bool aim::compensate_pos( sdk::player_t *player, math::vector3 &pos, math::qangle &ang )
{
    auto local_player = entity_list::get().get_local_player();

    auto bullet_info = this->get_bullet_info();
    if ( !bullet_info ) {
        return false;
    }

    auto [ bullet_vel, bullet_grav ] = *bullet_info;
    if ( bullet_vel > 1.f ) {
        auto velocity = player->get_velocity() - local_player->get_velocity();

        math::LinearPredictor pred { pos, velocity };
        math::Solution sol;

        math::ProjectileWeapon weapon { bullet_vel, bullet_grav * 750.f };
        if ( math::solve( local_player->get_camera_pos(), weapon, pred, sol ) ) {
            ang = { -math::to_degrees( sol.pitch ), math::to_degrees( sol.yaw ), 0.f };
            return true;
        }

        return false;
    }

    return true;
}
bool aim::compensate_recoil( math::qangle &angles )
{
    auto local_player = entity_list::get().get_local_player();

    auto punch = local_player->get_recoil();
    if ( !punch.is_zero() ) {
        angles = angles - punch;
    }

    return true;
}
void aim::reset_state()
{
    options->aimbot.current_target = 0ull;
    this->m_target_index = 0;

    // smooth values must be at least 0.7 apart
    this->smooth_x = 0.f;
    this->smooth_y = 0.f;
    while ( abs( smooth_x - smooth_y ) < 0.3f ) {
        this->smooth_x = options->aimbot.smooth_factor + math::rand_float( -1.f, 2.3f );
        this->smooth_y = options->aimbot.smooth_factor + math::rand_float( -0.8f, 1.3f );
    }
}

bool aim::smooth( math::qangle &angles )
{
    auto current_angle = entity_list::get().get_local_player()->get_angles();
    auto delta = angles - current_angle;
    delta.clamp();

    if ( !delta.is_zero() ) {
        delta.x() /= std::max( smooth_x, 1.5f );
        delta.y() /= std::max( smooth_y, 1.5f );
    }

    angles = current_angle + delta;
    return true;
}