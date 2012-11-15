#include <moon9/test/test.hpp>

#include <moon9/time/dt.hpp>

namespace
{
	bool test_dt()
	{
		test3( moon9::dt().ns(), >, 0 );

		return true;
	}

    const bool tested = test_dt();
}
