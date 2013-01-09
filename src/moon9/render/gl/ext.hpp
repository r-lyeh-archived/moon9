#pragma once

#include <GL/glew.h>

#ifndef GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT 0x8CD8
#endif

void init_ogl();
void check_ogl(const char *, int);

#ifndef NDEBUG
#define GL_CHECK() check_ogl(__FILE__, __LINE__)
#else
#define GL_CHECK() {}
#endif
