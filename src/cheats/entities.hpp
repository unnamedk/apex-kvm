#pragma once

#include <cstdint>
#include <vector>

#include "utils/singleton.hpp"
#include "sdk/entity.hpp"
#include <vector>

namespace apex::cheats
{
    class entity_list : public utils::singleton<entity_list>
    {
    public:
        entity_list();

        void run();

        sdk::entity_t *at( std::size_t i ) noexcept;
        sdk::player_t *get_local_player() noexcept { return static_cast<sdk::player_t *>( at( m_local_player_index ) ); }

        bool validate( sdk::entity_t *ent );

        auto begin() noexcept
        {
            return m_entity_list.begin();
        }
        auto cbegin() const noexcept { return m_entity_list.cbegin(); }
        auto end() noexcept { return m_entity_list.end(); }
        auto cend() const noexcept { return m_entity_list.cend(); }

    private:
        std::int16_t m_local_player_index;

        std::vector<sdk::entity_t *> m_entity_list;
        std::vector<sdk::ent_info_t> m_ent_info;
        std::vector<sdk::ent_info_t> m_old_ent_info;
    };
}