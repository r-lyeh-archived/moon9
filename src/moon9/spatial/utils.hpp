#pragma once

#include "point.hpp"
#include "vec.hpp"

namespace math2d
{
    float len( const moon9::vec3 &v );
    float len2( const moon9::vec3 &v );
    moon9::vec3 norm( const moon9::vec3 &v );
    moon9::vec3 perp90( const moon9::vec3 &v );
    moon9::vec3 perp45( const moon9::vec3 &v );
    moon9::vec3 inv( const moon9::vec3 &v );
    float dot( const moon9::vec3 &v, const moon9::vec3 &w );
    float perp_dot( const moon9::vec3 &v, const moon9::vec3 &w );
}
namespace math3d
{
    float len( const moon9::vec3 &v );
    float len2( const moon9::vec3 &v );
    moon9::vec3 norm( const moon9::vec3 &v );
    moon9::vec3 inv( const moon9::vec3 &v );
    float dot( const moon9::vec3 &v, const moon9::vec3 &w );
    moon9::vec3 cross( const moon9::vec3 &v, const moon9::vec3 &w );
}

using namespace math3d;

float sqr( float n);
moon9::vec3 lerp( const moon9::vec3 &a, const moon9::vec3 &b, float dt01 );

double deg2rad( double deg );
double rad2deg( double rad );

float shortest_angle_xy( const moon9::vec3 &a, const moon9::vec3 &b );
float angle_xy( const moon9::vec3 &a, const moon9::vec3 &b ); //[-PI..PI]
moon9::vec3  rotate( const moon9::vec3 &v, float angle );

