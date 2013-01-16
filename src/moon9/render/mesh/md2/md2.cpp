#include <cassert>

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>

#include "../../gl.hpp"

#include "md2.hpp"

namespace moon9
{
    std::map<std::string, md2::anim_t> md2::cached;

    md2::md2() : TexName("")
    {}

    md2::~md2()
    {}

    bool md2::import( const std::string &binary )
    {
        const std::string &data = binary;
        const size_t fSize = binary.size();

        if( !fSize )
            return MD2_ERR_FORMAT, false;

        std::vector<MD2Vtx> vtx;
        std::vector<MD2TexCoord> MD2_UV;
        MD2FrameInfo FrameInfo;

        // Get Header data
        MD2Header Head;
        memcpy(&Head,&data[0],sizeof(MD2Header));

        // Dump info about object
        std::cout << "ID - "			<< data[0] << data[1] << data[2] << data[3] << "\n";
        std::cout << "Version - "		<< Head.Version<< "\n";
        std::cout << "Tex Width - "	    << Head.TexWidth<< "\n";
        std::cout << "Tex Height - "	<< Head.TexHeight<< "\n";
        std::cout << "Frame Size - "	<< Head.FrameSize<< "\n";
        std::cout << "Textures - "		<< Head.nTextures<< "\n";
        std::cout << "Vertices - "		<< Head.nVertices<< "\n";
        std::cout << "UVs - "			<< Head.nTexCoords<< "\n";
        std::cout << "Faces - "		    << Head.nTriangles<< "\n";
        std::cout << "GL cmds - "		<< Head.nGLCmd<< "\n";
        std::cout << "Frames - "		<< Head.nFrames<< "\n";
        std::cout << "Skin Offset - "	<< Head.TexOffset<< "\n";
        std::cout << "UV Offset - " 	<< Head.UVOffset<< "\n";
        std::cout << "Face Offset - "	<< Head.FaceOffset<< "\n";
        std::cout << "Frame Offset - "	<< Head.FrameOffset<< "\n";
        std::cout << "GL Offset - "	    << Head.GLCmdOffset<< "\n";
        std::cout << "Filesize - "		<< Head.EOFOffset<<"\n";

        // A few checks to ensure this is an MD2 file
        //if( Head.ID != 844121161 ) // chars 'IDP2' as a dword
        if( Head.ID != ('I'<<0) + ('D'<<8) + ('P'<<16) + ('2'<<24) )
            return MD2_ERR_FORMAT, false;

        if( Head.Version !=8 )
            return MD2_ERR_FORMAT, false;

        if( Head.EOFOffset != fSize )
            return MD2_ERR_FORMAT, false;

        // Grab the info we'll need later
        nFrames = Head.nFrames;
        nTri = Head.nTriangles;
        nVtx = Head.nVertices;
        nUV = Head.nTexCoords;


        // Allocate arrays

        // Frames
        Frame.resize( nFrames );

        // Frame components
        for( int i = 0; i < nFrames; ++i )
        {
            Frame[i].Vtx.resize( nVtx );

            Frame[i].Norm.resize( nTri );
        }

        // MD2 vtx buffer
        vtx.resize( Head.nVertices );

        // Faces
        Face.resize( Head.nTriangles );

        // UVs
        UV.resize( nUV );

        // MD2 UV buffer
        MD2_UV.resize( Head.nTexCoords );

        // Extract and convert info from file
        // Read first texture name
        if( Head.nTextures > 0 )
        {
            TexName = (const char *)&data[Head.TexOffset];
            std::cout << "Texture Name - " << TexName << "\n";
        }

        // Read face data
        memcpy( &Face[0], &data[Head.FaceOffset], Head.nTriangles*sizeof(MD2Face));

        // Read MD2 UV data
        memcpy( &MD2_UV[0], &data[Head.UVOffset], Head.nTexCoords*sizeof(MD2TexCoord));

        // Convert into regular UVs
        for( int i = 0; i < nUV; ++i )
        {
            UV[i].u = ( (float)MD2_UV[i].u ) / Head.TexWidth;
            UV[i].v = ( (float)MD2_UV[i].v ) / Head.TexHeight;
            UV[i].w = 0;
        }

        // Finished with MD2 style UVs


        // Load frame vertex info
        for( int j = 0; j < nFrames; ++j )
        {
            // Get frame conversion data
            memcpy( &FrameInfo, &data[Head.FrameOffset + (Head.FrameSize * j)], sizeof(FrameInfo) );

            // Read MD2 style vertex data
            memcpy( &vtx[0], &data[Head.FrameOffset + (Head.FrameSize * j) + sizeof(FrameInfo)], nVtx * sizeof(MD2Vtx) );

            // Convert vertices
            for( int i = 0 ; i < nVtx; ++i )
            {
                Frame[j].Vtx[i].x = ( vtx[i].Vtx[0] * FrameInfo.Scale[0] ) + FrameInfo.Translate[0];
                Frame[j].Vtx[i].y = ( vtx[i].Vtx[1] * FrameInfo.Scale[1] ) + FrameInfo.Translate[1];
                Frame[j].Vtx[i].z = ( vtx[i].Vtx[2] * FrameInfo.Scale[2] ) + FrameInfo.Translate[2];
            }

            // Assign frame name
            Frame[j].Name = FrameInfo.Name;
        }

        // Calc normals for each frane
        for( int j = 0; j < nFrames; ++j )
        {
            // Calc face normal
            for( int i = 0; i < nTri; ++i )
            {
                Frame[j].Norm[i] = calc_normal( Frame[j].Vtx[Face[i].p1], Frame[j].Vtx[Face[i].p2], Frame[j].Vtx[Face[i].p3] );
            }
        }

#if 1

        // setup animations
        {
            std::string currentAnim;
            MD2Anim animInfo = { 0, 0 };

            for (int i = 0; i < nFrames; ++i)
            {
                std::string frameName = Frame[i].Name;
                std::string frameAnim;

                // Extract animation name from frame name
                std::string::size_type len = frameName.find_first_of ("0123456789");

                if ((len == frameName.length () - 3) && (frameName[len] != '0'))
                    len++;

                frameAnim.assign (frameName, 0, len);

                if (currentAnim != frameAnim)
                {
                    if (i > 0)
                    {
                        // Previous animation is finished, insert
                        // it and start new animation.
                        Anims.insert (AnimMap::value_type(currentAnim, animInfo));
                        std::cout << "Animation[ \"" << currentAnim << "\" ] = " << animInfo.start << ".." << animInfo.end << "\n";
                    }

                    // Initialize new anim info
                    animInfo.start = i;
                    animInfo.end = i;

                    currentAnim = frameAnim;
                }
                else
                {
                    animInfo.end = i;
                }
            }

            // Insert last animation
            Anims.insert (AnimMap::value_type(currentAnim, animInfo));
            std::cout << "Animation[ \"" << currentAnim << "\" ] = " << animInfo.start << ".." << animInfo.end << "\n";
        }

#endif


        return MD2_OK, true;
    }

