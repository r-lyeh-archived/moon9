#include <iostream>

#include "windows.hpp"

int main( int argc, char **argv )
{
    moon9::windows::keyboard keyboard(0);

    do
    {
        keyboard.update();

        // simulate keyboard events (~push())
        keyboard.zero.set( rand() > (RAND_MAX / 2) ? 1 : 0 );

        // simulate virtual key sample trigger
        keyboard.debug_key.set( rand() > (RAND_MAX / 2) ? 1 : 0 );

        std::cout
            <<   "0[" << ( keyboard.zero.release() ? "x]" : " ]" )
            << "DBG[" << ( keyboard.debug_key.hold() ? "x]" : " ]" )
            <<   "T[" << ( keyboard.t.trigger() ? "x]" : " ]" )
            <<   "H[" << ( keyboard.h.hold() ? "x]" : " ]" )
            <<   "R[" << ( keyboard.r.release() ? "x]" : " ]" )
            <<   "I[" << ( keyboard.i.idle() ? "x]" : " ]" )
            << "SPC[" << ( keyboard.space.hold() ? "x]" : " ]" )
            <<   "w[.click=" << ( keyboard.w.click()  ? "x1" : (keyboard.w.dclick() ? "x2" : "x0" ) )
            <<    " .dur=" << keyboard.w.duration()
            <<    " .len=" << keyboard.w.size() << "]\r";
    }
    while( !keyboard.escape.trigger() );

    return 0;
}

