#pragma once

#include <filesystem>
#include <optional>
#include "cheats/sdk/color.hpp"
#include <cereal/cereal.hpp>

namespace apex
{
    namespace config
    {
        namespace detail
        {
            template <class Archive, typename... Args>
            void safe_serialize( Archive &a, Args &&... args )
            {
                // don't crash when an expected NVP is not found
                try {
                    a( std::forward<Args>( args )... );
                } catch ( ... ) {
                    a.setNextName( nullptr );
                }
            }
        }

        struct options_t
        {
        private:
            std::filesystem::path current_config;

        public:
            options_t();

            void save();
            void load();

            struct
            {
                bool enabled = false;
                std::int32_t key = 0;

                bool smooth_enabled = true;
                float smooth_factor = 11.f;

                float maximum_distance = 75.f;
                float fov = 45.f;

                bool aim_on_friends = false;
                bool rcs_enabled = true;

                std::int32_t primary_hitbox = 0;

                std::int32_t secondary_hitbox_key = 0;
                std::int32_t secondary_hitbox = 0;

                bool target_selection_switch_on_key = false;
                std::int32_t target_selection_key = 0;

                bool aim_on_dummies = false;
                bool vis_check = false;
                bool random_aim_spot = true;
                bool target_lock = false;

                std::uintptr_t current_target = 0ull;

                template <typename Archive>
                void serialize( Archive &archive )
                {
                    detail::safe_serialize( archive, CEREAL_NVP( enabled ), CEREAL_NVP( key ), CEREAL_NVP( fov ),
                        CEREAL_NVP( smooth_enabled ), CEREAL_NVP( smooth_factor ), CEREAL_NVP( rcs_enabled ),
                        CEREAL_NVP( primary_hitbox ), CEREAL_NVP( secondary_hitbox ), CEREAL_NVP( secondary_hitbox_key ),
                        CEREAL_NVP( target_selection_switch_on_key ), CEREAL_NVP( target_selection_key ),
                        CEREAL_NVP( maximum_distance ), CEREAL_NVP( aim_on_friends ), CEREAL_NVP( target_lock ), CEREAL_NVP( vis_check ), CEREAL_NVP( random_aim_spot ) );
                }
            } aimbot;

            struct
            {
                bool enabled = false;

                bool aim_target_esp = true;
                bool glow_esp = true;
                bool item_esp = true;

                bool highlight_friends = false;
                bool color_health_based = true;
                float health_based_alpha = 0.4f;

                sdk::color_t esp_color = sdk::color_t::magenta();
                sdk::color_t aimbot_target_color = sdk::color_t::cyan();
                sdk::color_t downed_color = sdk::color_t::salmon();
                sdk::color_t friend_color = sdk::color_t::aquamarine();

                template <typename Archive>
                void serialize( Archive &archive )
                {
                    detail::safe_serialize( archive, CEREAL_NVP( enabled ), CEREAL_NVP( item_esp ), CEREAL_NVP( aim_target_esp ), CEREAL_NVP( glow_esp ),
                        CEREAL_NVP( color_health_based ), CEREAL_NVP( highlight_friends ), CEREAL_NVP( health_based_alpha ),
                        CEREAL_NVP( esp_color ), CEREAL_NVP( aimbot_target_color ), CEREAL_NVP( downed_color ), CEREAL_NVP( friend_color ) );
                }
            } visual;

            struct
            {
                bool should_run = false;
            } misc;

            template <typename Archive>
            void serialize( Archive &archive )
            {
                detail::safe_serialize( archive, CEREAL_NVP( aimbot ), CEREAL_NVP( visual ) );
            }
        };
    }

    extern std::optional<config::options_t> options;
}