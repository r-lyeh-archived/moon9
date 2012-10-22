#include "clipboard/clipboard.hpp"
#include "environment/environment.hpp"

const bool is_coder   =  ( moon9::get_clipboard() ==  "CODER" || moon9::get_environment("USER") ==  "CODER" );
