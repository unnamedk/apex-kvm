#include "offsets.hpp"
#include <spdlog/spdlog.h>

void apex::cheats::offsets_t::init()
{
    entity_list = 0x18c7af8;
    local_player_index = 0x11abf5c;
    clientstate = 0x127c040;
}