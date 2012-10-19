#include <string>
#include <deque>

#include <iostream>

#include "event.hpp"

void kill( const std::deque<std::string> &args )
{
    const std::string &subject = args[0];

    std::cout << args[1] << " says: gawd! " << subject << " killed me :(" << std::endl;
}

void score( const std::deque<std::string> &args )
{
    const std::string &subject = args[0];

    std::cout << "* " << subject << " scores a frag" << std::endl;
}

void fire( const std::deque<std::string> &args )
{
    const std::string &subject = args[0];

    for( size_t i = 1; i < args.size(); ++i )
    {
        if( args[i] == "gun" )
            std::cout << subject << " says: pewpew!" << std::endl;
        if( args[i] == "bomb" )
            std::cout << subject << " says: boooooomb!" << std::endl;
    }
}

void respawn( const std::deque<std::string> &args )
{
    const std::string &subject = args[0];
    std::cout << "* " << subject << " respawns at scene" << std::endl;
}

int main( int argc, char **argv )
{
    // safety test (any usage before setup should be stable)
    event::trigger( "user", "kills", "somebody" );
    event::trigger( "user", "tries", "something" );
    event::queue( "player0", "says", "ugh!" );
    event::queue( "player1", "says", "ugh!" );
    event::dispatch();

    // sample
    // setup callbacks (notice there are two listeners on 'kill' event)
    event::on( "respawns", respawn );
    event::on( "fires", fire );
    event::on( "kills", kill );
    event::on( "kills", score );

//    event::off( "kills", score );
//    event::off( "kills" );
//    event::off();

    // sample #1 immediate mode
    event::trigger( "player1", "respawns" );
    event::trigger( "player2", "respawns" );
    std::cout << "---" << std::endl;

    // sample #2 batch mode
    event::queue( "player1", "fires", "gun", "bomb" );
    event::queue( "player1", "kills", "player2" );
    event::queue( "player2", "respawns" );
    event::queue( "player1", "fires", "bomb", "gun" );
    // dispatch selected set only
    event::dispatch("fires");
    std::cout << "---" << std::endl;
    // dispatch all pending events
    event::dispatch();
    std::cout << "---" << std::endl;

    return 0;
}
