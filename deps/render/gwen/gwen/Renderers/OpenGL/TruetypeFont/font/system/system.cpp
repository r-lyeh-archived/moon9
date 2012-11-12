// Based on code by Cred King [ref] http://code.google.com/p/gdipp/source/browse/gdipp_server/font_link.cpp
// - rlyeh

#ifdef _WIN32

#include <cassert>

#include <string>
#include <iostream>
#include <map>

#include <windows.h>
#include <shlobj.h>

#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "Shell32.lib")


std::string get_system_fonts_path()
{
    std::string out;

    HRESULT hr;
    PWSTR pszPath = NULL;

    hr = SHGetKnownFolderPath(FOLDERID_Fonts, 0, NULL, &pszPath); // supported since Windows Vista
    if( SUCCEEDED( hr ) )
    {
        std::wstring ws( pszPath );

        out = std::string(ws.begin(), ws.end());

        CoTaskMemFree(pszPath);
    }

    return out;
}

std::map< std::string, std::string > get_system_fonts()
{
    enum { MAX_VALUE_NAME = 256 };

    std::string fonts_folder = get_system_fonts_path() + "\\";
    std::map< std::string, std::string > out;

    // read font linking information from registry, and store in std::map

    LONG ret;

    const char *Fonts = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";

    HKEY key_ft;
    ret = RegOpenKeyExA(HKEY_LOCAL_MACHINE, Fonts, 0, KEY_QUERY_VALUE, &key_ft);
    if (ret != ERROR_SUCCESS)
            return out;

    DWORD value_count;
    DWORD max_data_len;
    char value_name[MAX_VALUE_NAME];
    BYTE *value_data;

    // get font_file_name -> font_face mapping from the "Fonts" registry key
    ret = RegQueryInfoKeyA(key_ft, NULL, NULL, NULL, NULL, NULL, NULL, &value_count, NULL, &max_data_len, NULL, NULL);
    //assert(ret == ERROR_SUCCESS);

    // no font installed
    if (value_count == 0)
            return out;

    // max_data_len is in BYTE
    value_data = static_cast<BYTE *>(HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, max_data_len));

    if(value_data != NULL)
    {
        for (DWORD i = 0; i < value_count; ++i)
        {
            DWORD name_len = MAX_VALUE_NAME;
            DWORD data_len = max_data_len;

            ret = RegEnumValueA(key_ft, i, value_name, &name_len, NULL, NULL, value_data, &data_len);
            //assert(ret == ERROR_SUCCESS);

            std::string curr_face = value_name;
            std::string font_file = reinterpret_cast<char *>(value_data);
            curr_face = curr_face.substr(0, curr_face.find('(') - 1);
            out[curr_face] = fonts_folder + font_file;
        }

        HeapFree(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, value_data);
    }


    ret = RegCloseKey(key_ft);

    return out;
}

std::string locate_system_font( const std::string &ttfname )
{
    return get_system_fonts()[ ttfname ];
}

#else

#include <string>
#include <map>

std::map< std::string, std::string > get_system_fonts()
{
    return std::map< std::string, std::string >();
}

std::string locate_system_font( const std::string &ttfname )
{
    return std::string();
}

#endif

