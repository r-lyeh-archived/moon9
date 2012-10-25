// Simple Burg predictor. Based on code by Cedrick Collomb (unoptimized!)
// - rlyeh

#include <math.h> 
#include <deque> //#include <vector> 
#include <iostream>

namespace moon9
{
	// Returns in vector coefficients calculated using Burg algorithm applied to the input source std::deque<double> x 
	std::deque<double> get_burg_coefficients( const std::deque<double> &sequence, size_t num_coeffs ) 
	{ 
		std::deque<double> coeffs( num_coeffs, 0.0 );

		// GET SIZE FROM INPUT VECTORS 
		size_t N = sequence.size() - 1; 
		size_t m = coeffs.size(); 
		// INITIALIZE Ak 
		std::deque<double> Ak( m + 1, 0.0 ); 
		Ak[ 0 ] = 1.0; 
		// INITIALIZE f and b 
		std::deque<double> f( sequence ); 
		std::deque<double> b( sequence ); 
		// INITIALIZE Dk 
		double Dk = 0.0; 
		for ( size_t j = 0; j <= N; j++ ) 
		{ 
			Dk += 2.0 * f[ j ] * f[ j ]; 
		} 
		Dk -= f[ 0 ] * f[ 0 ] + b[ N ] * b[ N ]; 
		// BURG RECURSION 
		for ( size_t k = 0; k < m; k++ ) 
		{ 
			// COMPUTE MU 
			double mu = 0.0; 
			for ( size_t n = 0; n <= N - k - 1; n++ ) 
			{ 
				mu += f[ n + k + 1 ] * b[ n ]; 
			} 
			mu *= -2.0 / Dk; 
			// UPDATE Ak 
			for ( size_t n = 0; n <= ( k + 1 ) / 2; n++ ) 
			{ 
				double t1 = Ak[ n ] + mu * Ak[ k + 1 - n ]; 
				double t2 = Ak[ k + 1 - n ] + mu * Ak[ n ]; 
				Ak[ n ] = t1; 
				Ak[ k + 1 - n ] = t2; 
			} 
			// UPDATE f and b 
			for ( size_t n = 0; n <= N - k - 1; n++ ) 
			{ 
				double t1 = f[ n + k + 1 ] + mu * b[ n ]; 
				double t2 = b[ n ] + mu * f[ n + k + 1 ]; 
				f[ n + k + 1 ] = t1; 
				b[ n ] = t2; 
			} 
			// UPDATE Dk 
			Dk = ( 1.0 - mu * mu ) * Dk - f[ k + 1 ] * f[ k + 1 ] - b[ N - k - 1 ] * b[ N - k - 1 ]; 
		} 
		// ASSIGN COEFFICIENTS 
		coeffs.assign( ++Ak.begin(), Ak.end() ); 

		return coeffs;
	} 

	std::deque<double> get_burg_prediction( const std::deque<double> &original, const std::deque<double> &coeffs, size_t samples_to_expand )
	{
		std::deque<double> predicted( original ); 
		size_t m = coeffs.size();

		for( size_t i = original.size(); i < original.size() + samples_to_expand; ++i ) 
		{ 
			double sample = 0.0;

			for( size_t j = 0; j < m; j++ ) 
				sample -= coeffs[ j ] * predicted[ i - 1 - j ]; 

			predicted.push_back( sample );
		} 

		return predicted;
	}

	std::deque<double> expand_burg( const std::deque<double> &original, size_t samples_to_expand, const size_t number_of_coefficients )
	{
		// the higher coefficients, the better accuracy

		// GET LINEAR PREDICTION COEFFICIENTS 
		std::deque<double> coeffs = get_burg_coefficients( original, number_of_coefficients ); 

		// LINEAR PREDICT DATA
		std::deque<double> predicted( original ), predicted2; 
		size_t m = coeffs.size();

		for( size_t i = original.size(); i < original.size() + samples_to_expand; ++i ) 
		{ 
			double sample = 0.0;

			for( size_t j = 0; j < m; j++ ) 
				sample -= coeffs[ j ] * predicted[ i - 1 - j ]; 

			predicted.push_back( sample );
			//predicted2.push_back( sample );
		} 

		return predicted;
		//return predicted2;
	}	
}
