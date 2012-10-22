#ifdef test3

#include <iostream>

#include "confirm.hpp"

namespace
{
    void test()
    {
        test3( confirm( "Is this window visible?", "Test" ), ==, true );
        test3( confirm( "Has this window title?" ), ==, false );
    }

    const bool done = ( test(), true );
}

#endif
