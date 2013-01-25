#pragma once

#include "gl.hpp"

#include <string>
#include <map>

#include <glm/glm.hpp>

#include <moon9/render/nocopy.hpp>

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

            explicit
            shader( const std::string &sources );
            shader( const std::string &vss, const std::string &fss, const std::string &gss = std::string() );

            // load()
            // make(), compile()
            // link()
            // use()

            std::string setup( const std::string &vss, const std::string &fss, const std::string &gss = std::string() );
            std::string setup( const std::string &sources );

            bool is_set() const;

            void enable() const;
            void disable() const;

            unsigned int get_program() const;
            std::string  get_log() const;

            GLint uniform( const std::string &name ) const;
             void uniform( const std::string &name, int value ) const;
             void uniform( const std::string &name, float value ) const;
             void uniform( const std::string &name, float value1, float value2 ) const;
             void uniform( const std::string &name, float value1, float value2, float value3 ) const;
             void uniform( const std::string &name, float value1, float value2, float value3, float value4 ) const;
             void uniform( const std::string &name, const glm::vec2 &vec ) const;
             void uniform( const std::string &name, const glm::vec3 &vec ) const;
             void uniform( const std::string &name, const glm::vec4 &vec ) const;
             void uniform( const std::string &name, const glm::mat4 &mat ) const;

            GLint attrib( const std::string &name ) const;
            /*
             void attrib( const std::string &name, const glm::vec2 &vec ); const
             void attrib( const std::string &name, const glm::vec3 &vec ); const
             void attrib( const std::string &name, const glm::vec4 &vec ); const*/

        protected:

            unsigned int vs, fs, gs, prog;
            std::string vs_src, fs_src, gs_src;

            mutable std::map< std::string, GLint > uniforms;
            mutable std::map< std::string, GLint > attribs;

            std::string setup(); // returns an error string if failed
    };
}
