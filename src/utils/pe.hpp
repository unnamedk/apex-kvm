#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <optional>

namespace apex::utils
{
    namespace detail
    {
        constexpr auto image_dos_signature = 0x5a4d; /* MZ */
        constexpr auto image_nt_signature = 0x4550; /* PE00 */

        constexpr auto IMAGE_DIRECTORY_ENTRY_EXPORT = 0;
        constexpr auto IMAGE_DIRECTORY_ENTRY_IMPORT = 1;

#pragma pack( push, 4 )
        struct dos_header_t
        {
            uint16_t e_magic;
            uint16_t e_cblp;
            uint16_t e_cp;
            uint16_t e_crlc;
            uint16_t e_cparhdr;
            uint16_t e_minalloc;
            uint16_t e_maxalloc;
            uint16_t e_ss;
            uint16_t e_sp;
            uint16_t e_csum;
            uint16_t e_ip;
            uint16_t e_cs;
            uint16_t e_lfarlc;
            uint16_t e_ovno;
            uint16_t e_res[ 4 ];
            uint16_t e_oemid;
            uint16_t e_oeminfo;
            uint16_t e_res2[ 10 ];
            uint32_t e_lfanew;
        };
        struct file_header_t
        {
            uint16_t machine;
            uint16_t num_sections;
            uint32_t time_datestamp;
            uint32_t pointer_symbol_table;
            uint32_t num_symbols;
            uint16_t size_opt_header;
            uint16_t characteristics;
        };
        struct data_directory_t
        {
            uint32_t VirtualAddress;
            uint32_t Size;
        };
        struct optional_header_t
        {
            uint16_t Magic;
            uint8_t MajorLinkerVersion;
            uint8_t MinorLinkerVersion;
            uint32_t SizeOfCode;
            uint32_t SizeOfInitializedData;
            uint32_t SizeOfUninitializedData;
            uint32_t AddressOfEntryPoint;
            uint32_t BaseOfCode;
            uint64_t ImageBase;
            uint32_t SectionAlignment;
            uint32_t FileAlignment;
            uint16_t MajorOperatingSystemVersion;
            uint16_t MinorOperatingSystemVersion;
            uint16_t MajorImageVersion;
            uint16_t MinorImageVersion;
            uint16_t MajorSubsystemVersion;
            uint16_t MinorSubsystemVersion;
            uint32_t Win32VersionValue;
            uint32_t SizeOfImage;
            uint32_t SizeOfHeaders;
            uint32_t CheckSum;
            uint16_t Subsystem;
            uint16_t DllCharacteristics;
            uint64_t SizeOfStackReserve;
            uint64_t SizeOfStackCommit;
            uint64_t SizeOfHeapReserve;
            uint64_t SizeOfHeapCommit;
            uint32_t LoaderFlags;
            uint32_t NumberOfRvaAndSizes;
            data_directory_t DataDirectory[ 16 ];
        };
#pragma pack( pop )

#pragma pack( push, 1 )

        struct section_header_t
        {
            uint8_t name[ 8 ];
            union
            {
                uint32_t physical_address;
                uint32_t virtual_size;
            } misc;
            uint32_t virtual_address;
            uint32_t size_raw_data;
            uint32_t pointer_raw_data;
            uint32_t pointer_relocations;
            uint32_t pointer_line_numbers;
            uint16_t num_relocations;
            uint16_t num_line_numbers;
            uint32_t characteristics;
        };
        struct export_directory_t
        {
            uint32_t Characteristics;
            uint32_t TimeDateStamp;
            uint16_t MajorVersion;
            uint16_t MinorVersion;
            uint32_t Name;
            uint32_t Base;
            uint32_t NumberOfFunctions;
            uint32_t NumberOfNames;
            uint32_t AddressOfFunctions;
            uint32_t AddressOfNames;
            uint32_t AddressOfNameOrdinals;
        };

        struct nt_headers_t
        {
            uint32_t signature;
            file_header_t file_header;
            optional_header_t optional_header;
        };
#pragma pack( pop )
    }

    struct export_data_t
    {
        std::string name;
        std::uint32_t rva;
    };

    class pe_view
    {
    private:
        std::uintptr_t m_module_base;
        std::size_t m_module_size;

    public:
        pe_view( std::uintptr_t module_base, std::size_t module_size )
            : m_module_base( module_base )
            , m_module_size( module_size )
        { }

        std::vector<export_data_t> exports();
        std::optional<export_data_t> get_export( const char *name );

        detail::dos_header_t &dos();
        detail::nt_headers_t &nt();
        detail::section_header_t *sections();
    };
}