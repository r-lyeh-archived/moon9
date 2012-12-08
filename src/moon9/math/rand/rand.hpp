/*
 * @todo
 * seed s; // time(0)
 * seed s( 34 );
 * s.next( 0,1 );
 * s.next(-1,1 );
 * s.next( N,M );
 * s.next( 0,N );
 *
 * rand01( seed );
 * rand11( seed );
 * rand( seed, N );
 * rand( seed, N, M );
 *
 * rand( seed, N, M )
 * randf( seed, N, M )
 */

#pragma once

namespace moon9
{
	//[ 0,1]
	double rand01();
	//[-1,1]
	double rand11();
#if 0
	// [0,N]
	int rand( int N );
	// [N,M]
	int rand( int N, int M );
#else
	// [N,M]
	template<typename T>
	T rand( T N, T M )
	{
		double ret = N + moon9::rand01() * ( M + 1 - N );
		return T( ret > M ? M : ret );
	}
	// [0,N]
	template<typename T>
	T rand( T N )
	{
		return rand<T>( 0, N );
	}
#endif
    // {+1,-1}
    int randsgn();
}
