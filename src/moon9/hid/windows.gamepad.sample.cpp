// to do: rumble_low, rumble_high
// to do: vec2 {
// sugar newest().x -> current_x()
// sugar newest().y -> mejor sobrecargar -> ? -> lpad->x
// sensibilidad, deadzone
// aplicar filtros lfo, highpass, lowpass
// }

#include <iostream>

#include "windows.hpp"

int main( int argc, char **argv )
{
    moon9::windows::gamepad pad(0);

    while( !pad.back.trigger() && !pad.start.trigger() )
    {
        pad.update();

        pad.rumble[0].set( pad.ltrigger.newest().x );
        pad.rumble[1].set( pad.rtrigger.newest().x );

        std::cout
            << "pad[0]="
            << "abxy["
                << (pad.a.hold())
                << (pad.b.hold())
                << (pad.x.hold())
                << (pad.y.hold())
                << ']'
            << "lbrb["
                << (pad.lb.hold())
                << (pad.rb.hold())
                << ']'
            << "lrt["
                << (pad.ltrigger.hold())
                << (pad.rtrigger.hold())
                << ']'
            << "lrthumb["
                << (pad.lthumb.hold())
                << (pad.rthumb.hold())
                << ']'
            << "lpad("
                << pad.lpad.newest().x
                << ','
                << pad.lpad.newest().y
                << ')'
            << "rpad("
                << pad.rpad.newest().x
                << ','
                << pad.rpad.newest().y
                << ')'
            << pad.ltrigger.newest().x
            << "               \r";
    }
}
