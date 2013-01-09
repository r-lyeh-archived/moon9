#pragma once

#include "ext.hpp"

namespace moon9
{
    class fbo
    {
        GLuint frame;
        GLuint color;
        GLuint depth;

        GLsizei _w;
        GLsizei _h;

    public:

        fbo(GLint, GLint, GLsizei, GLsizei);
       ~fbo();

        void bind_frame();
        void bind_color(GLenum);
        void bind_depth(GLenum);
    };

    void get_framebuffer(GLint[1], GLint[4]);
    void set_framebuffer(GLint[1], GLint[4]);
}

