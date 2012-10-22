// Simple clipboard functions
// - rlyeh

#include <string>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib,"user32.lib")

namespace moon9
{
    std::string get_clipboard()
    {
        std::string result;
        if ( IsClipboardFormatAvailable(CF_TEXT) != 0 && OpenClipboard(0) != 0 ) {
            HGLOBAL hglb = GetClipboardData(CF_TEXT);
            if (hglb != NULL)
            {
                LPSTR lpstr = (LPSTR)GlobalLock(hglb);
                if (lpstr != NULL)
                {
                    result = lpstr;
                    GlobalUnlock(hglb);
                }
            }
            CloseClipboard();
        }
        return result;
    }

    void set_clipboard( const std::string &_msg )
    {
        // try to copy error to clipboard
        if( OpenClipboard(0) )
        {
            HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, _msg.size()+1 );
            if( hglbCopy )
            {
                // Lock the handle and copy the text to the buffer.

                LPSTR lpstrCopy = (LPSTR)GlobalLock(hglbCopy);
                if( lpstrCopy )
                {
                    strcpy( lpstrCopy, _msg.c_str() );
                    GlobalUnlock(hglbCopy);

                    EmptyClipboard();
                    SetClipboardData( CF_TEXT, hglbCopy );
                }
            }

            CloseClipboard();
        }
    }
}
#endif
