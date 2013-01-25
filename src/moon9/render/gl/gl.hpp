#pragma once

namespace moon9
{
    void init_ogl();
    void check_ogl(const char *file, int line);

    template<typename T>
    T check_ogl( const char *file, int line, const T &t )
    {
        check_ogl(file,line);
        return t;
    }
}

#if defined(NDEBUG) || defined(_NDEBUG)
#define MOON9_GL_0(...) (__VA_ARGS__)
#define MOON9_GL_N(...) (__VA_ARGS__)
#else
#define MOON9_GL_0(...) ((__VA_ARGS__), moon9::check_ogl(__FILE__, __LINE__))
#define MOON9_GL_N(...) moon9::check_ogl( __FILE__, __LINE__, (__VA_ARGS__) )
#endif

#define $glCreateProgram(...)           MOON9_GL_N( glCreateProgram(__VA_ARGS__) )
#define $glCreateShader(...)            MOON9_GL_N( glCreateShader(__VA_ARGS__) )
#define $glGetAttribLocation(...)       MOON9_GL_N( glGetAttribLocation(__VA_ARGS__) )
#define $glGetFragDataLocation(...)     MOON9_GL_N( glGetFragDataLocation(__VA_ARGS__) )
#define $glGetUniformLocation(...)      MOON9_GL_N( glGetUniformLocation(__VA_ARGS__) )
#define $glIsBuffer(...)                MOON9_GL_N( glIsBuffer(__VA_ARGS__) )
#define $glIsProgram(...)               MOON9_GL_N( glIsProgram(__VA_ARGS__) )
#define $glIsQuery(...)                 MOON9_GL_N( glIsQuery(__VA_ARGS__) )
#define $glIsShader(...)                MOON9_GL_N( glIsShader(__VA_ARGS__) )
#define $glIsVertexArray(...)           MOON9_GL_N( glIsVertexArray(__VA_ARGS__) )
#define $glMapBuffer(...)               MOON9_GL_N( glMapBuffer(__VA_ARGS__) )
#define $glUnmapBuffer(...)             MOON9_GL_N( glUnmapBuffer(__VA_ARGS__) )
#define $gluBuild2DMipmaps(...)         MOON9_GL_N( gluBuild2DMipmaps(__VA_ARGS__) )

