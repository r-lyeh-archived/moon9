#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include <chrono>
#include <thread>

namespace
{
	void die( const char *error )
	{
		fprintf( stderr, "%s\n", error );
		fflush( stderr );
		exit( 1 );
	}

	void sleep4( double seconds )
	{
	   	std::chrono::microseconds duration( (int)(seconds * 1000000) );
        std::this_thread::sleep_for( duration );
	}
}

#include <enet/enet.h>

#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "ws2_32.lib" )

namespace udpc
{
    namespace stats
    {
        double incoming = 0, outgoing = 0;
    }

	struct client_t
	{
		ENetHost  *client;
		ENetAddress address;
		ENetEvent  event;
		ENetPeer  *peer;
		ENetPacket  *packet;

		bool connected;

		std::string data;
	};

	static std::map< int, client_t > clients;
	static int id = 0;

	bool init( int &socket )
	{
		static struct once
		{
			once()
			{
				if( enet_initialize() != 0 )
					die( "Could not initialize enet." );
			}

			~once()
			{
				enet_deinitialize();
			}
		} _;

		socket = ++id;

		client_t &cl = ( clients[socket] = clients[socket] );

		cl.client = enet_host_create(NULL, 1, 2, 0, 0); // addr, peercount, incoming bandwidth, outgoing bandwidth
		cl.connected = false;

		if( !cl.client )
			return "Could not create client.", false;

		return true;
	}

	bool connect( int &socket, const std::string &ip, const std::string &port, double timeout_s )
	{
		if( !init( socket ) )
			return "cannot init socket", false;

		client_t &cl = clients[socket];

		enet_address_set_host( &cl.address, ip.c_str() );

		std::stringstream ss; ss << port; if( !( ss >> cl.address.port) ) return "Cannot parse port", false;

		cl.peer = enet_host_connect( cl.client, &cl.address, 2, 0 );

		if( !cl.peer )
			return "Could not connect to server", false;

		if( enet_host_service( cl.client, &cl.event, (enet_uint32)( timeout_s * 1000 ) ) > 0 )
			if( cl.event.type == ENET_EVENT_TYPE_CONNECT )
				return cl.connected = true;

		return "could not connect to host", false;
	}

	bool connect( int &socket, const std::string &ip, int port, double timeout_s )
	{
		std::string _port;
		std::stringstream ss;

		ss << port;
		if( !( ss >> _port) )
			return "Cannot parse port", false;

		return connect( socket, ip, _port, timeout_s );
	}

