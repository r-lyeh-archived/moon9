#include <cassert>
#include <iostream>

#include "string.hpp"

int main( int argc, char **argv )
{
    // many constructors
    std::cout << moon9::string( "hello world" ) << std::endl;
    std::cout << moon9::string( 'h' ) << std::endl;
    std::cout << moon9::string( 'h', 3 ) << std::endl;
    std::cout << moon9::string( 3, 'h' ) << std::endl;
    std::cout << moon9::string( 0 ) << std::endl;
    std::cout << moon9::string( -1 ) << std::endl;
    std::cout << moon9::string( 3.14159f ) << std::endl;
    std::cout << moon9::string( 3.1415926535897932384626433832795 ) << std::endl;
    std::cout << moon9::string( true ) << std::endl;
    std::cout << moon9::string( false ) << std::endl;

    // chaining
    moon9::string test;
    test << "hello world: " << 3 << 'a' << -1 << std::endl;
    std::cout << test;

    // implicit type conversion
    bool f = moon9::string("false");
    std::cout << f << std::endl;

    // explicit type conversion
    bool t = moon9::string("true").as<bool>();
    std::cout << t << std::endl;

    // safe at() method
    assert( moon9::string("Hi!").at(-6) == 'H' );
    assert( moon9::string("Hi!").at(-5) == 'i' );
    assert( moon9::string("Hi!").at(-4) == '!' );
    assert( moon9::string("Hi!").at(-3) == 'H' );
    assert( moon9::string("Hi!").at(-2) == 'i' );
    assert( moon9::string("Hi!").at(-1) == '!' );
    assert( moon9::string("Hi!").at( 0) == 'H' );
    assert( moon9::string("Hi!").at( 1) == 'i' );
    assert( moon9::string("Hi!").at( 2) == '!' );
    assert( moon9::string("Hi!").at( 3) == 'H' );
    assert( moon9::string("Hi!").at( 4) == 'i' );
    assert( moon9::string("Hi!").at( 5) == '!' );

    assert( moon9::string().at(-1) == '\0' );
    assert( moon9::string().at( 0) == '\0' );
    assert( moon9::string().at( 1) == '\0' );

    // expression evaluation
    std::cout << moon9::string("1+1").eval() << std::endl;
    std::cout << moon9::string("3+(2*4)").eval() << std::endl;
    std::cout << moon9::string("45*2").eval() << std::endl;
    std::cout << moon9::string("3.14159").eval() << std::endl;

    std::cout << "All ok." << std::endl;

    return 0;
}

