#ifndef NE_COLOUR_H
#define NE_COLOUR_H

///////////////////////////////////////////////////////////////////////////
// INCLUDES
///////////////////////////////////////////////////////////////////////////

#include "math/ne_math.h"

///////////////////////////////////////////////////////////////////////////
// NE_COLOUR
///////////////////////////////////////////////////////////////////////////
struct neColour
{
    u8  B, G, R, A;

	neColour( void );
	neColour( const u32 K );
	neColour( const neV3& C );
	neColour( const neV4& C );
	neColour( u8 r, u8 g, u8 b, u8 a );
    Set   ( u8 r, u8 g, u8 b, u8 a );


    // Access the color in diferent forms

    unsigned char&      operator[]          ( int Index );
    operator const      u32                 ();//{return *((u32*)this);};
    const neColour&       operator =          ( u32 C );

    void                SetFromRGB          ( neV3& Vector );
    void                SetFromRGB          ( f32  aR, f32  aG, f32  aB );
    void                BuildRGBA           ( f32& aR, f32& aG, f32& aB, f32& aA ) const;
    void                SetFromRGBA         ( f32  aR, f32  aG, f32  aB, f32  aA );
};


#include "ne_colour_inline.h"
#endif