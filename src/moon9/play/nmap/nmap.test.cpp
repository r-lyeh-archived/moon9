#include <moon9/test/test.hpp>

#include "nmap.hpp"

#ifdef test3

namespace
{
    bool done()
    {
        moon9::nmap< int > km;

        km["hello world"] = 4;
        km[123] = 1000;

        // search
        bool found;

        found = km.find("hello world") != km.end();
        test3( found, ==, true );

        found = km.find(124) != km.end();
        test3( found, ==, false );

        return true;
    }

    const bool tested = done();
}

#endif
