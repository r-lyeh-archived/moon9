#include <iostream>

#include "plane.hpp"

float distance( const moon9::plane &plane, const moon9::vec3 &point )
{
    return plane.d + dot( plane.normal, point );
}

std::ostream &operator<<( std::ostream &os, const moon9::plane &p )
{
    os << '{' << p.normal << ',' << p.d << '}';
    return os;
}
