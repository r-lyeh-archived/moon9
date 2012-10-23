#include <cassert>

#include "pack.hpp"
#include "netstring.hpp"
#include "compress.hpp"

int main( int argc, char **argv )
{
    bistring name = bistring( "name", "description" );
    bistring data = bistring( "data", "im a descriptor" );

    bistrings bi;
    bi.push_back( name );
    bi.push_back( data );

    std::cout << bi[0] << std::endl;
    std::cout << bi[1] << std::endl;

    std::cout << pack::json( bi ) << std::endl;
    std::cout << pack::xml( bi ) << std::endl;
    std::cout << pack::netstring( bi ) << std::endl;
    std::cout << pack::zip( bi ) << std::endl;

#define with(p) \
    std::cout << #p "=" << p( bi ).size() << " vs z(" #p ")=" << moon9::z( p( bi ) ).size() << "bytes" << std::endl;

    with( pack::json );
    with( pack::xml );
    with( pack::zip );
    with( pack::netstring );

#undef with

    {
        std::string saved = pack::xml( bi );

        bi[0] = bistring();
        bi[1] = bistring();

        bi = unpack::xml( saved );

        assert( bi[0] == name );
        assert( bi[1] == data );
    }

    {
        std::string saved = pack::netstring( bi );

        bi[0] = bistring();
        bi[1] = bistring();

        bi = unpack::netstring( saved );

        assert( bi[0] == name );
        assert( bi[1] == data );
    }

    {
        std::string saved = pack::zip( bi );

        bi[0] = bistring();
        bi[1] = bistring();

        bi = unpack::zip( saved );

        assert( bi[0] == name );
        assert( bi[1] == data );
    }

    return 0;
}

