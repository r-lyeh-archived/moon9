#include <moon9/test/test.hpp>

#include "multinmap.hpp"

#ifdef test3

namespace
{
    bool done()
    {
        moon9::multinmap<int> mnm;

        // 1 key, type A
        test3( mnm["hello world"].findit(), ==, false );
        mnm["hello world"].get() = 4;
        test3( mnm["hello world"].findit(), ==, true );
        test3( mnm["hello world"].found(), ==, 4 );
        test3( mnm["hello world"].get(), ==, 4 );

        // 1 key, type B
        test3( mnm[123].findit(), ==, false );
        mnm[123].get() = 1000;
        test3( mnm[123].findit(), ==, true );
        test3( mnm[123].found(), ==, 1000 );
        test3( mnm[123].get(), ==, 1000 );

        // 2 keys, different types
        float second_key = 31209.f;
        test3( mnm["first key"][second_key].findit(), ==, false );
        mnm["first key"][second_key].get() = 314159;
        test3( mnm["first key"][second_key].findit(), ==, true );
        test3( mnm["first key"][second_key].found(), ==, 314159 );
        test3( mnm["first key"][second_key].get(), ==, 314159 );

        // 2 keys, different types, different key order
        test3( mnm[second_key]["first key"].findit(), ==, true );
        test3( mnm[second_key]["first key"].get(), ==, 314159 );

        // 2 keys, same types
        test3( mnm["string1"]["string2"].findit(), ==, false );
        mnm["string1"]["string2"].get() = 4;
        test3( mnm["string1"]["string2"].findit(), ==, true );
        test3( mnm["string1"]["string2"].found(), ==, 4 );
        test3( mnm["string1"]["string2"].get(), ==, 4 );

        // 2 keys, same types, different key order
        test3( mnm["string2"]["string1"].findit(), ==, true );
        test3( mnm["string2"]["string1"].get(), ==, 4 );

        // N keys, custom find
        mnm["hello world"][1][3.f][3.14159][true].get() = 13;
        test3( mnm["hello world"][1][3.f][3.14159][true].findit(), ==, true );
        test3( mnm["hello world"][1][3.f][3.14159][true].get(), ==, 13 );

        // N keys, std find
        bool found = mnm["hello world"][1][3.f][3.14159][true].find() != mnm.end();
        test3( found, ==, true );

        return true;
    }

    const bool tested = done();
}

#endif
