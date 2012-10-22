#include <string>

#ifdef _WIN32

#include <Windows.h>
#include <Shellapi.h>
#pragma comment(lib,"shell32.lib")

void browse( const std::string &url )
{
    ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

#endif

