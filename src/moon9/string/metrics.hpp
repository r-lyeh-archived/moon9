/*
 * Simple human-readable metrics classes.

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

 * - rlyeh ~~ listening to My Morning Jacket / One Big Holiday
 */


#pragma once

#include <iomanip>
#include <iostream>
#include <locale>
#include <map>
#include <sstream>
#include <string>

#include "format.hpp"

namespace moon9
{
    namespace metrics
    {
        class time : public std::string
        {
            std::string str( double value, int precision = 1 ) // input is given in seconds
            {
                if( value == 0 )
                {
                    return moon9::format( "%.*f s", precision, value );
                }

                if( value < 0.000001 )
                {
                    return moon9::format( "%.*f ns", precision = 0, value * 1000000000.0 ); // no decimals here
                }
                if( value < 0.001 )
                {
                    return moon9::format( "%.*f us", precision = 0, value * 1000000.0 ); // no decimals here
                }
                if( value < 0 )
                {
                    return moon9::format( "%.*f ms", precision = 0, value * 1000.0 ); // no decimals here
                }
                if( value < 60 )
                {
                    return moon9::format( "%.*f s", precision, value );
                }
                if( value < 3600 )
                {
                    return moon9::format( "%.*f min", precision, value / 60.0 );
                }
                if( value < 86400 )
                {
                    return moon9::format( "%.*f h", precision, value / 3600.0 );
                }
                if( value < 604800 )
                {
                    return moon9::format( "%.*f d", precision, value / 86400.0 );
                }
                if( value < 31536000 )
                {
                    return moon9::format( "%.*f w", precision, value / 604800.0 );
                }

                return moon9::format( "%.*f y", precision, value / 31536000.0 );
            }

            public:

            explicit time( double value, int precision = 1 ) : std::string( str(value, precision) )
            {}
        };

        class bits : public std::string
        {
            std::string str( double value, int precision = 2 ) // input is given in bytes
            {
                if( value == 0 )
                {
                    return moon9::format( "%.*f bytes", precision = 0, value ); // no decimals here
                }

                if( value < 0 )
                {
                    return moon9::format( "%.*f bits", precision = 0, value * 8.0 ); // no decimals here
                }
                if( value < 1024 )
                {
                    return moon9::format( "%.*f bytes", precision = 0, value ); // no decimals here
                }
                if( value < 1048576.0 )
                {
                    return moon9::format( "%.*f KB", precision, value / 1024.0 );
                }
                if( value < 1073741824.0 )
                {
                    return moon9::format( "%.*f MB", precision, value / 1048576.0 );
                }
                if( value < 1099511627776.0 )
                {
                    return moon9::format( "%.*f GB", precision, value / 1073741824.0 );
                }

                return moon9::format( "%.*f PB", precision, value / 1099511627776.0 );
            }

            public:

            explicit bits( double value, int precision = 2 ) : std::string( str(value, precision) )
            {}
        };

        class money : public std::string
        {
            std::string str( double value, bool show_decimals = true, bool show_currency_symbol = true, bool show_international_representation = false, const std::string &currency = "English_US" )
            {
                std::stringstream ss;

                std::locale usloc( currency.c_str() );

                std::cout.imbue(usloc);
                const std::money_put<char> &us_mon = std::use_facet<std::money_put<char> >(std::cout.getloc());

                us_mon.put(ss, show_international_representation, std::cout, ' ', value * 100 ); // mult by 100 because c++ defaults input as cents

                std::string ret = ss.str();

                if( !show_decimals )
                    ret = ret.substr( 0, ret.size() - 3 );

                if( show_currency_symbol )
                    return ret;

                std::string out;

                for( std::string::const_iterator it = ret.begin(), end = ret.end(); it != end; ++it )
                    if( (*it >= '0' && *it <= '9') || *it == ' ' || *it == ',' || *it == '.' )
                        out += *it;

                return out;
            }

            public:

            explicit money( double value, bool show_decimals = true, bool show_currency_symbol = true, bool show_international_representation = false, const std::string &currency = "English_US" ) : std::string( str(value,show_decimals,show_currency_symbol,show_international_representation,currency) )
            {}
        };

        class decimate : public std::string
        {
            std::string str( double quantity, size_t digits = 3, char thousands_separator = ',', char decimal_separator = '.' )
            {
               // note: function does not round decimal digits [ decimate(128.1256, 3) becomes "128.125" ]

               std::locale lc( "" );

               std::stringstream ss;
               ss.imbue( lc );

               ss << std::setprecision(20);

               std::use_facet< std::num_put<char> >( lc ).put(ss, ss, ' ', quantity );

               char ot = std::use_facet< std::numpunct<char> >(lc).thousands_sep();
               char od = std::use_facet< std::numpunct<char> >(lc).decimal_point();

               std::string result = ss.str();

               if( ot != thousands_separator || od != decimal_separator )
                   for( std::string::iterator it = result.begin(), end = result.end(); it != end ; ++it )
                       if( *it == ot ) *it = thousands_separator; else if( *it == od ) *it = decimal_separator;

               if( digits )
               {
                   size_t mark_pos = result.find_first_of( decimal_separator );

                   if( mark_pos != std::string::npos && mark_pos + digits + 1 < result.size() ) // +1 to skip decimal mark
                       return result.substr( 0, mark_pos + digits + 1 );
               }

               return result;
            }

        public:

            explicit decimate( double value, size_t digits = 3, char thousands_separator = ',', char decimal_separator = '.' ) : std::string( str(value,digits,thousands_separator,decimal_separator) )
            {}
        };
    }
}

