#include <iostream>

#include "rtc.hpp"
#include "sleep.hpp"

int main( int argc, char **argv )
{
    moon9::rtc
        past( "1972-06-10 17:00:00" ),
        present,
        future( "2091-12-31 23:59:50" );

    // we want future to lapse at double speed
    future.shift( 2.0 );

    // eleven seconds
    for( int i = 0; i <= 10; ++i )
    {
           past.update();
        present.update();
         future.update();

        std::cout
            << "past: " << past.str() << " | "
            << "present: " << present.str() << " | "
            << "future: " << future.str()
            << std::endl;

        // sleep for one second
        moon9::sleep( 1.0 );
    }

    return 0;
}

