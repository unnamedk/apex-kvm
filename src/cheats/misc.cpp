#include "misc.hpp"
#include "config/options.hpp"
#include "utils/utils.hpp"
#include <thread>
#include <cstring>
#include "utils/process.hpp"
#include <spdlog/spdlog.h>
#include <csignal>

using namespace std::chrono_literals;

std::optional<std::tuple<std::string, PID, Address>> get_process( Kernel *kernel, std::initializer_list<const char *> process_names )
{
    Win32ProcessInfo *processes[ 512 ];
    auto max = kernel_process_info_list( kernel, processes, 512 );

    std::optional<std::tuple<std::string, PID, Address>> ans;

    for ( uintptr_t i = 0; i < max; ++i ) {
        auto process = processes[ i ];

        // only allocate these resources if the process isn't found
        if ( ans == std::nullopt ) {
            OsProcessInfoObj *info = process_info_trait( process );

            char name[ 32 ] = { 0 };
            os_process_info_name( info, name, 32 );

            printf( "process \"%s\"\tpid %d\tbase %lx\n", name, os_process_info_pid( info ), process_info_section_base( process ) );
            for ( auto &n : process_names ) {
                if ( strcasestr( name, n ) != nullptr ) {
                    ans = std::make_tuple( std::string( name ), os_process_info_pid( info ), process_info_section_base( process ) );
                }
            };

            os_process_info_free( info );
        }

        process_info_free( process );
    }

    return ans;
}

std::optional<std::tuple<std::string, PID, Address>> get_process_by_id( Kernel *kernel, std::initializer_list<uint32_t> pids )
{
    Win32ProcessInfo *processes[ 512 ];
    auto max = kernel_process_info_list( kernel, processes, 512 );

    std::optional<std::tuple<std::string, PID, Address>> ans;

    for ( uintptr_t i = 0; i < max; ++i ) {
        auto process = processes[ i ];

        // only allocate these resources if the process isn't found
        if ( ans == std::nullopt ) {
            OsProcessInfoObj *info = process_info_trait( process );

            char name[ 32 ] = { 0 };
            os_process_info_name( info, name, 32 );

            auto pid = os_process_info_pid( info );
            for ( auto &n : pids ) {
                if ( pid == n ) {
                    ans = std::make_tuple( std::string( name ), pid, process_info_section_base( process ) );
                }
            };

            os_process_info_free( info );
        }

        process_info_free( process );
    }

    return ans;
}

void apex::cheats::misc::run()
{
    bool print_once = false;
    while ( true ) {
        //auto info = get_process( main_kernel, { "r5apex.exe", "EasyAntiCheat_launcher.exe" } );
        auto info = get_process_by_id( main_kernel, { 11676 } );

        options->misc.should_run = info != std::nullopt;
        if ( info ) {
            auto [ name, pid, base ] = *info;

            if ( !print_once ) {
                spdlog::info( "found process \"{}\" pid={} base={:#x}", name, pid, base );

                apex::utils::process::get().set( base, pid );

                print_once = true;
            }
        } else {
            print_once = false;
        }

        if ( utils::is_key_down( utils::keycode_t::pause ) ) {
            std::raise( SIGINT );
        }

        std::this_thread::sleep_for( 5ms );
    }
}