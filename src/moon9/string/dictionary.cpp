/*
 * Simple dictionary/localization class (i18n/L10n/g11n)

 * Copyright (c) 2011 Mario 'rlyeh' Rodriguez
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.

 * to do:
 * - avoid dead loops by asserting for bad recursive input data
 * - let user define the symbol format rather than sticking with [symbol]
 * - easy locale support { order (1st/º), currency, decimation (.,),
 *   measurement units [km/mile], time [12/24h], etc }

 * - rlyeh ~~ listening to The Essence / Gone
 */

#include <iostream>
#include <string>
#include <map>

#include "string.hpp"
#include "dictionary.hpp"

void moon9::dictionary::set_symbol( const std::string &text, const std::string &translation )
{
	(*this)[ moon9::string( "[\1]", text ) ] = translation;
}

std::string moon9::dictionary::translate( const std::string &text ) const
{
	moon9::strings out, tokens = moon9::string( text ).tokenize_incl_separators("[]");

	bool open = false;

	for( size_t i = 0; i < tokens.size(); ++i )
	{
		if( tokens[i] == "[" || tokens[i] == "]" )
			{ open ^= true; continue; }

		out.push_back( tokens[i] );

		if( open )
		{
			const_iterator found = this->find( moon9::string( "[\1]", out.back() ) );

			if( found != this->end() )
				out.back() = translate( found -> second );
		}
	}

	return out.str("\1");
}

std::string moon9::dictionary::translate( const std::string &text, const moon9::string &arg1 ) const
{
	return translate( moon9::string( translate( text ), arg1 ) );
}

std::string moon9::dictionary::translate( const std::string &text, const moon9::string &arg1, const moon9::string &arg2 ) const
{
	return translate( moon9::string( translate( text ), arg1, arg2 ) );
}

std::string moon9::dictionary::translate( const std::string &text, const moon9::string &arg1, const moon9::string &arg2, const moon9::string &arg3 ) const
{
	return translate( moon9::string( translate( text ), arg1, arg2, arg3 ) );
}

std::string moon9::dictionary::translate( const std::string &text, const moon9::string &arg1, const moon9::string &arg2, const moon9::string &arg3, const moon9::string &arg4 ) const
{
	return translate( moon9::string( translate( text ), arg1, arg2, arg3, arg4 ) );
}

std::string moon9::dictionary::translate( const std::string &text, const moon9::string &arg1, const moon9::string &arg2, const moon9::string &arg3, const moon9::string &arg4, const moon9::string &arg5 ) const
{
	return translate( moon9::string( translate( text ), arg1, arg2, arg3, arg4, arg5 ) );
}

std::string moon9::dictionary::translate( const std::string &text, const moon9::string &arg1, const moon9::string &arg2, const moon9::string &arg3, const moon9::string &arg4, const moon9::string &arg5, const moon9::string &arg6 ) const
{
	return translate( moon9::string( translate( text ), arg1, arg2, arg3, arg4, arg5, arg6 ) );
}

std::string moon9::dictionary::translate( const std::string &text, const moon9::string &arg1, const moon9::string &arg2, const moon9::string &arg3, const moon9::string &arg4, const moon9::string &arg5, const moon9::string &arg6, const moon9::string &arg7 ) const
{
	return translate( moon9::string( translate( text ), arg1, arg2, arg3, arg4, arg5, arg6, arg7 ) );
}

// debug

std::string moon9::dictionary::debug( std::string format12 ) const
{
	moon9::string out;

	for( const_iterator it = this->begin(); it != this->end(); ++it )
	{
		out << moon9::string( format12, it->first, it->second );
	}

	return out;
}

/*

// serialization

#include <moon9/io/xml.hpp>

void moon9::dictionary::import_xml( const std::string &xml_content, const std::string &xpath_root, const std::string &attr_id, const std::string &attr_ln )
{
	//clear
	//this->clear();

	//parse
	moon9::xml xml;
	xml.str( xml_content );

	//fill
	for( size_t entry = 1, entries = xml.query<size_t>( moon9::string("count(\1)", xpath_root) ); entry <= entries; ++entry )
	{
		std::string id = xml.query<std::string>( moon9::string( "\1[\2]/@\3", xpath_root, entry, attr_id ) );
		std::string ln = xml.query<std::string>( moon9::string( "\1[\2]/@\3", xpath_root, entry, attr_ln ) );

		(*this)[ id ] = ln;
	}
}

std::string moon9::dictionary::export_xml( const std::string &xpath_root, const std::string &attr_id, const std::string &attr_ln ) const
{
	moon9::strings path = moon9::string( xpath_root ).tokenize("/");
	moon9::string out( "<?xml version=\"1.0\"?>\r\n" );

	out << moon9::string("<\1>\r\n", path[0]);

	for( const_iterator it = this->begin(); it != this->end(); ++it )
		out << moon9::string( "\t<\1 \2=\"\3\" \4=\"\5\" />\r\n", path[1], attr_id, it->first, attr_ln, it->second );

	out << moon9::string("</\1>\r\n", path[0]);

	return out;
}

*/
