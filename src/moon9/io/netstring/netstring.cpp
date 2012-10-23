#include <deque>
#include <string>
#include <sstream>

#include "netstring.hpp"

namespace
{
    class custom : public std::string
    {
        public:

        custom() : std::string()
        {}

        template <typename T>
        custom( const T &t ) : std::string()
        {
            std::stringstream ss;
            if( ss << t )
                this->assign( ss.str() );
        }

        template<typename T>
        T as() const
        {
            T t;
            std::stringstream ss;
            ss << *this;
            if( ss >> t )
                return t;
            return T();
        }

        std::deque<custom> split( char separator ) const
        {
            std::deque<custom> cs;

            cs.push_back( custom() );

            for( auto it = this->begin(), end = this->end(); it != end; ++it )
            {
                if( *it == separator )
                {
                    cs.push_back( custom() );
                    ++it;
                }
                else
                {
                    cs.back() += *it;
                }
            }

            return cs;
        }
    };
}

std::string to_netstring( const std::string &line )
{
    return custom( line.size() ) + ":" + line + ";";
}

std::string from_netstring( const std::string &line, size_t *offset )
{
    if( !line.size() )
        return std::string();

    std::deque< custom > tokens = custom(line).split(':');

    if( tokens.size() < 2 )
        return std::string();

    size_t len = tokens[0].as<size_t>();

    if( offset )
        *offset = tokens[0].size() + 1 /*:*/ + len + 1 /*;*/;

    return line.substr( tokens[0].size() + 1 /*:*/, len );
}

std::string to_netstrings( const std::vector<std::string> &lines )
{
    std::string out;

    for( auto &it : lines )
        out += to_netstring( it );

    return out;
}

std::vector<std::string> from_netstrings( const std::string &_line )
{
    std::string line = _line;
    std::vector<std::string> out;
    size_t offset;

    while( line.size() > 0 )
    {
        out.push_back( from_netstring( line, &offset ) );
        line = line.substr( offset );
    }

    return out;
}

namespace pack
{
    std::string netstring( const bistrings &p )
    {
        std::string out;

        for( auto &it : p )
        {
            out += to_netstring( it.first );
            out += to_netstring( it.second );
        }

        return out;
    }
}

namespace unpack
{
    bistrings netstring( const std::string &s )
    {
        bistrings bs;
        std::string line = s;

        while( line.size() > 0 )
        {
            size_t offset;

            bistring b;

                b.first = from_netstring( line, &offset );
                line = line.substr( offset );

                b.second = from_netstring( line, &offset );
                line = line.substr( offset );

            bs.push_back( b );
        }

        return bs;
    }
}
