#include <cassert>

#if defined( _WIN32 )

#   include <Windows.h>
#   include <ctime>

#   define kMoon9TimerHandle                      LARGE_INTEGER
#   define kMoon9TimerFreq( handle )              { kMoon9TimerHandle fhandle; DWORD_PTR oldmask = ::SetThreadAffinityMask(::GetCurrentThread(), 0); ::QueryPerformanceFrequency( &fhandle ); ::SetThreadAffinityMask(::GetCurrentThread(), oldmask); frequency = 1000000.0 / double(fhandle.QuadPart); }
#   define kMoon9TimerUpdate( handle )            {                          DWORD_PTR oldmask = ::SetThreadAffinityMask(::GetCurrentThread(), 0); ::QueryPerformanceCounter  ( &handle ); ::SetThreadAffinityMask(::GetCurrentThread(), oldmask); }
#   define kMoon9TimerSetCounter( handle, value ) handle.QuadPart = value
#   define kMoon9TimerDiffCounter( handle1, handle2 ) ( ( handle2.QuadPart - handle1.QuadPart ) * frequency )
#   define kMoon9TimerSleep( seconds_f )          Sleep( (int)(seconds_f * 1000) )
#   define kMoon9TimerWink( units_t )             Sleep( units_t )

#else

#   include <sys/time.h>
#   include <ctime>

//hmmm... check clock_getres() as seen in http://tdistler.com/2010/06/27/high-performance-timing-on-linux-windows#more-350
//frequency int clock_getres(clockid_t clock_id, struct timespec *res);
//clock     int clock_gettime(clockid_t clock_id, struct timespec *tp);

//nanosleep() instead?

#   define kMoon9TimerHandle                      timeval
#   define kMoon9TimerFreq( handle )
#   define kMoon9TimerUpdate( handle )            gettimeofday( &handle, NULL )
#   define kMoon9TimerSetCounter( handle, value ) do { handle.tv_sec = 0; handle.tv_usec = value; } while (0)
#   define kMoon9TimerDiffCounter( handle1, handle2 ) ( (handle2.tv_sec * 1000000.0) + handle2.tv_usec ) - ( (handle1.tv_sec * 1000000.0) + handle1.tv_usec )
#   define kMoon9TimerSleep( seconds_f )          usleep( seconds_f * 1000000.f )
#   define kMoon9TimerWink( units_t )             usleep( units_t )
//  do { float fractpart, intpart; fractpart = std::modf( seconds_f, &intpart); \
//    ::sleep( int(intpart) ); usleep( int(fractpart * 1000000) ); } while( 0 )

#endif

namespace //legacy
{
    class dt
    {
        public:

        dt()
        {
            reset();
        }

/*
        double w()
        {
            return operator double() / 604800.0;
        }

        double d()
        {
            return operator double() / 86400.0;
        }

        double h()
        {
            return operator double() / 3600.0;
        }

        double m()
        {
            return operator double() / 60.0;
        }
*/
        double s() //const
        {
            return operator double() / 1000000.0;
        }

        double ms() //const
        {
            return operator double() / 1000.0;
        }

        double us() //const
        {
            return operator double();
        }

        operator double() //const
        {
            kMoon9TimerUpdate( endCount );

            return kMoon9TimerDiffCounter( startCount, endCount );
        }

        double ns() //const
        {
            return operator double() * 1000.0;
        }

        void reset()
        {
            kMoon9TimerFreq( frequency ); //to dt2() ctor ?

            kMoon9TimerSetCounter( startCount, 0 );
            kMoon9TimerSetCounter( endCount, 0 );

            kMoon9TimerUpdate( startCount );
        }

        protected:

        kMoon9TimerHandle startCount, endCount;
        double frequency;
    };
}

namespace moon9
{
    void wink()
    {
        kMoon9TimerWink( 1 );
    }

#if 0
    void sleep( double seconds )
    {
        kMoon9TimerSleep( seconds );
    }

    void yield()
    {
#ifdef _WIN32
        kMoon9TimerWink( 0 );
#else
        // @todo
#endif
    }
#endif
}

#undef kMoon9TimerHandle
#undef kMoon9TimerFreq
#undef kMoon9TimerUpdate
#undef kMoon9TimerDiffCounter
#undef kMoon9TimerSetCounter
#undef kMoon9TimerGetCounter
#undef kMoon9TimerSleep
#undef kMoon9TimerWink
