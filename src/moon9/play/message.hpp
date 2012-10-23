#pragma once

#include <msgpack.hpp>
#include <iostream>

#if 1
typedef std::vector<char> msg;
#else
typedef msgpack::sbuffer msg;
#endif

namespace moon9
{
    namespace
    {
        struct detail_msg
        {
            enum { debug = false };

            template<typename T>
            static void encode( msg &binary, const T &t )
            {
                //binary.resize( t.size() );
                //binary.write( t.data(), t.size() );

                binary.resize( t.size() );
                memcpy( binary.data(), t.data(), t.size() );
            }

            template<typename T>
            static void decode( const msg &sbuf, T &q )
            {
                try
                {
                    // deserialize from buffer
                    msgpack::unpacked msg;
                    msgpack::unpack( &msg, sbuf.data(), sbuf.size() );

                    // get dynamically-typed object
                    msgpack::object obj = msg.get();

                    // debug it
                    if( debug )
                        std::cout << "msgpack: " << obj << std::endl;

                    // convert it to statically-typed object
                    //obj.convert(&q);
                    q = obj.as<T>();
                }
                #if 0
                catch( msgpack::type_error &e )
                {
                    std::cerr << "msgpack: failed to deserialize object! (invalid type conversion)" << std::endl;
                }
                catch( msgpack::unpack_error &e )
                {
                    std::cerr << "msgpack: " << e.what() << std::endl;
                }
                #endif
                catch(...)
                {
                    std::cerr << "msgpack: failed to deserialize object!" << std::endl;
                }
            }

            template<typename T>
            static void print( const msg &sbuf, T &stream )
            {
                // deserialize from buffer
                msgpack::unpacked msg;
                msgpack::unpack( &msg, sbuf.data(), sbuf.size() );

                // get dynamically-typed object
                msgpack::object obj = msg.get();

                // debug it
                stream << "msgpack (" << sbuf.size() << " bytes): " << obj << std::endl;
            }
        };
    }
}

namespace make
{
    template<typename T>
    ::msg msg( const T& t )
    {
        // serialize to buffer
        msgpack::sbuffer sbuf;
        msgpack::pack( &sbuf, t );

        // return buffer
        ::msg m;
        moon9::detail_msg::encode( m, sbuf );

        return m;
    }

    template<typename T>
    T obj( const ::msg &m )
    {
        T q;
        moon9::detail_msg::decode( m, q );

        return q;
    }
}

namespace
{
    std::ostream &operator<<( std::ostream &oss, const msg &sbuf )
    {
        moon9::detail_msg::print( sbuf, oss );
        return oss;
    }

    void print( const msg &sbuf )
    {
        moon9::detail_msg::print( sbuf, std::cout );
    }
}



#define MSGDEF MSGPACK_DEFINE

/*

    // usage:

    struct def_myplayer
    {
            MSGDEF(t, dt, hits, x1, y1, z1, x2, y2, z2);

            double t, dt, hits;
            double x1,y1,z1;
            double x2,y2,z2;
    }
    p, q;

    p.t = 1;
    p.dt = 2;
    p.hits = 3;
    p.x1 = 4;
    p.y1 = 5;
    p.z1 = 6;
    p.x2 = 7;
    p.y2 = 8;
    p.z2 = 9;

    msg m = make::msg( p );
    q = make::obj<def_myplayer>( m );

    assert( p.t == q.t );
    assert( p.z2 == q.z2 );

*/
