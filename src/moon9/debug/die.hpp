#pragma once

#include <string>

void die( const std::string &reason, int errorcode = -1 );
void die( int errorcode = -1 );
