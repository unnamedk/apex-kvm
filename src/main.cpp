#include <spdlog/spdlog.h>
#include <thread>
#include <atomic>
#include "config/options.hpp"

#include "memflow.h"
#include "memflow_win32.h"

#include "cheats/misc.hpp"
#include "cheats/esp.hpp"
#include "cheats/aim.hpp"
#include "cheats/offsets.hpp"
#include "cheats/entities.hpp"

#include "utils/pe.hpp"
#include "utils/utils.hpp"
#include "utils/raii.hpp"

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <httplib.h>

enum class types_t
{
    bool_t,
    int_t,
    float_t,
    color_t
};
struct ref_t
{
    void *ptr;
    types_t type;
};

std::unordered_map<std::string, ref_t> setting_map;
template <typename T>
ref_t to_type( T *ptr )
{
    if constexpr ( std::is_same_v<T, bool> ) {
        return ref_t { ptr, types_t::bool_t };
    } else if constexpr ( std::is_same_v<T, int> ) {
        return ref_t { ptr, types_t::int_t };
    } else if constexpr ( std::is_same_v<T, float> ) {
        return ref_t { ptr, types_t::float_t };
    } else if constexpr ( std::is_same_v<T, apex::sdk::color_t> ) {
        return ref_t { ptr, types_t::color_t };
    }
}

using apex::options;

void build_map()
{
    setting_map[ "aimbot.enabled" ] = to_type( &options->aimbot.enabled );
    setting_map[ "aimbot.smooth_enabled" ] = to_type( &options->aimbot.smooth_enabled );
    setting_map[ "aimbot.fov" ] = to_type( &options->aimbot.fov );
    setting_map[ "aimbot.smooth_factor" ] = to_type( &options->aimbot.smooth_factor );
    setting_map[ "aimbot.rcs_enabled" ] = to_type( &options->aimbot.rcs_enabled );
    setting_map[ "aimbot.target_selection_key" ] = to_type( &options->aimbot.target_selection_key );
    setting_map[ "aimbot.target_selection_switch_on_key" ] = to_type( &options->aimbot.target_selection_switch_on_key );
    setting_map[ "aimbot.maximum_distance" ] = to_type( &options->aimbot.maximum_distance );
    setting_map[ "aimbot.aim_on_friends" ] = to_type( &options->aimbot.aim_on_friends );
    setting_map[ "aimbot.aim_on_dummies" ] = to_type( &options->aimbot.aim_on_dummies );
    setting_map[ "aimbot.key" ] = to_type( &options->aimbot.key );
    setting_map[ "aimbot.primary_hitbox" ] = to_type( &options->aimbot.primary_hitbox );
    setting_map[ "aimbot.secondary_hitbox_key" ] = to_type( &options->aimbot.secondary_hitbox_key );
    setting_map[ "aimbot.secondary_hitbox" ] = to_type( &options->aimbot.secondary_hitbox );
    setting_map[ "aimbot.target_lock" ] = to_type( &options->aimbot.target_lock );
    setting_map[ "aimbot.vis_check" ] = to_type( &options->aimbot.vis_check );
    setting_map[ "aimbot.random_aim_spot" ] = to_type( &options->aimbot.random_aim_spot );

    setting_map[ "visual.enabled" ] = to_type( &options->visual.enabled );
    setting_map[ "visual.item_esp" ] = to_type( &options->visual.item_esp );
    setting_map[ "visual.glow_esp" ] = to_type( &options->visual.glow_esp );
    setting_map[ "visual.aim_target_esp" ] = to_type( &options->visual.aim_target_esp );
    setting_map[ "visual.highlight_friends" ] = to_type( &options->visual.highlight_friends );
    setting_map[ "visual.color_health_based" ] = to_type( &options->visual.color_health_based );
    setting_map[ "visual.health_based_alpha" ] = to_type( &options->visual.health_based_alpha );
    setting_map[ "visual.esp_color" ] = to_type( &options->visual.esp_color );
    setting_map[ "visual.aimbot_target_color" ] = to_type( &options->visual.aimbot_target_color );
    setting_map[ "visual.downed_color" ] = to_type( &options->visual.downed_color );
    setting_map[ "visual.friend_color" ] = to_type( &options->visual.friend_color );
}

