#ifndef __MOON9_ASSERT_HPP__
#define __MOON9_ASSERT_HPP__

#include <sstream>
#include <string>

namespace cb
{
	extern void (*custom_assert_cb)( const std::string &assertion, const char *at_file, int at_line );
}

void setup_assert( void (*custom_assert_cb)( const std::string &assertion, const char *at_file, int at_line ) );
bool is_asserting();

#endif

#include <cassert>

#ifdef assert1
#   undef assert1
#endif

#ifdef assert2
#   undef assert2
#endif

#ifdef assert3
#   undef assert3
#endif

#ifdef assert4
#   undef assert4
#endif

#if defined(NDEBUG) || defined(_NDEBUG)
#   define assert1(A)                       do {} while(0)
#   define assert2(A,description)           do {} while(0)
#   define assert3(A,op,B)                  do {} while(0)
#   define assert4(A,op,B,description)      do {} while(0)
#else
#   define assert1(A)                       assert2(A,"")
#   define assert2(A,description)           \
    do { auto __a__ = A; if( (__a__) != true ) { \
        std::stringstream ss; ss << "Assertion failed: " \
            << #A << "(" << (__a__) << ") at " << __FILE__ << ":" << __LINE__ \
            << " ; " << description; \
        if( cb::custom_assert_cb ) (*cb::custom_assert_cb)( ss.str(), __FILE__, __LINE__ ); \
        } } while(0)
#   define assert3(A,op,B)                  assert4(A,op,B,"")
#   define assert4(A,op,B,description)      \
    do { auto __a__ = A; auto __b__ = B; if( !(__a__ op __b__) ) { \
        std::stringstream ss; ss << "Assertion failed: " \
            << #A << "(" << (__a__) << ") " << #op << " " << #B << "(" << (__b__) << ") at " << __FILE__ << ':' << __LINE__ \
            << " ; " << description; \
        if( cb::custom_assert_cb ) (*cb::custom_assert_cb)( ss.str(), __FILE__, __LINE__ ); \
        } } while(0)
#endif


/*

usage:

    assert1( a == b );
    assert2( a == b, "a must be equal to b" );
    assert3( a, ==, b );
    assert4( a, ==, b, "a must be equal to b" );

*/
