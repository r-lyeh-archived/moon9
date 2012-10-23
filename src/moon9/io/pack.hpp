#pragma once

#include "bistring.hpp"

namespace pack
{
    std::string zip( const bistrings &bs );
    std::string json( const bistrings &bs );
    std::string xml( const bistrings &bs );
    std::string netstring( const bistrings &bs );
}

namespace unpack
{
    bistrings zip( const std::string &s );
    bistrings json( const std::string &s );
    bistrings xml( const std::string &s );
    bistrings netstring( const std::string &s );
}
