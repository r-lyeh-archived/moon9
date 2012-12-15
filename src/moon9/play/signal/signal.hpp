// signal/wait coroutines made easy
// -rlyeh

#pragma once

#include <string>

namespace moon9
{

#if 1

    // current api

    // puts a coroutines to sleep until requested signal is triggered
    void wait( const int &id );
    void wait( const size_t &id );
    void wait( const std::string &id );

    // send a global trigger to all waiting coroutines
    void signal( const int &id );
    void signal( const size_t &id );
    void signal( const std::string &id );

    // send a *single* signal to *any* waiting coroutine
    void signal1( const int &id );
    void signal1( const size_t &id );
    void signal1( const std::string &id );

    // send signals to all waiting coroutines
    void signals();

#else

    // proposed api

    // yields a coroutine waiting for a signal
    void wait( const int &id, double timeout_secs = 0 );
    void wait( const size_t &id, double timeout_secs = 0 );
    void wait( const std::string &id, double timeout_secs = 0 );

    // send signal to a waiting coroutine
    void signal( const int &id, double timeout_secs = 0 );
    void signal( const size_t &id, double timeout_secs = 0 );
    void signal( const std::string &id, double timeout_secs = 0 );

    // return number of waiting coroutines
    void waiting( const int &id );
    void waiting( const size_t &id );
    void waiting( const std::string &id );

#endif

}
