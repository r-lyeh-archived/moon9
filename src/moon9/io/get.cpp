#include <string>
#include <vector>

#include "constants.hpp"
#include "string.hpp"

namespace get
{
    std::string file = "constants.ini";

    namespace
    {
        moon9::constants &map()
        {
            static moon9::constants _map(file);
            return _map;
        }
    }

    std::map<std::string,moon9::iostring>::const_iterator begin() { return map().begin(); }
    std::map<std::string,moon9::iostring>::const_iterator end()   { return map().end();   }

    std::string s( const std::string &var ) { return map()[var]; }
         size_t z( const std::string &var ) { return map()[var].as<size_t>(); }
         double d( const std::string &var ) { return map()[var].as<double>(); }
          float f( const std::string &var ) { return map()[var].as<float>(); }
           bool b( const std::string &var ) { return map()[var].as<bool>(); }
            int i( const std::string &var ) { return map()[var].as<int>(); }

    std::string translated( const std::string &text )
    {
        std::vector<std::string> delimiters(2);
        delimiters[0] = "$(";
        delimiters[1] = ")";
        moon9::iostrings parsed = moon9::iostring( text ).parse( delimiters );

        std::string out;

        for( size_t i = 0; i < parsed.size(); ++i )
        {
            if( parsed[i] == delimiters[0] )
            {
                std::string token = parsed[++i];
                ++i;
                /*
                if( vars[token].size() )
                    out += vars[token];
                else */
                if( get::s(token).size() )
                    out += get::s(token);
                else
                    out += delimiters[0] + token + delimiters[1];
            }
            else
            if( parsed[i] != delimiters[1] )
                out += parsed[i];
        }

        return out;
    }
}
