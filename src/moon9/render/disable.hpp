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
    namespace disable
    {
        struct scissor : nocopy
        {
             scissor() { glDisable( GL_SCISSOR_TEST ); }
            ~scissor() {  glEnable( GL_SCISSOR_TEST ); }
        };

        struct blending : nocopy
        {
            blending() { glDisable(GL_BLEND); }
           ~blending() { glEnable(GL_BLEND); }
        };

        struct depth : nocopy
        { //bool on; GLboolean glIsEnabled( GLenum      cap);
               depth() { glDisable( GL_DEPTH_TEST ); }
              ~depth() {  glEnable( GL_DEPTH_TEST ); }
        };

        struct lighting : nocopy
        {
               lighting() { glDisable( GL_LIGHTING ); }
              ~lighting() {  glEnable( GL_LIGHTING ); }
        };

        struct culling : nocopy
        {
               culling() { glDisable( GL_CULL_FACE ); }
              ~culling() {  glEnable( GL_CULL_FACE ); }
        };

        struct texturing : nocopy
        {
               texturing() { glDisable( GL_TEXTURE_2D ); }
              ~texturing() {  glEnable( GL_TEXTURE_2D ); }
        };

        struct fog : nocopy
        {
               fog() { glDisable( GL_FOG ); }
              ~fog() {  glEnable( GL_FOG ); }
        };
    }
}
