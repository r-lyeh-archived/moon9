#include <iostream>

#include "xml.hpp"

//xlm2json
//json2xml

int main( int argc, char **argv )
{
	moon9::xml xml;

	xml.
		push("root").
			node("integer", 13).
			node("float", 3.14159f).
			node("string", "hello world").
		back();

	std::cout << xml.str() << std::endl;

	return 0;
}
