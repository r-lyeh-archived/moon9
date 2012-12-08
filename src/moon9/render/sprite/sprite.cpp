// - rlyeh ~~ listening to Toundra - Requiem

//#include <moon9/moon9.hpp>
#include <moon9/render/render.hpp>
#include <moon9/render/image.hpp>
#include <moon9/render/texture.hpp>

#include <moon9/string/string.hpp>

#include "sprite.hpp"

namespace moon9
{
    void render( sprite_ &spr )
    {
        //xyz
    //        moon9::matrix::translate t( spr.x.position / scale, spr.y.position / scale, 0 );
        //xzy

            moon9::matrix::translate t( spr.position.x, 0, spr.position.y );

            moon9::disable::culling both_faces;
            moon9::enable::alpha hard_alpha;

            moon9::enable::billboard bb;
            moon9::matrix::rotate_x rx(90);

            moon9::matrix::scale sc( ( spr.mirror.x ? -1 : +1 ) * spr.scale.x, 1.f, ( spr.mirror.y ? -1 : +1 ) * spr.scale.y );

            moon9::style::texture tx( spr.diffuse.id );
            moon9::geometry::plane floor( 1.f );
    }

    void render2d( sprite_ &spr, float gscale )
    {
        moon9::disable::culling both_faces;
        moon9::enable::alpha hard_alpha;

        {
            moon9::style::texture tx( spr.diffuse.id );

            // @todo: optimize this {

            float px = spr.position.x;
            float py = spr.position.y;
            float iw = spr.diffuse.iw;
            float ih = spr.diffuse.ih;
            float scx = spr.scale.x;
            float scy = spr.scale.y;
            float ax = spr.anchor.x;
            float ay = spr.anchor.y;
            float mx = spr.mirror.x;
            float my = spr.mirror.y;

            float sscx = gscale * scx * iw;
            float sscy = gscale * scy * ih;

            // translate to position plus anchors
            moon9::matrix::translate position( ( px / iw + ( ax - 1 ) / iw) * sscx, ( py / ih + ( ay - 1 ) / ih ) * sscy, 0 );
            // scale to image size (!= texture size)
            moon9::matrix::scale image( mx ? -sscx : sscx, my ? -sscy : sscy, 1 );

            // }

            glBegin(GL_QUADS);
                glTexCoord2f( spr.diffuse.u0, spr.diffuse.v0 );
                glVertex2f(0, 0.f);
                glTexCoord2f( spr.diffuse.u0, spr.diffuse.v1 );
                glVertex2f(0.f, 1.f);
                glTexCoord2f( spr.diffuse.u1, spr.diffuse.v1 );
                glVertex2f(1.f, 1.f);
                glTexCoord2f( spr.diffuse.u1, spr.diffuse.v0 );
                glVertex2f(1.f, 0.f);
            glEnd();
        }
    }
}

namespace moon9
{
    static moon9::texture error_texture;

    bool is_error_texture( const moon9::texture *t )
    {
        return t->id == error_texture.id;
    }

    moon9::texture &get_error_texture()
    {
        if( !error_texture.w )
        {
            error_texture = moon9::texture( 32, 32, moon9::pixel::rgba( 0,0,0,1.f ) );

            for( int y = 0; y < 32; y++ )
            for( int x = (y < 16 ? 0 : 16), end = (y < 16 ? 16 : 32); x < end; x++ )
                error_texture.at( x, y ) = moon9::pixel::rgba( 1.f, 1.f, 0, 1.f );

            error_texture.submit();
        }

        return error_texture;
    }

