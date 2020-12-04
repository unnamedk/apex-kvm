#pragma once

#include "utils/singleton.hpp"
#include "sdk/sdk.hpp"
#include <array>

#include "utils/process.hpp"

namespace apex
{
    namespace sdk
    {
        struct entity_t;
        struct player_t;
        struct item_t;
    }

    namespace cheats
    {
        class esp : public utils::singleton<esp>
        {
        public:
            void run();

        private:
            bool should_iterate() const noexcept;
            bool validate_player( sdk::player_t *player ) const noexcept;

            void apply_glow( sdk::player_t *entity );
            void apply_glow( sdk::item_t *entity );
            void get_color_for_entity( sdk::player_t *player, std::array<float, 3> &clr );
        };
    }
}