#pragma once

#include <cstdint>
#include <mutex>
#include <vector>
#include "singleton.hpp"

#include "memflow.h"
#include "memflow_win32.h"

namespace apex::utils
{
    struct io_data_t
    {
        Address remote_address;
        std::uint8_t *local_buffer;
        std::uint64_t size;
    };

    class process : public singleton<process>
    {
    public:
        void set( std::uintptr_t base, std::uint32_t pid );
        bool good() { return this->m_process != nullptr; }

        template <typename T>
        inline auto read( std::uintptr_t remote_address, T &local, std::size_t sz = sizeof( T ) )
        {
            return read_ptr( remote_address, static_cast<void *>( &local ), sz );
        }

        template <typename T>
        inline auto write( std::uintptr_t remote_address, T &local, std::size_t size = sizeof( T ) )
        {
            return write_ptr( remote_address, &local, size );
        }

        bool read_ptr( std::uintptr_t remote_address, void *local_address, std::size_t size ) noexcept;
        bool read_list( const std::vector<io_data_t> &list ) noexcept;

        bool write_ptr( std::uintptr_t remote_address, void *local_address, std::size_t size ) noexcept;
        bool write_list( const std::vector<io_data_t> &list ) noexcept;
        bool in_range( std::uintptr_t address ) noexcept;

        std::uintptr_t base_address() const noexcept
        {
            return this->m_base_address;
        }

    private:
        Win32Process *m_process;
        VirtualMemoryObj *m_mem;
        std::mutex m_mutex;
        std::uintptr_t m_base_address;
        std::uint64_t m_section_size;
    };
}