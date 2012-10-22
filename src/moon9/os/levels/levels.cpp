#include "clipboard/clipboard.hpp"
#include "environment/environment.hpp"

const bool is_coder   =  ( moon9::get_clipboard() ==  "CODER" || moon9::get_environment("USER") ==  "CODER" );
const bool is_tester  =  ( moon9::get_clipboard() == "TESTER" || moon9::get_environment("USER") == "TESTER" );
const bool is_enduser = !( is_coder || is_tester );
