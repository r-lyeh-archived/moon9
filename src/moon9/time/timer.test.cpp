#include <moon9/test/test.hpp>

#include <moon9/time/timer.hpp>

namespace
{
	bool timer_dt()
    {
        moon9::timer timer;

        // logical/arithmetical tests

        test3( timer, ==, 0 );

        timer.update();

        test3( timer, ==, timer );
        test3( timer + 1, >, timer );

        return 0;
    }

    const bool tested = timer_dt();
}
