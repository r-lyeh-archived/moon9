#pragma once

#include <moon9/render/gl.hpp>
#include <moon9/render/nocopy.hpp>

namespace moon9
{
/*
    this is a safer approach

    disable::lighting()
        glPushAttrib(GL_LIGHTING_BIT);
        glDisable(GL_LIGHTING);
    disable::~lighting()
        glPopAttrib();

    enable::lighting()
        glPushAttrib(GL_LIGHTING_BIT);
        glEnable(GL_LIGHTING);
    enable::~lighting()
        glPopAttrib();

*/
    namespace enable
    {
        struct alpha : nocopy
        {
             explicit alpha( float value = 0.5f ) { glAlphaFunc(GL_GREATER, value); glEnable(GL_ALPHA_TEST); }
            ~alpha()                     { glDisable(GL_ALPHA_TEST); }
        };

        struct depth : nocopy
        {
             depth() { glEnable(GL_DEPTH_TEST); }
            ~depth() { glDisable(GL_DEPTH_TEST); }
        };

        struct scissor : nocopy
        {
             scissor(float x, float y, float w, float h) { glScissor( x, y, w, h ); glEnable(GL_SCISSOR_TEST); }
             scissor() { glEnable(GL_SCISSOR_TEST); }
            ~scissor() { glDisable(GL_SCISSOR_TEST); }
        };

        struct lighting : nocopy
        {
               lighting() {  glEnable( GL_LIGHTING ); }
              ~lighting() { glDisable( GL_LIGHTING ); }
        };

        struct blending : nocopy
        {
            // FIXME
            // http://www.opengl.org/resources/faq/technical/transparency.htm

            blending()
            {
                glEnable (GL_BLEND);
                glDisable(GL_DEPTH_TEST);

                //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glBlendFunc( GL_SRC_ALPHA, GL_ONE );
                //glBlendFunc(GL_ONE, GL_ONE);

                glColor4f( 1.f, 1.f, 1.f, 0.5f );
            }

            ~blending()
            {
                glDisable( GL_BLEND );
                glEnable( GL_DEPTH_TEST );

                //glBlendFunc( GL_SRC_ALPHA, GL_ONE );

                glColor4f( 1.f, 1.f, 1.f, 1.f );
            }
        };

        struct billboard : nocopy
        {
            explicit billboard( const bool &bSpherical = true )
            {
                if( bSpherical ) // cheap spherical billboard
                {
                    float modelview[16];

                    // save the current modelview matrix
                    glPushMatrix();

                    // get the current modelview matrix
                    glGetFloatv(GL_MODELVIEW_MATRIX , modelview);

                    // undo all rotations
                    // beware all scaling is lost as well
                    /*
                    for( int i=0; i<3; i++ )
                    for( int j=0; j<3; j++ ) {
                    if ( i==j )
                    modelview[i*4+j] = 1.0;
                    else
                    modelview[i*4+j] = 0.0;
                    }
                    */

                    modelview[0*4+0] = 1.0;
                    modelview[0*4+1] = 0.0;
                    modelview[0*4+2] = 0.0;

                    modelview[1*4+0] = 0.0;
                    modelview[1*4+1] = 1.0;
                    modelview[1*4+2] = 0.0;

                    modelview[2*4+0] = 0.0;
                    modelview[2*4+1] = 0.0;
                    modelview[2*4+2] = 1.0;

                    // set the modelview with no rotations
                    glLoadMatrixf(modelview);
                }
                else
                    if( 1 ) // cheap cylindrical billboard
                    {
                        float modelview[16];

                        // save the current modelview matrix
                        glPushMatrix();

                        // get the current modelview matrix
                        glGetFloatv(GL_MODELVIEW_MATRIX , modelview);

                        /*
                        for( int i=0; i<3; i+=2 )
                        for( int j=0; j<3; j++ ) {
                        if ( i==j )
                        modelview[i*4+j] = 1.0;
                        else
                        modelview[i*4+j] = 0.0;
                        }
                        */

                        modelview[0*4+0] = 1.0;
                        modelview[0*4+1] = 0.0;
                        modelview[0*4+2] = 0.0;

                        modelview[2*4+0] = 0.0;
                        modelview[2*4+1] = 0.0;
                        modelview[2*4+2] = 0.0;

                        // set the modelview matrix
                        glLoadMatrixf(modelview);
                    }

                    // add missing ones from here
                    // http://www.lighthouse3d.com/opengl/billboarding/index.php?billCyl

            }
            ~billboard()
            {
                // restore the previously
                // stored modelview matrix
                glPopMatrix();
            }
        };
    }
}
