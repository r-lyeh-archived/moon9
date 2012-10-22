#pragma once

#include <string>

namespace moon9
{
    // Both functions return an empty string if dialog is canceled
	std::string load_dialog( const char *filter = "All Files (*.*)\0*.*\0" );
	std::string save_dialog( const char *filter = "All Files (*.*)\0*.*\0" );
}
