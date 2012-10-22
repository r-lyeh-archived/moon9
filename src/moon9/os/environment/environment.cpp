// - rlyeh

#include <string>
#include <cstdlib>

namespace moon9
{
    std::string get_environment( const std::string &variable )
    {
        const char *r = std::getenv( variable.c_str() );
        return r ? r : std::string();
    }

    void set_environment( const std::string &variable, const std::string &value ); // @todo: to implement (useful?)
}
