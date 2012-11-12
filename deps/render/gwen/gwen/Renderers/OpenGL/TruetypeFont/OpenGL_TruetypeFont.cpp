#include <GL/glew.h>

#include "Gwen/Renderers/OpenGL_TruetypeFont.h"
#include "Gwen/Utility.h"
#include "Gwen/Font.h"
#include "Gwen/Texture.h"

#include <math.h>

#include <sstream>

#include "font/font.hpp" // moon9::font is an abstraction for fontstash,
#include "font/font.cpp" // which is itself an abstraction for stb_truetype :D

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
			int getfaceid( const std::string &facename, float pt, float scale )
			{
				using namespace moon9;

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

			namespace tweaks
			{
				float zoom = 1.0f, interlining = 2;
			}
		}

		void OpenGL_TruetypeFont::RenderText( Gwen::Font* pFont, Gwen::Point pos, const Gwen::UnicodeString& text )
		{
			using namespace moon9;

			if ( !text.length() )
				return;

			float fScale = Scale() * tweaks::zoom;
			int faceid = getfaceid( std::string( pFont->facename.begin(), pFont->facename.end() ), pFont->size, fScale );

			Gwen::Rect r( pos.x, pos.y );
			Translate( r );

			font::color( m_Color.r, m_Color.g, m_Color.b, m_Color.a );
			font::batch( text, r.x, r.y + tweaks::interlining - font::metrics::height(faceid) * 0.5f, faceid );
			font::submit();
		}

		Gwen::Point OpenGL_TruetypeFont::MeasureText( Gwen::Font* pFont, const Gwen::UnicodeString& text )
		{
			using namespace moon9;

			float fScale = Scale() * tweaks::zoom;
			int faceid = getfaceid( std::string( pFont->facename.begin(), pFont->facename.end() ), pFont->size, fScale );

			// Gwen gets font height by measuring a single space
			if( text == L" " )
				Gwen::Point( 0, font::metrics::height(faceid) );

			auto r = font::make::rect( text, faceid );
			return Gwen::Point( r.w, r.h + tweaks::interlining * 2 ); // 2 = top + bottom
		}

	}
}