    std::string md2::get_tex_name() const
    {
        return TexName;
    }

    // draw a single keyframe

    void md2::render_keyframe( int frameNo ) const
    {
        assert( frameNo >= 0 && frameNo < nFrames );

        const MD2Frame &frame = Frame[ frameNo ];

        glBegin(GL_TRIANGLES);

        for( int Part = 0; Part < nTri; ++Part )
        {
            const MD2Face &face = Face[ Part ];

            glNormal3fv( (float *)&frame.Norm[Part] );

            glTexCoord2fv( (float *)&UV[face.uv1] );
            glVertex3fv( (float *)&frame.Vtx[face.p1] );

            glTexCoord2fv( (float *)&UV[face.uv2] );
            glVertex3fv( (float *)&frame.Vtx[face.p2] );

            glTexCoord2fv( (float *)&UV[face.uv3] );
            glVertex3fv( (float *)&frame.Vtx[face.p3] );
        }

        glEnd();
    }

    // draw a keyframe based on animation and delta 0..1

    void md2::render_keyframe( const std::string &anim, float dt01 ) const
    {
        //assert( Anims.find( anim ) != Anims.end() );
        if( Anims.find( anim ) == Anims.end() )
            return;

        assert( dt01 >= 0.f && dt01 <= 1.f );

        MD2Anim keyframe = (*Anims.find( anim )).second;

        //Draw( keyframe.start + ( keyframe.end - keyframe.start ) * dt01 );

        render_keyframe( keyframe.start + ( (dt01 == 1.f ? 0 : 1) + keyframe.end - keyframe.start ) * dt01 );
    }

    //

