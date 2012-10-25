#include <moon9/test/test.hpp>

#include <moon9/string/arc4.hpp>
#include <moon9/string/arc4.cpp>

namespace
{
    void unittest()
    {
        test3( moon9::ARC4("hello world", "my key"), !=, "hello world" );
        test3( moon9::ARC4(moon9::ARC4("hello world", "my key"), "my key"), ==, "hello world" );
    }

    const bool tested = (unittest(), true);
}
