// Vertex/normal/color/texture/edge buffers (pointers) made easy
// - rlyeh ~~ listening to washed out / hold out

// "render.hpp" includes this file

namespace moon9
{
    namespace array
    {
        struct texunit
        {
            static std::vector<size_t> stack;

            explicit
            texunit( size_t unit, size_t texture_id = 0 )
            {
                stack.push_back( unit );

                set( unit );

                glEnable(GL_TEXTURE_2D);

                if( texture_id )
                    glBindTexture(GL_TEXTURE_2D, texture_id );
            }

            ~texunit()
            {
                stack.pop_back();

                if( !stack.size() )
                    return;

                glDisable(GL_TEXTURE_2D);

                set( stack.back() );
            }

            protected:

            void set( size_t unit )
            {
                const int units[] =
                {
                    GL_TEXTURE0_ARB,
                    GL_TEXTURE1_ARB,
                    GL_TEXTURE2_ARB,
                    GL_TEXTURE3_ARB,
                    GL_TEXTURE4_ARB,
                    GL_TEXTURE5_ARB,
                    GL_TEXTURE6_ARB,
                    GL_TEXTURE7_ARB
                };

                glActiveTextureARB( units[unit & 7] );
                glClientActiveTextureARB( units[unit & 7] );
            }
        };

        template< int typeof_array, int typeof_component, int num_components >
        struct state
        {
            explicit
            state( const void *_ptr, size_t stride_in_bytes = 0 ) : ptr(_ptr)
            {
                if( !ptr )
                    return;

                glEnableClientState( typeof_array );

                if( typeof_array == GL_TEXTURE_COORD_ARRAY )
                    glTexCoordPointer( num_components, typeof_component, stride_in_bytes, ptr );
                else
                if( typeof_array == GL_COLOR_ARRAY )
                    glColorPointer( num_components, typeof_component, stride_in_bytes, ptr );
                else
                if( typeof_array == GL_VERTEX_ARRAY )
                    glVertexPointer( num_components, typeof_component, stride_in_bytes, ptr );
                else
                if( typeof_array == GL_NORMAL_ARRAY ) // @todo: static assert num_components == 3
                    glNormalPointer( typeof_component, stride_in_bytes, ptr );
                else
                if( typeof_array == GL_EDGE_FLAG_ARRAY ) // @todo: static assert num_components == 1, typeof_array == edge flag
                    glEdgeFlagPointer( stride_in_bytes, ptr );
            }
            ~state()
            {
                if( ptr )
                    glDisableClientState( typeof_array );
            }
            protected: const void *ptr;
            private: state( const state & ); state &operator=( const state & );
        };

        typedef state<GL_TEXTURE_COORD_ARRAY, GL_FLOAT, 1> texcoord1f;
        typedef state<GL_TEXTURE_COORD_ARRAY, GL_FLOAT, 2> texcoord2f;
        typedef state<GL_TEXTURE_COORD_ARRAY, GL_FLOAT, 3> texcoord3f;
        typedef state<GL_TEXTURE_COORD_ARRAY, GL_FLOAT, 4> texcoord4f;

        typedef state<GL_COLOR_ARRAY, GL_UNSIGNED_BYTE, 3> color3ub;
        typedef state<GL_COLOR_ARRAY, GL_FLOAT,         3> color3f;
        typedef state<GL_COLOR_ARRAY, GL_UNSIGNED_BYTE, 4> color4ub;
        typedef state<GL_COLOR_ARRAY, GL_FLOAT,         4> color4f;

        typedef state<GL_VERTEX_ARRAY, GL_FLOAT, 2> vertex2f;
        typedef state<GL_VERTEX_ARRAY, GL_FLOAT, 3> vertex3f;
        typedef state<GL_VERTEX_ARRAY, GL_FLOAT, 4> vertex4f;

        typedef state<GL_NORMAL_ARRAY, GL_FLOAT, 3> normal3f;

        typedef state<GL_EDGE_FLAG_ARRAY, GL_EDGE_FLAG, 1> edge1b;

        namespace submit
        {
            template <typename T>
            void points( const T &indices )
            {
                // draw indices as points
                glDrawElements( GL_POINTS, indices.size(), GL_UNSIGNED_INT, indices.data() );
            }

            template <typename T>
            void lines( const T &indices )
            {
                // draw indices as lines
                glDrawElements( GL_LINES, indices.size(), GL_UNSIGNED_INT, indices.data() );
            }

            template <typename T>
            void triangles( const T &indices )
            {
                // draw indices as triangles
                glDrawElements( GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data() );
            }

#if 0 // these are deprecated
            template <typename T>
            void quads( const T &indices )
            {
                // draw indices as quads
                glDrawElements( GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data() );
            }

            template <typename T>
            void polys( const T &indices )
            {
                glDrawElements( GL_POLYGON, indices.size(), GL_UNSIGNED_INT, indices.data() );
            }
#endif
        }
    }
}
