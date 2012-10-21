#include <chrono>
#include <thread>

namespace moon9
{
	void sleep( double seconds )
	{
	// 	std::chrono::milliseconds duration( (int)(seconds * 1000) );
	   	std::chrono::microseconds duration( (int)(seconds * 1000000) );
	// 	std::chrono::nanoseconds  duration( (int)(seconds * 1000000000) );
	    std::this_thread::sleep_for( duration );
	}	
}
