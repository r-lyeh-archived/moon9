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

    std::string s( const std::string &var ) { return map().get( var ); }
         size_t z( const std::string &var ) { return map().get( var ).as<size_t>(); }
         double d( const std::string &var ) { return map().get( var ).as<double>(); }
          float f( const std::string &var ) { return map().get( var ).as< float>(); }
           bool b( const std::string &var ) { return map().get( var ).as<  bool>(); }
            int i( const std::string &var ) { return map().get( var ).as<   int>(); }
}

namespace set
{
    void s( const std::string &var, const std::string &val ) { get::map().set( var, val ); }
    void z( const std::string &var,             size_t val ) { get::map().set( var, val ); }
    void d( const std::string &var,             double val ) { get::map().set( var, val ); }
    void f( const std::string &var,              float val ) { get::map().set( var, val ); }
    void b( const std::string &var,               bool val ) { get::map().set( var, val ); }
    void i( const std::string &var,                int val ) { get::map().set( var, val ); }
}

