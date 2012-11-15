#include <cassert>

#if defined( _WIN32 )

#   include <Windows.h>
#   include <ctime>

#   define kMoon9TimerWink( units_t )             Sleep( units_t )

#else

#   include <sys/time.h>
#   include <ctime>

//  nanosleep() instead? socket sleep instead?

#   define kMoon9TimerWink( units_t )             usleep( units_t )
//  do { float fractpart, intpart; fractpart = std::modf( seconds_f, &intpart); \
//    ::sleep( int(intpart) ); usleep( int(fractpart * 1000000) ); } while( 0 )

#endif

namespace moon9
{
    void wink()
    {
        kMoon9TimerWink( 1 );
    }
}

#undef kMoon9TimerWink
