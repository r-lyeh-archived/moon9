#include <cstdlib>
#include <chrono>
#include <thread>
#include <string>

#ifdef _WIN32
#   include <Windows.h>
#   include <Shlobj.h>                     // SHGetFolderPath
#   pragma comment(lib, "Shell32.lib")     // SHGetFolderPath
namespace
{
        // http://www.guidgen.com/
        // http://support.microsoft.com/kb/243953/en-us
        // This code is from Q243953 in case you lose the article and wonder where this code came from.
        class limit_single_instance
        {
            protected:

            DWORD  last_error;
            HANDLE mutex;

            public:

            limit_single_instance( const char *guidgen_sz )
            {
                //Make sure that you use a name that is unique for this application otherwise
                //two apps may think they are the same if they are using same name for
                //3rd parm to CreateMutex
                mutex = CreateMutexA(NULL, FALSE, guidgen_sz); //do early
                last_error = GetLastError(); //save for use later...
            }

            ~limit_single_instance()
            {
                if( mutex )
                {
                    CloseHandle(mutex);
                    mutex = NULL;
                }
            }

            bool is_another_instance_running()
            {
                return (ERROR_ALREADY_EXISTS == last_error ? true : false);
            }
        };

        //limit_single_instance obj( "Global\\{83D5A466-6473-4FF1-9544-93FBF84DC225}" );
        limit_single_instance obj( "Global\\{AppName:Moon9Sample}" );
}

#endif



namespace app
{
    namespace
    {
        std::string convert( std::string t, char from, char to )
        {
            for( std::string::iterator it = t.begin(), end = t.end(); it != end; ++it )
            {
                if( *it == from )
                    *it = to;
            }

            return t;
        }
    }

    bool makedir( const std::string &fullpath )
    {
#ifdef _WIN32
        int ret = SHCreateDirectoryExA(0, convert( fullpath, '/', '\\' ).c_str(), 0);
        return ( ret == ERROR_SUCCESS || ret == ERROR_ALREADY_EXISTS );
#else
        //  read/write/search permissions for owner /* and group, and with read/search permissions for others. */
        int ret = mkdir( fullpath.c_str(), S_IRWXU  /*| S_IRWXG | S_IROTH | S_IXOTH */ ) == 0;
        return ( ret == 0 || ret == EEXIST );
#endif
    }

    std::string homedir()
    {
#ifdef _WIN32
        // get user documents path
        char cpath[MAX_PATH];
        SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_DEFAULT, cpath);
        return convert( cpath, '\\', '/' );
#else
        return getenv("HOME");
#endif
    }

    std::string workdir()
    {
#ifdef _WIN32
        return convert( getenv("LOCALAPPDATA"), '\\', '/' );
#else
        return getenv("TEMP");
#endif
    }

    bool is_another_instance_running()
    {
#ifdef _WIN32
        return obj.is_another_instance_running();
#else
        return false;
#endif
    }

    std::string get_name()
    {
#ifdef _WIN32
        HMODULE self = GetModuleHandle(0);
        char filename[512];
        DWORD result = GetModuleFileNameA(self,filename,sizeof(filename));
        return result > 0 ? filename : std::string();
#else
        return std::string();
#endif
    }

    void attach_console()
    {
#ifdef _WIN32
        // these next few lines create and attach a console
        // to this process.  note that each process is only allowed one console.
        AllocConsole() ;
/*
        AttachConsole( GetCurrentProcessId() ) ;
        freopen( "CON", "w", stdout ) ;

        printf("HELLO!!! I AM THE CONSOLE!" ) ;
*/
#else
#endif
    }

    void detach_console()
    {
#ifdef _WIN32
        FreeConsole();
#else
#endif
    }

    void title_console( const std::string &title )
    {
#ifdef _WIN32
        SetConsoleTitleA( title.c_str() );
#else
#endif
    }

    void spawn( const std::string &appname )
    {
        struct spawner
        {
            bool wait;
            std::string name;

            spawner( bool wait_for_execution, const std::string &appname ) : wait( wait_for_execution ), name( appname )
            {}

            void operator()()
            {
                std::chrono::microseconds duration( (int)(0.5 * 1000000) );
                std::this_thread::sleep_for( duration );

                if( wait )
                    std::system( name.c_str() );
                else
                    std::system( ( std::string( "start " ) + name ).c_str() );
            }
        };

        const bool wait_for_execution = false;

        std::thread( spawner(wait_for_execution, appname) ).detach();
    }

    void respawn()
    {
        spawn( get_name() );
    }

    void sleep( double seconds )
    {
    //  std::chrono::milliseconds duration( (int)(seconds * 1000) );
        std::chrono::microseconds duration( (int)(seconds * 1000000) );
    //  std::chrono::nanoseconds  duration( (int)(seconds * 1000000000) );
        std::this_thread::sleep_for( duration );
    }

    namespace
    {
        void preclose();

        volatile bool running = ( std::atexit(preclose), true );

        void preclose()
        {
            running = false;

            app::sleep( 0.5 );
        }
    }

    bool is_running()
    {
        return running;
    }

    bool is_closing()
    {
        return !running;
    }

    void close()
    {
        std::atexit( preclose );
        std::exit(0);
    }

    void restart()
    {
#if 0
        respawn();
        close();
#else
        std::atexit( respawn );
        close();
#endif
    }
}

