#include <cassert>
#include <iostream>
#include <vector>

#include <moon9/debug/assert.hpp>
#include <moon9/io/file.hpp>
#include <moon9/render/render.hpp>
#include <moon9/render/mesh.hpp>
#include <moon9/spatial/vec.hpp>
#include <moon9/string/string.hpp>

#include "obj.h"

namespace moon9
{
    // @todo: layers: http://www.clockworkcoders.com/oglsl/tutorial8.htm
    // @todo: lights: http://www.clockworkcoders.com/oglsl/tutorial5.htm

    namespace lightwave
    {
        bool import( mesh &self, const std::string &pathfile )
        {
            self = mesh();

            auto &texcoords = self.texcoords;
            auto &vertices = self.vertices;
            auto &normals = self.normals;

            auto &triangles = self.triangles;
            auto &points = self.points;
            auto &lines = self.lines;

            obj_model_t mesh;
            obj_model_t *mdl = &mesh;

            if( !ReadOBJModel( pathfile.c_str(), &mesh) )
                return false;

            if( mdl->num_texCoords > 0 )
            {
                texcoords.resize( mdl->num_texCoords );
                for( int i = 0; i < mdl->num_texCoords; ++i )
                {
                    texcoords[i][0] = mdl->texCoords[i].uvw[0];
                    texcoords[i][1] = mdl->texCoords[i].uvw[1];
                    texcoords[i][2] = mdl->texCoords[i].uvw[2];
                }
            }

            if( 0 )
            if( mdl->num_normals > 0 )
            {
                normals.resize( mdl->num_normals );
                for( int i = 0; i < mdl->num_normals; ++i )
                {
                    normals[i][0] = mdl->normals[i].ijk[0];
                    normals[i][1] = mdl->normals[i].ijk[1];
                    normals[i][2] = mdl->normals[i].ijk[2];
                }
            }

            if( mdl->num_verts > 0 )
            {
                vertices.resize( mdl->num_verts );
                for( int i = 0; i < mdl->num_verts; ++i )
                {
                    vertices[i][0] = mdl->vertices[i].xyzw[0];
                    vertices[i][1] = mdl->vertices[i].xyzw[1];
                    vertices[i][2] = mdl->vertices[i].xyzw[2];
                    vertices[i][3] = mdl->vertices[i].xyzw[3];
                }
            }

            bool has_uv = mdl->has_texCoords;
            bool has_normals = mdl->has_normals;

            float3 normzero = { 0,0,0 };
            normals.resize( mdl->num_faces, normzero );

            for( int i = 0; i < mdl->num_faces; ++i )
            {
                size_t elements = mdl->faces[i].num_elems;

                std::vector<unsigned int> *indices = 0;

                if( elements == 0 )
                {
                    // huh?
                    continue;
                }
                else
                if( elements == 1 )
                {
                    indices = &points;
                }
                else
                if( elements == 2 )
                {
                    indices = &lines;
                }
                else
                if( elements == 3 )
                {
                    indices = &triangles;
                }
                else
                if( elements == 4 )
                {
                    // quad... unsupported type (deprecated)
                    continue;
                }
                else
                {
                    // polygon... unsupported type (deprecated)
                    continue;
                }

                if( 0 ) // this seems to be working :)
                for( int j = 0; j < elements; ++j )
                {
                    indices->push_back( mdl->faces[i].vert_indices[j] );
/*
                    if( has_normals )
                    indices.push_back( mdl->faces[i].norm_indices[j] );

                    if( has_uv )
                    indices.push_back( mdl->faces[i].uvw_indices[j] );
*/
                }

                if( elements == 3 )
                {
                    // from http://www.iquilezles.org/www/articles/normals/normals.htm

                    const int ia = mdl->faces[i].vert_indices[0];
                    const int ib = mdl->faces[i].vert_indices[1];
                    const int ic = mdl->faces[i].vert_indices[2];

                    const moon9::vec3 e1 = moon9::vec3( vertices[ia] ) - vertices[ib];
                    const moon9::vec3 e2 = moon9::vec3( vertices[ic] ) - vertices[ib];
                    const moon9::vec3 no = -cross( e1, e2 );

                    normals[ia].x += no.x;
                    normals[ia].y += no.y;
                    normals[ia].z += no.z;

                    normals[ib].x += no.x;
                    normals[ib].y += no.y;
                    normals[ib].z += no.z;

                    normals[ic].x += no.x;
                    normals[ic].y += no.y;
                    normals[ic].z += no.z;

                    // verts
                    indices->push_back( ia );
                    indices->push_back( ib );
                    indices->push_back( ic );

                    // norms
                    indices->push_back( ia );
                    indices->push_back( ib );
                    indices->push_back( ic );
                }
            }

            FreeModel(&mesh);

//            normals.clear(); // disable normals by clearing them :D

            return true;
        }

        void render( const moon9::mesh &mesh )
        {
            // activate and specify pointers to vertex and normal arrays. order is important
            array::vertex4f v4f( mesh.vertices.data() );
            array::normal3f n3f( mesh.normals.size() ? mesh.normals.data() : 0 );

            // submit triangles
            array::submit::triangles( mesh.triangles );
        }
    }
}
