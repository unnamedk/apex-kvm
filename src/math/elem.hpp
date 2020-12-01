#pragma once
#include "base_array.hpp"
#include <algorithm>
#include <cstring>

namespace apex::math
{
    class vector2 : public base_array<vector2, float, 2>
    {
    public:
        using base_array::base_array;

        vector2( float x = 0.f, float y = 0.f )
        {
            at( 0 ) = x;
            at( 1 ) = y;
        }

        constexpr float &x() { return at( 0 ); }
        constexpr float &y() { return at( 1 ); }
        constexpr const float &x() const { return at( 0 ); }
        constexpr const float &y() const { return at( 1 ); }
    };

    class vector3 : public base_array<vector3, float, 3>
    {
    public:
        using base_array::base_array;

        vector3( float x = 0.f, float y = 0.f, float z = 0.f )
        {
            at( 0 ) = x;
            at( 1 ) = y;
            at( 2 ) = z;
        }

        constexpr float &x() { return at( 0 ); }
        constexpr float &y() { return at( 1 ); }
        constexpr float &z() { return at( 2 ); }

        constexpr const float &x() const { return at( 0 ); }
        constexpr const float &y() const { return at( 1 ); }
        constexpr const float &z() const { return at( 2 ); }
        bool operator==( const vector3 &rhs )
        {
            return ( x() == rhs.x() ) && ( y() == rhs.y() ) && ( z() == rhs.z() );
        }

        vector3 &operator=( const vector3 &rhs ) = default;

        inline float dot( const float *arr ) const
        {
            return this->x() * arr[ 0 ] + this->y() * arr[ 1 ] + this->z() * arr[ 2 ];
        }

        vector3 &normalize()
        {
            auto inv = 1.f / length();
            x() *= inv;
            y() *= inv;
            z() *= inv;

            return *this;
        }

        float length_2d() const { return std::sqrt( ( x() * x() ) + ( y() * y() ) ); }
        float length_2d_sqr() const { return x() * x() + y() * y(); }
    };

    class vector4 : public base_array<vector4, float, 4>
    {
    public:
        using base_array::base_array;

        void operator=( const math::vector3 &rhs )
        {
            std::copy( rhs.cbegin(), rhs.cend(), this->begin() );
        }

        vector4 operator+( const math::vector3 &rhs )
        {
            math::vector4 ans;
            std::transform( rhs.cbegin(), rhs.cend(), cbegin(), ans.begin(), std::plus<float>() );

            return ans;
        }
    };

    class qangle : public base_array<qangle, float, 3>
    {
    public:
        using base_array::base_array;

        qangle( float x = 0.f, float y = 0.f, float z = 0.f )
        {
            at( 0 ) = x;
            at( 1 ) = y;
            at( 2 ) = z;
        }

        constexpr float &x() { return at( 0 ); }
        constexpr float &y() { return at( 1 ); }
        constexpr float &z() { return at( 2 ); }

        constexpr const float &x() const { return at( 0 ); }
        constexpr const float &y() const { return at( 1 ); }
        constexpr const float &z() const { return at( 2 ); }

        qangle &operator-=( const qangle &other )
        {
            x() -= other.x();
            y() -= other.y();
            z() -= other.z();

            return *this;
        }

        inline qangle operator*( float scale ) const
        {
            return qangle { x() * scale, y() * scale, z() * scale };
        }

        constexpr inline void normalize()
        {
            if ( x() > 89.f || x() < -89.f ) {
                const auto revs = std::round( std::abs( x() / 180.f ) );
                x() = ( x() < 0.f ) ?
                    x() + 180.f * revs :
                    x() - 180.f * revs;
            }

            if ( y() > 180.f || y() < -180.f ) {
                const auto revs = std::round( std::abs( y() / 360.f ) );
                y() = ( y() < 0.f ) ?
                    y() + 360.f * revs :
                    y() - 360.f * revs;
            }
        }

        void clamp()
        {
            for ( auto &pos : *this ) {
                if ( !std::isfinite( pos ) ) {
                    pos = 0.0f;
                }
            }

            this->normalize();

            this->x() = std::clamp( this->x(), -89.0f, 89.0f );
            this->y() = std::clamp( this->y(), -180.0f, 180.0f );
            this->z() = 0.0f;
        }
    };

