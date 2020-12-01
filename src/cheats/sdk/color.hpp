#pragma once

#include <cereal/cereal.hpp>

#include <cstdint>
#include <array>
#include <charconv>
#include <byteswap.h>

#define SETCOLOR( name, r, g, b )          \
    static color_t name( int alpha = 175 ) \
    {                                      \
        return { r, g, b, alpha };         \
    }

namespace apex::sdk
{
    class color_t
    {
    public:
        constexpr color_t( std::uint8_t r = 0, std::uint8_t g = 0, std::uint8_t b = 0, std::uint8_t a = 175 )
            : m_buffer { { r, g, b, a } }
        { }

        constexpr color_t( int r, int g = 0, int b = 0, int a = 175 )
            : color_t( static_cast<std::uint8_t>( r & 0xff ),
                  static_cast<std::uint8_t>( g & 0xff ),
                  static_cast<std::uint8_t>( b & 0xff ),
                  static_cast<std::uint8_t>( a & 0xff ) )
        { }

        constexpr color_t( unsigned int hex )
            : color_t( static_cast<int>( hex & 0xff000000 ) >> 24,
                  static_cast<int>( hex & 0xff0000 ) >> 16,
                  static_cast<int>( hex & 0xff00 ) >> 8,
                  static_cast<int>( hex & 0xff ) )
        { }

        constexpr color_t( float r, float g = 0.f, float b = 0.f, float a = 0.75f )
            : color_t( static_cast<int>( r * 255.f ),
                  static_cast<int>( g * 255.f ),
                  static_cast<int>( b * 255.f ),
                  static_cast<int>( a * 255.f ) )
        { }

        const std::uint8_t *data() const { return this->m_buffer.data(); }
        std::uint8_t *data() { return this->m_buffer.data(); }

        uint8_t operator[]( size_t index ) const { return m_buffer[ index ]; }
        uint8_t &operator[]( size_t index ) { return m_buffer[ index ]; }

        int r() const { return m_buffer[ 0 ]; }
        int g() const { return m_buffer[ 1 ]; }
        int b() const { return m_buffer[ 2 ]; }
        int a() const { return m_buffer[ 3 ]; }

        template <typename Archive, cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value> = cereal::traits::sfinae>
        std::string save_minimal( Archive const & ) const
        {
            std::string ans;
            ans.resize( 8 );
            std::to_chars( ans.data(), ans.data() + ans.size(), bswap_32( *reinterpret_cast<const std::uint32_t *>( this->data() ) ), 16 );

            return ans;
        }

        template <typename Archive, cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value> = cereal::traits::sfinae>
        void load_minimal( Archive &, std::string const &value )
        {
            std::uint32_t v = 0u;
            std::from_chars( value.data(), value.data() + value.size(), v, 16 );

            *reinterpret_cast<std::uint32_t *>( this->data() ) = bswap_32( v );
        }

        void from_argb( std::uint64_t argb )
        {
            m_buffer[ 3 ] = ( argb >> 24 ) & 0xff;
            m_buffer[ 0 ] = ( argb >> 16 ) & 0xff;
            m_buffer[ 1 ] = ( argb >> 8 ) & 0xff;
            m_buffer[ 2 ] = ( argb ) &0xff;
        }

        SETCOLOR( red, 255, 0, 0 );
        SETCOLOR( green, 0, 255, 0 );
        SETCOLOR( blue, 0, 0, 255 );

        SETCOLOR( light_green, 144, 238, 144 );
        SETCOLOR( light_blue, 173, 216, 230 );

        SETCOLOR( white, 255, 255, 255 );
        SETCOLOR( antique_white, 250, 235, 215 );
        SETCOLOR( medium_turquoise, 72, 209, 204 );
        SETCOLOR( cyan, 0, 255, 255 );
        SETCOLOR( aquamarine, 127, 255, 212 );
        SETCOLOR( black, 0, 0, 0 );
        SETCOLOR( blue_violet, 138, 43, 226 );
        SETCOLOR( brown, 165, 42, 42 );

        SETCOLOR( sky_blue, 135, 206, 235 );
        SETCOLOR( spring_green, 0, 255, 127 );
        SETCOLOR( tomato, 255, 99, 71 );
        SETCOLOR( violet, 238, 130, 238 );
        SETCOLOR( yellow, 255, 255, 0 );
        SETCOLOR( turquoise, 64, 224, 208 );
        SETCOLOR( purple, 128, 0, 128 );
        SETCOLOR( pink, 255, 192, 203 );
        SETCOLOR( magenta, 255, 0, 255 );
        SETCOLOR( indigo, 75, 0, 130 );
        SETCOLOR( gold, 255, 215, 0 );

        SETCOLOR( dark_orange, 255, 140, 0 );
        SETCOLOR( dark_orchid, 153, 50, 204 );
        SETCOLOR( dark_salmon, 233, 150, 122 );

        SETCOLOR( orange, 255, 165, 0 );
        SETCOLOR( orchid, 218, 112, 214 );
        SETCOLOR( salmon, 250, 128, 114 );

        SETCOLOR( silver, 192, 192, 192 );

    private:
        std::array<std::uint8_t, 4u> m_buffer;
    };
}