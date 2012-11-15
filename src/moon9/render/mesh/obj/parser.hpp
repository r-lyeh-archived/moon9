#pragma once

#include <moon9/render/render.hpp>
#include <moon9/render/mesh.hpp>

namespace moon9
{
    namespace lightwave
    {
        bool import( moon9::mesh &self, const std::string &pathfile );
        void render( const moon9::mesh &mesh );
    }
}