#define $glActiveTexture(...)           MOON9_GL_0( glActiveTexture(__VA_ARGS__) )
#define $glActiveTextureARB(...)        MOON9_GL_0( glActiveTextureARB(__VA_ARGS__) )
#define $glAlphaFunc(...)               MOON9_GL_0( glAlphaFunc(__VA_ARGS__) )
#define $glAttachShader(...)            MOON9_GL_0( glAttachShader(__VA_ARGS__) )
#define $glBegin(...)                   MOON9_GL_0( glBegin(__VA_ARGS__) )
#define $glBindBuffer(...)              MOON9_GL_0( glBindBuffer(__VA_ARGS__) )
#define $glBindFramebufferEXT(...)      MOON9_GL_0( glBindFramebufferEXT(__VA_ARGS__) )
#define $glBindTexture(...)             MOON9_GL_0( glBindTexture(__VA_ARGS__) )
#define $glBlendFunc(...)               MOON9_GL_0( glBlendFunc(__VA_ARGS__) )
#define $glBufferData(...)              MOON9_GL_0( glBufferData(__VA_ARGS__) )
#define $glClear(...)                   MOON9_GL_0( glClear(__VA_ARGS__) )
#define $glClearColor(...)              MOON9_GL_0( glClearColor(__VA_ARGS__) )
#define $glColorMask(...)               MOON9_GL_0( glColorMask(__VA_ARGS__) )
#define $glCompileShader(...)           MOON9_GL_0( glCompileShader(__VA_ARGS__) )
#define $glDeleteBuffers(...)           MOON9_GL_0( glDeleteBuffers(__VA_ARGS__) )
#define $glDeleteFramebuffersEXT(...)   MOON9_GL_0( glDeleteFramebuffersEXT(__VA_ARGS__) )
#define $glDeleteProgram(...)           MOON9_GL_0( glDeleteProgram(__VA_ARGS__) )
#define $glDeleteShader(...)            MOON9_GL_0( glDeleteShader(__VA_ARGS__) )
#define $glDeleteTextures(...)          MOON9_GL_0( glDeleteTextures(__VA_ARGS__) )
#define $glDetachShader(...)            MOON9_GL_0( glDetachShader(__VA_ARGS__) )
#define $glDisable(...)                 MOON9_GL_0( glDisable(__VA_ARGS__) )
#define $glEnable(...)                  MOON9_GL_0( glEnable(__VA_ARGS__) )
#define $glEnd(...)                     MOON9_GL_0( glEnd(__VA_ARGS__) )
#define $glFogf(...)                    MOON9_GL_0( glFogf(__VA_ARGS__) )
#define $glFogfv(...)                   MOON9_GL_0( glFogfv(__VA_ARGS__) )
#define $glFogi(...)                    MOON9_GL_0( glFogi(__VA_ARGS__) )
#define $glFramebufferTexture2DEXT(...) MOON9_GL_0( glFramebufferTexture2DEXT(__VA_ARGS__) )
#define $glFrustum(...)                 MOON9_GL_0( glFrustum(__VA_ARGS__) )
#define $glGenBuffers(...)              MOON9_GL_0( glGenBuffers(__VA_ARGS__) )
#define $glGenFramebuffersEXT(...)      MOON9_GL_0( glGenFramebuffersEXT(__VA_ARGS__) )
#define $glGenTextures(...)             MOON9_GL_0( glGenTextures(__VA_ARGS__) )
#define $glGetIntegerv(...)             MOON9_GL_0( glGetIntegerv(__VA_ARGS__) )
#define $glGetProgramInfoLog(...)       MOON9_GL_0( glGetProgramInfoLog(__VA_ARGS__) )
#define $glGetProgramiv(...)            MOON9_GL_0( glGetProgramiv(__VA_ARGS__) )
#define $glGetShaderInfoLog(...)        MOON9_GL_0( glGetShaderInfoLog(__VA_ARGS__) )
#define $glGetShaderiv(...)             MOON9_GL_0( glGetShaderiv(__VA_ARGS__) )
#define $glLightfv(...)                 MOON9_GL_0( glLightfv(__VA_ARGS__) )
#define $glLightModelfv(...)            MOON9_GL_0( glLightModelfv(__VA_ARGS__) )
#define $glLinkProgram(...)             MOON9_GL_0( glLinkProgram(__VA_ARGS__) )
#define $glLoadIdentity(...)            MOON9_GL_0( glLoadIdentity(__VA_ARGS__) )
#define $glMatrixMode(...)              MOON9_GL_0( glMatrixMode(__VA_ARGS__) )
#define $glMultMatrixf(...)             MOON9_GL_0( glMultMatrixf(__VA_ARGS__) )
#define $glOrtho(...)                   MOON9_GL_0( glOrtho(__VA_ARGS__) )
#define $glPixelStorei(...)             MOON9_GL_0( glPixelStorei(__VA_ARGS__) )
#define $glPolygonMode(...)             MOON9_GL_0( glPolygonMode(__VA_ARGS__) )
#define $glPolygonOffset(...)           MOON9_GL_0( glPolygonOffset(__VA_ARGS__) )
#define $glPopAttrib(...)               MOON9_GL_0( glPopAttrib(__VA_ARGS__) )
#define $glPopMatrix(...)               MOON9_GL_0( glPopMatrix(__VA_ARGS__) )
#define $glPushAttrib(...)              MOON9_GL_0( glPushAttrib(__VA_ARGS__) )
#define $glPushMatrix(...)              MOON9_GL_0( glPushMatrix(__VA_ARGS__) )
#define $glReadPixels(...)              MOON9_GL_0( glReadPixels(__VA_ARGS__) )
#define $glRotatef(...)                 MOON9_GL_0( glRotatef(__VA_ARGS__) )
#define $glScalef(...)                  MOON9_GL_0( glScalef(__VA_ARGS__) )
#define $glScissor(...)                 MOON9_GL_0( glScissor(__VA_ARGS__) )
#define $glShaderSource(...)            MOON9_GL_0( glShaderSource(__VA_ARGS__) )
#define $glTexCoord2i(...)              MOON9_GL_0( glTexCoord2i(__VA_ARGS__) )
#define $glTexCoord2f(...)              MOON9_GL_0( glTexCoord2f(__VA_ARGS__) )
#define $glTexImage2D(...)              MOON9_GL_0( glTexImage2D(__VA_ARGS__) )
#define $glTexParameteri(...)           MOON9_GL_0( glTexParameteri(__VA_ARGS__) )
#define $glTranslatef(...)              MOON9_GL_0( glTranslatef(__VA_ARGS__) )
#define $glUniform1f(...)               MOON9_GL_0( glUniform1f(__VA_ARGS__) )
#define $glUniform1i(...)               MOON9_GL_0( glUniform1i(__VA_ARGS__) )
#define $glUniform2f(...)               MOON9_GL_0( glUniform2f(__VA_ARGS__) )
#define $glUniform2fv(...)              MOON9_GL_0( glUniform2fv(__VA_ARGS__) )
#define $glUniform2i(...)               MOON9_GL_0( glUniform2i(__VA_ARGS__) )
#define $glUniform2iv(...)              MOON9_GL_0( glUniform2iv(__VA_ARGS__) )
#define $glUniform3f(...)               MOON9_GL_0( glUniform3f(__VA_ARGS__) )
#define $glUniform3fv(...)              MOON9_GL_0( glUniform3fv(__VA_ARGS__) )
#define $glUniform3i(...)               MOON9_GL_0( glUniform3i(__VA_ARGS__) )
#define $glUniform3iv(...)              MOON9_GL_0( glUniform3iv(__VA_ARGS__) )
#define $glUniform4f(...)               MOON9_GL_0( glUniform4f(__VA_ARGS__) )
#define $glUniform4fv(...)              MOON9_GL_0( glUniform4fv(__VA_ARGS__) )
#define $glUniform4i(...)               MOON9_GL_0( glUniform4i(__VA_ARGS__) )
#define $glUniform4iv(...)              MOON9_GL_0( glUniform4iv(__VA_ARGS__) )
#define $glUniformMatrix4fv(...)        MOON9_GL_0( glUniformMatrix4fv(__VA_ARGS__) )
#define $glUseProgram(...)              MOON9_GL_0( glUseProgram(__VA_ARGS__) )
#define $glVertex3f(...)                MOON9_GL_0( glVertex3f(__VA_ARGS__) )
#define $glViewport(...)                MOON9_GL_0( glViewport(__VA_ARGS__) )

#include "ext.hpp"
#include "shader.hpp"
#include "vbo.hpp"
#include "fbo.hpp"

// context
// window
// light
// mesh
// sprite
// snapshot
// camera
