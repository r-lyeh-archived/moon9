#include <iostream>
#include <sstream>
#include <string>
#include <vector>

//#include <moon9/string/id.hpp>

#include "obj.hpp"

std::string make_obj_tinyid( const char *name )
{
    // pro: small overhead
    // con: diff load/save versions are *not* compatible

    static size_t id = 0;
    std::stringstream ss;
    ss << ( ++id ); // @todo: if id becomes 'char' type someday, then avoid ':' (netstring), '</=">' (xml), etc
    return ss.str();

    // pro: diff load/save versions are compatible ; medium overhead
    // con: objfield name clashes are posible
    // return moon9::id( name );

    // pro: diff load/save versions are compatible ; no objfield name clashes
    // con: bigger overhead
    // return name;
}



