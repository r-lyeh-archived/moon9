#ifdef _WIN32
#	include <Windows.h>
#	pragma comment( lib, "user32.lib" )
#endif

#include <cstdlib>

#include <string>
#include <sstream>

#include "alert.inl"

namespace
{
	void show( const moon9::custom &text = std::string(), const std::string &title = std::string() )
	{
#ifdef _WIN32
		MessageBoxA( 0, text.c_str(), title.size() ? title.c_str() : "", 0 | MB_SYSTEMMODAL );
#endif
#ifdef __linux__
        std::string cmd = std::string("zenity --information --text \"") + text + std::string("\" --title=\") + title + "\"";
		std::system( cmd.c_str() );
#endif
	}
}

void alert(                                                   ) { show(); }
void alert( const        void *addr, const std::string &title ) { show( addr, title ); }
void alert( const std::string &text, const std::string &title ) { show( text, title ); }
void alert( const      size_t &text, const std::string &title ) { show( text, title ); }
void alert( const      double &text, const std::string &title ) { show( text, title ); }
void alert( const       float &text, const std::string &title ) { show( text, title ); }
void alert( const         int &text, const std::string &title ) { show( text, title ); }
void alert( const        char &text, const std::string &title ) { show( text, title ); }
void alert( const        bool &text, const std::string &title ) { show( text, title ); }
