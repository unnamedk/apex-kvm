#include "process.hpp"
#include "utils.hpp"
#include <cstdio>

void apex::utils::process::set( std::uintptr_t base, std::uint32_t pid )
{
    if ( this->m_process ) {
        process_free( this->m_process );
        virt_free( this->m_mem );

        this->m_process = nullptr;
    }

    auto kernel = kernel_clone( main_kernel );

    this->m_process = kernel_into_process_pid( kernel, pid );
    this->m_mem = process_virt_mem( this->m_process );
    this->m_base_address = base;
}

bool apex::utils::process::read_ptr( std::uintptr_t remote_address, void *local_address, std::size_t size ) noexcept
{
    std::lock_guard lg { m_mutex };
    return virt_read_raw_into( this->m_mem, remote_address, static_cast<uint8_t *>( local_address ), size ) != -1;
}

bool apex::utils::process::write_ptr( std::uintptr_t remote_address, void *local_address, std::size_t size ) noexcept
{
    std::lock_guard lg { m_mutex };
    return virt_write_raw( this->m_mem, remote_address, static_cast<uint8_t *>( local_address ), size ) != -1;
}
bool apex::utils::process::write_list( const std::vector<io_data_t> &list ) noexcept
{
    std::lock_guard lg { m_mutex };
    return virt_write_raw_list( this->m_mem, reinterpret_cast<const VirtualWriteData *>( list.data() ), list.size() ) != -1;
}
bool apex::utils::process::in_range( std::uintptr_t address ) noexcept
{
    return ( address >= this->m_base_address );
}