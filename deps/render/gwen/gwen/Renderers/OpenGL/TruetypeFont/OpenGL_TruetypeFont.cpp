#include <GL/glew.h>

#include "Gwen/Renderers/OpenGL_TruetypeFont.h"
#include "Gwen/Utility.h"
#include "Gwen/Font.h"
#include "Gwen/Texture.h"

#include <math.h>

#include <sstream>

#include "stb_truetype.h" // portable truetype library
//#include "stb_truetype.c"

#include "fontstash.hpp"  // opengl renderer for stb_truetype
//#include "fontstash.cpp"

#include "font.hpp"       // abstraction for fontstash
//#include "font.cpp"

namespace Gwen
{
	namespace Renderer
	{
		OpenGL_TruetypeFont::OpenGL_TruetypeFont()
		{}

		OpenGL_TruetypeFont::~OpenGL_TruetypeFont()
		{}

		namespace
		{
			float fontZoomFactor = 1.0f, fontOffsetY = 2, fontHeight = 0; //1,2 -> ok si size = 11 ||

			int getfaceid( const std::string &facename, float pt, float scale )
			{
				static std::map< std::string, int > faces;

				if( faces.find( facename ) == faces.end() )
				{
					int face = faces.size() + 1;
					bool ok = font::make::style( face, facename, pt * scale );
					faces[ facename ] = ok ? face : 1;
				}
				int face = faces[ facename ];

				return face;
			}
		}

		void OpenGL_TruetypeFont::RenderText( Gwen::Font* pFont, Gwen::Point pos, const Gwen::UnicodeString& text )
		{
			if ( !text.length() )
				return;

			float fScale = Scale() * fontZoomFactor;
			int face = getfaceid( std::string( pFont->facename.begin(), pFont->facename.end() ), pFont->size, fScale );

			Gwen::Rect r( pos.x, pos.y );
			Translate( r );

			std::string ctext( text.begin(), text.end() );
//			std::string ctext = Gwen::Utility::UnicodeToString( text );

			font::color( m_Color.r, m_Color.g, m_Color.b, m_Color.a );
			font::batch( ctext, r.x, r.y + fontOffsetY - fontHeight * 0.5f, face );
			font::submit();
		}

		Gwen::Point OpenGL_TruetypeFont::MeasureText( Gwen::Font* pFont, const Gwen::UnicodeString& text )
		{
			float fScale = Scale() * fontZoomFactor;
			int face = getfaceid( std::string( pFont->facename.begin(), pFont->facename.end() ), pFont->size, fScale );

			std::string ctext( text.begin(), text.end() );
//			std::string ctext = Gwen::Utility::UnicodeToString( text );

			if( ctext == " " ) // Gwen uses a space to retrieve font height, but stb_truetype seems to
				ctext = "|";   // return a null height, so we use a '|' char instead.

			auto r = font::make::rect( ctext, face );

			if( ctext == "|" )
				fontHeight = r.h;

			return Gwen::Point( r.w, r.h + fontOffsetY * 2 );
		}

	}
}
