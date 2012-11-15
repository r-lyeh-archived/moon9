#include <cassert>

#if defined( _WIN32 )

#   include <Windows.h>
#   include <ctime>

#   define kBooTimerSleep( seconds_f )          Sleep( (int)(seconds_f * 1000) )
#   define kBooTimerWink( units_t )             Sleep( units_t )

#else

#   include <sys/time.h>
#   include <ctime>

//  nanosleep() instead?

#   define kBooTimerSleep( seconds_f )          usleep( seconds_f * 1000000.f )
#   define kBooTimerWink( units_t )             usleep( units_t )
//  do { float fractpart, intpart; fractpart = std::modf( seconds_f, &intpart); \
//    ::sleep( int(intpart) ); usleep( int(fractpart * 1000000) ); } while( 0 )

#endif

namespace legacy
{
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

#undef kBooTimerSleep
#undef kBooTimerWink
