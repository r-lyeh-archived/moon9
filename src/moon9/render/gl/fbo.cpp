#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>

#include "ext.hpp"
#include "fbo.hpp"

namespace moon9
{

//-----------------------------------------------------------------------------

fbo::fbo(GLint color_format,
         GLint depth_format, GLsizei w, GLsizei h) : _w(w), _h(h)
{
    GLint o[1], v[4];

    get_framebuffer(o, v);

    glGenFramebuffersEXT(1, &frame);
    glGenTextures       (1, &color);
    glGenTextures       (1, &depth);

    // Initialize the color render buffer object.

    glBindTexture(GL_TEXTURE_2D, color);
    glTexImage2D (GL_TEXTURE_2D, 0, color_format, w, h, 0,
                  GL_RGBA, GL_INT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

    // Initialize the depth render buffer object.

    glBindTexture(GL_TEXTURE_2D, depth);
    glTexImage2D (GL_TEXTURE_2D, 0, depth_format, w, h, 0,
                  GL_DEPTH_COMPONENT, GL_INT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
                                   GL_COMPARE_R_TO_TEXTURE);

    // Initialize the frame buffer object.

    glBindFramebufferEXT     (GL_FRAMEBUFFER_EXT, frame);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                              GL_COLOR_ATTACHMENT0_EXT,
                              GL_TEXTURE_2D, color, 0);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                              GL_DEPTH_ATTACHMENT_EXT,
                              GL_TEXTURE_2D, depth, 0);

    // Confirm the frame buffer object status.

    switch (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT))
    {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
        throw std::runtime_error("Framebuffer incomplete attachment");
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
        throw std::runtime_error("Framebuffer missing attachment");
    case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
        throw std::runtime_error("Framebuffer duplicate attachment");
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        throw std::runtime_error("Framebuffer dimensions");
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        throw std::runtime_error("Framebuffer formats");
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
        throw std::runtime_error("Framebuffer draw buffer");
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
        throw std::runtime_error("Framebuffer read buffer");
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        throw std::runtime_error("Framebuffer unsupported");
    default:
        throw std::runtime_error("Framebuffer error");
    }

    // Zero the buffer.

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    set_framebuffer(o, v);

    GL_CHECK();
}

fbo::~fbo()
{
    glDeleteFramebuffersEXT(1, &frame);
    glDeleteTextures       (1, &depth);
    glDeleteTextures       (1, &color);

    GL_CHECK();
}

void fbo::bind_frame()
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame);
    glViewport(1, 1, _w - 2, _h - 2);
    glScissor (1, 1, _w - 2, _h - 2);

    GL_CHECK();
}

void fbo::bind_color(GLenum T)
{
    glActiveTexture(T);
    {
        glBindTexture(GL_TEXTURE_2D, color);
    }
    glActiveTexture(GL_TEXTURE0);

    GL_CHECK();
}

void fbo::bind_depth(GLenum T)
{
    glActiveTexture(T);
    {
        glBindTexture(GL_TEXTURE_2D, depth);
    }
    glActiveTexture(GL_TEXTURE0);

    GL_CHECK();
}

//-----------------------------------------------------------------------------

void get_framebuffer(GLint o[1], GLint v[4])
{
    glGetIntegerv(GL_VIEWPORT,                v);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, o);
}

void set_framebuffer(GLint o[1], GLint v[4])
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, o[0]);
    glViewport(v[0], v[1], v[2], v[3]);
}

//-----------------------------------------------------------------------------

}
