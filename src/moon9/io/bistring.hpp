#pragma once

#include <vector>
#include <string>

typedef std::pair< std::string /*key*/, std::string /*value*/ > bistring;
typedef std::vector< bistring > bistrings;

#include <iostream>

namespace
{
    std::ostream &operator <<( std::ostream &os, const bistring &b )
    {
        os << b.first << '=' << b.second;
        return os;
    }
    std::ostream &operator <<( std::ostream &os, const bistrings &bs )
    {
        for( auto &it : bs )
            os << it << ',';
        return os;
    }
}
