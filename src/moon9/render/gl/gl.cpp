#include "gl.hpp"

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>

#include <GL/glew.h>
#include <GL/glu.h>

#include <moon9/debug/callstack.hpp>
#include <moon9/debug/debugger.hpp>
#include <moon9/debug/die.hpp>

namespace moon9
{
    void init_ogl()
    {
    //    glewInit();
    //    glutInit();
    //    glfwInit();
    }

    void check_ogl(const char *file, int line)
    {
        GLenum err = glGetError();

        if (err != GL_NO_ERROR)
        {
            const char *error = (const char *) gluErrorString(err);

            std::stringstream hex, ss;

            // ridiculous C++
            hex << "0x" << std::hex << std::setw(4) << std::setfill('0') << err;

            ss << "OpenGL error #" << hex.str();
            ss << ( error ? std::string(": ") + error : std::string("") ) << "; ";
            ss << "seen at " << file << ":" << line << std::endl;
            ss << moon9::callstack().str() << std::endl;

            std::cerr << ss.str() << std::endl;

            debugger(ss.str());
            die(-1);
        }
    }
}
