#include "utils.hpp"

#include "pe.hpp"
#include <memory>
#include <cmath>
#include <thread>
#include <mutex>

Kernel *apex::main_kernel = nullptr;

bool apex::utils::is_key_down( keycode_t code )
{
    static auto values = []() {
        Address win32kbase_address = 0ull;
        size_t win32kbase_size = 0ull;

        {
            auto ntoskrnl = kernel_into_kernel_process( kernel_clone( main_kernel ) );

            auto win32_info = process_module_info( ntoskrnl, "win32kbase.sys" );
            auto win32_mod_info = module_info_trait( win32_info );

            win32kbase_address = os_process_module_base( win32_mod_info );
            win32kbase_size = os_process_module_size( win32_mod_info );

            os_process_module_free( win32_mod_info );
            module_info_free( win32_info );
            process_free( ntoskrnl );
        }

        auto user_process = kernel_into_process( kernel_clone( main_kernel ), "winlogon.exe" );
        auto vmem = process_virt_mem( user_process );

        auto module_buf = std::make_unique<std::uint8_t[]>( win32kbase_size );
        virt_read_raw_into( vmem, win32kbase_address, module_buf.get(), win32kbase_size );

        auto mod = apex::utils::pe_view { reinterpret_cast<std::uintptr_t>( module_buf.get() ), win32kbase_size };
        auto exp_gafAsyncKeyState = mod.get_export( "gafAsyncKeyState" );

        return std::make_pair( win32kbase_address + exp_gafAsyncKeyState->rva, vmem );
    }();
    static std::mutex key_mtx;
    std::lock_guard lock { key_mtx };

    auto [ gafAsyncKeyState, vmem ] = values;

    static std::array<std::uint8_t, 256 * 2 / 8>
        key_state_bitmap;
    static long last_tick_updated = 0;

    // only read if the bitmap wasn't updated in 10ms
    if ( auto tick = current_tick(); ( tick - last_tick_updated ) > 10 ) {
        virt_read_raw_into( vmem, gafAsyncKeyState, key_state_bitmap.data(), key_state_bitmap.size() );
        last_tick_updated = tick;
    }

    return key_state_bitmap[ ( static_cast<uint8_t>( code ) * 2 / 8 ) ] & 1 << static_cast<uint8_t>( code ) % 4 * 2;
}

long apex::utils::current_tick()
{
    timespec ts;
    clock_gettime( CLOCK_MONOTONIC, &ts );

    return ( ts.tv_nsec / 1'000'000l ) + ( ts.tv_sec * 1000l );
}
bool apex::utils::are_movement_keys_pressed()
{
    // i wonder how slow this is, the best solution is obviously to read the bitmap once
    return is_key_down( static_cast<keycode_t>( 'W' ) ) || is_key_down( static_cast<keycode_t>( 'A' ) ) ||
        is_key_down( static_cast<keycode_t>( 'S' ) ) || is_key_down( static_cast<keycode_t>( 'D' ) );
}
std::optional<std::tuple<std::string, PID, Address>> apex::utils::get_process_by_id( Kernel *kernel, std::initializer_list<uint32_t> pids )
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