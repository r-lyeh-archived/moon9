#pragma once

#include <string>

namespace app
{
    std::string get_name();

    std::string homedir(); // home path
    std::string workdir(); // temp path
    bool makedir( const std::string &fullpath );

    void attach_console();
    void detach_console();
    void title_console( const std::string &title );

    bool is_another_instance_running();
    bool is_running();
    bool is_closing();

    void sleep( double seconds );

    void spawn( const std::string &appname );
    void close();
    void respawn();

    void restart();
}
