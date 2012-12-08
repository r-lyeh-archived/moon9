#include <string>
#include <sstream>

#include <moon9/render/render.hpp>
#include <moon9/render/image.hpp>

#include "sprite.hpp"
#include "gif.hpp"

namespace moon9
{
    namespace loader
    {
        std::deque< moon9::texture * > gif( const std::string &filename, size_t frame_start, size_t frame_end )
        {
            std::deque< moon9::texture * > textures;

            for( size_t frame = frame_start; frame < frame_end; ++frame )
            {
                std::stringstream ss;
                ss << filename << "|" << frame;

                textures.push_back( &get_texture(ss.str()) );

                if( is_error_texture( textures.back() ) )
                {
                    textures.pop_back();
                    return textures;
                }
            }

            return textures;
        }
    }
}

