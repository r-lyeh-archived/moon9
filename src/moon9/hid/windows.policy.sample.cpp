#include <cassert>
#include <string>
#include <iostream>

#include "windows.hpp"

int main( int argc, char **argv )
{
    int passed = 0;

    try
    {
        // following lines are ok
        moon9::windows::gamepad gamepad1(0);
        moon9::windows::gamepad gamepad2(1);
        moon9::windows::gamepad gamepad3(2);
        moon9::windows::gamepad gamepad4(3);
        // following line is wrong: moon9::windows::gamepad driver supports only 4 instances at once
        moon9::windows::gamepad gamepad5(4);

        // following line is unreachable
        ;
    }
    catch( std::string &error )
    {
        std::cerr << "hey developer! " << error << std::endl;
        passed++;
    }

    try
    {
        // following lines are ok
        moon9::windows::gamepad gamepad1(0);
        moon9::windows::gamepad gamepad2(1);
        moon9::windows::gamepad gamepad3(2);
        moon9::windows::gamepad gamepad4(3);
        // following line is wrong: device id #0 is already being used
        moon9::windows::gamepad gamepad5(0);

        // following line is unreachable
        ;
    }
    catch( std::string &error )
    {
        std::cerr << "hey developer! " << error << std::endl;
        passed++;
    }

    try
    {
        // following lines are ok
        moon9::windows::mouse mouse1(0);
        // following line is wrong: moon9::windows::mouse driver supports only 1 instance at once
        moon9::windows::mouse mouse2(1);

        // following line is unreachable
        ;
    }
    catch( std::string &error )
    {
        std::cerr << "hey developer! " << error << std::endl;
        passed++;
    }

    try
    {
        // following lines are ok
        moon9::windows::gamepad mouse1(0);
        // following line is wrong: device id #0 is already being used
        moon9::windows::gamepad mouse2(0);

        // following line is unreachable
        ;
    }
    catch( std::string &error )
    {
        std::cerr << "hey developer! " << error << std::endl;
        passed++;
    }

    try
    {
        // following line is ok
        moon9::windows::keyboard keyboard1(0);
        // following line is wrong: moon9::windows::keyboard driver supports only 1 instance at once
        moon9::windows::keyboard keyboard2(1);

        // following line is unreachable
        ;
    }
    catch( std::string &error )
    {
        std::cerr << "hey developer! " << error << std::endl;
        passed++;
    }

    try
    {
        // following line is ok
        moon9::windows::keyboard keyboard1(0);
        // following line is wrong: device id #0 is already in use
        moon9::windows::keyboard keyboard2(0);

        // following line is unreachable
        ;
    }
    catch( std::string &error )
    {
        std::cerr << "hey developer! " << error << std::endl;
        passed++;
    }

    assert( passed == 6 );

    return 0;
}

