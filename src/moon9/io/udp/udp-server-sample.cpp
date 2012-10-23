#include <stdlib.h>
#include <stdio.h>

#include "udp.hpp"

namespace
{
	void die( const char *error )
	{
		fprintf( stderr, "%s\n", error );
		exit( 1 );
	}

	void say( const char *msg )
	{
		fprintf( stdout, "%s\n", msg );
	}
}

int main(int argc, char ** argv)
{
	int socket, peers = 100;

	namespace udp = udps;

	if( !udp::listen( socket, 1234, peers ) )
		die( "could not start server" );
	else
		say( "im ready" );

	udp::event event;

	while( udp::poll( socket, event, 1.0 ) )
	{
		switch( event.type )
		{
			case udp::OK:
				break;
			case udp::JOIN:
				udp::send( socket, "Welcome to dungeon!", event.peer );
				udp::broadcast_but( socket, event.name + " has connected", event.peer );
				break;
			case udp::DATA:
				udp::broadcast_but( socket, event.name + ": " + event.data, event.peer );
				break;
			case udp::PART:
				udp::broadcast_but( socket, event.name + " has disconnected", event.peer );
				break;
		}
	}

	return 0;
}
