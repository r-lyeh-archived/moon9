#include "compress.hpp"

#include <string>
#include <cassert>

int main( int argc, char **argv )
{
	std::string original = std::string( 1000, 'a' );

	std::string compressed = moon9::z(original);
	std::string decompressed = moon9::unz(compressed);

    std::cout << "Original: " << original.size() << " bytes" << std::endl;
    std::cout << "Compressed: " << compressed.size() << " bytes" << std::endl;
    std::cout << "Decompressed: " << decompressed.size() << " bytes" << std::endl;

	assert( original == decompressed );
	assert( moon9::is_z(compressed) == true );
	assert( moon9::is_unz(decompressed) == true );

	std::cout << "all ok" << std::endl;

	return 0;
}
