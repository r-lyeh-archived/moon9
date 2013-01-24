#pragma once

namespace moon9
{
    struct nocopy
    {
        protected: nocopy() {}
        private:   nocopy( const nocopy &other );
        private:   nocopy operator=( const nocopy &other );
    };
}
