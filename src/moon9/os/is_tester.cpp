#include "clipboard/clipboard.hpp"
#include "environment/environment.hpp"

const bool is_tester  =  ( moon9::get_clipboard() == "TESTER" || moon9::get_environment("USER") == "TESTER" );
