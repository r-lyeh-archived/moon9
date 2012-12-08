#pragma once

#include "dt.hpp"

namespace moon9
{
    class looper
    {
        moon9::dt dt;
        float factor;

        public:

        explicit
        looper( const float seconds = 1.f ) : factor(1.f/seconds)
        {}

        void reset( float seconds = 1.f )
        {
            factor = 1.f/seconds;
            dt.reset();
        }

        double s()
        {
            double now = dt.s() * factor;

            if( now < 1.0 )
                return now;

            dt = moon9::dt();
            return 1;
        }
    };
}
