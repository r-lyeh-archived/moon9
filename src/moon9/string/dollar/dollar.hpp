// Simple string interpolation and string casting macros. MIT licensed.
// - rlyeh

#pragma once

#include <string>
#include <vector>

#include "string/string.hpp"

namespace moon9
{
    // public api, define/update

#   define  $(a)       ( moon9::locate( "$" #a ) )

    // public api, translate

#   define $$(a)       ( moon9::translate( a )   )

    // public api, cast and sugars

#   define $cast(a,b)  ( $(a).as<b>()       )

#   define $string(a)  ( $(a)               ) // no need to cast
#   define $bool(a)    ( $cast( a, bool   ) )
#   define $char(a)    ( $cast( a, char   ) )
#   define $int(a)     ( $cast( a, int    ) )
#   define $float(a)   ( $cast( a, float  ) )
#   define $double(a)  ( $cast( a, double ) )
#   define $size_t(a)  ( $cast( a, size_t ) )

    // private details

    std::vector< std::string > extract( const moon9::string &dollartext, char sep0 = '$', char sep1 = '\0' );
    moon9::string translate( const moon9::string &dollartext, const moon9::string &recursive_parent = std::string() );
    moon9::string &locate( const moon9::string &text );
}
