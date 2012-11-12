#pragma once

#include "../os.hpp"

#include <string>
#include <map>

#include <glm/glm.hpp>

namespace moon9
{
    /*
    class resource
    {
        public:

            struct impl;
            impl *pImpl;
    };

    if( shader.resource.is_empty() )
        shader.resource.reload("file://minimal.shader.glsl"), // file, disk, server, local, cache, http, ftp...
        shader.setup();

    if( shader.resource.has_changed() )
        shader.resource.reload(),
        shader.setup();
    */


    class shader
    {
        public:

             shader();
            ~shader();

            // load()
            // make(), compile()
            // link()
            // use()

            std::string setup( const std::string &vss, const std::string &fss, const std::string &gss = std::string() );
            std::string setup( const std::string &sources );

            bool is_set() const;

            void enable();
            void disable();

            unsigned int get_program() const;
            std::string  get_log() const;

            GLint uniform( const std::string &name );
             void uniform( const std::string &name, float value );
             void uniform( const std::string &name, const glm::vec2 &vec );
             void uniform( const std::string &name, const glm::vec3 &vec );
             void uniform( const std::string &name, const glm::vec4 &vec );
             void uniform( const std::string &name, const glm::mat4 &mat );

            GLint attrib( const std::string &name );
            /*
             void attrib( const std::string &name, const glm::vec2 &vec );
             void attrib( const std::string &name, const glm::vec3 &vec );
             void attrib( const std::string &name, const glm::vec4 &vec ); */

        protected:

            unsigned int vs, fs, gs, prog;
            std::string vs_src, fs_src, gs_src;

            std::map< std::string, GLint > uniforms;
            std::map< std::string, GLint > attribs;

            std::string setup(); // returns an error string if failed
    };
}
