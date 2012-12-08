// tools
#if 0
#include "mersenne-twister.hpp"
#else
// uniform_real_distribution example
#include <iostream>
#include <chrono>
#include <random>

struct MTRand
{
    unsigned seed;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution;

    MTRand()
    {
        seed = std::chrono::system_clock::now().time_since_epoch().count();
        generator = std::default_random_engine( seed );
        distribution = std::uniform_real_distribution<double>( 0.0, 1.0 );
    }

    double operator()()
    {
        // construct a trivial random generator engine from a time-based seed:
        return distribution(generator);
    }
};

#endif

namespace moon9
{
	double rand01() //[0,1]
	{
		static MTRand r;
		return r();
	}

	double rand11() //[-1,1]
	{
		static MTRand r;
		return r() - r();
	}
#if 0
	int rand( int N, int M ) // [N,M]
	{
		float ret = N + moon9::rand01() * ( M + 1 - N );
		return int( ret > M ? M : ret );
	}

	int rand( int N ) // [0,N]
	{
		return rand( 0, N );
	}
#endif
	int randsgn() // {+1,-1}
	{
		return moon9::rand01() >= 0.5 ? +1 : -1;
	}
}
