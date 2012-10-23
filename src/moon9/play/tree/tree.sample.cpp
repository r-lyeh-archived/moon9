#include <cassert>
#define test1( A )        assert( A )
#define test3( A, op, B ) assert( A op B )

#include <string>
#include <iostream>

#include "tree.hpp"

namespace
{
    bool unittest()
    {
        {
            moon9::tree<std::string> the_scene("/");

            the_scene.
                push( "box" ).
                    node( "1a" ).
                    node( "2a" ).
                    node( "2b" ).
                    node( "player1" ).
                    back().
                node( "player2" );

            test3( the_scene.str(), ==, "/(box(1a, 2a, 2b, player1), player2)" );

            the_scene.move("player1", "/");
            test3( the_scene.str(), ==, "/(box(1a, 2a, 2b), player2, player1)" );

            the_scene.move("player1", "/");
            test3( the_scene.str(), ==, "/(box(1a, 2a, 2b), player2, player1)" );
        }

        {
            moon9::tree<int> scene(0);

            scene.
                push( 1 ).
                    node( 2 ).
                        node( 3 ).
                        back().
                    back().
                node( 4 );

            // parse all nodes adding them to global sumatory

            int sum = 0;
            scene.apply( [&sum](int &x){ sum += x; } );

            test3( sum, ==, 10 );
        }

        {
            moon9::tree<std::string> scene("/");

            scene.
                push("fruits").
                    node("lemon").
                    node("orange").
                back().
                push("numbers").
                    node("0").
                    node("1").
                    node("2").
                back();

            test3(  scene.str(), ==, "/(fruits(lemon, orange), numbers(0, 1, 2))" );

            test1(  scene.find("0") );
            test1(  scene.find("1") );
            test1(  scene.find("2") );
            test1( !scene.find("3") );
        }

        return true;
    }

    const bool initialized = unittest();
}

int main( int argc, char **argv )
{
    std::cout << "All ok." << std::endl;

    return 0;
}
