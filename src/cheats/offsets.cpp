#include "offsets.hpp"
#include <spdlog/spdlog.h>

void apex::cheats::offsets_t::init()
{
    // 0F B7 05 ? ? ? ? 39 42 08 75 11 48 8B 0A
    // lea     rax, qword_7FF7E52A7AF8
    entity_list = 0x18c7af8;

    // 0F 84 ? ? ? ? 49 89 5B 10 0F
    // mov     eax, cs:dword_7FF7E4B8BF5C
    local_player_index = 0x11abf5c;

        clientstate = 0x127c040;
}