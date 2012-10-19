#include <string>
#include <sstream>
#include <iostream>

#include "obj.hpp"

// our obj components

objfield<       float > position("position");
objfield< std::string > description("description");
objfield< std::string > id("id");

// our obj initialization

void obj::user_defined_clone( const obj *other )
{
    copy_field( description, other );
    copy_field( position, other );
    copy_field( id, other );
}

void obj::user_defined_erase()
{
    erase_field( description );
    erase_field( position );
    erase_field( id );
}

void obj::user_defined_print() const
{
    print_field( description );
    print_field( position );
    print_field( id );
}

void obj::user_defined_diff( const obj *other ) const {}
void obj::user_defined_patch() {}

// our sample

#include <vector>

int main( int argc, char **argv )
{
    {
        // construct two objects dinamically

        obj obj1, obj2;

        obj1[ id ] = "obj1";
        obj1[ position ] = 1.f;
        obj1[ description ] = "this is our first object";

        obj2 = obj1;
        obj2[ id ] = "obj2";
        obj2[ position ]++;
        obj2.del( description );

        // check properties

        std::cout << "obj1 " << ( obj1.has( description ) ? "has" : "misses" ) << " description field" << std::endl;
        std::cout << "obj2 " << ( obj2.has( description ) ? "has" : "misses" ) << " description field" << std::endl;

        // print objects

        std::cout << obj1.str() << std::endl;
        std::cout << obj2.str() << std::endl;
    }

    {
        // test: ensure copy-constructor works

        std::vector<obj> objs( 100 );
    }

    return 0;
}
