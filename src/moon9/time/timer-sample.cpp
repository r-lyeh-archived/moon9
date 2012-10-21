#include <cassert>

#include <iostream>
#include <iomanip>

#include "rtc.hpp"
#include "timer.hpp"
#include "sleep.hpp"

int main( int argc, char **argv )
{
    std::cout << "- moon9::rtc is a realtime clock class,\n  moon9::timer is a high resolution timer class" << std::endl;
    std::cout << "- treat classes as doubles, measuring in seconds. update by using .update() method" << std::endl;
    std::cout << "- human readable/serialization by using .str() method" << std::endl;
    std::cout << "- check header for other methods" << std::endl;
    std::cout << std::endl;

    moon9::rtc rtc;
    moon9::rtc rtc_date( "2010-12-31 23:59:59" );
    moon9::timer timer;

    std::cout << "* after creation (as double)" << std::endl;

        std::cout << "- rtc : " << std::setprecision(20) <<  rtc << std::endl;
        std::cout << "- rtc date : " << std::setprecision(20) << rtc_date << std::endl;
        std::cout << "- timer : " << timer << std::endl;

        std::cout << std::endl;

    std::cout << "* after creation (as string)" << std::endl;

        std::cout << "- rtc : " << rtc.str() << std::endl;
        std::cout << "- rtc date : " << rtc_date.str() << std::endl;
        std::cout << "- timer : " << timer.str() << std::endl;

        std::cout << std::endl;

    std::cout << "* after freezing app for 1 second" << std::endl;

        moon9::sleep(1);

        std::cout << "- rtc : " << rtc.str() << std::endl;
        std::cout << "- rtc date : " << rtc_date.str() << std::endl;
        std::cout << "- timer : " << timer.str() << std::endl;

        std::cout << std::endl;

    std::cout << "* after updating (as double)" << std::endl;

        rtc.update();
        rtc_date.update();
        timer.update();

        std::cout << "- rtc : " << std::setprecision(20) << rtc << std::endl;
        std::cout << "- rtc date : " << std::setprecision(20) << rtc_date << std::endl;
        std::cout << "- timer : " << timer << std::endl;

        std::cout << std::endl;

    std::cout << "* after updating (as string)" << std::endl;

        std::cout << "- rtc : " << rtc.str() << std::endl;
        std::cout << "- rtc date : " << rtc_date.str() << std::endl;
        std::cout << "- timer : " << timer.str() << std::endl;

        std::cout << std::endl;

    std::cout << "* logical/arithmetical tests:" << std::endl;

        std::cout << "test #1 rtc       == rtc   : " << ( rtc       == rtc   ? "ok" : "FAILED" ) << std::endl;
        std::cout << "test #2 rtc       != timer : " << ( rtc       != timer ? "ok" : "FAILED" ) << std::endl;
        std::cout << "test #3 timer + 1 >  timer : " << ( timer + 1 >  timer ? "ok" : "FAILED" ) << std::endl;

        std::cout << std::endl;

    std::cout << "* daylight savings tests:" << std::endl;

        moon9::rtc winter( "2010-12-31 23:59:59" );
        moon9::rtc autumn( "1991-10-10 17:00:00" );

        std::cout << winter.str() << " vs " << "2010-12-31 23:59:59" << std::endl;
        std::cout << autumn.str() << " vs " << "1991-10-10 17:00:00" << std::endl;

        assert( winter.str() == "2010-12-31 23:59:59" );
        assert( autumn.str() == "1991-10-10 17:00:00" );

    return 0;
}
