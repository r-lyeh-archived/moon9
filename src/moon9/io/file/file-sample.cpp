#include <cassert>
#include <iostream>

#include "file.hpp"

int main( int argc, char **argv )
{
	std::string tmpname = moon9::file().name();
	assert( tmpname.size() > 0 );

	std::cout << moon9::file("test.c").ext() << std::endl;
	std::cout << moon9::file("test.exe").ext() << std::endl;
	std::cout << moon9::file(argv[0]).ext() << std::endl;

	assert( moon9::file(tmpname).overwrite(tmpname) );
	assert( moon9::file(tmpname).exist() );
	assert( moon9::file(tmpname).append(tmpname) );
	assert( moon9::file(tmpname).size() > 0 );
	assert( moon9::file(tmpname).read() == tmpname + tmpname );

	time_t date = moon9::file(tmpname).date();
	assert( moon9::file(tmpname).date() > 0 );
	assert( moon9::file(tmpname).touch() );
	assert( moon9::file(tmpname).date() >= date );

	assert( moon9::file(tmpname).patch("hello world") );
	assert( moon9::file(tmpname).read() == "hello world" );

	std::string tmpname2 = moon9::file().name();
	assert( moon9::file(tmpname).rename(tmpname2) );
	assert( moon9::file(tmpname2).remove() );

	moon9::file file;
	file.set("has_custom_variants_double", 123.0 );
	file.set("has_custom_variants_boolean", "false" );
	assert( file.get("has_custom_variants_double").as<int>() == 123 );
	assert( file.get("has_custom_variants_boolean").as<bool>() == false );

	std::cout << "All ok." << std::endl;

	return 0;
}
