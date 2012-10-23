#include <vector>
#include <cstdio>

//#include <fastlz/fastlz.h>

#include <iostream>
#include <iomanip>
#include <sstream>

#include "compress.hpp"

namespace moon9
{
    // compression methods

    bool is_z( const std::string &self )
    {
        return self.size() > 0 && self.at(0) == '\0';
    }

    bool is_unz( const std::string &self )
    {
        return !is_z( self );
    }

    std::string z( const std::string &self, bool highest_compression )
    {
        if( is_z( self ) )
            return self;

		if( !self.size() )
			return self;

        std::vector<char> binary;
        std::string input = self;

        // compress

        if( !lz4::compress( binary, input, highest_compression ) )
            return self;

        // encapsulated by exploiting std::string design (portable? standard?)
        // should i encapsulate by escaping characters instead? (safer but slower)

        std::ostringstream oss;

        oss << '\x01' <<
            std::setfill('0') << std::setw(8) << std::hex << input.size() <<
            std::setfill('0') << std::setw(8) << std::hex << binary.size();

        std::string output = oss.str();

        output.resize( 1 + 8 + 8 + binary.size() );

        std::memcpy( &output.at( 1 + 8 + 8 ), &binary.at( 0 ), binary.size() );

        output.at( 0 ) = '\0';

        return output;
    }

    std::string unz( const std::string &self )
    {
        if( is_z( self ) )
        {
            // decapsulate
            size_t size1, size2;

            std::stringstream ioss1;
            ioss1 << std::setfill('0') << std::setw(8) << std::hex << self.substr( 1, 8 );
            ioss1 >> size1;

            std::stringstream ioss2;
            ioss2 << std::setfill('0') << std::setw(8) << std::hex << self.substr( 9, 8 );
            ioss2 >> size2;

            std::string output;
            output.resize( size1 );

            std::vector<char> content( size2 );
            std::memcpy( &content.at( 0 ), &self.at( 17 ), size2 );

            // decompress

            if( lz4::decompress( output, content ) )
                return output;
        }

        return self;
    }


}

