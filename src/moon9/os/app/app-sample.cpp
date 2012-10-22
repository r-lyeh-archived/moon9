#include <iostream>
#include <string>

#include "app.hpp"

int main( int argc, char **argv )
{
    std::cout << app::get_name() << ": ";

    if( !app::is_another_instance_running() )
    {
        app::respawn();

        std::cout << "im a parent and spawned a child. ";
    }
    else
    {
        std::cout << "im a child. ";
    }

    std::cout << "type enter to exit..." << std::endl;

    std::string s;
    std::getline( std::cin, s );

    app::close();

    return 0;
}
