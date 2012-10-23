#ifdef test3

#include <moon9/io/message.hpp>

namespace
{
    struct def1
    {
        int a, b;
        float c;

        MSGDEF( a, b, c );
    };

    void msg1()
    {
        def1 t1 = { 1,-1000,3.14159f };
        msg m = make::msg( t1 );
        def1 t2 = make::obj<def1>( m );
        test3( t1.a, ==, t2.a );
        test3( t1.b, ==, t2.b );
        test3( t1.c, ==, t2.c );
//      test3( t1, ==, t2 );
    }

    struct def2
    {
            MSGDEF(t, dt, hits, x1, y1, z1, x2, y2, z2);

            double t, dt, hits;
            double x1,y1,z1;
            double x2,y2,z2;
    };

    void msg2()
    {
        def2 p, q;

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
        q = make::obj<def2>( m );

        test3( p.t, ==, q.t );
        test3( p.z2, ==, q.z2 );
    }

    const bool tested = (msg1(), msg2(), true);
}

#endif

#if 0

#include <string>
#include <moon9/os/types.hpp>
#include <moon9/io/message.hpp>

struct say
{
    moon9::bits64 user_id;
    std::string what;

    MSGDEF( user_id, what );
};

int main( int argc, char **argv )
{
    say s = { 12314123, "hello world" };

    std::vector< msg > msgs;
    msgs.push_back( make::msg( s ) );

    std::cout << msgs.back() << std::endl;

    return 0;
}

#endif
