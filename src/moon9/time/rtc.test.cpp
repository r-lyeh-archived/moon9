#include <moon9/test/test.hpp>

#include <moon9/time/rtc.hpp>

namespace
{
	bool timer_dt()
    {
        moon9::rtc rtc;

        test3( rtc, ==, 0 );

        rtc.update();

        // logical/arithmetical tests

        test3( rtc, ==, rtc );
        test3( rtc + 1, >, rtc);

        // daylight savings tests

        moon9::rtc winter( "2010-12-31 23:59:59" );
        moon9::rtc autumn( "1991-10-10 17:00:00" );

        test3( winter.str(), ==, "2010-12-31 23:59:59" );
        test3( autumn.str(), ==, "1991-10-10 17:00:00" );

        return 0;
    }

    const bool tested = timer_dt();
}
