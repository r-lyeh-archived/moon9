#include <cassert>
#include <string>

#include "errorbox.hpp"
#include "debugger.hpp"
#include "die.hpp"

namespace
{
    void default_assert_cb( const std::string &assertion, const char *at_file, int at_line )
    {
        errorbox( assertion );

        if( !debugger() )
            die( -1 );
    }
}

namespace cb
{
    void (*custom_assert_cb)( const std::string &assertion, const char *at_file, int at_line ) = default_assert_cb;
}

void setup_assert( void (*custom_assert_cb)( const std::string &assertion, const char *at_file, int at_line ) )
{
    cb::custom_assert_cb = custom_assert_cb ? custom_assert_cb : default_assert_cb;
}

bool is_asserting()
{
    static bool enabled = ( enabled = false, assert( enabled ^= true ), enabled );
    return enabled;
    /*
    static struct once { bool are_enabled; once() : are_enabled(false) {
        assert( are_enabled ^= true );
    } } asserts;
    return asserts.are_enabled;
    */
}
