//ref : http://www.codeproject.com/KB/cpp/xzipunzip.aspx

//@todo: improve api
//@todo: remove xzip/xunzip win32 dependancies
//@todo: mem-only+stl based

#pragma once

#include <vector>
#include <map>
#include <string>

#include "string.hpp"
#include "file.hpp"

namespace moon9
{
    struct pakfile : public std::map< std::string, moon9::string >
    {
        bool has_field( const std::string &property ) const;

        bool has_operator( const std::string &plugin, const std::string &opcode ) const;

        template <typename T>
        T get_value( const std::string &property ) const
        {
            return (*this->find( property )).second.as<T>();
        }

        std::string debug( const std::string &format12 = "\t.\1=\2\n" ) const;
    };

    class pak : public std::vector< pakfile >
    {
        public:

        // binary serialization

        bool bin( const std::string &bin_import ); //const
        std::string bin(); //const

        // debug

        std::string debug( const std::string &format1 = "\1\n" ) const;
    };
}
