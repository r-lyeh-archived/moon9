#include <cassert>

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>

#include "gl.hpp"
#include "fbo.hpp"

namespace moon9
{

//-----------------------------------------------------------------------------

fbo::fbo(GLint color_format,
         GLint depth_format, GLsizei w, GLsizei h)
{
    setup( color_format, depth_format, w, h );
}

bool fbo::setup( GLint color_format, GLint depth_format, GLsizei w, GLsizei h)
{
    ready = false;
    _w = w, _h = h;

    GLint o[1], v[4];

    get_framebuffer(o, v);

    $glGenFramebuffersEXT(1, &frame);
    $glGenTextures       (1, &color);
    $glGenTextures       (1, &depth);

    // Initialize the color render buffer object.

    $glBindTexture(GL_TEXTURE_2D, color);
    $glTexImage2D (GL_TEXTURE_2D, 0, color_format, w, h, 0,GL_RGBA, GL_INT, NULL);

    $glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    $glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    $glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    $glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

    // Initialize the depth render buffer object.

    $glBindTexture(GL_TEXTURE_2D, depth);
    $glTexImage2D (GL_TEXTURE_2D, 0, depth_format, w, h, 0,GL_DEPTH_COMPONENT, GL_INT, NULL);

    $glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    $glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    $glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    $glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
    $glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    // Initialize the frame buffer object.

    $glBindFramebufferEXT     (GL_FRAMEBUFFER_EXT, frame);
    $glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_2D, color, 0);
    $glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, depth, 0);

    // Confirm the frame buffer object status.
    auto result = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
   ;
    switch( result )
    {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
        assert( !"Framebuffer incomplete attachment" );
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
        assert( !"Framebuffer missing attachment" );
    case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
        assert( !"Framebuffer duplicate attachment" );
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        assert( !"Framebuffer dimensions" );
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        assert( !"Framebuffer formats" );
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
        assert( !"Framebuffer draw buffer" );
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
        assert( !"Framebuffer read buffer" );
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        assert( !"Framebuffer unsupported" );
    default:
        assert( !"Framebuffer error" );
    }

    // Zero the buffer.

    $glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    set_framebuffer(o, v);

    ready = true;
    return true;
}

fbo::~fbo()
{
    if( ready )
    {
        $glDeleteFramebuffersEXT(1, &frame);
        $glDeleteTextures       (1, &depth);
        $glDeleteTextures       (1, &color);
    }
}

void fbo::bind_frame() const
{
    assert( ready );

    $glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame);
    $glViewport(1, 1, _w - 2, _h - 2);
    $glScissor (1, 1, _w - 2, _h - 2);
}

void fbo::bind_color(GLenum T) const
{
    assert( ready );

    $glActiveTexture(T);
    {
        $glBindTexture(GL_TEXTURE_2D, color);
    }
    $glActiveTexture(GL_TEXTURE0);
}

void fbo::bind_depth(GLenum T) const
{
    assert( ready );

    $glActiveTexture(T);
    {
        $glBindTexture(GL_TEXTURE_2D, depth);
    }
    $glActiveTexture(GL_TEXTURE0);
}

//-----------------------------------------------------------------------------

void get_framebuffer(GLint o[1], GLint v[4])
{
    $glGetIntegerv(GL_VIEWPORT,                v);
    $glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, o);
}

void set_framebuffer(GLint o[1], GLint v[4])
{
    $glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, o[0]);
    $glViewport(v[0], v[1], v[2], v[3]);
}

//-----------------------------------------------------------------------------

}
