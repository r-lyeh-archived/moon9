#include "test.hpp"

int main( int argc, char **argv )
{
    int a = 1, b = 2;

    // test1() expects given expression to be true. behaves similar to assert()
    test1( a == 1 );    // pass
    test1( a - a );     // fail ( a - a = 0 -> false )

    // test3() expects given expression to be true.
    // expression is split by commas to do some preprocessor magic.
    test3( 1, ==, 2 );  // fail ( 1 != 2 )
    test3( 1, !=, 2 );  // pass
    test3( a, <, b );   // pass
    test3( a, >, b );   // fail ( b > a )

    return 0;
}
