#include <iostream>
#include <cstdlib>

#include "tcp.hpp"

void error( const std::string &message )
{
    std::cerr << message.c_str() << std::endl;
    std::exit( 1 );
}

void echo( int master_fd, int child_fd, std::string client_addr_ip, std::string client_addr_port )
{
    std::string input;

    if( !tcp::receive( child_fd, input ) )
        error("server error: cant recv");

    if( !tcp::send( child_fd, input ) )
        error("server error: cant send");

    if( !tcp::close( child_fd ) )
        error("server error: cant close");

    std::cout << "server says: hit from " << client_addr_ip << ':' << client_addr_port << std::endl;
}

int main( int argc, char **argv )
{
    // server

    int server_socket;

    if( !tcp::listen( server_socket, "4040", echo, 1024 ) )
        error("server error: cant listen at port 4040");

    // client

    int client_socket;
    std::string answer;

    if( !tcp::connect( client_socket, "127.0.0.1", "4040" ) )
        error("client error: cant connect");

    if( !tcp::send( client_socket, "Hello world" ) )
        error("client error: cant send");

    if( !tcp::receive( client_socket, answer ) )
        error("client error: cant receive");

    if( !tcp::close( client_socket ) )
        error("client error: cant close");

    std::cout << "client says: answer='" << answer << "'" << std::endl;

    return 0;
}
