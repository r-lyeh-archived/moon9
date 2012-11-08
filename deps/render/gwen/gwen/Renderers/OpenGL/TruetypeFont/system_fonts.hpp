#pragma once

#include <string>
#include <map>

std::map< std::string, std::string > get_system_fonts();
std::string find_system_font( const std::string &ttfname );
