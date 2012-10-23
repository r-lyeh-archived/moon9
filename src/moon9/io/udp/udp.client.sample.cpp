#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <string>

#include "udp.hpp"

namespace
{
	void die( const char *error )
	{
		fprintf( stderr, "%s\n", error );
		exit( 1 );
	}
}

int main(int argc, char ** argv)
{
	namespace udp = udpc;

	int socket;

	std::string clientName;
	std::cout << "Your name: ";
	std::getline( std::cin, clientName );

	if( !udp::connect( socket, "localhost", 1234 ) )
		die("Could not connect to server");

	udp::send( socket, clientName );

	while( udp::is_connected( socket ) )
	{
		std::string data;
		while( udp::recv( socket, data ) )
			std::cout << data << std::endl;

		std::string buffer;
		std::cout << "You: ";
		std::getline( std::cin, buffer );

		if( buffer == "q" || buffer == "quit" )
			break;

		udp::send( socket, buffer );
	}

	udp::close( socket );

	return 0;
}
