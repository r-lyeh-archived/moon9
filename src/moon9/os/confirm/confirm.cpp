#include <string>

#ifdef _WIN32
#   include <Windows.h>
#   pragma comment (lib, "user32.lib")

bool confirm( const std::string &question, const std::string &title )
{
    bool result =
    	( MessageBoxA( HWND_DESKTOP, question.c_str(), title.c_str(), MB_ICONQUESTION | MB_YESNO | MB_SYSTEMMODAL ) == IDYES );

    return result;
}

#endif

#ifdef __linux__
#	include <cstdlib>

bool confirm( const std::string &question, const std::string &title )
{
	std::string str =
		std::string("zenity --question --title=\"") + title + ("\" --text \"") + question + ("\"");

	return system( str.c_str() ) != 0;
}

#endif