    class matrix3x4
    {
        float values[ 3 ][ 4 ];

    public:
        matrix3x4() = default;
        matrix3x4( float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23 )
        {
            values[ 0 ][ 0 ] = m00;
            values[ 0 ][ 1 ] = m01;
            values[ 0 ][ 2 ] = m02;
            values[ 0 ][ 3 ] = m03;
            values[ 1 ][ 0 ] = m10;
            values[ 1 ][ 1 ] = m11;
            values[ 1 ][ 2 ] = m12;
            values[ 1 ][ 3 ] = m13;
            values[ 2 ][ 0 ] = m20;
            values[ 2 ][ 1 ] = m21;
            values[ 2 ][ 2 ] = m22;
            values[ 2 ][ 3 ] = m23;
        }
        matrix3x4( const vector3 &x_axis, const vector3 &y_axis, const vector3 &z_axis, const vector3 &origin )
        {
            values[ 0 ][ 0 ] = x_axis.x();
            values[ 0 ][ 1 ] = y_axis.x();
            values[ 0 ][ 2 ] = z_axis.x();
            values[ 0 ][ 3 ] = origin.x();
            values[ 1 ][ 0 ] = x_axis.y();
            values[ 1 ][ 1 ] = y_axis.y();
            values[ 1 ][ 2 ] = z_axis.y();
            values[ 1 ][ 3 ] = origin.y();
            values[ 2 ][ 0 ] = x_axis.z();
            values[ 2 ][ 1 ] = y_axis.z();
            values[ 2 ][ 2 ] = z_axis.z();
            values[ 2 ][ 3 ] = origin.z();
        }

        void set_origin( const vector3 &point )
        {
            values[ 0 ][ 3 ] = point.x();
            values[ 1 ][ 3 ] = point.y();
            values[ 2 ][ 3 ] = point.z();
        }

        auto data() { return values; }
        const auto data() const { return values; }

        constexpr void invalidate()
        {
            for ( int i = 0; i < 3; ++i )
                for ( int j = 0; j < 4; ++j )
                    values[ i ][ j ] = std::numeric_limits<float>::infinity();
        }

        float *operator[]( size_t index ) { return values[ index ]; }
        const float *operator[]( size_t index ) const { return values[ index ]; }
    };

    class matrix4x4
    {
        float values[ 4 ][ 4 ];

        void init( float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33 )
        {
            values[ 0 ][ 0 ] = m00;
            values[ 0 ][ 1 ] = m01;
            values[ 0 ][ 2 ] = m02;
            values[ 0 ][ 3 ] = m03;
            values[ 1 ][ 0 ] = m10;
            values[ 1 ][ 1 ] = m11;
            values[ 1 ][ 2 ] = m12;
            values[ 1 ][ 3 ] = m13;
            values[ 2 ][ 0 ] = m20;
            values[ 2 ][ 1 ] = m21;
            values[ 2 ][ 2 ] = m22;
            values[ 2 ][ 3 ] = m23;
            values[ 3 ][ 0 ] = m30;
            values[ 3 ][ 1 ] = m31;
            values[ 3 ][ 2 ] = m32;
            values[ 3 ][ 3 ] = m33;
        }

    public:
        matrix4x4( float m00 = 0.f, float m01 = 0.f, float m02 = 0.f, float m03 = 0.f, float m10 = 0.f, float m11 = 0.f, float m12 = 0.f, float m13 = 0.f, float m20 = 0.f,
            float m21 = 0.f, float m22 = 0.f, float m23 = 0.f, float m30 = 0.f, float m31 = 0.f, float m32 = 0.f, float m33 = 0.f )
        {
            init(
                m00, m01, m02, m03,
                m10, m11, m12, m13,
                m20, m21, m22, m23,
                m30, m31, m32, m33 );
        }

        matrix4x4( const matrix3x4 &mat )
        {
            std::memcpy( values, mat.data(), sizeof( matrix3x4 ) );
            values[ 3 ][ 0 ] = values[ 3 ][ 1 ] = values[ 3 ][ 2 ] = 0.f;
            values[ 3 ][ 3 ] = 1.0f;
        }

        auto data() { return values; }
        const auto data() const { return values; }

        float *operator[]( size_t index ) { return values[ index ]; }
        const float *operator[]( size_t index ) const { return values[ index ]; }
    };
}