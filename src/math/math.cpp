#include "math.hpp"
#include <cmath>
#include <random>

apex::math::qangle apex::math::vector_angles( const math::vector3 &src )
{
    math::qangle ans;

    if ( src.x() == 0.f && src.y() == 0.f ) {
        ans.x() = ( src.z() > 0.f ) ? 270.f : 90.f;
    } else {
        ans.x() = static_cast<float>( to_degrees( ( atan2( src.z(), src.length_2d() ) ) ) );
        ans.y() = static_cast<float>( to_degrees( atan2( src.y(), src.x() ) ) );
    }

    return ans;
}

apex::math::qangle apex::math::vector_angles( const math::vector3 &a, const math::vector3 &b )
{
    auto delta = a - b;
    auto ans = vector_angles( delta.normalize() );

    if ( ans.y() > 90.f )
        ans.y() -= 180.f;
    else if ( ans.y() < 90.f )
        ans.y() += 180.f;
    else if ( ans.y() == 90.f )
        ans.y() = 0.f;

    ans.clamp();

    return ans;
}

float apex::math::get_fov( const math::qangle &a, const math::qangle &b )
{
    auto delta = ( b - a );
    delta.clamp();

    return sinf( to_radians( delta.length() ) / 2.f ) * 180.f;
}
float apex::math::rand_float( float min, float max )
{
    static std::minstd_rand engine { std::random_device()() };
    std::uniform_real_distribution<float> dist { min, max };
    return dist( engine );
}