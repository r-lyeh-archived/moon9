// simple prime functions
// - rlyeh

#include <cmath>
#include <vector>

namespace math9
{
	bool is_prime( size_t n )
	{
		// original code by Yves Gallot

		if( n < 2 ) return false;
		if( n < 4 ) return true;
		if( n % 2 == 0) return false;

		const size_t iMax = (size_t)std::sqrt(double(n)) + 1;

		for( size_t i = 3; i <= iMax; i += 2 )
			if( n % i == 0 )
				return false;

		return true;
	}

	size_t find_prime_larger_than( size_t n )
	{
		do ++n; while( !is_prime(n) );
		return n;
	}

	size_t find_prime_larger_or_equal_than( size_t n )
	{
		while( !is_prime(n) )
			++n;
		return n;
	}

	std::vector<size_t> get_prime_list_upto( size_t n )
	{
		std::vector<size_t> primes;
		for( size_t i = 1; i < n; ++i )
		{
			i = find_prime_larger_or_equal_than( i );
			primes.push_back( i );
		}
		return primes;	
	}
}
