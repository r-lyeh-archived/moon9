#include <cassert>
#include <iostream>
#include <vector>

#include "component.hpp"

class label : moon9::component<label> // make this class trackable
{
    public:

    size_t id;

    label()
    {
        static size_t sid = 0;
        id = sid++;
    }
};

int main( int argc, char **argv )
{
    label a,b,c,d,e,f;                               // stack test
    label *g = new label;                            // heap test
    std::vector< label > h; h.push_back( label() );  // in-place test

    std::cout << "addr: " << &a << std::endl;
    std::cout << "addr: " <<  g << std::endl;
    std::cout << "addr: " << &h[0] << std::endl;

    // this should display addresses for 8 pointers

    for( auto &it : moon9::all<label>() )
        std::cout << "label .id=" << it->id << " .addr=" << (&*it) << std::endl;

    assert( moon9::size<label>() == 8 );

    std::cout << "All ok." << std::endl;

    return 0;
}
