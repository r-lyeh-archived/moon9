#include <moon9/test/test.hpp>

#include "pod.hpp"

#ifdef test3

namespace
{
    bool done()
    {
        moon9::pod<bool,0> zero;
        moon9::pod<bool,1> one;

        test3( zero, !=, one );
        test3( zero, ==, false );
        test3( one, ==, true );

        zero = false;
        test3( zero, ==, false );
        one = true;
        test3( one, ==, true );

        zero = one;
        test3( zero, ==, one );
        test3( zero, ==, true );

        return true;
    }

    const bool tested = done();
}

#endif