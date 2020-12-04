#include "process.hpp"
#include "utils.hpp"
#include <cstdio>

apex::utils::process::~process()
{
    this->reset();
}

void apex::utils::process::set( std::uintptr_t base, std::uint32_t pid )
{
    this->reset();

    this->m_process = kernel_into_process_pid( kernel_clone( main_kernel ), pid );
    this->m_mem = process_virt_mem( this->m_process );
    this->m_base_address = base;
}

void apex::utils::process::reset()
{
    if ( !this->good() ) {
        return;
    }

    process_free( this->m_process );
    virt_free( this->m_mem );

    this->m_process = nullptr;
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