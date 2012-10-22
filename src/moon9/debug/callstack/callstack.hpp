#pragma once

#include <string>

namespace moon9
{
    class callstack
    {
        public:

            // save
            callstack();

            // print
            std::string str( const char *format12 = "#\1 \2\n", size_t skip_initial = 0 );

        private:

            enum { max_frames = 32 };
            void *frames[max_frames];
            size_t num_frames;
    };
}