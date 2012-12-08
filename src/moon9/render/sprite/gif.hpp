#pragma once

#include <moon9/render/texture.hpp>
#include <deque>

namespace moon9
{
    namespace loader
    {
        std::deque< moon9::texture * > gif( const std::string &filename, size_t frame_start = 0, size_t frame_end = ~0 );
    }
}
