#include "dt.hpp"
#include "rtc.hpp"

namespace moon9
{
    namespace
    {
        moon9::dt global_timer;
        double epoch_at_startup = moon9::rtc().get();
        double offset = 0;
    }

    double now()
    {
        return epoch_at_startup + offset + global_timer.s();
    }

    // for testing and QA purposes, ie, advance a whole year in runtime and check all now() calls are still working
    void now_advance( double t )
    {
        offset += t;
    }
}
