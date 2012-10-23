#include <iostream>

#include "tcp.hpp"

void error( const std::string &message )
{
    std::cout << message.c_str() << std::endl;
    exit( 1 );
}

void echo( int master_fd, int child_fd, std::string client_addr_ip, std::string client_addr_port )
{
    std::cout << "server ok: " << master_fd << ',' << child_fd << std::endl;

    std::cout << "server ok: hit from " << client_addr_ip << ':' << client_addr_port << std::endl;

    std::string input;

    if( !tcp::receive( child_fd, input ))
        error("server error: cant recv");

    std::cout << "server ok: recv: " << input << std::endl;

    std::string output = input;

    if( !tcp::send( child_fd, output ) )
        error("server error: cant send");

    std::cout << "server ok: sent: '" << output << "' back." << std::endl;

    if( !tcp::close( child_fd ) )
        error("server error: cant close");

    std::cout << "server ok: closed" << std::endl;
}

int main( int argc, char **argv )
{
    // server

    int server_socket;

    if( !tcp::listen( server_socket, "4040", echo, 1024 ) )
        error("server error: cant listen at port 4040");

    // client

    int client_socket;
    std::string outgoing = "Hello world", incoming;

    std::cout << "client ok: connecting..." << std::endl;
    if( !tcp::connect( client_socket, "127.0.0.1", "4040" ) )
        error("client error: cant connect");
    std::cout << "client ok: connected" << std::endl;
    std::cout << "client ok: sending..." << std::endl;
    if( !tcp::send( client_socket, outgoing ) )
        error("client error: cant send");
    std::cout << "client ok: sent" << std::endl;
    std::cout << "client ok: receiving..." << std::endl;
    if( !tcp::receive( client_socket, incoming ) )
        error("client error: cant receive");
    std::cout << "client ok: received" << std::endl;
    std::cout << "client ok: closing..." << std::endl;
    if( !tcp::close( client_socket ) )
        error("client error: cant close");
    std::cout << "client ok: closed" << std::endl;

    std::cout << "client ok: " << incoming << std::endl;

    return 0;
}
