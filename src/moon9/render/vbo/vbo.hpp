#pragma once

#include <moon9/render/os.hpp>
#include <cassert>

namespace moon9
{
	class vbo // mesh
	{
		public:

		GLuint id;
		size_t size;

		vbo() : id(0), size(0)
		{}

		explicit
		vbo( size_t _size_in_bytes ) : id(0), size(0)
		{
			bool ok = resize( _size_in_bytes );
			assert( ok == true );
		}

		void clear()
		{
			if( id )
			{
				glDeleteBuffers( 1, &id );
				id = 0;
				size = 0;
			}
		}

		bool resize( size_t size_in_bytes )
		{
			if( !size_in_bytes )
				return false;

			if( size == size_in_bytes )
				return true;

			clear();

			glGenBuffers( 1, &id );

			bind();
			glBufferData( GL_ARRAY_BUFFER, size = size_in_bytes, 0, GL_STATIC_DRAW );
			unbind();

			return true;
		}

		// access vbo data directly :)
		// [ref] http://en.wikibooks.org/wiki/OpenGL_Programming/Scientific_OpenGL_Tutorial_01#Point_sprites

		void *map()
		{
			return glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );
		}

		void unmap() const
		{
			assert( id > 0 );

			glUnmapBuffer( GL_ARRAY_BUFFER );
		}

		//

		void bind() const
		{
			assert( id > 0 );
			glBindBuffer( GL_ARRAY_BUFFER, id );
		}

		void unbind() const
		{
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
		}

		//

		template<typename T>
		void fill( const T &t )
		{
			resize( sizeof(T) );

			bind();
			*( static_cast<T *>( map() ) ) = t;
			unmap();
			unbind();
		}

		void fill( void *ptr, size_t size )
		{
			resize( size );

			bind();
			memcpy( map(), ptr, size );
			unmap();
			unbind();
		}
	};
}
