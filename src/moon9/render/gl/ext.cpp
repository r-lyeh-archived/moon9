#include "ext.hpp"

#include <iostream>

void init_ogl()
{
    // glewInit();
    // glutInit();
    // glfwInit();
}

void check_ogl(const char *file, int line)
{
    GLenum err = glGetError();

    if (err != GL_NO_ERROR)
        std::cerr << file << ":"
                  << line << " "
                  << (const char *) gluErrorString(err) << std::endl;
}

