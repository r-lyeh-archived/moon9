// Simple Burg predictor
// - rlyeh 

#pragma once

#include <deque> //#include <vector> 

namespace moon9
{
	// api
	std::deque<double> get_burg_coefficients( const std::deque<double> &sequence, size_t num_coeffs ) ;
	std::deque<double> get_burg_prediction( const std::deque<double> &original, const std::deque<double> &coeffs, size_t samples_to_expand );

	// simplified api
	std::deque<double> expand_burg( const std::deque<double> &original, const size_t samples_to_expand, const size_t number_of_coefficients = 4 );  // the higher coefficients, the better accuracy
}
