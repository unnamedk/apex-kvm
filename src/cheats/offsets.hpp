#pragma once

#include <cstdint>
#include "utils/singleton.hpp"

#include "memflow.h"
#include "memflow_win32.h"

namespace apex::cheats
{
    class offsets_t : public utils::singleton<offsets_t>
    {
    public:
        void init();

        std::uintptr_t entity_list;
        std::uintptr_t local_player_index;
        std::uintptr_t clientstate;
        std::uintptr_t timescale;
        std::uintptr_t name_list;
        std::uintptr_t global_vars;
        uint32_t team = 0x0430;
        uint32_t health = 0x0420;
        uint32_t max_health = 0x558;
        uint32_t bleedout = 0x2628;

        uint32_t camera = 0x1e6c;
        uint32_t local_angles = 0x24a0;
        uint32_t breath = local_angles - 0x10;
        uint32_t origin = 0x14c;

        uint32_t is_zooming = 0x1b81;
        uint32_t sign_on_state = 0x98;
        uint32_t level_name_short_offset = 0x1b0;

        uint32_t latest_primary_weapon = 0x1a0c;
        uint32_t latest_non_offhand_weapon = 0x1a0c;

        uint32_t entity_bones = 0xf18;
        uint32_t collision = 0x0498;
        uint32_t last_visible_time = 0x1A6C;
        uint32_t weaponx_target_fov = 0x165c;

        uint32_t bullet_velocity = 0x1e1c;
        uint32_t bullet_gravity_scale = 0x1e1c + 0x8;

        uint32_t head_component = 0x3F04;
        uint32_t eye_pos = 0x3F04;
        uint32_t velocity = 0x140 /*0x41C*/;
        uint32_t signifier_name_ptr = 0x560;

        uint32_t recoil = 0x23c8;
        uint32_t sway = local_angles - 0x10;
        uint32_t duckstate = 0x287c;
        uint32_t fflags = 0x0098;
        uint32_t jump_state = 0x40df018 + 0x8; // kbutton_t::state
        uint32_t studiohdr = 0x1110;
        uint32_t custom_script_int = 0x1608;
        uint32_t shield_health = 0x0170;
        uint32_t weapon_name_index = 0x17b8;
        uint32_t shield_health_max = 0x0174;
        uint32_t player_name = 0x3cb8;
        std::uintptr_t gafAsyncKeyState;
    };
}