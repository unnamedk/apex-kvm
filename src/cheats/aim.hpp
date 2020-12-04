#pragma once

#include "utils/process.hpp"
#include "utils/singleton.hpp"
#include "math/elem.hpp"
#include <optional>

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
        class aim : public utils::singleton<aim>
        {
        public:
            void run();

        private:
            bool should_iterate() const noexcept;
            bool validate_entity( sdk::entity_t *player ) const noexcept;

            sdk::entity_t *get_best_entity();
            math::vector3 select_pos( sdk::entity_t *player );

            std::optional<std::pair<float, float>> get_bullet_info();

            bool compensate_pos( sdk::player_t *player, math::vector3 &pos, math::qangle &ang );
            bool compensate_recoil( math::qangle &angles );

            bool smooth( math::qangle &angles );

            void aim_at( sdk::entity_t *entity );

            float smooth_x, smooth_y;
            float rand_aim_x, rand_aim_y, rand_aim_z;

            int m_target_index;
            void reset_state();
        };
    }
}