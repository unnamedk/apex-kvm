#include "pe.hpp"
#include <cstdio>

using namespace apex::utils::detail;
using apex::utils::pe_view;

std::vector<apex::utils::export_data_t> pe_view::exports()
{
    auto [ va, sz ] = nt().optional_header.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ];
    if ( !va || !sz ) {
        return {};
    }

    auto export_directory = reinterpret_cast<detail::export_directory_t *>( m_module_base + va );
    auto names = ( std::uint32_t * ) ( m_module_base + export_directory->AddressOfNames );
    auto functions = ( std::uint32_t * ) ( m_module_base + export_directory->AddressOfFunctions );
    auto ordinals = ( std::uint16_t * ) ( m_module_base + export_directory->AddressOfNameOrdinals );

    std::vector<export_data_t> ans;
    for ( auto i = 0u; i < export_directory->NumberOfFunctions; ++i ) {
        const bool contains_name = i < export_directory->NumberOfNames;
        std::uint16_t ordinal = contains_name ? ordinals[ i ] : i;

        auto fn_rva = functions[ ordinal ];
        if ( !fn_rva || !contains_name ) {
            continue;
        }

        auto fn = m_module_base + fn_rva;
        char *name = ( char * ) ( m_module_base + names[ i ] );

        const bool is_forwarded = ( fn >= ( m_module_base + va ) ) && ( fn <= ( m_module_base + va + sz ) );
        if ( is_forwarded ) {
            continue;
        }

        if ( contains_name ) {
            ans.emplace_back( name, fn_rva );
        }
    }
    return ans;
}

std::optional<apex::utils::export_data_t> pe_view::get_export( const char *name )
{
    auto list = exports();
    for ( auto e : list ) {
        if ( e.name == name ) {
            return e;
        }
    }

    return std::nullopt;
}

section_header_t *pe_view::sections()
{
    return reinterpret_cast<section_header_t *>( reinterpret_cast<std::uint8_t *>( &nt().optional_header ) +
        nt().file_header.size_opt_header );
}

nt_headers_t &pe_view::nt()
{
    return *reinterpret_cast<nt_headers_t *>( m_module_base + dos().e_lfanew );
}

dos_header_t &pe_view::dos()
{
    return *reinterpret_cast<dos_header_t *>( m_module_base );
}