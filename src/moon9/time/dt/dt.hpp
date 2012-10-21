#pragma once

#include <chrono>

namespace moon9
{
	class dt
	{
		typedef std::chrono::high_resolution_clock clock;
		clock::time_point start;

		public:

		dt()
		{
			start = clock::now();
		}

		void reset()
		{
			*this = dt();
		}

		double s()
		{
			return std::chrono::nanoseconds( clock::now() - start ).count() / 1000000000.0;	//::seconds
		}

		double ms()
		{
			return std::chrono::nanoseconds( clock::now() - start ).count() / 1000000.0; //::milliseconds
		}

		double us()
		{
			return std::chrono::nanoseconds( clock::now() - start ).count() / 1000.0; //::microseconds
		}

		double ns()
		{
			return std::chrono::nanoseconds( clock::now() - start ).count() / 1.0;
		}
	};
}

namespace legacy
{
    void wink();
    void sleep( double seconds );
    void yield();
}
