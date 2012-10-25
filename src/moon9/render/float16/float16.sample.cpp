#include <moon9/render/float16.hpp>
#include <iostream>
#include <cassert>

void check_precision_error( float value )
{
    float original = value;
    float converted1 = half2float(float2half(value));

    if( std::abs(original-converted1) > 0 )
        std::cout << "Precision warning: " << value << " (" << (original-converted1 > 0 ? "+" : "") << (original-converted1) << " err)" << std::endl;
    else ;// value ok;

    float converted2 = float16(value);
    assert( converted1 == converted2 );
}

int main()
{
    assert( sizeof(float16) == 2 );
    assert( float16() == 0.f );

    for( int i = -2048; i <= 2048; ++i )
        check_precision_error( i );

    check_precision_error(  3.14159f );
    check_precision_error( -3.14159f );
    check_precision_error(  0.00001f );
    check_precision_error( -0.00001f );
    check_precision_error(  0.00000f );
    check_precision_error( -0.00000f );
    check_precision_error(  1.00000f );
    check_precision_error( -1.00000f );

    float16 f(3.1459f);
    check_precision_error( f += 1.f );
    check_precision_error( f  = 4.f );
    check_precision_error( f  = float16() );
    check_precision_error( f *= 0.0001f );

    std::cout << "All ok." << std::endl;

    return 0;
}
