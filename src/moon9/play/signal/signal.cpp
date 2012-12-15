#include <iostream>
#include <condition_variable>
#include <thread>
#include <chrono>

#include <map>
#include <string>
#include <sstream>

#include <stdio.h>

namespace
{
    extern volatile bool closing;

    struct coroutine
    {
        bool on;
        bool owner;
        std::condition_variable *cv;
        std::mutex *cvm;

        coroutine() : on(false), owner(true)
        {
            cv = new std::condition_variable;
            cvm = new std::mutex;
        }

        ~coroutine()
        {
            if( !owner ) return;
            if( cv ) delete cv, cv = 0;
            if( cvm ) delete cvm, cvm = 0;
        }

        coroutine( coroutine &s )
        {
            cv = s.cv;
            cvm = s.cvm;
            owner = true;

            s.owner = false;
        }

        void wait()
        {
            on = false;
            std::unique_lock<std::mutex> lk(*cvm);
            cv->wait(lk, [=](){ return closing || !cvm || !cv || on == true; } );
        }

        void signal1()
        {
            on = true;
            cv->notify_one();
        }

        void signal()
        {
            on = true;
            cv->notify_all();
        }
    };

    std::map< std::string, coroutine > signals;

    void notify_exit()
    {
        closing = true;

        for( auto &it : signals )
        {
            /*
            std::stringstream ss;
            if( ss << "<moon9/play/signal.hpp> says: aborting coroutine '" << it.first << "'" << std::endl )
                fprintf( stdout, "%s", ss.str().c_str() );
            */
            it.second.signal();
        }
    }

    bool volatile closing = ( atexit(notify_exit), false );
}

namespace moon9
{
    void wait( const int &t )
    {
        std::stringstream ss;
        if( ss << t )
            signals[ ss.str() ].wait();
    }
    void wait( const size_t &t )
    {
        std::stringstream ss;
        if( ss << t )
            signals[ ss.str() ].wait();
    }
    void wait( const std::string &t )
    {
        std::stringstream ss;
        if( ss << t )
            signals[ ss.str() ].wait();
    }

    void signal1( const int &t )
    {
        std::stringstream ss;
        if( ss << t )
            signals[ ss.str() ].signal1();
    }
    void signal1( const size_t &t )
    {
        std::stringstream ss;
        if( ss << t )
            signals[ ss.str() ].signal1();
    }
    void signal1( const std::string &t )
    {
        std::stringstream ss;
        if( ss << t )
            signals[ ss.str() ].signal1();
    }

    void signal( const int &t )
    {
        std::stringstream ss;
        if( ss << t )
            signals[ ss.str() ].signal();
    }
    void signal( const size_t &t )
    {
        std::stringstream ss;
        if( ss << t )
            signals[ ss.str() ].signal();
    }
    void signal( const std::string &t )
    {
        std::stringstream ss;
        if( ss << t )
            signals[ ss.str() ].signal();
    }

    void signals()
    {
        closing = true;

        for( auto &it : ::signals )
            it.second.signal();

        closing = false;
    }
}

