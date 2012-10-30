#include <iostream>
#include <cstdlib>

#include "tcp.hpp"

void error( const std::string &message )
{
    std::cerr << "error: " << message.c_str() << std::endl;
    std::exit( 1 );
}

void http_sample()
{
    std::string answer;
    int socket;

    bool ok = false;

    if( tcp::connect( socket, "www.google.com", "80" ) )
        if( tcp::send( socket, "GET /\r\n\r\n" ) )
            if( tcp::receive( socket, answer ) )
                ok = true;

    tcp::close( socket );

    if( !ok )
        error("failed to contact google website");

    std::cout << "ok, answer from google website: " << answer << std::endl;
}

void ntp_sample()
{
    std::string answer;
    int socket;

    bool ok = false;

    if( tcp::connect( socket, "time-C.timefreq.bldrdoc.gov", "13" ) )
        if( tcp::send( socket, "dummy" ) )
            if( tcp::receive( socket, answer ) )
                ok = true;

    tcp::close( socket );

    if( !ok )
        error("failed to contact NTP server");

    std::cout << "ok, answer from NTP server: " << answer << std::endl;
}

int main( int argc, char **argv )
{
    http_sample();
    ntp_sample();
}
