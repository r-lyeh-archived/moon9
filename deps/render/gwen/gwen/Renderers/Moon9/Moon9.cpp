#include <GL/glew.h>

#include "Gwen/Renderers/Moon9.h"
#include "Gwen/Utility.h"
#include "Gwen/Font.h"
#include "Gwen/Texture.h"
#include "Gwen/WindowProvider.h"

#include <math.h>
#include <sstream>

#include <moon9/render/font.hpp>
//#include <moon9/render/image.hpp>

#include "stb/stb_image.h"

namespace Gwen
{
	namespace Renderer
	{
		Moon9::Moon9()
		{
			m_iVertNum = 0;
			m_pContext = NULL;


			for ( int i=0; i<MaxVerts; i++ )
			{
				m_Vertices[ i ].z = 0.5f;
			}
		}

		Moon9::~Moon9()
		{
		}

		void Moon9::Init()
		{

		}

		void Moon9::Begin()
		{
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			glAlphaFunc( GL_GREATER, 1.0f );
			glEnable ( GL_BLEND );
		}

		void Moon9::End()
		{
			Flush();
		}

		void Moon9::Flush()
		{
			if ( m_iVertNum == 0 ) return;

			glVertexPointer( 3, GL_FLOAT,  sizeof(Vertex), (void*) &m_Vertices[0].x );
			glEnableClientState( GL_VERTEX_ARRAY );

			glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)&m_Vertices[0].r );
			glEnableClientState( GL_COLOR_ARRAY );

