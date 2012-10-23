#include <iostream>

#include "tcp.hpp"

void error( const std::string &message )
{
    std::cout << "error: " << message.c_str() << std::endl;
    exit( 1 );
}

int main( int argc, char **argv )
{
    bool ok;
    int socket;
    std::string answer;

    // HTTP sample

    ok = false;

    if( tcp::connect( socket, "www.google.com", "80" ) )
        if( tcp::send( socket, "GET /\r\n\r\n" ) )
            if( tcp::receive( socket, answer ) )
                ok = true;

    tcp::close( socket, 0.5 );

    if( !ok )
        error("failed to contact google website");

    std::cout << "ok, answer from google website: " << answer << std::endl;

    // NTP sample

    ok = false;

    if( tcp::connect( socket, "time-C.timefreq.bldrdoc.gov", "13", 4.0 ) )
        if( tcp::send( socket, "dummy", 1.0 ) )
            if( tcp::receive( socket, answer, 1.0 ) )
                ok = true;

    tcp::close( socket, 0.5 );

    if( !ok )
        error("failed to contact NTP server");

    std::cout << "ok, answer from NTP server: " << answer << std::endl;

    // retrieving ip/port (once a connection is established)

    std::string ip, port;

    ok = false;

    if( tcp::connect( socket, "time-C.timefreq.bldrdoc.gov", "13", 4.0 ) )
        if( tcp::get_interface_address( socket, ip, port ) )
            ok = true;

    tcp::close( socket, 0.5 );

    if( !ok )
        error("failed to contact NTP server");

    std::cout << "Local address = " << ip << ':' << port << std::endl;

    return 0;
}
