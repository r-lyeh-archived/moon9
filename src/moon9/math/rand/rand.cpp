// tools

#include "mersenne-twister.hpp"

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

	int rand( int N, int M ) // [N,M]
	{
		float ret = N + moon9::rand01() * ( M + 1 - N );
		return int( ret > M ? M : ret );
	}

	int rand( int N ) // [0,N]
	{
		return rand( 0, N );
	}

	int randsgn() // {+1,-1}
	{
		return moon9::rand01() >= 0.5 ? +1 : -1;
	}
}
