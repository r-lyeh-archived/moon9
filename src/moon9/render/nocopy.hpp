#pragma once

namespace moon9
{
    struct nocopy
    {
        nocopy() {}
        protected: nocopy( const nocopy &other );
        protected: nocopy operator=( const nocopy &other );
    };
}
