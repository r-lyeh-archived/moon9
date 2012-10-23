#include <string>

#include <map>
#include <moon9/string/string.hpp>

namespace get
{
	extern std::string file;

	std::map<std::string,moon9::string>::const_iterator begin();
	std::map<std::string,moon9::string>::const_iterator end();

    std::string translated( const std::string &text );

    std::string s( const std::string &var );
         size_t z( const std::string &var );
         double d( const std::string &var );
          float f( const std::string &var );
           bool b( const std::string &var );
            int i( const std::string &var );
}
