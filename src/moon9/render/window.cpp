#include <cstdlib>

#include <iostream>

#include "window.hpp"

const moon9::window2 *moon9::window2::active = 0;

namespace moon9
{
    void init_glut()
    {
        static struct once { once() {
            int argc = 0;
            glutInit( &argc, 0 );
            glfwInit();
            std::atexit( glfwTerminate );
        }} _1;
    }

    void init_glew()
    {
        static struct once { once() {
            // Initialize GLEW
            glewExperimental= GL_TRUE;
            GLenum err = glewInit ();
            if (GLEW_OK != err)
            {
                // Problem: glewInit failed, something is seriously wrong.
                std::cerr << "<moon9/render/window.cpp> says: Error: " << glewGetErrorString (err) << std::endl;
                std::exit(1);
            }
            // Print some infos about user's OpenGL implementation
            if(0)
            {
                std::cout << "OpenGL Version String: " << glGetString (GL_VERSION) << std::endl;
                std::cout << "GLU Version String: " << gluGetString (GLU_VERSION) << std::endl;
                std::cout << "GLEW Version String: " << glewGetString (GLEW_VERSION) <<std::endl;
            }
        }} _2;
    }
}
