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

#pragma once

#include <string>
#include <map>

#include "string.hpp"


namespace moon9
{
	class dictionary : public std::map< std::string /* text */, std::string /* translated text */ >
	{
		public:

		void set_symbol( const std::string &text, const std::string &translation );

		std::string translate( const std::string &text ) const;
		std::string translate( const std::string &text, const moon9::string &arg1 ) const;
		std::string translate( const std::string &text, const moon9::string &arg1, const moon9::string &arg2 ) const;
		std::string translate( const std::string &text, const moon9::string &arg1, const moon9::string &arg2, const moon9::string &arg3 ) const;
		std::string translate( const std::string &text, const moon9::string &arg1, const moon9::string &arg2, const moon9::string &arg3, const moon9::string &arg4 ) const;
		std::string translate( const std::string &text, const moon9::string &arg1, const moon9::string &arg2, const moon9::string &arg3, const moon9::string &arg4, const moon9::string &arg5 ) const;
		std::string translate( const std::string &text, const moon9::string &arg1, const moon9::string &arg2, const moon9::string &arg3, const moon9::string &arg4, const moon9::string &arg5, const moon9::string &arg6 ) const;
		std::string translate( const std::string &text, const moon9::string &arg1, const moon9::string &arg2, const moon9::string &arg3, const moon9::string &arg4, const moon9::string &arg5, const moon9::string &arg6, const moon9::string &arg7 ) const;

		// debug

		std::string debug( std::string format12 = "\t\1\t=\t\2\n" ) const;

/*
		// serialization

		void import_xml( const std::string &xml_content, const std::string &xpath_root = "/dictionary/entry", const std::string &attr_id = "id", const std::string &attr_ln = "en" );

		std::string export_xml( const std::string &xpath_root = "/dictionary/entry", const std::string &attr_id = "id", const std::string &attr_ln = "en" ) const;
*/
	};
}

