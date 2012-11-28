#pragma once

#include "dt.hpp"

namespace moon9
{
    class looper
    {
        moon9::dt dt;

        public:

        looper()
        {}

        void reset()
        {
            dt.reset();
        }

        double s()
        {
            double now = dt.s();

            if( now < 1.0 )
                return now;

            dt = moon9::dt();
            return 1;
        }
    };
}
