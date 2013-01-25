#pragma once

#include "ext.hpp"

#include <moon9/render/nocopy.hpp>

namespace moon9
{
    class fbo : moon9::nocopy
    {
        GLuint frame;
        GLuint color;
        GLuint depth;

        GLsizei _w;
        GLsizei _h;

        bool ready;

    public:

        fbo() : ready(false) {}
        fbo(GLint, GLint, GLsizei, GLsizei);
       ~fbo();

        bool setup(GLint, GLint, GLsizei, GLsizei);

        void bind_frame() const;
        void bind_color(GLenum) const;
        void bind_depth(GLenum) const;
    };

    void get_framebuffer(GLint[1], GLint[4]);
    void set_framebuffer(GLint[1], GLint[4]);

    struct preserve_fbo : moon9::nocopy
    {
         preserve_fbo() { get_framebuffer(o,v); }
        ~preserve_fbo() { set_framebuffer(o,v); }
         private: GLint o[1], v[4];
         private: preserve_fbo( const preserve_fbo & );
         private: preserve_fbo& operator=( const preserve_fbo & );
    };
}

