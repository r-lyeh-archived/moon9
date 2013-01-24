#pragma once

#include <cassert>

#include "dt.hpp"

namespace moon9
{
    class chrono
    {
        moon9::dt dt;
        double top;

        public:

        explicit
        chrono( double _top = 1 ) : top(_top)
        {
            assert( _top > 0 );
        }

        double s()
        {
            double now = dt.s() / top;

            return now >= 1.0 ? 1.0 : now;
        }

        void reset()
        {
            dt.reset();
        }

        void reset( double _top )
        {
            assert( _top > 0 );
            top = _top;
            dt.reset();
        }
    };
}
