// simple prime functions
// - rlyeh

#pragma once 

#include <vector>

namespace math9
{
	bool is_prime( size_t n );
	size_t find_prime_larger_than( size_t n );
	size_t find_prime_larger_or_equal_than( size_t n );
	std::vector<size_t> get_prime_list_upto( size_t n );
}
