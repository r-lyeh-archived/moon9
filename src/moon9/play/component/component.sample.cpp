#include <cassert>
#include <iostream>
#include <vector>

#include "component.hpp"

// define components

struct scale : moon9::component<scale>
{
    float size;

    scale() : size(0)
    {}
};

struct position : moon9::component<position>
{
    float px,py,pz;

    position() : px(0), py(0), pz(0)
    {}
};

struct velocity : moon9::component<velocity>
{
    float vx,vy,vz;

    velocity() : vx(0), vy(0), vz(0)
    {}
};

struct renderable : moon9::component<renderable>
{};

// compose components

struct character : scale, position, velocity, renderable
{};

//

int main( int argc, char **argv )
{
    character player1, player2;

    assert( moon9::size<position>() == 2 ); // number of entities with 'position' component
    assert( moon9::size<scale>()    == 2 ); // number of entities with 'scale' component

    player1.size = 1.f;

    player2.vx = 100.f;
    player2.vy = 100.f;
    player2.size = 2.f;

    // process properties grouped by components

    for( auto &it : moon9::all<position>() )
    {
        std::cout << (it) << std::endl;
        std::cout << &(*it) << std::endl;
        std::cout << "position { " << it->px << ',' << it->py << ',' << it->pz << " }" << std::endl;
    }

    for( auto &it : moon9::all<velocity>() )
    {
        std::cout << (it) << std::endl;
        std::cout << &(*it) << std::endl;
        std::cout << "velocity { " << it->vx << ',' << it->vy << ',' << it->vz << " }" << std::endl;
    }

    for( auto &it : moon9::all<scale>() )
    {
        std::cout << "size { " << it->size << " }" << std::endl;
    }

    std::cout << "All ok." << std::endl;

    return 0;
}
