#pragma once

#include <cstdint>

#include "math/math.hpp"
#include "sdk.hpp"

namespace apex::sdk
{
    struct ent_info_t
    {
        std::uintptr_t entity_ptr;
        std::int64_t serial;

        std::uintptr_t previous;
        std::uintptr_t next;
    };

    struct entity_t
    {
        virtual void update() = 0;
        virtual ~entity_t() = default;

        bool is_player();
        bool is_dummy();
        bool is_item();
        bool is_weapon();

        template <typename T>
        T *as()
        {
            return static_cast<T *>( this );
        }

        int class_id = -1;
        int index = -1;
    };

    struct player_t : public entity_t
    {
        player_t( std::uintptr_t base )
            : priority( 0 )
            , m_base( base )
            , m_handle( 0u )
            , m_bonematrix( 0ull )
            , m_name_ptr( 0ull )

            , m_health( 0 )
            , m_shield_health( 0 )
            , m_team( 0 )
            , m_max_shield_health( 0 )
            , m_flags( 0 )
            , m_max_health( 0 )
            , m_weapon_handle( 0u )
            , m_bleedout_state( bleedout_state_t::alive )

            , m_angles( 0.f )
            , m_breath( 0.f )
            , m_recoil( 0.f )
            , m_camera( 0.f )
            , m_velocity( 0.f )
            , m_origin( 0.f )
            , m_mins( 0.f )
            , m_max( 0.f )
            , m_last_visible_time( 0 )
            , m_headpos( 0.f )
            , m_in_zoom( false )
        { }

        virtual void update();

        auto get_base() { return m_base; }
        auto get_handle() { return m_handle; }
        auto get_health() { return m_health; }
        auto get_shield_health() { return m_shield_health; }
        auto get_head_pos() { return m_headpos; }
        auto get_max_shield_health() { return m_max_shield_health; }
        auto get_max_health() { return m_max_health; }
        auto get_pos() { return m_origin; }
        auto get_team() { return m_team; }
        auto get_velocity() { return m_velocity; }
        auto get_bleedout_state() { return m_bleedout_state; }
        auto get_bone_matrix() { return m_bonematrix; }
        auto get_mins() { return m_mins; }
        auto get_maxs() { return m_max; }
        auto get_primary_weapon_handle() { return m_weapon_handle; }
        auto get_flags() { return m_flags; }
        auto get_angles() { return m_angles; }
        auto get_camera_pos() { return m_camera; }
        auto get_breath() { return m_breath; }
        auto get_recoil() { return m_recoil; }
        auto get_priority() { return this->priority; }
        auto get_name_ptr() { return this->m_name_ptr; }
        auto is_in_zoom() { return this->m_in_zoom; }
        auto get_last_visible_time() { return this->m_last_visible_time; }

        bool is_visible();
        bool is_alive();

        std::int32_t priority;

    private:
        std::uintptr_t m_base;
        std::uint32_t m_handle;
        std::uintptr_t m_bonematrix;
        std::uintptr_t m_name_ptr;

        std::int32_t m_health;
        std::int32_t m_shield_health;
        std::int32_t m_team;
        std::int32_t m_max_shield_health;
        std::int32_t m_flags;
        std::int32_t m_max_health;
        std::uint32_t m_weapon_handle;
        sdk::bleedout_state_t m_bleedout_state;

        math::qangle m_angles;
        math::qangle m_breath;
        math::qangle m_recoil;
        math::vector3 m_camera;
        math::vector3 m_velocity;
        math::vector3 m_origin;
        math::vector3 m_mins;
        math::vector3 m_max;
        float m_last_visible_time;
        math::vector3 m_headpos;

        bool m_in_zoom;
    };

    struct item_t : public entity_t
    {
    public:
        item_t( std::uintptr_t base )
            : m_base( base )
            , m_handle( 0u )
            , m_custom_script_int( script_int_id::undefined )
            , m_origin( 0.f )
            , m_mins( 0.f )
            , m_max( 0.f )
            , m_is_glown( false )
        { }
        virtual void update();

        auto get_base() { return m_base; }
        auto get_handle() { return m_handle; }
        auto get_item_id() { return m_custom_script_int; }
        auto get_pos() { return m_origin; }
        auto get_mins() { return m_mins; }
        auto get_maxs() { return m_max; }

        bool is_weapon();
        bool is_armor();
        bool is_helmet();
        bool is_backpack();
        bool is_ammo();
        bool is_utility();
        bool is_attachable();
        bool is_legendary();

        bool is_weapon_legendary();
        bool is_weapon_light();
        bool is_weapon_shotgun();
        bool is_weapon_heavy();
        bool is_weapon_energy();

        bool is_glown();

    private:
        std::uintptr_t m_base;
        std::uint32_t m_handle;
        sdk::script_int_id m_custom_script_int;
        math::vector3 m_origin;
        math::vector3 m_mins;
        math::vector3 m_max;
        bool m_is_glown;
    };

    class weapon_t : public entity_t
    {
    public:
        weapon_t( std::uintptr_t base )
            : m_base( base )
            , m_handle( 0u )
            , m_weapon_id( 0 )
            , m_bullet_velocity( 0.f )
            , m_bullet_gravity_scale( 0.f )
            , m_target_zoom_fov( 0.f )
        { }
        virtual void update();

        auto get_base() { return m_base; }
        auto get_handle() { return m_handle; }
        auto get_weapon_id() { return m_weapon_id; }
        auto get_bullet_velocity() { return m_bullet_velocity; }
        auto get_bullet_gravity_scale() { return m_bullet_gravity_scale; }
        auto get_target_zoom_fov() { return m_target_zoom_fov; }

    private:
        std::uintptr_t m_base;
        std::uint32_t m_handle;
        std::int32_t m_weapon_id;
        float m_bullet_velocity;
        float m_bullet_gravity_scale;
        float m_target_zoom_fov;
    };
}