			glTexCoordPointer( 2, GL_FLOAT, sizeof(Vertex), (void*) &m_Vertices[0].u );
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );

			glDrawArrays( GL_TRIANGLES, 0, (GLsizei) m_iVertNum );

			glDisableClientState( GL_TEXTURE_COORD_ARRAY ); // @rlyeh: added these to prevent potential visual artifacts
			glDisableClientState( GL_COLOR_ARRAY );
			glDisableClientState( GL_VERTEX_ARRAY );

			m_iVertNum = 0;
			glFlush();
		}

		void Moon9::AddVert( int x, int y, float u, float v )
		{
			if ( m_iVertNum >= MaxVerts-1 )
			{
				Flush();
			}

			m_Vertices[ m_iVertNum ].x = (float)x;
			m_Vertices[ m_iVertNum ].y = (float)y;
			m_Vertices[ m_iVertNum ].u = u;
			m_Vertices[ m_iVertNum ].v = v;

			m_Vertices[ m_iVertNum ].r = m_Color.r;
			m_Vertices[ m_iVertNum ].g = m_Color.g;
			m_Vertices[ m_iVertNum ].b = m_Color.b;
			m_Vertices[ m_iVertNum ].a = m_Color.a;

			m_iVertNum++;
		}

		void Moon9::DrawFilledRect( Gwen::Rect rect )
		{
			GLboolean texturesOn;

			glGetBooleanv(GL_TEXTURE_2D, &texturesOn);
			if ( texturesOn )
			{
				Flush();
				glDisable(GL_TEXTURE_2D);
			}

			Translate( rect );

			AddVert( rect.x, rect.y );
			AddVert( rect.x+rect.w, rect.y );
			AddVert( rect.x, rect.y + rect.h );

			AddVert( rect.x+rect.w, rect.y );
			AddVert( rect.x+rect.w, rect.y+rect.h );
			AddVert( rect.x, rect.y + rect.h );
		}

		void Moon9::SetDrawColor(Gwen::Color color)
		{
			glColor4ubv( (GLubyte*)&color );
			m_Color = color;
		}

		void Moon9::StartClip()
		{
			Flush();
			Gwen::Rect rect = ClipRegion();

			// OpenGL's coords are from the bottom left
			// so we need to translate them here.
			{
				GLint view[4];
				glGetIntegerv( GL_VIEWPORT, &view[0] );
				rect.y = view[3] - (rect.y + rect.h);
			}

			glScissor( rect.x * Scale(), rect.y * Scale(), rect.w * Scale(), rect.h * Scale() );
			glEnable( GL_SCISSOR_TEST );
		};

		void Moon9::EndClip()
		{
			Flush();
			glDisable( GL_SCISSOR_TEST );

		};

		void Moon9::DrawTexturedRect( Gwen::Texture* pTexture, Gwen::Rect rect, float u1, float v1, float u2, float v2 )
		{
			GLuint* tex = (GLuint*)pTexture->data;

			// Missing image, not loaded properly?
			if ( !tex )
			{
				return DrawMissingImage( rect );
			}

			Translate( rect );
			GLuint boundtex;

			GLboolean texturesOn;
			glGetBooleanv(GL_TEXTURE_2D, &texturesOn);
			glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&boundtex);
			if ( !texturesOn || *tex != boundtex )
			{
				Flush();
				glBindTexture( GL_TEXTURE_2D, *tex );
				glEnable(GL_TEXTURE_2D);
			}

			AddVert( rect.x, rect.y,			u1, v1 );
			AddVert( rect.x+rect.w, rect.y,		u2, v1 );
			AddVert( rect.x, rect.y + rect.h,	u1, v2 );

			AddVert( rect.x+rect.w, rect.y,		u2, v1 );
			AddVert( rect.x+rect.w, rect.y+rect.h, u2, v2 );
			AddVert( rect.x, rect.y + rect.h, u1, v2 );
		}

		void Moon9::LoadTexture( Gwen::Texture* pTexture ) // @rlyeh: using stb_image now
		{
			std::string sFileName = Gwen::Utility::UnicodeToString( pTexture->name.GetUnicode() ).c_str();

			int width, height, depth;
			unsigned char *data = stbi_load( sFileName.c_str(), &width, &height, &depth, 4 );

			if ( !data )
			{
				pTexture->failed = true;
				return;
			}

			// Create a little texture pointer..
			GLuint* pglTexture = new GLuint;

			// Sort out our GWEN texture
			pTexture->data = pglTexture;
			pTexture->width = width;
			pTexture->height = height;

			// Create the opengl texture
			glGenTextures( 1, pglTexture );
			glBindTexture( GL_TEXTURE_2D, *pglTexture );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

			GLenum format = GL_RGBA;

			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, pTexture->width, pTexture->height, 0, format, GL_UNSIGNED_BYTE, (const GLvoid*)data );

			stbi_image_free( data );
		}

		void Moon9::FreeTexture( Gwen::Texture* pTexture )
		{
			GLuint* tex = (GLuint*)pTexture->data;
			if ( !tex ) return;

			glDeleteTextures( 1, tex );
			delete tex;
			pTexture->data = NULL;
		}

		Gwen::Color Moon9::PixelColour( Gwen::Texture* pTexture, unsigned int x, unsigned int y, const Gwen::Color& col_default )
		{
			GLuint* tex = (GLuint*)pTexture->data;
			if ( !tex ) return col_default;

			unsigned int iPixelSize = sizeof(unsigned char) * 4;

			glBindTexture( GL_TEXTURE_2D, *tex );

			unsigned char* data = (unsigned char*) malloc( iPixelSize * pTexture->width * pTexture->height );

				glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

				unsigned int iOffset = (y * pTexture->width + x) * 4;

				Gwen::Color c;
				c.r = data[0 + iOffset];
				c.g = data[1 + iOffset];
				c.b = data[2 + iOffset];
				c.a = data[3 + iOffset];

			//
			// Retrieving the entire texture for a single pixel read
			// is kind of a waste - maybe cache this pointer in the texture
			// data and then release later on? It's never called during runtime
			// - only during initialization.
			//
			free( data );

			return c;
		}

		bool Moon9::InitializeContext( Gwen::WindowProvider* pWindow )
		{
			#ifdef _WIN32

				HWND pHwnd = (HWND) pWindow->GetWindow();
				if ( !pHwnd ) return false;

				HDC hDC = GetDC( pHwnd );

				//
				// Set the pixel format
				//
				PIXELFORMATDESCRIPTOR pfd;
					memset( &pfd, 0, sizeof( pfd ) );
					pfd.nSize = sizeof( pfd );
					pfd.nVersion = 1;
					pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
					pfd.iPixelType = PFD_TYPE_RGBA;
					pfd.cColorBits = 24;
					pfd.cDepthBits = 32;
					pfd.iLayerType = PFD_MAIN_PLANE;
					int iFormat = ChoosePixelFormat( hDC, &pfd );
					SetPixelFormat( hDC, iFormat, &pfd );

				HGLRC hRC;
				hRC = wglCreateContext( hDC );
				wglMakeCurrent( hDC, hRC );

				RECT r;
				if ( GetClientRect( pHwnd, &r ) )
				{
					glMatrixMode( GL_PROJECTION );
					glLoadIdentity();
					glOrtho( r.left, r.right, r.bottom, r.top, -1.0, 1.0);
					glMatrixMode( GL_MODELVIEW );
					glViewport(0, 0, r.right - r.left, r.bottom - r.top);
				}

				m_pContext = (void*) hRC;

				return true;
			#endif

			return false;
		}

		bool Moon9::ShutdownContext( Gwen::WindowProvider* pWindow )
		{
			#ifdef _WIN32
				wglDeleteContext( (HGLRC)m_pContext );
				return true;
			#endif

			return false;
		}

		bool Moon9::PresentContext( Gwen::WindowProvider* pWindow )
		{
			#ifdef _WIN32
				HWND pHwnd = (HWND) pWindow->GetWindow();
				if ( !pHwnd ) return false;

				HDC hDC = GetDC( pHwnd );
				SwapBuffers( hDC );
				return true;
			#endif

			return false;
		}

		bool Moon9::ResizedContext( Gwen::WindowProvider* pWindow, int w, int h )
		{
			#ifdef _WIN32

				RECT r;
				if ( GetClientRect( (HWND)pWindow->GetWindow(), &r ) )
				{
					glMatrixMode( GL_PROJECTION );
					glLoadIdentity();
					glOrtho( r.left, r.right, r.bottom, r.top, -1.0, 1.0 );
					glMatrixMode( GL_MODELVIEW );
					glViewport( 0, 0, r.right - r.left, r.bottom - r.top );
				}

				return true;
			#endif

			return false;
		}

		bool Moon9::BeginContext( Gwen::WindowProvider* pWindow )
		{
			glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			return true;
		}

		bool Moon9::EndContext( Gwen::WindowProvider* pWindow )
		{
			return true;
		}


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

		void Moon9::RenderText( Gwen::Font* pFont, Gwen::Point pos, const Gwen::UnicodeString& text )
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

		Gwen::Point Moon9::MeasureText( Gwen::Font* pFont, const Gwen::UnicodeString& text )
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
