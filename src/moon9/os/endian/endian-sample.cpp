#include <cassert>

#include <iostream>

#include "endian.hpp"
#include "types/types.hpp"

int main( int argc, char **argv )
{
    moon9::bits16 u16 = 0x1234;
    moon9::bits32 u32 = 0x12345678;
    moon9::bits64 u64 = 0x123456789ABCDEF0;

    moon9::bits16 u16sw = moon9::endian::swap( u16 );
    moon9::bits32 u32sw = moon9::endian::swap( u32 );
    moon9::bits64 u64sw = moon9::endian::swap( u64 );

    std::cout << "Target is " << ( moon9::endian::is_target_little ? "little endian" : "big endian") << std::endl;

    if( const bool do_test_1 = true )
    {
        assert( moon9::endian::swap(u16sw) == u16 );
        assert( moon9::endian::swap(u32sw) == u32 );
        assert( moon9::endian::swap(u64sw) == u64 );
    }

    if( const bool do_test_2 = true )
    {
        std::cout << std::hex;

        std::cout << u16 << " -> " << u16sw << std::endl;
        std::cout << u32 << " -> " << u32sw << std::endl;
        std::cout << u64 << " -> " << u64sw << std::endl;
    }

    if( const bool do_test_3 = true )
    {
        std::cout << "big to native:    " << moon9::endian::big_to_native(u64)    << std::endl;
        std::cout << "little to native: " << moon9::endian::little_to_native(u64) << std::endl;
        std::cout << "native to big:    " << moon9::endian::native_to_big(u64)    << std::endl;
        std::cout << "native to little: " << moon9::endian::native_to_little(u64) << std::endl;
    }

    if( const bool do_test_4 = true )
    {
        // non-pod types wont get reversed
        moon9::endian::swap( std::string() );
    }

    return 0;
}

