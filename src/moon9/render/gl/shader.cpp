#include "ext.hpp"

#include <string>
#include <sstream>
#include <deque>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "shader.inl"

#define CHECK_GL() check_ogl(__FILE__,__LINE__)

namespace moon9
{
    namespace
    {
        void check_ogl(const char *file, int line)
        {
            return;

            GLenum err = glGetError();

            if (err != GL_NO_ERROR)
                std::cerr << file << ":"
                          << line << " "
                          << (const char *) gluErrorString(err) << std::endl;
        }
    }

    shader::shader() : vs(0), fs(0), gs(0), prog(0)
    {}

    shader::~shader()
    {
        if( prog )
        {
            if( vs )
                glDetachShader( prog, vs ),
                glDeleteShader( vs );
        CHECK_GL();

            if( fs )
                glDetachShader( prog, fs ),
                glDeleteShader( fs );
        CHECK_GL();

            if( gs )
                glDetachShader( prog, gs ),
                glDeleteShader( gs );
        CHECK_GL();

            glDeleteProgram( prog );
        CHECK_GL();
        }
    }

    shader::shader( const std::string &sources )
    {
        setup( sources );
    }

    shader::shader( const std::string &vss, const std::string &fss, const std::string &gss )
    {
        setup( vss, fss, gss );
    }

    bool shader::is_set() const
    {
        GLint success = 0;
        return prog > 0 && ( glGetProgramiv( prog, GL_LINK_STATUS, &success ), CHECK_GL(), success ? true : false );
    }

    std::string shader::setup( const std::string &vss, const std::string &fss, const std::string &gss )
    {
        return setup( to_string("#vertex\n\1\n#fragment\n\2\n#geometry\n\3\n", vss, fss, gss ) );
    }

    std::string shader::setup( const std::string &sources )
    {
        *this = shader(); // reset

        std::deque<std::string> common = tokenize( sources, "\r\n" );
        std::string *ptr = 0;

        while( common.size() )
        {
            std::string line = common.front();
            common.pop_front();

            if( starts_with( line, "#vertex" ) )
                ptr = &vs_src;
            else
            if( starts_with( line, "#fragment" ) )
                ptr = &fs_src;
            else
            if( starts_with( line, "#geometry" ) )
                ptr = &gs_src;
            else
            if( ptr )
                *ptr += line + "\n\n";
        }

        if( vs_src.size() == 0 || fs_src.size() == 0 ) // gs_src is optional
            return "Cannot determine size of the shader";

        return setup();
    }

    void shader::enable()
    {
        glUseProgram( prog );
        CHECK_GL();
    }

    void shader::disable()
    {
        glUseProgram( 0 );
        CHECK_GL();
    }

    unsigned int shader::get_program() const
    {
        return prog;
    }

    GLint shader::uniform( const std::string &name )
    {
        GLint coord = glGetUniformLocation( prog, name.c_str() );
        CHECK_GL();

        if( coord == -1 )
            std::cerr << to_string( "<moon9/render/shader.hpp> says: warning! Could not bind uniform '\1'\n", name ) << std::endl;

        ( uniforms[ name ] = uniforms[ name ] ) = coord;

        return coord;
    }
    void shader::uniform( const std::string &name, int value )
    {
        glUniform1i( uniform(name), GLint(value) );
        CHECK_GL();
    }
    void shader::uniform( const std::string &name, float value )
    {
        glUniform1f( uniform(name), GLfloat(value) );
        CHECK_GL();
    }
    void shader::uniform( const std::string &name, float value1, float value2 )
    {
        glUniform2f( uniform(name), GLfloat(value1), GLfloat(value2) );
        CHECK_GL();
    }
    void shader::uniform( const std::string &name, float value1, float value2, float value3 )
    {
        glUniform3f( uniform(name), GLfloat(value1), GLfloat(value2), GLfloat(value3) );
        CHECK_GL();
    }
    void shader::uniform( const std::string &name, float value1, float value2, float value3, float value4 )
    {
        glUniform4f( uniform(name), GLfloat(value1), GLfloat(value2), GLfloat(value3), GLfloat(value4) );
        CHECK_GL();
    }
    void shader::uniform( const std::string &name, const glm::vec2 &vec )
    {
        glUniform2fv( uniform(name), 1, glm::value_ptr(vec) );
        CHECK_GL();
    }
    void shader::uniform( const std::string &name, const glm::vec3 &vec )
    {
        glUniform3fv( uniform(name), 1, glm::value_ptr(vec) );
        CHECK_GL();
    }
    void shader::uniform( const std::string &name, const glm::vec4 &vec )
    {
        glUniform4fv( uniform(name), 1, glm::value_ptr(vec) );
        CHECK_GL();
    }
    void shader::uniform( const std::string &name, const glm::mat4 &mat )
    {
        glUniformMatrix4fv( uniform(name), 1, GL_FALSE, glm::value_ptr(mat) );
        CHECK_GL();
    }

    /*
    void shader::attrib( const char *attribName, int size, GLenum type, bool normalized, int stride, void *pointer )
    {
        int id = glGetAttribLocation(prog, attribName);
        if(id < 0)
            throw to_string("Failed to load attribute \1", attribName);

        glVertexAttribPointer(id, size, type, normalized, stride, pointer);
        glEnableVertexAttribArray(id);
    }
    */

