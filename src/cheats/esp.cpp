#include "esp.hpp"
#include <thread>
#include <chrono>

#include "entities.hpp"
#include "utils/process.hpp"
#include "utils/utils.hpp"
#include "config/options.hpp"
#include "offsets.hpp"

void apex::cheats::esp::run()
{
    while ( true ) {
        std::this_thread::sleep_for( std::chrono::milliseconds( 3 ) );
        if ( !utils::process::get().good() || !this->should_iterate() ) {
            continue;
        }

        if ( !entity_list::get().get_local_player() || ( entity_list::get().get_local_player()->get_pos().z() > 11000.f ) ) {
            continue;
        }

        for ( auto &e : entity_list::get() ) {
            if ( !e ) {
                continue;
            }

            if ( ( options->visual.glow_esp ) && e->is_player() ) {
                apply_glow( e->as<sdk::player_t>() );
            }

            if ( ( options->visual.item_esp ) && e->is_item() ) {
                apply_glow( e->as<sdk::item_t>() );
            }
        }
    }
}
bool apex::cheats::esp::should_iterate() const noexcept
{
    // only run if the cheat needs to actually render something
    return ( options->visual.enabled && ( options->visual.glow_esp || options->visual.item_esp ) ) && utils::are_movement_keys_pressed();
}

void apex::cheats::esp::apply_glow( sdk::player_t *entity )
{
    if ( !this->validate_player( entity ) ) {
        return;
    }

    auto local_player = entity_list::get().get_local_player();

    auto color = [ & ]() -> sdk::color_t {
        auto is_friend = local_player->get_team() == entity->get_team();
        if ( is_friend ) {
            return options->visual.friend_color;
        }

        if ( ( options->aimbot.current_target == entity->get_base() ) && options->visual.aim_target_esp ) {
            return options->visual.aimbot_target_color;
        }

        if ( entity->get_bleedout_state() != sdk::bleedout_state_t::alive ) {
            return options->visual.downed_color;
        }

        if ( options->visual.color_health_based ) {
            const auto red = std::abs( 255.f - ( entity->get_health() * 2.55f ) );
            return sdk::color_t( red, 255.f - red, 0.f, options->visual.health_based_alpha );
        }
        return options->visual.esp_color;
    }();

    const auto brightness = std::clamp( 255.f / color.a(), 0.f, 1.f );

    std::array col {
        static_cast<float>( color.r() ) * brightness,
        static_cast<float>( color.g() ) * brightness,
        static_cast<float>( color.b() ) * brightness,
    };

    bool glow_enabled = true;
    int glow_time = 1;
    float glow_distance = 5000.f;
    std::array<float, 6> max;
    max.fill( std::numeric_limits<float>::max() );

    if ( entity_list::get().validate( entity ) ) {
        utils::process::get().write( entity->get_base() + 0x360, glow_enabled );
        utils::process::get().write( entity->get_base() + 0x350, glow_time );
        utils::process::get().write_ptr( entity->get_base() + 0x1D0, col.data(), sizeof( float ) * 3 );
        utils::process::get().write_ptr( entity->get_base() + 0x310, max.data(), sizeof( float ) * max.size() );
        utils::process::get().write( entity->get_base() + 0x33c, glow_distance );
    }
}
void apex::cheats::esp::apply_glow( sdk::item_t *entity )
{
    if ( entity_list::get().validate( entity ) && !entity->is_glown() ) {
        int enable = 1363184265;
        utils::process::get().write( entity->get_base() + 0x2a8, enable );
    }
}
bool apex::cheats::esp::validate_player( sdk::player_t *player ) const noexcept
{
    auto local_player = entity_list::get().get_local_player();
    if ( player->get_base() == local_player->get_base() ) {
        return false;
    }
    if ( player->get_pos().z() > 11000.f /* in dropship */ ) {
        return false;
    }

    auto is_friend = local_player->get_team() == player->get_team();
    if ( is_friend && !options->visual.highlight_friends ) {
        return false;
    }

    return true;
}