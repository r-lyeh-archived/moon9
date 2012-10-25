// Example program using Burg’s algorithm
// Based on code by Cedrick Collomb

#include <iostream>
#include <deque>

#include "burg.hpp"

std::deque<double> create_sequence_1( size_t samples )
{
	std::deque<double> sequence( samples, 0.0 );

	for ( size_t i = 0; i < sequence.size(); i++ )
	{
		sequence[ i ] = cos( i * 0.01 ) + 0.75 * cos( i * 0.03 )
			    + 0.5 * cos( i * 0.05 ) + 0.25 * cos( i * 0.11 );
	}

	return sequence;
}

std::deque<double> create_sequence_2( size_t samples )
{
	std::deque<double> sequence( samples, 0.0 );

	for ( size_t i = 0; i < sequence.size(); i++ )
	{
		sequence[ i ] = i * i;
	}

	return sequence;
}

void test( std::deque<double> (*fn)(size_t), size_t samples_from, size_t samples_to )
{
	// original sequence is 128 elements long
	std::deque<double> original = (*fn)(samples_to);
	// predicted sequence is 120 elements long, then expanded to 128 (120+8)
	std::deque<double> predicted = moon9::expand_burg( (*fn)(samples_from), samples_to - samples_from );

	// display error by comparing both sequences
	double error = 0.0;
	for ( size_t i = samples_from; i < samples_to; i++ )
	{
		printf( "Index: %.2d / Original: %.6f / Predicted: %.6f\n", i, original[ i ], predicted[ i ] );
		double delta = predicted[ i ] - original[ i ];
		error += delta * delta;
	}

	std::cout << "Burg Approximation Error: " << error << std::endl;
}


int main( int argc, char *argv[] )
{
	test( &create_sequence_1, 8, 12 ); // 120, 128 );
	test( &create_sequence_2, 8, 12 );

	return 0;
}