void start_server( const char *local_ip )
{
    using namespace httplib;

    auto menu_dir = std::filesystem::current_path() / "menu";
    if ( !std::filesystem::exists( menu_dir ) ) {
        spdlog::error( "directory 'menu/' not found in current folder '{}'", std::filesystem::current_path().string() );
        return;
    }

    build_map();

    Server srv;
    if ( !srv.is_valid() ) {
        spdlog::error( "error starting server" );
        return;
    }

    srv.set_base_dir( "menu/", "/menu" );

    srv.Get( "/settings", [ = ]( const Request &, Response &res ) {
        std::stringstream json;
        {
            cereal::JSONOutputArchive archive( json );
            archive( cereal::make_nvp( "config", *options ) );
        }

        res.set_header( "Access-Control-Allow-Origin", "*" );
        res.set_content( json.str(), "application/json" );
    } );

    srv.Get( "/changed", [ & ]( const Request &r, Response &res ) {
        auto type = r.get_param_value( "t" );
        if ( type == "fn" ) {
            auto var = r.get_param_value( "r" );
            spdlog::info( "function called: {}", var );

            /* functions */
            if ( var == "save" ) {
                options->save();
            } else if ( var == "load" ) {
                options->load();
            } else if ( var == "exit" ) {
                srv.stop();
            }
        } else if ( type == "checkbox" ) {
            auto var = r.get_param_value( "r" );
            auto val = r.get_param_value( "v" );

            auto [ ptr, t ] = setting_map[ var ];
            if ( !ptr ) {
                spdlog::warn( "pointer to '{}' variable is null", var );
                return;
            }
            spdlog::info( "'{}' -> '{}'", var, val );

            *( bool * ) ptr = ( val[ 0 ] != 'f' /* 'false' */ );
        } else if ( type == "value" ) {
            auto var = r.get_param_value( "r" );
            auto val = r.get_param_value( "v" );

            auto [ ptr, type ] = setting_map[ var ];
            if ( !ptr ) {
                spdlog::warn( "pointer to '{}' variable is null", var );
                return;
            }
            spdlog::info( "'{}' -> '{}'", var, val );
            switch ( type ) {
                case types_t::float_t: {
                    float result = std::stof( val );
                    *( float * ) ptr = result;
                    break;
                }
                case types_t::int_t: {
                    int result = std::stoi( val );
                    *( int * ) ptr = result;
                    break;
                }
                case types_t::color_t: {
                    auto color = std::stoull( val, nullptr, 16 );
                    reinterpret_cast<apex::sdk::color_t *>( ptr )->from_argb( color );
                    break;
                }

                default: {
                    spdlog::warn( "invalid type: {}", static_cast<int>( type ) );
                    break;
                }
            }
        }
        res.set_header( "Access-Control-Allow-Origin", "*" );
        res.set_content( "{\"success\":\"true\"}", "application/json" );
        auto var = r.get_param_value( "r" );
    } );

    std::thread stop_thread { [ & ]() {
        while ( options->misc.should_run ) {
            std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
        }

        srv.stop();
    } };

    stop_thread.detach();

    spdlog::info( "starting web server" );
    srv.listen( local_ip, 2222 );
}

void print_help( const char *path )
{
    spdlog::info( "usage: ./{} local_ip qemu_pid apex_pid", std::filesystem::path { path }.filename().string() );
}

std::atomic_bool g_interrupted;
void catch_interrupt( int )
{
    options->misc.should_run = false;
    g_interrupted = true;
}

int main( int argc, const char **argv )
{
    if ( argc < 3 ) {
        spdlog::error( "invalid arguments" );
        print_help( argv[ 0 ] );

        return 1;
    }

    struct sigaction sa
    { };
    sa.sa_handler = catch_interrupt;
    sigfillset( &sa.sa_mask );
    sigaction( SIGINT, &sa, nullptr );

    auto local_ip = argv[ 1 ];
    auto qemu_pid = argv[ 2 ];
    auto apex_pid = ( argc == 4 ) ? argv[ 3 ] : nullptr;

    uint32_t apex_process_id = 0;
    if ( apex_pid ) {
        std::from_chars( apex_pid, apex_pid + strlen( apex_pid ), apex_process_id );
    }

    apex::options.emplace();
    log_init( 1 );

    apex::utils::owned_resource inv { inventory_try_new(), inventory_free };
    CloneablePhysicalMemoryObj *conn = inventory_create_connector( inv, "kvm", qemu_pid );
    if ( !conn ) {
        return 0;
    }

    apex::utils::owned_resource kernel { kernel_build( conn ), kernel_free };
    if ( !kernel ) {
        spdlog::error( "error initializing kernel" );
        return 0;
    }

    auto ver = kernel_winver( kernel );
    spdlog::info( "initialized successfully: windows {}.{} build {}", ver.nt_major_version,
        ver.nt_minor_version,
        ver.nt_build_number );

    apex::main_kernel = kernel;

    // if it specifies the target pid then use it
    // apparently i don't free something so dead process stay alive in the process list
    auto apex_process = apex::utils::get_process( kernel, [ apex_process_id ]( std::string_view name, PID pid ) {
        if ( apex_process_id ) {
            return pid == apex_process_id;
        }

        else {
            auto processes = { "r5apex.exe", "EasyAntiCheat_launcher.exe" };
            for ( auto &n : processes ) {
                if ( n == name ) {
                    return true;
                }
            }
        }

        return false;
    } );
    if ( !apex_process ) {
        spdlog::error( "game process (id {}) not found", apex_process_id );
        return 1;
    }

    auto [ name, pid, base ] = *apex_process;
    spdlog::info( "found process \"{}\" pid={} base={:#x}", name, pid, base );

    apex::utils::process::get().set( base, pid );
    options->misc.should_run = true;

    apex::cheats::offsets_t::get().init();

    std::thread t_misc( &apex::cheats::misc::run, std::ref( apex::cheats::misc::get() ) );
    std::thread t_aim( &apex::cheats::aim::run, std::ref( apex::cheats::aim::get() ) );
    std::thread t_esp( &apex::cheats::esp::run, std::ref( apex::cheats::esp::get() ) );
    std::thread t_entities( &apex::cheats::entity_list::run, std::ref( apex::cheats::entity_list::get() ) );

    t_misc.detach();
    while ( !apex::utils::process::get().good() ) {
        std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );
    }
    t_entities.detach();
    t_aim.detach();
    t_esp.detach();

    // blocks until user wants to exit
    start_server( local_ip );

    spdlog::info( "exiting..." );
    options->save();
    apex::utils::process::get().reset();
}
