#include <cassert>

#include <map>
#include <string>
#include <sstream>
#include <iostream>

#include <msgpack.hpp> // include <msgpack.hpp> before <moon9/play/obj.hpp> in order to enable serialization
#include "obj.hpp"

// our obj components

objfield<       float > position("position", "This is our object position");
objfield< std::string > description("description", "This is unused at the moment");
objfield< std::string > id("id", "Object weak id");

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

void obj::user_defined_patch()
{
    patch_field( description );
    patch_field( position );
    patch_field( id );
}

void obj::user_defined_diff( const obj *other ) const
{
    diff_field( description, other );
    diff_field( position, other );
    diff_field( id, other );
}

// our sample

#include <vector>

int main( int argc, char **argv )
{
    std::string savegame;

    {
        // construct two objects dinamically

        obj obj1;

        obj1[ position ] = 1.f;
        obj1[ description ] = "this is our first object";

        savegame = obj1.save();
    }

    {
        obj obj1;

        obj1.load( savegame );

        std::cout << obj1.str() << std::endl;

        assert( obj1.has(position) );
        assert( obj1.has(description) );
        assert( obj1[position] == 1.f );
        assert( obj1[description] == "this is our first object" );

        std::cout << obj1.str() << std::endl;
    }

    {
        // construct two objects dinamically

        obj obj1;
        obj1.load( savegame );

        obj obj2 = obj1;
        obj2[ position ]++;
        obj2.del( description );

        std::string diff = obj1.diff( obj2 );
        obj1.patch( diff );

        assert( obj1.has(position) );
        assert( obj1[position] == 2.f );
        assert( !obj1.has(description) );

        std::cout << obj1.str() << std::endl;

        std::cout << "fullstate: " << savegame.size() << " bytes" << std::endl;
        std::cout << "diffstate: " << diff.size() << " bytes" << std::endl;
    }

    {
        obj o;

        std::cout << o.diff() << std::endl;

        o[ id ] = "hey";
        o[ position ] = 3.14159f;

        std::cout << o.diff() << std::endl;

        o[ position ] = 4.f;

        std::cout << o.diff() << std::endl;

        o.del( position );

        assert( o.diff().size() != 0 ); // diff() size should be larger than 0, since reflects position field removal at this point
        assert( o.diff().size() == 0 ); // however, this diff size should be zero. as we didnt modify object since previous diff()
    }

    {
        // ensure a few sanity checks

        obj obj1;
        obj1.load( savegame );

        assert( obj().save().size() == 0 );
        assert( obj().diff(obj()).size() == 0 );
        assert( obj1.diff(obj1).size() == 0 );

        // these should be safe

        obj().load(std::string());
        obj().load(obj().save());
        obj().patch(obj().save());
        obj().patch(obj().diff(obj()));
    }


    return 0;
}
