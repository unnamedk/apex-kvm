#pragma once

#include "elem.hpp"

namespace apex::math
{
    class ProjectileWeapon
    {
    public:
        ProjectileWeapon( float speed, float grav )
            : m_speed( speed )
            , m_gravity( grav )
        { }

        float get_projectile_speed() const { return this->m_speed; }
        float get_projectile_gravity() const { return this->m_gravity; }
        virtual vector3 get_projectile_fire_setup( const vector3 &origin, const vector3 &target ) const
        {
            return target - origin;
        }

    private:
        float m_speed;
        float m_gravity;
    };

    // Simple linear extrapolation.
    class LinearPredictor
    {
    public:
        inline LinearPredictor( vector3 origin, vector3 velocity )
            : origin( origin )
            , velocity( velocity )
        { }
        vector3 predict_position( float time ) const;

    public:
        vector3 origin;
        vector3 velocity;
    };

    struct Solution
    {
        // Aim the weapon at these angles to hit the target.
        // NOTE! These angles are in radians and not normalized to the game's conventions!
        float pitch, yaw;
        // Projectile travel time.
        float time;
    };

    // Given a projectile weapon and a predictable target, solve where to aim the weapon to thit the target.
    // NOTE! Solution is returned in radians! Don't forget to convert to degrees if needed!
    bool solve( const vector3 &origin, const ProjectileWeapon &weapon, const LinearPredictor &target, Solution &sol );
}