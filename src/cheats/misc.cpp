#include "misc.hpp"
#include "config/options.hpp"
#include "utils/utils.hpp"
#include <thread>
#include <cstring>
#include "utils/process.hpp"
#include <spdlog/spdlog.h>
#include <csignal>

using namespace std::chrono_literals;

void apex::cheats::misc::run()
{
    while ( true ) {
#ifndef NDEBUG
        if ( utils::is_key_down( utils::keycode_t::pause ) ) {
            std::raise( SIGINT );
        }
#endif
        std::this_thread::sleep_for( 5ms );
    }
}