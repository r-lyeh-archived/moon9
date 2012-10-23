#include <iostream>
#include <chrono>
#include <thread>

#include "dt.hpp"

int main()
{

    // WHYYYYYYYYYYYY? ARGH
    std::cout << moon9::dt().ns() << "  WHYYYYYYYYYYYY? ARGH " << std::endl;

	{
		moon9::dt dt;

	   	std::chrono::microseconds duration( (int)(1.0 * 1000000) );
	    std::this_thread::sleep_for( duration );

	    std::cout << dt.s() << ',' << dt.ns() << ',' << dt.us() << std::endl;
	}
	/*
	{
		legacy::dt dt;
	    moon9::sleep(1);
	    std::cout << dt.s() << ',' << dt.ns() << ',' << dt.us() << std::endl;
	}
	*/

	return 0;
}
