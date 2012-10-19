#include <map>
#include <string>
#include <sstream>
#include <iostream>

#include "obj.hpp"

// our obj components

objfield< float > busy("busy");
objfield<  bool > on("on");

// our obj initialization

void obj::user_defined_clone( const obj *other )
{
    copy_field( busy, other );
    copy_field( on, other );
}

void obj::user_defined_erase()
{
    erase_field( busy );
    erase_field( on );
}

void obj::user_defined_print() const
{
    print_field( busy );
    print_field( on );
}

void obj::user_defined_diff( const obj *other ) const {}
void obj::user_defined_patch() {}

// our sample

int main( int argc, char **argv )
{
    {
        // create two objects, different types

        obj toggle, elevator;

        toggle[ on ] = true;
        elevator[ busy ] = true;

        // std::cout << ( toggle.type() == elevator.type() ) << std::endl;

        // object mutation

        obj any;

        any = toggle;
        std::cout << any.str() << std::endl;

        any = elevator;
        std::cout << any.str() << std::endl;

        // combine (composition)
        // any = toggle + elevator;
    }

    return 0;
}
