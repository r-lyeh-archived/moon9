// notice we do not need to include <memtracer.hpp> in order to catch memory leaks! :)

void *make_a_leak()
{
	return new int [400];
}

int main( int argc, char **argv )
{
	make_a_leak();

	new int(); // make another leak

	return 0;
}
