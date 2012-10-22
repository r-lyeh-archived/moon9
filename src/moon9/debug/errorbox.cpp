#ifdef _WIN32
#	include <Windows.h>
#	pragma comment( lib, "user32.lib" )
#endif

#include <cstdlib>

#include <string>
#include <sstream>

namespace
{
    class custom : public std::string
    {
        public:

        custom() : std::string()
        {}

        template<typename T>
        custom( const T &t ) : std::string()
        {
            std::stringstream ss;
            ss.precision( 20 );
            if( ss << t )
                this->assign( ss.str() );
        }

        template<>
        custom( const bool &b ) : std::string()
        {
            this->assign( b ? "true" : "false" );
        }
    };

	void show( const custom &text = std::string(), const std::string &title = std::string() )
	{
#ifdef _WIN32
		MessageBoxA( 0, text.c_str(), title.size() ? title.c_str() : "", 0 | MB_ICONERROR | MB_SYSTEMMODAL );
        return;
#endif
#ifdef __linux__
        std::string cmd = std::string("zenity --information --text \"") + text + std::string("\" --title=\") + title + "\"";
		std::system( cmd.c_str() );
        return;
#endif
        fprintf( stderr, "%s", ( title.size() > 0 ? title + ": " + text + "\n" : text + "\n" ).c_str() );
	}
}

void errorbox( const std::string &text, const std::string &title ) { show( text, title ); }
