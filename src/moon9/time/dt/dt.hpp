#pragma once

#if 0 // this C++11 solution not working anymore! Visual Studio 2012 issue? It used to work.. sigh :(

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

#else

#include "now/now.hpp"

namespace moon9
{
	class dt
	{
		double start;

		public:

		dt() : start( moon9::now() )
		{}

		void reset()
		{
			start = moon9::now();
		}

		double s()
		{
			return ( moon9::now() - start ) * 1.0;
		}

		double ms()
		{
			return ( moon9::now() - start ) * 1000.0;
		}

		double us()
		{
			return ( moon9::now() - start ) * 1000000.0;
		}

		double ns()
		{
			return ( moon9::now() - start ) * 1000000000.0;
		}
	};
}

#endif


namespace legacy
{
    void wink();
    void sleep( double seconds );
    void yield();
}