	bool send( int &socket, const std::string &data )
	{
		if( socket <= 0 )
			return false;

		client_t &cl = clients[socket];

		if( !cl.connected )
			return false;

		if( !data.size() )
			return true; // :)

		stats::outgoing += cl.client->totalSentData;
		stats::incoming += cl.client->totalReceivedData;
		cl.client->totalSentData = 0;
		cl.client->totalReceivedData = 0;

		cl.data = data;
		cl.packet = enet_packet_create( &data[0], data.size(), ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send( cl.peer, 0, cl.packet );

		return true;
	}

	bool recv( int &socket, std::string &data, double timeout_s )
	{
		data = std::string();

		if( socket <= 0 )
			return false;

		client_t &cl = clients[socket];

		if( !cl.connected )
			return false;

		stats::outgoing += cl.client->totalSentData;
		stats::incoming += cl.client->totalReceivedData;
		cl.client->totalSentData = 0;
		cl.client->totalReceivedData = 0;

		int result = enet_host_service( cl.client, &cl.event, (enet_uint32)( timeout_s * 1000 ) );

		if( result > 0 )
		{
			switch( cl.event.type )
			{
				case ENET_EVENT_TYPE_RECEIVE:
					data.resize( cl.event.packet->dataLength );
					memcpy( &data[0], cl.event.packet->data, cl.event.packet->dataLength );
					return true;
				case ENET_EVENT_TYPE_DISCONNECT:
					cl.connected = false;
					return "disconnected from host", false;
			}
		}
		// = 0 -> no ev
		// < 0 -> error

		return false;
	}

	bool is_connected( int &socket )
	{
		if( socket <= 0 )
			return false;

		client_t &cl = clients[socket];

		stats::outgoing += cl.client->totalSentData;
		stats::incoming += cl.client->totalReceivedData;
		cl.client->totalSentData = 0;
		cl.client->totalReceivedData = 0;

		return cl.connected;
	}

	bool close( int &socket )
	{
		if( socket > 0 )
		{
			client_t &cl = clients[socket];

			if( cl.connected )
			{
				enet_peer_disconnect( cl.peer, 0 );
				cl.connected = false;
			}

			enet_peer_reset( cl.peer );
		}

		socket = 0;

		return true;
	}

    // tools

    double get_bytes_sent()
    {
    	return stats::outgoing;
    }

    double get_bytes_received()
    {
    	return stats::incoming;
    }

    void sleep( double seconds )
    {
		sleep4( seconds );
    }
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <map>
#include <sstream>
#include <string>

#include <enet/enet.h>

#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "ws2_32.lib" )

#include "udp.hpp"

namespace udps
{
    namespace stats
    {
        double incoming = 0, outgoing = 0;
        size_t connected = 0;
    }

	struct server
	{
		ENetEvent  event;
		ENetHost   *self;
		int port;
		int peers;

		bool ready;

		server() : ready(false)
		{}
	};

	std::map< int, server > servers;
	int id = 0;

	bool listen( int &socket, int port, int peers )
	{
		socket = ++id;

		server &s = ( servers[socket] = servers[socket] );

		ENetAddress address;
		address.host = ENET_HOST_ANY;
		address.port = port;

		s.port = port;
		s.peers = peers;
		s.self = enet_host_create( &address, peers, 2, 0, 0 );

		if( !s.self )
			return "Could not start server", false;

		s.ready = true;

		return true;
	}

	bool listen( int &socket, const std::string &port, int peers )
	{
		int _port;
		std::stringstream ss;

		ss << port;
		if( !(ss >> _port ) )
			return "Could not parse port string", false;

		return listen( socket, _port, peers );
	}

	bool stop( int &socket )
	{
		if( socket <= 0 )
			return false;

		server &s = servers[socket];

		enet_host_destroy( s.self );

		servers.erase( servers.find( socket ) );
		socket = 0;

		return true;
	}


	bool recv( int &socket, std::string &data )
	{
		if( socket <= 0 )
			return "invalid server socket", false;

		server &s = servers[socket];

		data = std::string();

		if( s.event.packet && s.event.packet->data && s.event.packet->dataLength > 0 )
		{
			data.resize( s.event.packet->dataLength );
			memcpy( &data[0], s.event.packet->data, s.event.packet->dataLength );
		}

		//stats::incoming += data.size();
		//stats::incoming = s.self->totalReceivedData; // totalReceivedPackets
		return true;
	}

	bool send( int &socket, const std::string &data, int peer )
	{
		if( socket <= 0 )
			return "invalid server socket", false;

		if( !data.size() )
			return true; // :)

		server &s = servers[socket];

		ENetPacket *packet;
		packet = enet_packet_create( &data[0], data.size(), 0 );
		enet_peer_send( &s.self->peers[peer], 0, packet );
		enet_host_flush( s.self );

		//stats::outgoing += data.size();
		//stats::outgoing = s.self->totalSentData; // totalSentPackets
		return true;
	}

	bool broadcast( int &socket, const std::string &data )
	{
		if( socket <= 0 )
			return "invalid server socket", false;

		if( !data.size() )
			return true; // :)

		server &s = servers[socket];

		ENetPacket *packet;
		packet = enet_packet_create( &data[0], data.size(), 0 );
		enet_host_broadcast( s.self, 1, packet );
		enet_host_flush( s.self );

		//stats::outgoing += ( s.peers ) * data.size();
		//stats::outgoing = s.self->totalSentData; // totalSentPackets
		return true;
	}

	bool broadcast_but( int &socket, const std::string &data, int peer )
	{
		if( socket <= 0 )
			return "invalid server socket", false;

		if( !data.size() )
			return true; // :)

		server &s = servers[socket];

		ENetPacket *packet;
		packet = enet_packet_create( &data[0], data.size(), 0 );

		for( int i = 0; i < s.peers; i++ )
		{
			if( i != peer ) //&s.self->peers[i] != s.event.peer )
			{
				enet_peer_send( &s.self->peers[i], 0, packet );
			}
		}

		enet_host_flush( s.self );

		//stats::outgoing += ( s.peers - 1 ) * data.size();
		//stats::outgoing = s.self->totalSentData; // totalSentPackets
		return true;
	}

	bool poll( int &socket, event &e, double timeout_s )
	{
		if( socket <= 0 )
			return "invalid server socket", false;

		server &s = servers[socket];

		stats::incoming += s.self->totalReceivedData; // totalReceivedPackets
		stats::outgoing += s.self->totalSentData; // totalSentPackets

		s.self->totalReceivedData = 0;
		s.self->totalSentData = 0;

		int result = ( enet_host_service( s.self, &s.event, (enet_uint32)( timeout_s * 1000 ) ) );

		e.name = std::string();
		e.data = std::string();
		e.type = OK;
		e.peer = 0;

		for( int i = 0; i < s.peers; i++ )
		{
			if( &s.self->peers[i] == s.event.peer )
			{
				e.peer = i;
			}
		}

		if( result < 0 )
		{
			e.type = ERROR;
			return false; // error
		}
		else
		if( result == 0 )
		{
			e.type = OK;
			return true; // do nothing
		}
		else
//		if( result > 0 )
		{
			if( s.event.type == ENET_EVENT_TYPE_CONNECT )
			{
				s.event.peer->data = 0;

				e.type = OK;
				return true;
			}
			else
			if( s.event.type == ENET_EVENT_TYPE_RECEIVE )
			{
				std::string data;
				udps::recv( socket, data );

				if( !s.event.peer->data )
				{
					s.event.peer->data = new char [ data.size() + 1 ];
					memcpy( s.event.peer->data, &data[0], data.size() );
					((char *)s.event.peer->data)[ data.size() ] = '\0';

					stats::connected++;

					e.name = (char*)s.event.peer->data;
					e.type = JOIN;
					return true;
				}
				else
				{
					e.name = (char*)s.event.peer->data;
					e.type = DATA;
					e.data = data;
					return true;
				}
			}
			else
			if( s.event.type == ENET_EVENT_TYPE_DISCONNECT )
			{
				if( s.event.peer->data )
				{
					e.name = (char*)s.event.peer->data;

					delete [] s.event.peer->data;
					s.event.peer->data = NULL;
				}

				stats::connected--;

				e.type = PART;
				return true;
			}
			else
			{
				e.type = OK;
				return true;
			}
		}
	}

	/* sugars: */

	bool say( int &socket, const std::string &data, int peer )
	{
		return send( socket, data, peer );
	}

	bool echo( int &socket, const std::string &data, int peer )
	{
		return broadcast_but( socket, data, peer );
	}

	bool shout( int &socket, const std::string &data )
	{
		return broadcast( socket, data );
	}

    // tools

    double get_bytes_sent()
    {
    	return stats::outgoing;
    }

    double get_bytes_received()
    {
    	return stats::incoming;
    }

    size_t get_peers_connected()
    {
    	return stats::connected;
    }

    void sleep( double seconds )
    {
		sleep4( seconds );
    }

	/* self init: */

	static struct once
	{
		 once() { if( !enet_initialize() ) return; fprintf( stderr, "%s", "Could not initialize enet" ); exit(1); }
		~once() { for( auto &it : servers ) if( it.second.ready ) { int socket = it.first; fprintf( stdout, "Closing server at port #%d\n", it.second.port ); stop( socket ); } enet_deinitialize(); }
	}_;
}
