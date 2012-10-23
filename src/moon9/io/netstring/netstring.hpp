#pragma once

#include <vector>
#include <string>

std::string to_netstring( const std::string &lines );
std::string from_netstring( const std::string &line, size_t *offset = 0 );

std::string              to_netstrings( const std::vector<std::string> &lines );
std::vector<std::string> from_netstrings( const std::string &line );

namespace pack
{
    // note: <moon9/io/bistring.hpp> defines also a compatible 'bistring(s)' type
    typedef std::pair<std::string,std::string> bistring;
    typedef std::vector< bistring > bistrings;
    std::string netstring( const bistrings &bs );
}

namespace unpack
{
    // note: <moon9/io/bistring.hpp> defines also a compatible 'bistring(s)' type
    typedef std::pair<std::string,std::string> bistring;
    typedef std::vector< bistring > bistrings;
    bistrings netstring( const std::string &s );
}