    GLint shader::attrib( const std::string &name )
    {
        GLint coord = glGetAttribLocation( prog, name.c_str() );
        CHECK_GL();

        if( coord == -1 )
            std::cerr << to_string( "Warning: Could not bind attribute '\1'\n", name ) << std::endl;

        ( attribs[ name ] = attribs[ name ] ) = coord;

        return coord;
    }
    /*
    void shader::attrib( const std::string &name, const glm::vec2 &vec )
    {
        // @todo
        attrib( name );
    }
    void shader::attrib( const std::string &name, const glm::vec3 &vec )
    {
        // @todo
        attrib( name );
    }
    void shader::attrib( const std::string &name, const glm::vec4 &vec )
    {
        // @todo
        attrib( name );
    }
    */

    std::string shader::get_log() const
    {
        int size = 0, written = 0;

        std::string log;

        log += "Vertex: ";
        glGetShaderiv( vs, GL_INFO_LOG_LENGTH, &size );
        CHECK_GL();
        if( size > 0 )
        {
            char *text = new char [ size ];
            glGetShaderInfoLog( vs, size, &written, text );
        CHECK_GL();
            log += std::string( text );
            delete [] text;
        }
        log += '\n';

        log += "Fragment: ";
        glGetShaderiv( fs, GL_INFO_LOG_LENGTH, &size );
        CHECK_GL();
        if( size > 0 )
        {
            char *text = new char [ size ];
            glGetShaderInfoLog( fs, size, &written, text );
        CHECK_GL();
            log += std::string( text );
            delete [] text;
        }
        log += '\n';

        log += "Geometry: ";
        if( gs )
        {
            glGetShaderiv( gs, GL_INFO_LOG_LENGTH, &size );
        CHECK_GL();
            if( size > 0 )
            {
                char *text = new char [ size ];
                glGetShaderInfoLog( gs, size, &written, text );
        CHECK_GL();
                log += std::string( text );
                delete [] text;
            }
        }
        log += '\n';

        log += "Program: ";
        glGetProgramiv( prog, GL_INFO_LOG_LENGTH, &size );
        CHECK_GL();
        if( size > 0 )
        {
            char *text = new char [ size ];
            glGetProgramInfoLog( prog, size, &written, text );
        CHECK_GL();
            log += std::string( text );
            delete [] text;
        }
        log += '\n';

        return log;
    }

    std::string shader::setup() // returns an error string if failed
    {
        GLint success;

        // Create shaders, load source code into them and compile them

#if 0
        const GLchar* sources[] =
        {
            // Define GLSL version
            #ifdef GL_ES_VERSION_2_0
                "#version 100\n",

                // Define default float precision for fragment shaders:
                (type == GL_FRAGMENT_SHADER) ?
                "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
                "precision highp float;           \n"
                "#else                            \n"
                "precision mediump float;         \n"
                "#endif                           \n"
                : "",

                vs.c_str()
                // Note: OpenGL ES automatically defines this:
                // #define GL_ES
            #else
                "#version 120\n",

                // Ignore GLES 2 precision specifiers:
                "#define lowp   \n"
                "#define mediump\n"
                "#define highp  \n"

                vs.c_str()
            #endif
        };
        glShaderSource( vs, 3, &vs_src_ptr, NULL );
#endif

        const char *vs_src_ptr = vs_src.c_str();
        const char *fs_src_ptr = fs_src.c_str();
        const char *gs_src_ptr = gs_src.c_str();

        vs = glCreateShader( GL_VERTEX_SHADER );
        CHECK_GL();
        glShaderSource( vs, 1, &vs_src_ptr, NULL );
        CHECK_GL();
        glCompileShader( vs );
        CHECK_GL();
        glGetShaderiv( vs, GL_COMPILE_STATUS, &success );
        CHECK_GL();

        if( !success )
            return "Error: cant compile vertex shader";

        fs = glCreateShader( GL_FRAGMENT_SHADER );
        CHECK_GL();
        glShaderSource( fs, 1, &fs_src_ptr, NULL );
        CHECK_GL();
        glCompileShader( fs );
        CHECK_GL();
        glGetShaderiv( fs, GL_COMPILE_STATUS, &success );
        CHECK_GL();

        if( !success )
            return "Error: cant compile fragment shader";

        if( gs_src.size() > 0 )
        {
            gs = glCreateShader( GL_GEOMETRY_SHADER );
        CHECK_GL();
            glShaderSource( gs, 1, &gs_src_ptr, NULL );
        CHECK_GL();
            glCompileShader( gs );
        CHECK_GL();
            glGetShaderiv( gs, GL_COMPILE_STATUS, &success );
        CHECK_GL();

            if( !success )
                return "Error: cant compile geometry shader";
        }

        // Create a program object and attach all compiled shaders

        prog = glCreateProgram();
        CHECK_GL();
        glAttachShader( prog, vs );
        CHECK_GL();
        glAttachShader( prog, fs );
        CHECK_GL();

        if( gs )
            glAttachShader( prog, gs );
        CHECK_GL();

        // Link the program object

        glLinkProgram( prog );
        CHECK_GL();
        glGetProgramiv( prog, GL_LINK_STATUS, &success );
        CHECK_GL();

        if( !success )
            return "Error: cant link shader";

        // Ok

        return std::string();
    }
}



