#include "point.hpp"
#include "vec.hpp"

#include "utils.hpp"


namespace math2d
{
    float len( const moon9::vec3 &v )
    {
        return sqrt( v.x*v.x + v.y*v.y );
    }
    float len2( const moon9::vec3 &v )
    {
        return v.x*v.x + v.y*v.y;
    }
    moon9::vec3 norm( const moon9::vec3 &v )
    {
        float length = len(v);
        return length ? moon9::vec3( v.x/length, v.y/length, 0 ) : moon9::vec3( 0,0,0 );
    }
    moon9::vec3 perp90( const moon9::vec3 &v )
    {
        return moon9::vec3( -v.y, v.x );
    }
    moon9::vec3 perp45( const moon9::vec3 &v )
    {
        const double invroot2 = 0.70710678118654752440084436210485;
        moon9::vec3 vv = perp90( v );
        return moon9::vec3( (v.x+vv.x)*invroot2, (v.y+vv.y)*invroot2 );
    }
    moon9::vec3 inv( const moon9::vec3 &v )
    {
        return moon9::vec3( -v.x, -v.y );
    }
    float dot( const moon9::vec3 &v, const moon9::vec3 &w )
    {
        return v.x * w.x + v.y * w.y;
    }
    float perp_dot( const moon9::vec3 &v, const moon9::vec3 &w )
    {
        return v.x * w.y - v.y * w.x;
    }
}
namespace math3d
{
    float len( const moon9::vec3 &v )
    {
        return sqrt( v.x*v.x + v.y*v.y + v.z*v.z );
    }
    float len2( const moon9::vec3 &v )
    {
        return v.x*v.x + v.y*v.y + v.z*v.z;
    }
    moon9::vec3 norm( const moon9::vec3 &v )
    {
        float length = len(v);
        return length ? moon9::vec3( v.x/length, v.y/length, v.z/length ) : moon9::vec3( 0,0,0 );
    }
    moon9::vec3 inv( const moon9::vec3 &v )
    {
        return moon9::vec3( -v.x, -v.y, -v.z );
    }
    float dot( const moon9::vec3 &v, const moon9::vec3 &w )
    {
        return v.x * w.x + v.y * w.y + v.z * w.z;
    }
    moon9::vec3 cross( const moon9::vec3 &a, const moon9::vec3 &b )
    {
    #if 1
        moon9::vec3 result;
        result.x = a.y * b.z - a.z * b.y;
        result.y = a.z * b.x - a.x * b.z;
        result.z = a.x * b.y - a.y * b.x;
        return result;
    #else
        return math3d::cross( a, b );
    #endif
    }
}

float sqr( float n )
{
    return n * n;
}

moon9::vec3 lerp( const moon9::vec3 &a, const moon9::vec3 &b, float dt01 )
{
    return a * moon9::vec3( dt01, dt01, dt01 ) + b * moon9::vec3( 1.f - dt01, 1.f - dt01, 1.f - dt01 );
}

double deg2rad( double deg )
{
    const double _M_PI = 3.1415926535897932384626433832795;
    return (deg * _M_PI / 180);
}

double rad2deg( double rad )
{
    const double _M_PI = 3.1415926535897932384626433832795;
    return (rad * 180 / _M_PI);
}

float shortest_angle_xy( const moon9::vec3 &a, const moon9::vec3 &b )
{
    float angle_rad = acos( dot( norm(a), norm(b) ) );
    return rad2deg( angle_rad );
}

float angle_xy( const moon9::vec3 &a, const moon9::vec3 &b ) //[-PI..PI]
{
    float angle = atan2( math2d::perp_dot(a,b), math2d::dot(a,b) );
    return rad2deg(angle);
}

moon9::vec3 rotate( const moon9::vec3 &v, float angle )
{
    angle = deg2rad(angle);

    float c = cos(angle);
    float s = sin(angle);
    return moon9::vec3( v.x * c - v.y * s, v.x * s  + v.y * c, v.z );
}
