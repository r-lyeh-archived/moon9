#pragma once

#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace moon9
{
    class pakstring : public std::string
    {
        public:

        pakstring() : std::string()
        {}

        template< typename T >
        explicit
        pakstring( const T &t ) : std::string()
        {
            operator=(t);
        }

        template< typename T >
        pakstring &operator=( const T &t )
        {
            std::stringstream ss;
            ss.precision(20);
            if( ss << /* std::boolalpha << */ t )
                this->assign( ss.str() );
            return *this;
        }

        template< typename T >
        T as() const
        {
            T t;
            std::stringstream ss;
            ss << *this;
            return ss >> t ? t : T();
        }

        template< typename T0 >
        pakstring( const std::string &_fmt, const T0 &t0 ) : std::string()
        {
            std::string out, t[1] = { pakstring(t0) };

            for( const char *fmt = _fmt.c_str(); *fmt; ++fmt )
            {
                if( *fmt == '\1' )
                    out += t[0];
                else
                    out += *fmt;
            }

            this->assign( out );
        }

        template< typename T0, typename T1 >
        pakstring( const std::string &_fmt, const T0 &t0, const T1 &t1 ) : std::string()
        {
            std::string out, t[2] = { pakstring(t0), pakstring(t1) };

            for( const char *fmt = _fmt.c_str(); *fmt; ++fmt )
            {
                if( *fmt == '\1' )
                    out += t[0];
                else
                if( *fmt == '\2' )
                    out += t[1];
                else
                    out += *fmt;
            }

            this->assign( out );
        }
    };

    struct pakfile : public std::map< std::string, pakstring >
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

    struct paktype
    {
        enum enumeration { ZIP };
    };

    class pak : public std::vector< pakfile >
    {
        public:

        const paktype::enumeration type;

        explicit
        pak( const paktype::enumeration &etype = paktype::ZIP ) : type(etype)
        {}

        // binary serialization

        bool bin( const std::string &bin_import ); //const
        std::string bin(); //const

        // debug

        std::string debug( const std::string &format1 = "\1\n" ) const;
    };
}

namespace pack
{
    // note: <moon9/io/bistring.hpp> defines also a compatible 'bistring(s)' type
    typedef std::pair<std::string,std::string> bistring;
    typedef std::vector< bistring > bistrings;
    std::string zip( const bistrings &bs );
}

namespace unpack
{
    // note: <moon9/io/bistring.hpp> defines also a compatible 'bistring(s)' type
    typedef std::pair<std::string,std::string> bistring;
    typedef std::vector< bistring > bistrings;
    bistrings zip( const std::string &s );
}
