/*
	GWEN
	Copyright (c) 2011 Facepunch Studios
	See license in Gwen.h
*/

#ifndef GWEN_RENDERERS_OPENGL_TRUETYPEFONT_H
#define GWEN_RENDERERS_OPENGL_TRUETYPEFONT_H

#include "Gwen/Gwen.h"
#include "Gwen/Renderers/OpenGL.h"

namespace Gwen
{
	namespace Renderer
	{

		class OpenGL_TruetypeFont : public Gwen::Renderer::OpenGL
		{
			public:

				OpenGL_TruetypeFont();
				~OpenGL_TruetypeFont();

				void RenderText( Gwen::Font* pFont, Gwen::Point pos, const Gwen::UnicodeString& text );
				Gwen::Point MeasureText( Gwen::Font* pFont, const Gwen::UnicodeString& text );
		};

	}
}
#endif
