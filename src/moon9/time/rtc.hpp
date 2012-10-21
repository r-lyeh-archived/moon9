#pragma once

#include <cassert>

#include <string>
#include <sstream>
#include <deque>

#include <ctime>
//#include <moon9/core/os.hpp>
//#include <moon9/core/string.hpp>

#include "dt.hpp"

namespace moon9
{
    class rtc
    {
        moon9::dt dt;

        bool held;
        std::time_t creation;
        double factor;

        // object time (in seconds.microseconds)
        time_t time_obj();
        time_t elapsed();

        public:

        rtc();
        rtc( const std::string &import );

        void reset(); //useful?
        void set( const double &t );
        void shift( double f ); // factor(), phase(), speed() instead?
        void pause();
        double resume();
        double update();

        double get() const;
        operator double() const;

        std::string format( const std::string &fmt ) const;

        int Y() const;
        int M() const;
        int D() const;

        int h() const;
        int m() const;
        int s() const;

        std::string str() const;
        void str( const std::string& import );

        void sleep( double seconds );
    };
}
