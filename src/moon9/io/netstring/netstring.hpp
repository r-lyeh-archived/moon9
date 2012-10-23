#pragma once

#include <vector>
#include <string>

std::string to_netstring( const std::string &lines );
std::string from_netstring( const std::string &line, size_t *offset = 0 );

std::string              to_netstrings( const std::vector<std::string> &lines );
std::vector<std::string> from_netstrings( const std::string &line );

namespace pack
{
    // <moon9/io/bistring.hpp> defines a handy 'bistrings' type to next argument type
    std::string netstring( const std::vector< std::pair<std::string,std::string> > &bs );
}

namespace unpack
{
    // <moon9/io/bistring.hpp> defines a handy 'bistrings' type to next return type
    std::vector< std::pair<std::string,std::string> > netstring( const std::string &s );
}

