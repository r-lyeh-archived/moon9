#pragma once

#include "timer.hpp"

namespace moon9
{
    class clock
    {
        moon9::timer timer;

        double mt;
        float mdt;

        public:

        clock() : mt(0), mdt(0)
        {}

        void speed( float shift )
        {
            timer.shift( shift );
        }

        void update()
        {
            timer.update();

            mdt = float( timer.get() - mt );
            mt += mdt;
        }

        double t() const
        {
            return mt;
        }

        float dt() const
        {
            return mdt;
        }
    };
}

#include <map>

namespace legacy
{
    // game::clocks["physics"].speed( 0.10 );  //matrix mode on
    // game::clocks["physics"].speed( 1.00 );  //matrix mode off
    // game::clocks["physics"].speed( -1.00 ); //rewind time... braid? check frogstep

    //hold(), resume(), accel()... useful?

    extern std::map< const char * /*id*/, moon9::clock > clocks;
}
