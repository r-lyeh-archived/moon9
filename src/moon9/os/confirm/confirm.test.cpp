#include <moon9/test/test.hpp>

#include <moon9/os/confirm.hpp>
#include <moon9/os/confirm.cpp>

namespace
{
    void unittest()
    {
        test3( confirm( "Is this window visible?", "Test" ), ==, true );
        test3( confirm( "Has this window title?" ), ==, false );
    }

    const bool tested = ( unittest(), true );
}
