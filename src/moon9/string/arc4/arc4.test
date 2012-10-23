#ifdef test3

#include "arc4.hpp"

namespace crypt_tests
{
    void test()
    {
        test3( moon9::ARC4("hello world", "my key"), !=, "hello world" );
        test3( moon9::ARC4(moon9::ARC4("hello world", "my key"), "my key"), ==, "hello world" );
    }

    const bool tested = (test(), true);
}

#endif
