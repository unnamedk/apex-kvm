#include "entity.hpp"
#include "utils/process.hpp"
#include "cheats/offsets.hpp"
#include <memory>
#include <map>

using namespace apex::math;
using namespace apex::sdk;
using apex::cheats::offsets_t;

template <typename T>
T &get_from_buf( std::byte *buf, int offset )
{
    return *( T * ) ( buf + offset );
}

void apex::sdk::player_t::update()
{
    auto data = std::make_unique<std::byte[]>( 17'100 );
    apex::utils::process::get().read_ptr( m_base, data.get(), 17'100 );

    m_handle = get_from_buf<uint32_t>( data.get(), 0x8 );
    m_health = get_from_buf<int32_t>( data.get(), offsets_t::get().health );
    m_max_health = get_from_buf<int32_t>( data.get(), offsets_t::get().max_health );
    m_shield_health = get_from_buf<int32_t>( data.get(), offsets_t::get().shield_health );
    m_max_shield_health = get_from_buf<int32_t>( data.get(), offsets_t::get().shield_health_max );
    m_team = get_from_buf<int32_t>( data.get(), offsets_t::get().team );
    m_origin = get_from_buf<math::vector3>( data.get(), offsets_t::get().origin );
    m_flags = get_from_buf<int32_t>( data.get(), offsets_t::get().fflags );
    m_bleedout_state = get_from_buf<sdk::bleedout_state_t>( data.get(), offsets_t::get().bleedout );
    m_velocity = get_from_buf<math::vector3>( data.get(), offsets_t::get().velocity );
    m_bonematrix = get_from_buf<uintptr_t>( data.get(), offsets_t::get().entity_bones );
    m_angles = get_from_buf<math::qangle>( data.get(), offsets_t::get().local_angles );
    m_mins = get_from_buf<math::vector3>( data.get(), offsets_t::get().collision + 0x10 );
    m_max = get_from_buf<math::vector3>( data.get(), offsets_t::get().collision + 0x1c );
    m_last_visible_time = get_from_buf<float>( data.get(), offsets_t::get().last_visible_time );
    //m_headpos = get_from_buf<math::vector3>( data.get(), offsets_t::get().head_component );
    m_weapon_handle = get_from_buf<std::uint32_t>( data.get(), offsets_t::get().latest_primary_weapon );
    m_camera = get_from_buf<math::vector3>( data.get(), offsets_t::get().camera );
    m_breath = get_from_buf<math::qangle>( data.get(), offsets_t::get().breath );
    m_recoil = get_from_buf<math::qangle>( data.get(), offsets_t::get().recoil );
    m_name_ptr = get_from_buf<std::uintptr_t>( data.get(), offsets_t::get().player_name );
    m_in_zoom = get_from_buf<bool>( data.get(), offsets_t::get().is_zooming );
}

bool player_t::is_visible()
{
    static std::map<int, float> visible_map;
    const auto vis_check = this->get_last_visible_time();

    // If the player was never visible the value is -1
    const auto is_vis = ( vis_check >= visible_map[ this->index ] );

    visible_map[ this->index ] = vis_check;
    return is_vis;
}

bool apex::sdk::player_t::is_alive()
{
    return ( this->get_health() > 0 );
}

void apex::sdk::item_t::update()
{
    std::byte data[ 0x2000 ];
    apex::utils::process::get().read_ptr( m_base, data, 0x2000 );

    m_handle = get_from_buf<uint32_t>( data, 0x8 );
    m_custom_script_int = get_from_buf<sdk::script_int_id>( data, offsets_t::get().custom_script_int );
    m_origin = get_from_buf<math::vector3>( data, offsets_t::get().origin );
    m_mins = get_from_buf<math::vector3>( data, offsets_t::get().collision + 0x10 );
    m_max = get_from_buf<math::vector3>( data, offsets_t::get().collision + 0x1c );
    m_is_glown = get_from_buf<int>( data, 0x350 ) == 7;
}

bool apex::sdk::item_t::is_weapon()
{
    return ( m_custom_script_int > script_int_id::undefined ) && ( m_custom_script_int < script_int_id::light_rounds );
}

bool apex::sdk::item_t::is_armor()
{
    return ( m_custom_script_int > script_int_id::helmet_lv4 ) && ( m_custom_script_int < script_int_id::knockdown_shield_lv1 );
}

bool apex::sdk::item_t::is_helmet()
{
    return ( m_custom_script_int > script_int_id::shield_cell ) && ( m_custom_script_int < script_int_id::body_armor_lv1 );
}

bool apex::sdk::item_t::is_backpack()
{
    return ( m_custom_script_int > script_int_id::knockdown_shield_lv1 ) && ( m_custom_script_int < script_int_id::thermite );
}

bool apex::sdk::item_t::is_ammo()
{
    return ( m_custom_script_int > script_int_id::re45 ) && ( m_custom_script_int < script_int_id::ultimate_accelerant );
}

bool apex::sdk::item_t::is_utility()
{
    return ( m_custom_script_int > script_int_id::heavy_rounds ) && ( m_custom_script_int < script_int_id::helmet_lv1 );
}

bool apex::sdk::item_t::is_attachable()
{
    return ( m_custom_script_int > script_int_id::digital_sniper_thread_4x10x ) && ( m_custom_script_int <= script_int_id::hammerpoint );
}

bool apex::sdk::item_t::is_legendary()
{
    return is_weapon_legendary() ||
        ( m_custom_script_int == script_int_id::backpack_lv4 ) ||
        ( m_custom_script_int == script_int_id::body_armor_lv4 ) ||
        ( m_custom_script_int == script_int_id::helmet_lv4 ) ||
        ( m_custom_script_int == script_int_id::knockdown_shield_lv4 );
}

bool apex::sdk::item_t::is_weapon_legendary()
{
    return ( ( m_custom_script_int > script_int_id::undefined ) && ( m_custom_script_int < script_int_id::havoc ) ) ||
        ( m_custom_script_int == script_int_id::peacekeeper_legendary ) ||
        ( m_custom_script_int == script_int_id::wingman_legendary ) ||
        ( m_custom_script_int == script_int_id::alternator_legendary ) ||
        ( m_custom_script_int == script_int_id::flatline_legendary ) ||
        ( m_custom_script_int == script_int_id::mozambique_legendary ) ||
        ( m_custom_script_int == script_int_id::p2020_legendary );
}

bool apex::sdk::item_t::is_weapon_light()
{
    return ( m_custom_script_int == script_int_id::g7_scout ) || ( m_custom_script_int == script_int_id::alternator ) ||
        ( m_custom_script_int == script_int_id::r301 ) || ( m_custom_script_int == script_int_id::r99 ) || ( m_custom_script_int == script_int_id::p2020 ) || ( m_custom_script_int == script_int_id::re45 );
}

bool apex::sdk::item_t::is_weapon_shotgun()
{
    return ( m_custom_script_int == script_int_id::eva8_auto ) || ( m_custom_script_int == script_int_id::peacekeeper ) || ( m_custom_script_int == script_int_id::mozambique );
}

bool apex::sdk::item_t::is_weapon_heavy()
{
    return ( m_custom_script_int == script_int_id::flatline ) || ( m_custom_script_int == script_int_id::hemlok ) || ( m_custom_script_int == script_int_id::prowler ) ||
        ( m_custom_script_int == script_int_id::longbow ) || ( m_custom_script_int == script_int_id::spitfire ) || ( m_custom_script_int == script_int_id::wingman );
}

bool apex::sdk::item_t::is_weapon_energy()
{
    return ( m_custom_script_int == script_int_id::havoc ) || ( m_custom_script_int == script_int_id::triple_take ) || ( m_custom_script_int == script_int_id::devotion );
}

bool apex::sdk::item_t::is_glown()
{
    return this->m_is_glown;
}

void apex::sdk::weapon_t::update()
{
    std::byte data[ 0x2000 ];
    apex::utils::process::get().read_ptr( m_base, data, 0x2000 );

    m_handle = get_from_buf<uint32_t>( data, 0x8 );
    m_weapon_id = get_from_buf<int>( data, offsets_t::get().weapon_name_index );
    m_bullet_velocity = get_from_buf<float>( data, offsets_t::get().bullet_velocity );
    m_bullet_gravity_scale = get_from_buf<float>( data, offsets_t::get().bullet_gravity_scale );
    m_target_zoom_fov = get_from_buf<float>( data, offsets_t::get().weaponx_target_fov );
}

bool apex::sdk::entity_t::is_player()
{
    return ( this->class_id == static_cast<int>( apex::sdk::class_id::CPlayer ) );
}

bool entity_t::is_dummy()
{
    return ( this->class_id == static_cast<int>( apex::sdk::class_id::Titan_Cockpit ) );
}

bool apex::sdk::entity_t::is_item()
{
    return this->class_id == static_cast<int>( apex::sdk::class_id::CPropSurvival );
}

bool apex::sdk::entity_t::is_weapon()
{
    return this->class_id == static_cast<int>( apex::sdk::class_id::CWeaponX );
}
