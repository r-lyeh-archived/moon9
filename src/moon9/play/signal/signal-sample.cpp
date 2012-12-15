#include <thread>
#include <iostream>
#include <sstream>

#include "signal.hpp"

void print( const std::string &text )
{
    std::stringstream ss;
    ss << std::this_thread::get_id() << ": " << text << std::endl;
    std::cerr << ss.str();
}

void waits()
{
    print("Waiting...");

    moon9::wait("exiting-signal");

    print("Finished waiting.");
}

void signals()
{
    print("Notifying...");

    moon9::signal("exiting-signal");
}

int main()
{
    std::thread t1(waits), t2(waits), t3(waits), t4(signals);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return 0;
}