    void md2::render_keyframe_lerp( const std::string &anim, float dt01 ) const //, float xSmoothing ) const
    {
        //assert( Anims.find( anim ) != Anims.end() );
        if( Anims.find( anim ) == Anims.end() )
            return;

        assert( dt01 >= 0.f && dt01 <= 1.f );

        //#pragma warning ("remove static/cache from class")

        if( cached.find( TexName + anim ) == cached.end() )
        {
            cached[ TexName + anim ] = anim_t();
        }

        anim_t &cached_anim = ((*cached.find( TexName + anim )).second);

        if( !cached_anim.init ) //Anims.find( anim + "::interpolated" ) == Anims.end() )
        {
            //c9::scope scope("md2::DrawInterpolated");

            //Draw( keyframe.start + ( keyframe.end - keyframe.start ) * dt01 );
            //Draw( keyframe.start + ( (dt01 == 1.f ? 0 : 1) + keyframe.end - keyframe.start ) * dt01 );

            MD2Anim keyframe = (*Anims.find( anim )).second;

            std::vector< moon9::spline<moon9::vec3> > N( nTri );
            std::vector< moon9::spline<moon9::vec3> > A( nTri );
            std::vector< moon9::spline<moon9::vec3> > B( nTri );
            std::vector< moon9::spline<moon9::vec3> > C( nTri );

            for( int Part = 0; Part < nTri; ++Part )
            {
                const MD2Face &face = Face[ Part ];

                if( true ) //start edge
                {
                    const MD2Frame &frame = Frame[ keyframe.start ];

                    N[ Part ].push_back( (float *)&frame.Norm[Part] );
                    A[ Part ].push_back( (float *)&frame.Vtx[face.p1] );
                    B[ Part ].push_back( (float *)&frame.Vtx[face.p2] );
                    C[ Part ].push_back( (float *)&frame.Vtx[face.p3] );
                }

                for( size_t f = keyframe.start; f <= keyframe.end; ++f )
                {
                    const MD2Frame &frame = Frame[ f ];

                    N[ Part ].push_back( (float *)&frame.Norm[Part] );
                    A[ Part ].push_back( (float *)&frame.Vtx[face.p1] );
                    B[ Part ].push_back( (float *)&frame.Vtx[face.p2] );
                    C[ Part ].push_back( (float *)&frame.Vtx[face.p3] );
                }

                if( true ) //bAnimHasLoop )
                {
                    const MD2Frame &frame = Frame[ keyframe.start ];

                    N[ Part ].push_back( (float *)&frame.Norm[Part] );
                    A[ Part ].push_back( (float *)&frame.Vtx[face.p1] );
                    B[ Part ].push_back( (float *)&frame.Vtx[face.p2] );
                    C[ Part ].push_back( (float *)&frame.Vtx[face.p3] );
                }

                if( true ) //end edge
                {
                    const MD2Frame &frame = Frame[ /*bAnimHasLoop*/ true ? keyframe.start : keyframe.end ];

                    N[ Part ].push_back( (float *)&frame.Norm[Part] );
                    A[ Part ].push_back( (float *)&frame.Vtx[face.p1] );
                    B[ Part ].push_back( (float *)&frame.Vtx[face.p2] );
                    C[ Part ].push_back( (float *)&frame.Vtx[face.p3] );
                }
            }

            float factor = 4.f;

            cached_anim.frames = ( keyframe.end - keyframe.start ) * factor;
            cached_anim.loops = nTri;

            for( size_t frame = 0; frame <= cached_anim.frames; ++frame )
            {
                float dt = frame / (float)cached_anim.frames;

                cached_anim.vec3buf[frame].reserve( nTri * 7 ); //elems
                //cached_anim.vec3buf[frame].reserve( nTri * 7 * 3 * sizeof(float) ); //bytes

                for( int Part = 0; Part < nTri; ++Part )
                {
                    const MD2Face &face = Face[ Part ];

                    cached_anim.vec3buf[frame].push_back( N[ Part ].position( dt ) );

                    cached_anim.vec3buf[frame].push_back( (float *)&UV[face.uv1] );
                    cached_anim.vec3buf[frame].push_back( A[ Part ].position( dt ) );

                    cached_anim.vec3buf[frame].push_back( (float *)&UV[face.uv2] );
                    cached_anim.vec3buf[frame].push_back( B[ Part ].position( dt ) );

                    cached_anim.vec3buf[frame].push_back( (float *)&UV[face.uv3] );
                    cached_anim.vec3buf[frame].push_back( C[ Part ].position( dt ) );
                }
            }

            cached_anim.init = true;
        }

        auto *buf = &cached_anim.vec3buf[ (int)( cached_anim.frames * dt01 ) ][0];

        glFrontFace(GL_CW); // @todo: reorganize vertexs and remove this from here. make vertex array ccw

        glBegin(GL_TRIANGLES);

        size_t loops = cached_anim.loops;
        while( loops-- )
        {
            glNormal3fv( (float *)buf++ );

            glTexCoord2fv( (float *)buf++ );
            glVertex3fv( (float *)buf++ );

            glTexCoord2fv( (float *)buf++ );
            glVertex3fv( (float *)buf++ );

            glTexCoord2fv( (float *)buf++ );
            glVertex3fv( (float *)buf++ );
        }

        glEnd();

        glFrontFace(GL_CCW);
    }



    md2::Mesh_Vtx md2::calc_normal(Mesh_Vtx v1,Mesh_Vtx v2,Mesh_Vtx v3)
    {
        double v1x,v1y,v1z,v2x,v2y,v2z;
        double nx,ny,nz;
        double vLen;

        // Calculate vectors
        v1x = v1.x - v2.x;
        v1y = v1.y - v2.y;
        v1z = v1.z - v2.z;

        v2x = v2.x - v3.x;
        v2y = v2.y - v3.y;
        v2z = v2.z - v3.z;

        // Get cross product of vectors

        nx = (v1y * v2z) - (v1z * v2y);
        ny = (v1z * v2x) - (v1x * v2z);
        nz = (v1x * v2y) - (v1y * v2x);

        // Normalise final vector
        vLen = sqrt( (nx * nx) + (ny * ny) + (nz * nz) );

        Mesh_Vtx Result;

        Result.x = (float)( nx / vLen );
        Result.y = (float)( ny / vLen );
        Result.z = (float)( nz / vLen );

        return Result;
    }

}