    moon9::texture &get_texture( const std::string &texture_name )
    {
        // @fixme GIF texture loading way too slow

        static moon9::texturemap<std::string> map;

        if( map.find(texture_name) )
            return map.found();

        moon9::texture &tx = map.insert( texture_name, moon9::texture( moon9::image(texture_name) ) );

        if( tx.w > 0 )
        {
            std::cout << "Texture '" << texture_name << "' submitted!" << std::endl;
            tx.submit();
        }
        else
        {
            tx.delegate();
            tx.id = get_error_texture().id;
        }

        return tx;
    }

#if 0
    // point sprites, issues:
    // setting sprite size
    // sprites get culled as soon as center of sprite is out of viewport
    // wont rotate (to verify)
    void sprite( const std::string &texture_name, float texture_scale = 1.f, float unused = 1.f )
    {
        moon9::texture &texture = get_texture( texture_name );
        texture.bind();

        moon9::enable::alpha hard_alpha;

            float quadratic[] =  { 0.0f, 0.0f, 0.01f };
            glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );
            float maxSize = 0.0f;
            glGetFloatv( GL_POINT_SIZE_MAX_ARB, &maxSize );
            glPointSize( maxSize );
            glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, maxSize );
            glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 1.0f );
            glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
            glEnable( GL_POINT_SPRITE_ARB );
            glBegin( GL_POINTS );
                    //glColor4f(VecParticle[i].Color.x,VecParticle[i].Color.y,VecParticle[i].Color.z,1.0f);
                    //glVertex3f(VecParticle[i].Position.x,VecParticle[i].Position.y,VecParticle[i].Position.z);
                    glColor4f(1,1,1,1);
                    glVertex3f(0,0,0);
            glEnd();
            glDisable( GL_POINT_SPRITE_ARB );

        texture.unbind();
    }
#else

    void texturedquad( bool mirror_w, bool mirror_h, float scale_w, float scale_h, float texture_tiling )
    {
            // opengl < 1.5
            moon9::enable::billboard bd;
            moon9::matrix::rotate_x rx(90.f);
//            moon9::matrix::scale sc( texture_scale );

            moon9::disable::culling both_faces;

            float tw = scale_w, itw = ( mirror_w ? -1.f : +1.f ) / tw;
            float th = scale_h, ith = ( mirror_h ? -1.f : +1.f ) / th;

            glScalef( tw, 1, th );
            moon9::geometry::plane floor( texture_tiling );
            glScalef( itw, 1, ith );
    }

    void spritebox( const moon9::texture &texture, bool mirror_w, bool mirror_h, float texture_scale_w, float texture_scale_h, float texture_tiling )
    {
#if 1
        //moon9::enable::alpha hard_alpha;

        texture.bind();

        texturedquad( mirror_w, mirror_h, texture_scale_w, texture_scale_h, texture_tiling );

        texture.unbind();
#else
        moon9::texture &tx = get_texture( texture_name );

        moon9::disable::lighting nolights;
        moon9::disable::depth nodepth;

        moon9::matrix::scale sc( tx.w * 0.01f, tx.h * 0.01f, 0.1f );

        moon9::style::red color;
        moon9::style::wireframe wf;

        moon9::cube cb;
#endif
    }

    void sprite( const moon9::texture &texture, bool mirror_w, bool mirror_h, float texture_scale_w, float texture_scale_h, float texture_tiling )
    {
        moon9::enable::alpha hard_alpha;

        texture.bind();

        texturedquad( mirror_w, mirror_h, texture_scale_w, texture_scale_h, texture_tiling );

        texture.unbind();
    }

    void sprite( const std::string &texture_name, bool mirror_w, bool mirror_h, float texture_scale_w, float texture_scale_h, float texture_tiling )
    {
        sprite( get_texture( texture_name ), mirror_w, mirror_h, texture_scale_w, texture_scale_h, texture_tiling );
    }

    void spritebox( const std::string &texture_name, bool mirror_w, bool mirror_h, float texture_scale_w, float texture_scale_h, float texture_tiling )
    {
        spritebox( get_texture( texture_name ), mirror_w, mirror_h, texture_scale_w, texture_scale_h, texture_tiling );
    }

#endif

    // @todo: triparticle, biparticle, particle

#if 0
    // proposal
    sprite( pos, texture_scaling, texture_tiling )
    spritesheet().
        play_forward/backward/pingpong( "anim.gif", 100%, times = 0,1,2,3... );
        play( x0 )

    sprite().
        position( pos ).
        scaling( 1.f ).
        tiling( 1.f ).
        speed( 1.0 ).
        forward( 3 ).
        show( true ).
        play( "anim.gif" ); // stop()
#endif

    std::deque< moon9::texture * > import_gif( const std::string &filename, size_t frame_start, size_t frame_end )
    {
        std::deque< moon9::texture * > textures;

        for( size_t frame = frame_start; frame < frame_end; ++frame )
        {
            moon9::string filename_and_frame( "\1|\2", filename, frame );

            textures.push_back( &get_texture(filename_and_frame) );

            if( is_error_texture( textures.back() ) )
            {
                textures.pop_back();
                return textures;
            }
        }

        return textures;
    }
}

