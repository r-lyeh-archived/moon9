#include <cassert>

#include <string>
#include <sstream>
#include <deque>

#include <ctime>
//#include <moon9/core/os.hpp>
//#include <moon9/core/string.hpp>

#include "dt.hpp"
#include "rtc.hpp"
#include "sleep.hpp"

namespace moon9
{
    namespace
    {
        class custom : public std::string
        {
            public:

            custom() : std::string()
            {}

            template<typename T>
            custom( const T &t ) : std::string()
            {
                std::stringstream ss;
                if( ss << t )
                    this->assign( ss.str() );
            }

            std::deque<custom> tokenize( const std::string &chars ) const
            {
                std::string map( 256, '\0' );

                for( auto it = chars.begin(), end = chars.end(); it != end; ++it )
                    map[ *it ] = '\1';

                std::deque<custom> tokens;

                tokens.push_back( custom() );

                for( int i = 0, end = this->size(); i < end; ++i )
                {
                    unsigned char c = at(i);

                    std::string &str = tokens.back();

                    if( !map.at(c) )
                        str.push_back( c );
                    else
                    if( str.size() )
                        tokens.push_back( custom() );
                }

                while( tokens.size() && !tokens.back().size() )
                    tokens.pop_back();

                return tokens;
            }

            template <typename T>
            T as() const
            {
                T t;
                std::stringstream ss;
                ss << *this;
                return ss >> t ? t : T();
            }
        };
    }

    // object time (in seconds.microseconds)
    time_t rtc::time_obj()
    {
        return creation + time_t( factor * dt.s() );
    }

    time_t rtc::elapsed()
    {
        return time_t( factor * dt.s() );
    }

    rtc::rtc() : factor( 1.0 )
    {
        set( double( std::time( 0 ) ) );
    }

    rtc::rtc( const std::string &import ) : factor( 1.0 )
    {
        this->str( import );
    }

    void rtc::reset() //useful?
    {
        set( 0 );
    }

    void rtc::set( const double &t )
    {
        held = false;
        creation = time_t( t );
        dt.reset();
    }

    void rtc::shift( double f ) // factor(), phase(), speed() instead?
    {
        assert( f > 0.0 );

        factor = f;
    }

    void rtc::pause()
    {
        held = true;
    }

    double rtc::resume()
    {
        held = false;

        return double( time_obj() - creation );
    }

    double rtc::update()
    {
        //return double( creation = ( held ? creation : time_obj() ) );

        if( held )
            return double( creation );

        set( double( creation ) + elapsed() );

        return double( creation );
    }

    double rtc::get() const
    {
        return double( creation );
    }

    rtc::operator double() const
    {
        return get();
    }

    std::string rtc::format( const std::string &fmt ) const
    {
        char pBuffer[80];

        struct tm * timeinfo;
        time_t stored = (time_t)( get() );
        timeinfo = localtime ( &stored );
        strftime(pBuffer, 80, fmt.c_str(), timeinfo);

        return pBuffer;
    }

    int rtc::Y() const
    {
        return custom( format("%Y") ).as<int>();
    }
    int rtc::M() const
    {
        return custom( format("%m") ).as<int>();
    }
    int rtc::D() const
    {
        return custom( format("%d") ).as<int>();
    }

    int rtc::h() const
    {
        return custom( format("%H") ).as<int>();
    }
    int rtc::m() const
    {
        return custom( format("%M") ).as<int>();
    }
    int rtc::s() const
    {
        return custom( format("%S") ).as<int>();
    }

    std::string rtc::str() const
    {
        return format( "%Y-%m-%d %H:%M:%S" );
    }

    void rtc::str( const std::string& import )
    {
        std::deque< custom > token = custom( import ).tokenize(":-/ "); //:)

        //assert( token.size() >= 6 );

        if( token.size() < 6 )
        {
            set( 0 );
            return;
        }

        struct tm timeinfo;

        //months are in [0..] range where days, hours, mins and secs use [1..] (doh!)
        timeinfo.tm_year  = token[0].as<int>() - 1900;
        timeinfo.tm_mon   = token[1].as<int>() - 1;
        timeinfo.tm_mday  = token[2].as<int>();
        timeinfo.tm_hour  = token[3].as<int>();
        timeinfo.tm_min   = token[4].as<int>();
        timeinfo.tm_sec   = token[5].as<int>();
        //-1 = do not adjust daylight savings
        timeinfo.tm_isdst = -1;

        factor = 1.0; //ahem
        set( double( mktime( &timeinfo ) ) );
    }

    void rtc::sleep( double seconds )
    {
        moon9::sleep( seconds );
    }
}
