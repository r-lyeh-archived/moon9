#include <cassert>

#if defined( _WIN32 )

#   include <Windows.h>
#   include <ctime>

#   define kBooTimerHandle                      LARGE_INTEGER
#   define kBooTimerFreq( handle )              { kBooTimerHandle fhandle; DWORD_PTR oldmask = ::SetThreadAffinityMask(::GetCurrentThread(), 0); ::QueryPerformanceFrequency( &fhandle ); ::SetThreadAffinityMask(::GetCurrentThread(), oldmask); frequency = 1000000.0 / double(fhandle.QuadPart); }
#   define kBooTimerUpdate( handle )            {                          DWORD_PTR oldmask = ::SetThreadAffinityMask(::GetCurrentThread(), 0); ::QueryPerformanceCounter  ( &handle ); ::SetThreadAffinityMask(::GetCurrentThread(), oldmask); }
#   define kBooTimerSetCounter( handle, value ) handle.QuadPart = value
#   define kBooTimerDiffCounter( handle1, handle2 ) ( ( handle2.QuadPart - handle1.QuadPart ) * frequency )
#   define kBooTimerSleep( seconds_f )          Sleep( (int)(seconds_f * 1000) )
#   define kBooTimerWink( units_t )             Sleep( units_t )

#else

#   include <sys/time.h>
#   include <ctime>

//hmmm... check clock_getres() as seen in http://tdistler.com/2010/06/27/high-performance-timing-on-linux-windows#more-350
//frequency int clock_getres(clockid_t clock_id, struct timespec *res);
//clock     int clock_gettime(clockid_t clock_id, struct timespec *tp);

//nanosleep() instead?

#   define kBooTimerHandle                      timeval
#   define kBooTimerFreq( handle )
#   define kBooTimerUpdate( handle )            gettimeofday( &handle, NULL )
#   define kBooTimerSetCounter( handle, value ) do { handle.tv_sec = 0; handle.tv_usec = value; } while (0)
#   define kBooTimerDiffCounter( handle1, handle2 ) ( (handle2.tv_sec * 1000000.0) + handle2.tv_usec ) - ( (handle1.tv_sec * 1000000.0) + handle1.tv_usec )
#   define kBooTimerSleep( seconds_f )          usleep( seconds_f * 1000000.f )
#   define kBooTimerWink( units_t )             usleep( units_t )
//  do { float fractpart, intpart; fractpart = std::modf( seconds_f, &intpart); \
//    ::sleep( int(intpart) ); usleep( int(fractpart * 1000000) ); } while( 0 )

#endif

namespace legacy
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
            kBooTimerUpdate( endCount );

            return kBooTimerDiffCounter( startCount, endCount );
        }

        double ns() //const
        {
            return operator double() * 1000.0;
        }

        void reset()
        {
            kBooTimerFreq( frequency ); //to dt2() ctor ?

            kBooTimerSetCounter( startCount, 0 );
            kBooTimerSetCounter( endCount, 0 );

            kBooTimerUpdate( startCount );
        }

        protected:

        kBooTimerHandle startCount, endCount;
        double frequency;
    };

    void wink()
    {
        kBooTimerWink( 1 );
    }

    void sleep( double seconds )
    {
        kBooTimerSleep( seconds );
    }

    void yield()
    {
#ifdef _WIN32
        kBooTimerWink( 0 );
#else
        // @todo
#endif
    }
}

#undef kBooTimerHandle
#undef kBooTimerFreq
#undef kBooTimerUpdate
#undef kBooTimerDiffCounter
#undef kBooTimerSetCounter
#undef kBooTimerGetCounter
#undef kBooTimerSleep
#undef kBooTimerWink
