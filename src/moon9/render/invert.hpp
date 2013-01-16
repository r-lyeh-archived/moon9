#pragma once

#include <moon9/render/gl.hpp>
#include <moon9/render/nocopy.hpp>

namespace moon9
{
    namespace invert
    {
        struct normals : nocopy
        {
            normals()
            {
                glGetIntegerv( GL_FRONT_FACE, &mode );
                glFrontFace( mode == GL_CW ? GL_CCW : GL_CW );
            }

            ~normals()
            {
                glFrontFace( mode );
            }

            protected: GLint mode;
        };
    }
}
