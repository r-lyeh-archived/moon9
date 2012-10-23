#include <string>

#include "alert.hpp"

int main( int argc, char **argv )
{
	std::string str = "hello world";
	alert( str );

	int integer = 10;
	alert( integer );

	float float_pi = 3.14159f;
	alert( float_pi );

    double exp_double = 10.0;
    alert( exp_double );

    bool boolean = false;
    alert( boolean );

	return 0;
}
