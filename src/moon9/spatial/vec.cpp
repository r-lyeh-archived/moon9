#include <iostream>

#include "vec.hpp"

std::ostream &operator <<( std::ostream &os, const moon9::vec2 &v )
{
    os << "{x:" << v.x;
    os << ",y:" << v.y;
    os << "}";
    return os;
}
std::ostream &operator <<( std::ostream &os, const moon9::vec3 &v )
{
    os << "{x:" << v.x;
    os << ",y:" << v.y;
    os << ",z:" << v.z;
    os << "}";
    return os;
}
std::ostream &operator <<( std::ostream &os, const moon9::vec4 &v )
{
    os << "{x:" << v.x;
    os << ",y:" << v.y;
    os << ",z:" << v.z;
    os << ",w:" << v.w;
    os << "}";
    return os;
}
