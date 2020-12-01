#pragma once

#include "elem.hpp"

namespace apex::math
{
    constexpr auto pi = 3.14159265358979323846f;
    constexpr auto rad_pi = 180.f / pi;
    constexpr auto pi_rad = pi / 180.f;
    constexpr auto golden_ratio = 1.6180339887f;

    constexpr auto to_degrees( float rad ) { return rad * rad_pi; }
    constexpr auto to_radians( float deg ) { return deg * pi_rad; }
    math::qangle vector_angles( const math::vector3 &src );
    math::qangle vector_angles( const math::vector3 &a, const math::vector3 &b );

    float rand_float( float min, float max );
    float get_fov( const math::qangle &a, const math::qangle &b );
}