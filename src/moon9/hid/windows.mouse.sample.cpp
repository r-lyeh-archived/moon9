#include <iostream>

#include "windows.hpp"

int main( int argc, char **argv )
{
    moon9::windows::mouse mouse(0);

    while( true )
    {
        mouse.update();

        mouse.clipped.set( mouse.right.hold() );
        mouse.hidden.set( mouse.left.hold() && mouse.right.hold() );

        std::cout

            << "mouse[0]="

            << "left["    << (mouse.left.hold()	   ? "x]" : " ]")
            << "middle["  << (mouse.middle.hold()  ? "x]" : " ]")
            << "right["   << (mouse.right.hold()   ? "x]" : " ]")
            << "hover["   << (mouse.hover.hold()   ? "x]" : " ]")
            << "hidden["  << (mouse.hidden.hold()  ? "x]" : " ]")
            << "clipped[" << (mouse.clipped.hold() ? "x]" : " ]")

            << " @global(" << mouse.global.newest().x << ',' << mouse.global.newest().y << ")"
            << " @local("  << mouse.local.newest().x  << ',' << mouse.local.newest().y  << ")"
            << " @wheel("  << mouse.wheel.newest().y << ")"
            << "               \r";
    }
}

