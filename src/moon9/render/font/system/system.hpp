// Get location of system fonts. MIT licensed
// - rlyeh

#pragma once

#include <string>
#include <map>

namespace moon9
{
    namespace font
    {
        namespace system
        {
            std::map< std::string, std::string > list();
            std::string locate( const std::string &ttfname );
        }
    }
}

