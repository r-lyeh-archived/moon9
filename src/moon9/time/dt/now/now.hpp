#pragma once

namespace moon9
{
    double now();

    // for QA and testing purposes; ie, advance a whole year in runtime and check all now() calls are still working
    void lapse( double t );
}

/*
// proposal

#include <string>
std::string date();
std::string hour();
std::string minute();
etc...
*/
