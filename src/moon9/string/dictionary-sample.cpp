#include "dictionary.hpp"

// example

int main( int argc, char **argv )
{
	// Create ready-to-use dictionary.

		moon9::dictionary dict;

	// 1) simple dictionary usage

		dict.set_symbol("PLAYER_1", "Mark");
		dict.set_symbol("PLAYER_2", "John");

		// any existing [symbol] is replaced
		// undefined symbols are just quoted

		std::cout << dict.translate("[PLAYER_1] joined the game") << std::endl;
		// -> "Mark joined the game"
		std::cout << dict.translate("[PLAYER_2] joined the game") << std::endl;
		// -> "John joined the game"
		std::cout << dict.translate("[PLAYER_3] joined the game") << std::endl;
		// -> "PLAYER_3 joined the game"

	// 2) composition/chaining is supported thru recursive lookups

		dict.set_symbol("HEY", "Hey there!");
		dict.set_symbol("GREETING", "[HEY] How are you?");

		std::cout << dict.translate("[GREETING]") << std::endl;
		// -> "Hey there! How are you?"

	// 3) symbol hot swapping is supported

		// hot swap [HEY] symbol
		dict.set_symbol("HEY", "Hey [PLAYER_1] and [PLAYER_2]!");

		std::cout << dict.translate("[GREETING]") << std::endl;
		// -> "Hey Mark and John! How are you?"

	// 4) arguments are supported as well: \1, \2, \3 ... \7

		dict.set_symbol("MONEYx2", "You just won \1 \2!");
		std::cout << dict.translate("[MONEYx2]", 1337, "dollars") << std::endl;
		// -> "You just won 1337 dollars"

		dict.set_symbol("THRICEx1", "\1\1\1");
		std::cout << dict.translate("[THRICEx1]", "Help!") << std::endl;
		// -> "Help!Help!Help!"

	return 0;
}
