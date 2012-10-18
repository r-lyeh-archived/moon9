/*
   Copyright (c) 2002 Tokamak Limited

   Date: 2002/10/27 12:00:00 

   This software, the Tokamak Game Physics SDK and the documentaion
   has been developed by Tokamak Ltd (Tokamak) and the copyright 
   and all other intellectual property rights in this software 
   belong to Tokamak. 
   
   All rights conferred by law (including rights under international
   copyright conventions) are reserved to Tokamak. This software
   may also incorporate information which is confidential to
   Tokamak.

   Without the permission by Tokamak, this software and documentaion
   must not be copied (in whole or in part), re-arranged, altered or 
   adapted in any way. In addition, the information contained in the 
   software may not be disseminated without the prior written consent
   of Tokamak.
*/

#ifndef NE_COLOUR_INLINE_H
#define NE_COLOUR_INLINE_H

///////////////////////////////////////////////////////////////////////////
// FUNCTIONS
///////////////////////////////////////////////////////////////////////////

//=========================================================================

inline neColour::neColour( void )
{

}

//=========================================================================

inline neColour::neColour( const u32 K )
{ 
    *this = K; 
}

//=========================================================================

inline neColour::neColour( u8 r, u8 g, u8 b, u8 a )
{
    A = a;
    R = r;
    G = g;
    B = b;
}

//=========================================================================

inline neColour::neColour( const neV3& C )
{
    SetFromRGBA( C.n.X, C.n.Y, C.n.Z, 1 );
}

//=========================================================================

inline neColour::neColour( const neV4& C )
{
    SetFromRGBA( C.X, C.Y, C.Z, C.W );
}

//=========================================================================

inline neColour::Set( u8 r, u8 g, u8 b, u8 a )
{
    A = a;
    R = r;
    G = g;
    B = b;
}

//=========================================================================

inline unsigned char& neColour::operator[]( int Index )
{
    ASSERT( Index >=  0 );
    ASSERT( Index <=  3 );
    return ((unsigned char*)this)[ Index ];
}

//=========================================================================

inline neColour::operator const u32()
{
    return *((u32*)this);
}

//=========================================================================

inline const neColour& neColour::operator = ( u32 C )
{
    A = (u8)((C>>24)&0xff);
    R = (u8)((C>>16)&0xff);
    G = (u8)((C>>8 )&0xff);
    B = (u8)((C>>0 )&0xff);

    return *this;
}

//=========================================================================

inline void neColour::BuildRGBA( f32& aR, f32& aG, f32& aB, f32& aA  ) const
{
    aR = R*(1/255.0f);   
    aG = G*(1/255.0f);   
    aB = B*(1/255.0f);   
    aA = A*(1/255.0f);   
}
/*
//=========================================================================

inline neV3 neColour::BuildRGB( void ) const
{
	neV3 tmp;
    return tmp.Set( R*(1/255.0f), G*(1/255.0f), B*(1/255.0f) );
}
*/
//=========================================================================

inline void neColour::SetFromRGB( neV3& Vector )
{
    SetFromRGB( Vector.n.X, Vector.n.Y, Vector.n.Z );
}


//=========================================================================

inline void neColour::SetFromRGBA( f32 aR, f32 aG, f32 aB, f32 aA )
{
    float r,g,b,a;

    r = aR * 255.0f;
    g = aG * 255.0f;
    b = aB * 255.0f;
    a = aA * 255.0f;    

    R = (u8)neMin( 255, r );
    G = (u8)neMin( 255, g );
    B = (u8)neMin( 255, b );
    A = (u8)neMin( 255, a );
}

///////////////////////////////////////////////////////////////////////////
// END
///////////////////////////////////////////////////////////////////////////
#endif