// simple udp interface based on enet (reliable udp transport layer)
// - rlyeh ~~ listening to Lamb / Gabriel

#pragma once

#include <string>

#ifdef OK
#undef OK // i hate people
#endif

#ifdef ERROR
#undef ERROR // i really do
#endif

namespace udpc
{
    bool connect( int &socket, const std::string &ip, const std::string &port, double timeout_s = 5 );
    bool connect( int &socket, const std::string &ip, int port, double timeout_s = 5 );

    bool send( int &socket, const std::string &data );
    bool recv( int &socket, std::string &data, double timeout_s = 0 );

    bool is_connected( int &socket );

    bool close( int &socket );

    // tools
    double get_bytes_sent();
    double get_bytes_received();
    void sleep( double seconds );
}

namespace udps
{
    enum udp_server_action { OK = 0, ERROR = 1 };
    enum udp_peer_action { JOIN = 2, DATA = 3, PART = 4 };

    bool listen( int &socket, const std::string &port, int peers = 100 );
    bool listen( int &socket, int port, int peers = 100 );
    bool stop( int &socket );

    struct event
    {
        int type;
        int peer;

        std::string name;
        std::string data;
    };

    bool poll( int &socket, event &e, double timeout_s );

    bool recv( int &socket, std::string &data );
    bool send( int &socket, const std::string &data, int peer );

    bool broadcast( int &socket, const std::string &data );
    bool broadcast_but( int &socket, const std::string &data, int peer );

    bool say( int &socket, const std::string &data, int peer );
    bool echo( int &socket, const std::string &data, int peer );    // repost() ?
    bool shout( int &socket, const std::string &data );             // publish() ?

    // tools
    double get_bytes_sent();
    double get_bytes_received();
    size_t get_peers_connected();
    void sleep( double seconds );
}
