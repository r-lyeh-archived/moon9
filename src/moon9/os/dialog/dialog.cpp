// save ref: http://msdn.microsoft.com/en-us/library/ms646928(v=vs.85).aspx

#include <string>

#if _WIN32

#include <Windows.h>
#include <Commdlg.h>
#pragma comment(lib,"Comdlg32.lib")

namespace moon9
{
    namespace windows
    {
        std::string load_dialog( const char *filter = "All Files (*.*)\0*.*\0", HWND owner = 0 )
        {
            OPENFILENAMEA ofn;
            char fileName[ MAX_PATH ] = "";
            ZeroMemory( &ofn, sizeof(ofn) );

            ofn.lStructSize = sizeof( OPENFILENAMEA );
            ofn.hwndOwner = owner;
            ofn.lpstrFilter = filter;
            ofn.lpstrFile = fileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
            ofn.lpstrDefExt = ""; // optional

            return GetOpenFileNameA( &ofn ) == TRUE ? fileName : std::string();
        }

        std::string save_dialog( const char *filter = "All Files (*.*)\0*.*\0", HWND owner = 0 )
        {
            OPENFILENAMEA ofn;
            char fileName[ MAX_PATH ] = "";
            ZeroMemory( &ofn, sizeof(ofn) );

            ofn.lStructSize = sizeof( OPENFILENAMEA );
            ofn.hwndOwner = owner;
            ofn.lpstrFilter = filter;
            ofn.lpstrFile = fileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
            ofn.lpstrDefExt = ""; // optional

            return GetSaveFileNameA( &ofn ) == TRUE ? fileName : std::string();
        }
    }

    // Both functions return an empty string if dialog is canceled

    std::string load_dialog( const char *filter )
    {
        return windows::load_dialog( filter, 0 );
    }

    std::string save_dialog( const char *filter )
    {
        return windows::save_dialog( filter, 0 );
    }
}

#endif
