#pragma once

namespace moon9
{
	struct binary
	{
		void *data;
		size_t size;
		
		enum { sizeof_data = sizeof(void*), sizeof_size = sizeof(size_t) };
	};
}

/*

int main( int argc, char **argv )
{
	binary b = { 0, 13 };

	assert3( b.size, ==, 13 );

	std::cout << "types ok..." << std::endl;

	return 0;
}

*/