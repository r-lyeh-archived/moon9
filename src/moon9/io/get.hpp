#pragma once

#include <map>
#include <string>

#include "string.hpp"

namespace get
{
    extern std::string file;

    std::map<std::string,moon9::iostring>::const_iterator begin();
    std::map<std::string,moon9::iostring>::const_iterator end();

    std::string s( const std::string &var );
         size_t z( const std::string &var );
         double d( const std::string &var );
          float f( const std::string &var );
           bool b( const std::string &var );
            int i( const std::string &var );
}

namespace set
{
    void s( const std::string &var, const std::string &val );
    void z( const std::string &var, size_t val );
    void d( const std::string &var, double val );
    void f( const std::string &var,  float val );
    void b( const std::string &var,   bool val );
    void i( const std::string &var,    int val );
}
