/* Variable map class (supports hot loading)

 * todo:
 * - split into options (load/save) + constants (reload)

 * - rlyeh
 */

#pragma once

#include <iostream>
#include <map>
#include <string>

#include <stdlib.h>
#include <stdio.h>

#include "string.hpp"
#include "file.hpp"

namespace moon9
{
	class constants
	{
		typedef std::map<std::string,moon9::iostring> map_t;
		map_t map, used;

		std::string filename;

	public:

		constants( const std::string &_filename ) : filename( _filename )
		{
			update();
		}

		~constants()
		{
#ifndef NDEBUG
			for( map_t::const_iterator it = map.begin(); it != map.end(); ++it )
				if( !used[ it->first ].as<bool>() )
					fprintf( stdout, ( moon9::iostring() << "<moon9/play/constants.hpp> says: warning, unused constant '" << it->first << "'" << std::endl ).c_str() );
#endif
		}

		map_t::iterator begin() //const
		{
			return map.begin();
		}

		map_t::iterator end() //const
		{
			return map.end();
		}

		void update()
		{
			if( !moon9::file( filename ).has_changed() )
				return;

			moon9::iostrings lines = moon9::iostring( moon9::file( filename ).read() ).tokenize("\r\n");

			map = map_t();

			// parse lines

			for( size_t i = 0; i < lines.size(); ++i )
			{
				moon9::iostrings tokens = ( lines[i].left_of("//") ).tokenize("=");

				if( tokens.size() != 2 ) continue;

				// clean input

				tokens[0] = tokens[0].replace("\t", "").replace(" ", "");
				tokens[1] = moon9::iostrings( tokens[1].tokenize("\t ") ).str("\1 ");
				if( tokens[1].at(-1) == ' ' ) tokens[1].pop_back();

				if( tokens[0].size() >=2 && tokens[0].at(0) == '/' && tokens[0].at(1) == '/' ) continue;

				// assign

				map[ tokens[0] ] = tokens[1].eval(); // eval() in case its an expression
			}

#ifndef NDEBUG
			if( map["debug"].as<bool>() )
				std::cout << debug("vars {\n", "\t\1=\2\n", "}\n") << std::endl;
#endif
		}

		moon9::iostring get( const moon9::iostring &key ) //const
		{
			update();

			map[ key ] = map[ key ];

#ifndef NDEBUG
			used[ key ] = used[ key ];

			if( !map[ key ].size() )
				if( !used[ key ].size() )
					std::cout << "<moon9/play/constants.hpp> says: error, constant '" << key << "' not found!" << std::endl;

			used[ key ] = true;
#endif

			return map[ key ];
		}

		moon9::iostring operator[]( const moon9::iostring &key ) //const
		{
			return get( key );
		}

		std::string debug( const moon9::iostring &head = moon9::iostring(), const moon9::iostring &format12 = "\t\1=\2\n", const moon9::iostring &footer = moon9::iostring() ) //const
		{
			moon9::iostring body;

			for( map_t::const_iterator it = map.begin(); it != map.end(); ++it )
				body << moon9::iostring( format12, it->first, it->second );

			return head + body + footer;
		}
	};
}



/*

alternative

class options : public std::map<boo::string, boo::string>
{
    enum { verbose = false };

public:

    options()
    {}

    void read( std::istream &is, );
	void write( std::ostream &os, const std::string &header = std::string() );
};

*/
