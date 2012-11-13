#pragma once

#include <vector>
#include <string>

#include <moon9/spatial/spline.hpp>
#include <moon9/spatial/vec.hpp>

namespace moon9
{
    class md2
    {
        enum { MD2_OK         = 0x0 };
        enum { MD2_ERR_MEM    = 0x1 };
        enum { MD2_ERR_FILE   = 0x2 };
        enum { MD2_ERR_FORMAT = 0x4 };

        struct MD2Header
        {
            int ID;         // File Type 'IPD2'
            int Version;
            int TexWidth;   // Texture width
            int TexHeight;  // Texture height
            int FrameSize;  // Size for frames in bytes
            int nTextures;  // Number of textures
            int nVertices;  // Number of vertices
            int nTexCoords; // Number of UVs
            int nTriangles; // Number of polys
            int nGLCmd;     // Number of GL Commmands
            int nFrames;    // Number of frames
            int TexOffset;  // Offset to texture name(s)
            int UVOffset;   // Offset to UV data
            int FaceOffset; // Offset to poly data
            int FrameOffset;// Offset to first frame
            int GLCmdOffset;// Offset to GL Cmds
            int EOFOffset;  // Size of file
        };

        struct MD2TexName
        {
            char name[64];
        };

        struct MD2Face      // triangle
        {
            unsigned short p1,p2,p3;
            unsigned short uv1,uv2,uv3;
        };

        struct MD2Vtx
        {
            unsigned char Vtx[3];
            unsigned char lNorm;
        };

        struct MD2Anim
        {
            unsigned int start, end;
        };

        struct MD2FrameInfo
        {
            float Scale[3];
            float Translate[3];
            char Name[16];
        };

        struct MD2GlCommand
        {
            float s;                // S texture coord.
            float t;                // T texture coord.
            unsigned int index;     // Vertex index
        };

        struct Mesh_Vtx
        {
            float x,y,z;
        };

        struct MD2Uvw     // short -> convert ?
        {
            float u,v,w;
        };

        struct MD2Frame
        {
            std::vector<Mesh_Vtx> Vtx;
            std::vector<Mesh_Vtx> Norm;
            std::string Name;
        };

        struct MD2TexCoord
        {
            short u,v;
        };

        int nFrames,nTri,nVtx,nUV;
        std::vector<MD2Face> Face;
        std::vector<MD2Frame> Frame;
        std::vector<MD2Uvw> UV;
        std::string TexName;

    public:

        typedef std::map<std::string, MD2Anim> AnimMap;

    protected:

        AnimMap Anims;

        struct anim_t
        {
            size_t frames, loops;
            std::map<int, moon9::spline<moon9::vec3> > vec3buf;
            bool init;

            anim_t() : init( false )
            {}
        };

        static std::map< std::string, anim_t > cached;

        Mesh_Vtx calc_normal(Mesh_Vtx v1,Mesh_Vtx v2,Mesh_Vtx v3);

    public:

         md2();
        ~md2();

        bool import( const std::string &binary ); //load,import,bin

        std::string get_tex_name() const;

        void render_keyframe( int frame ) const;
        void render_keyframe( const std::string &anim, float dt01 ) const;
        void render_keyframe_lerp( const std::string &anim, float dt01 ) const;

        const AnimMap &get_anims() const { return Anims; }
    };
}

