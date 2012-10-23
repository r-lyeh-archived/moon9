/*
 * Extended C++ standard strings & variants
 * Copyright (c) 2010-2011, Mario 'rlyeh' Rodriguez

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

 * todo:
 * - Geszaltz -> delta
 * - regex (~ http://www.pcre.org/) // std::regex
 * - base conversions: hex2dec, dec2hex, bin2dec, dec2bin, bin2hex, etc...
 * - color conversions?
 * - locale/unicode class
 * - no full std::string manipulator support (useful?)
 * - // http://www.velocityreviews.com/forums/t291605-unicode-in-c.html
 * - // http://www.codeproject.com/KB/stl/upgradingstlappstounicode.aspx
 * - a few python functions seem handy (check https://github.com/imageworks/pystring/blob/master/pystring.h)
 * - expreval()
 * - tokenize( delimiters, delimiter_stride = 1, max_tokens = 0 ) -> "\r\n\f\t", 1 ; "/**\", 2 ; "//", 2 ; etc
 * - regexp: parse( "this_is_a_func(){ hello world; }", ".*{(.*)}" ) -> returns [1] = " hello world; "

 * further improvements:
 * - Boyer-Moore -> fast search (useful?)

 * - rlyeh
 */

#pragma once

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstring>

#include <algorithm>
#include <cctype>       // std::toupper
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>    // std::out_of_range

#include <deque>
#include <map>
#include <string>
#include <vector>

//#include <stdint.h>
//#include <inttypes.h>

//#include <moon9/core/directives.hpp>
#include "hexdump/format/format.hpp"

//#include <type_traits>  // template <T> is_pod


namespace moon9
{
    namespace detail
    {
        struct tokenize // iago gayoso's tokenizer
        {
            size_t index;
            bool hole, escape;
            std::string fsm, word;

            tokenize( const std::string& str = std::string() ) : index(0), hole(false), escape(false), fsm(str)
            {}

            void init()
            {
                index = 0;
                word = "";
                escape = hole = false;
            }

            void transition( const char symbol )
            {
                word += symbol;

                if( !is_hole() && !is_accepted() )
                {
                    if( fsm[ index ] == '*' )
                    {
                        while( index != std::string::npos && fsm[ index ] == '*' )
                            index++;

                        escape = true;
                    }

                    if( fsm[ index ] == symbol )
                    {
                        index++;
                        escape = false;
                    }
                    else hole = !escape;
                }
            }

            bool is_accepted() const
            {
                struct local
                {
                    static bool match( const char *pattern, const char *str )
                    {
                        if( *pattern=='\0' ) return !*str;
                        if( *pattern=='*' )  return match(pattern+1, str) || *str && match(pattern, str+1);
                        if( *pattern=='?' )  return *str && (*str != '.') && match(pattern+1, str+1);
                        return (*str == *pattern) && match(pattern+1, str+1);
                    }
                };

                return local::match( fsm.c_str(), word.c_str() );
            }

            bool is_hole() const
            {
                return hole;
            }

            size_t get_word_size() const
            {
                return word.size();
            }
        };
    }

    class string : public std::string
    {
        public:

        // default constructors

        string() : std::string()
        {}

        string( const std::string &s ) : std::string( s )
        {}

        string( const char &c, size_t n ) : std::string( c, n )
        {}

        /*
        experimental {
        */

        // safe formatted constructor
        // style: moon9::string("hello \1,\2 and \3", a,b,c)

        // experimental: type safe formatted constructors by using octal escape characters (are they really useful for anyone?)
        // to do:
        // - should i use %1, %2, %3... %N instead ? this helps avoiding octal numbers, but might be confusing as we're not
        //   supporting printf modificators like %s, %f, etc. compromise: \\1, \\2, \\3, \\4... \\N ?
        // - these run-time replacements (->replace()) may be expensive. optimize.

        #define kArg( N, _tN ) \
        { std::stringstream oss; oss << t##N; *this = this->replace(_tN, oss.str() ); }

        template< typename T1 > string( const std::string &fmt, const T1 &t1 ) : std::string(fmt) {
            kArg(1,"\1");
        }
        template< typename T1, typename T2 > string( const std::string &fmt, const T1 &t1, const T2 &t2 ) : std::string(fmt) {
            kArg(1,"\1"); kArg(2,"\2");
        }
        template< typename T1, typename T2, typename T3 > string( const std::string &fmt, const T1 &t1, const T2 &t2, const T3 &t3 ) : std::string(fmt) {
            kArg(1,"\1"); kArg(2,"\2"); kArg(3,"\3");
        }
        template< typename T1, typename T2, typename T3, typename T4 > string( const std::string &fmt, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4 ) : std::string(fmt) {
            kArg(1,"\1"); kArg(2,"\2"); kArg(3,"\3"); kArg(4, "\4");
        }
        template< typename T1, typename T2, typename T3, typename T4, typename T5 > string( const std::string &fmt, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5 ) : std::string(fmt) {
            kArg(1,"\1"); kArg(2,"\2"); kArg(3,"\3"); kArg(4, "\4"); kArg(5, "\5");
        }
        template< typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 > string( const std::string &fmt, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6 ) : std::string(fmt) {
            kArg(1,"\1"); kArg(2,"\2"); kArg(3,"\3"); kArg(4, "\4"); kArg(5, "\5"); kArg(6, "\6");
        }
        template< typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7 > string( const std::string &fmt, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7 ) : std::string(fmt) {
            kArg(1,"\1"); kArg(2,"\2"); kArg(3,"\3"); kArg(4, "\4"); kArg(5, "\5"); kArg(6, "\6"); kArg(7, "\7");
        }

        #undef kArg


        // experimental: Constructors & operator<< for generic pointers, function pointers and method pointers

        #define kBooStringUnrollConstructor( ptr_name, ptr_type ) \
        { \
            (*this) = moon9::format("%d:", sizeof( ptr_type ) * 2 ); \
            for( size_t i = 0; i < sizeof( ptr_type ); i += sizeof( ptr_type ) ) \
                (*this) += moon9::format("%0.*X", 2 * sizeof(size_t), *(size_t *)( size_t(&ptr_name) + i ) ); \
            (*this) += ";"; \
        }

        string( const void *generic_ptr ) kBooStringUnrollConstructor( generic_ptr, const void * )

        template <typename RETURN_TYPE>
        string( RETURN_TYPE (*ptr_to_function)() ) kBooStringUnrollConstructor( ptr_to_function, ptr_to_function )
        template <typename RETURN_TYPE, typename A0>
        string( RETURN_TYPE (*ptr_to_function)(A0) ) kBooStringUnrollConstructor( ptr_to_function, ptr_to_function )
        template <typename RETURN_TYPE, typename A0, typename A1>
        string( RETURN_TYPE (*ptr_to_function)(A0,A1) ) kBooStringUnrollConstructor( ptr_to_function, ptr_to_function )

        template <typename CLASS_TYPE, typename RETURN_TYPE>
        string( RETURN_TYPE (CLASS_TYPE::*ptr_to_method)() ) kBooStringUnrollConstructor( ptr_to_method, ptr_to_method )
        template <typename CLASS_TYPE, typename RETURN_TYPE, typename A0>
        string( RETURN_TYPE (CLASS_TYPE::*ptr_to_method)(A0) ) kBooStringUnrollConstructor( ptr_to_method, ptr_to_method )
        template <typename CLASS_TYPE, typename RETURN_TYPE, typename A0, typename A1>
        string( RETURN_TYPE (CLASS_TYPE::*ptr_to_method)(A0,A1) ) kBooStringUnrollConstructor( ptr_to_method, ptr_to_method )

        #undef kBooStringUnrollConstructor


        /* } */

        // specialized POD constructors

	private:

		// from: http://tinodidriksen.com/2010/02/07/cpp-convert-int-to-string-speed/
		template<typename T>
		std::string naive_u(T t) {
			std::string rv(std::numeric_limits<T>::digits10+2, 0);
			size_t i = 0;
			if (!t) {
				rv[i++] = '0';
			}
			else {
				size_t ro = 0;
				for (T b ; t ; t = b) {
					b = t/10;
					T c = t%10;
					rv[i++] = static_cast<char>('0' + c);
				}
				std::reverse(&rv[ro], &rv[i]);
			}
			rv.resize(i);
			return rv;
		}

		template<typename T>
		std::string naive_s(T t) {
			std::string rv(std::numeric_limits<T>::digits10+2, 0);
			size_t i = 0;
			if (!t) {
				rv[i++] = '0';
			}
			else {
				size_t ro = 0;
				if (t < 0) {
					rv[i++] = '-';
					t = ( t < 0 ? -t : t );
					ro = 1;
				}
				for (T b ; t ; t = b) {
					b = t/10;
					T c = t%10;
					rv[i++] = static_cast<char>('0' + c);
				}
				std::reverse(&rv[ro], &rv[i]);
			}
			rv.resize(i);
			return rv;
		}

	public:

        string(           const bool& val ) : std::string( val ? "true" : "false" ) {}
#if 0
        string(          const short& val ) : std::string() { std::stringstream ss; ss << val; ss >> (*this); }
        string( const unsigned short& val ) : std::string() { std::stringstream ss; ss << val; ss >> (*this); }
        string(            const int& val ) : std::string() { std::stringstream ss; ss << val; ss >> (*this); }
        string(   const unsigned int& val ) : std::string() { std::stringstream ss; ss << val; ss >> (*this); }
        string(           const long& val ) : std::string() { std::stringstream ss; ss << val; ss >> (*this); }
        string(  const unsigned long& val ) : std::string() { std::stringstream ss; ss << val; ss >> (*this); }
#else
        string(          const short& val ) : std::string( naive_s(val) ) {}
        string( const unsigned short& val ) : std::string( naive_u(val) ) {}
        string(            const int& val ) : std::string( naive_s(val) ) {}
        string(   const unsigned int& val ) : std::string( naive_u(val) ) {}
        string(           const long& val ) : std::string( naive_s(val) ) {}
        string(  const unsigned long& val ) : std::string( naive_u(val) ) {}
#endif
		string(          const float& val ) : std::string() { std::stringstream ss; ss.precision( 20 ); ss << val; ss >> (*this); } // set more precision than a float can handle
        string(         const double& val ) : std::string() { std::stringstream ss; ss.precision( 20 ); ss << val; ss >> (*this); } // set more precision than a double can handle
        string(    const long double& val ) : std::string() { std::stringstream ss; ss.precision( 99 ); ss << val; ss >> (*this); } // set more precision than a long double can handle
        //string(           const void* ptr ) { std::stringstream ss; ss << ptr; ss >> (*this); }

        ////string( const std::streambuf* sb ) {}

        ////string( const std::ostream& ( *pf )(std::ostream&)) {}
        ////string( const std::ios& ( *pf )(std::ios&)) {}
        ////string( const std::ios_base& ( *pf )(std::ios_base&)) {}

        string(          const char c ) : std::string() { (*this) = moon9::format("%c", c); }         // std::stringstream mejor?
        string(   const signed char c ) : std::string() { (*this) = moon9::format("%d", (int)c); }    // std::stringstream mejor?
        string( const unsigned char c ) : std::string() { (*this) = moon9::format("%d", (int)c); }    // std::stringstream mejor?

        string( const char *cstr ) : std::string( cstr ) {}
        //string( const signed char *cstr ) : std::string( (const char *)cstr ) {}
        //string( const unsigned char *cstr ) : std::string( (const char *)cstr ) {}

        template< typename T >
        string( const T& t ) : std::string()
        {
            std::stringstream ss;
            ss << t;
            this->assign( ss.str() );
        }

#if 0

        operator            bool() const { return this->size() && *this != "false" && *this != "0"; }
        operator           short() const { std::stringstream ss( *this );          short t; return ( ss >> t ? t : (         short)(operator bool()) ); }
        operator  unsigned short() const { std::stringstream ss( *this ); unsigned short t; return ( ss >> t ? t : (unsigned short)(operator bool()) ); }
        operator             int() const { std::stringstream ss( *this );            int t; return ( ss >> t ? t : (           int)(operator bool()) ); }
        operator    unsigned int() const { std::stringstream ss( *this );   unsigned int t; return ( ss >> t ? t : (  unsigned int)(operator bool()) ); }
        operator            long() const { std::stringstream ss( *this );           long t; return ( ss >> t ? t : (          long)(operator bool()) ); }
        operator   unsigned long() const { std::stringstream ss( *this );  unsigned long t; return ( ss >> t ? t : (unsigned  long)(operator bool()) ); }
        operator           float() const { std::stringstream ss( *this );          float t; return ( ss >> t ? t : (         float)(operator bool()) ); }
        operator          double() const { std::stringstream ss( *this );         double t; return ( ss >> t ? t : (        double)(operator bool()) ); }
        operator     long double() const { std::stringstream ss( *this );    long double t; return ( ss >> t ? t : (   long double)(operator bool()) ); }
        ////operator     const void*() const { return this->c_str(); }

        #if defined(_MSC_VER) && !defined(_M_X64)
        // msvc 2008 64bit: size_t is a type
        // msvc 2008 32bit: size_t is unsigned int (grrr! why?)
        //operator          size_t() const { std::stringstream ss( *this );    size_t t; return ( ss >> t ? t : (   size_t)(operator bool()) ); }
        #endif

        #if !defined( size_t ) && defined(_MSC_VER) && defined(_M_X64)
        operator          size_t() const { std::stringstream ss( *this );    size_t t; return ( ss >> t ? t : (   size_t)(operator bool()) ); }
        #endif

        ////operator std::streambuf*() const { std::streambuf* sb; }

        ////operator   std::ostream&() const { std::ostream& ( *pf )(std::ostream&); }
        ////operator       std::ios&() const { std::ios& ( *pf )(std::ios&); }
        ////operator  std::ios_base&() const { std::ios_base& ( *pf )(std::ios_base&); }

        operator            char() const { return (char)(operator int()); }
        operator     signed char() const { return (signed char)(operator int()); }
        operator   unsigned char() const { return (unsigned char)(operator int()); }

        ////operator           char*() const { char* s; std::stringstream ss; ss << s; ss >> t; }
        ////operator    signed char*() const { signed char* s; std::stringstream ss; ss << s; ss >> t; }
        ////operator  unsigned char*() const { unsigned char* s; std::stringstream ss; ss << s; ss >> t; }

        operator     const char*() const { return this->c_str(); }

#else

        operator bool() const { return this->size() && *this != "false" && *this != "0"; }

        template<typename T>
        operator T() const { std::stringstream ss( *this ); T t; return ( ss >> t ? t : (T)(operator bool()) );  }

        ////operator std::streambuf*() const { std::streambuf* sb; }

        ////operator   std::ostream&() const { std::ostream& ( *pf )(std::ostream&); }
        ////operator       std::ios&() const { std::ios& ( *pf )(std::ios&); }
        ////operator  std::ios_base&() const { std::ios_base& ( *pf )(std::ios_base&); }

        operator char() const { return (char)(as<int>()); }
        operator signed char() const { return (signed char)(as<int>()); }
        operator unsigned char() const { return (unsigned char)(as<int>()); }

        ////operator           char*() const { char* s; std::stringstream ss; ss << s; ss >> t; }
        ////operator    signed char*() const { signed char* s; std::stringstream ss; ss << s; ss >> t; }
        ////operator  unsigned char*() const { unsigned char* s; std::stringstream ss; ss << s; ss >> t; }

        //template<const char *>
        operator const char *() const { return this->c_str(); }

#endif

        // chaining operators

        template <typename T>
        moon9::string &operator <<( const T &t )
        {
            std::ostringstream oss;
            oss << moon9::string(t);
            (*this) += oss.str();

            return *this;
        }

        moon9::string &operator <<( std::ostream& ( *pf )(std::ostream&))
        {
            return *pf == static_cast<std::ostream& ( * )(std::ostream&)>( std::endl ) ? (*this) += "\n", *this : *this;
        }

        // handy conversion method

        template <typename T>
        T as() const
        {
            return (T)(*this);
        }

        // extra methods

        const char &at( const int &pos ) const
        {
            //classic behaviour: "hello"[5] = undefined, "hello"[-1] = undefined
            //return this->std::string::at( pos );

            //extended behaviour: "hello"[5] = h, "hello"[-1] = o,
            int size = this->size();
            if( !size ) //throw std::out_of_range("moon9::string::at() empty string");
			{ static std::map< const moon9::string *, char > map; return ( ( map[ this ] = map[ this ] ) = '\0' ); }
            return this->data()[ pos >= 0 ? pos % size : size - 1 + ((pos+1) % size) ];
        }

        char &at( const int &pos )
        {
            //classic behaviour: "hello"[5] = undefined, "hello"[-1] = undefined
            //return this->std::string::at( pos );

            //extended behaviour: "hello"[5] = h, "hello"[-1] = o,
            int size = this->size();
            if( !size ) // std::out_of_range("moon9::string::at() empty string");
			{ static std::map< const moon9::string *, char > map; return ( ( map[ this ] = map[ this ] ) = '\0' ); }
            return const_cast<char &>( this->data()[ pos >= 0 ? pos % size : size - 1 + ((pos+1) % size) ] );
        }

        void pop_back()
        {
            if( this->size() )
                this->erase( this->end() - 1 );
        }

        void pop_front()
        {
            if( this->size() )
                this->erase( 0, 1 ); //this->substr( 1 ); //this->assign( this->begin() + 1, this->end() );
        }

        const char &back() const
        {
            return at(-1);
        }

        char &back()
        {
            return at(-1);
        }

        const char &front() const
        {
            return at(0);
        }

        char &front()
        {
            return at(0);
        }

        size_t count( const std::string &substr ) const
        {
            size_t n = 0;
            std::string::size_type pos = 0;
            while( (pos = this->find( substr, pos )) != std::string::npos ) {
                n++;
                pos += substr.size();
            }
            return n;
        }

        std::deque< moon9::string > parse( const std::vector< std::string >& delimiters ) const
        {
            std::vector< detail::tokenize > vFSM;
            std::deque< moon9::string > tokens;

            size_t base = 0, index = 0, size = (*this).size();

            for( size_t i = 0; i < delimiters.size(); i++ )
                vFSM.push_back( detail::tokenize( delimiters[i] ) );

            while( index < size )
            {
                for( size_t i = 0; i < vFSM.size(); i++ )
                    vFSM[i].transition( (*this).std::string::at(index) );

                index++;

                bool accepted = false;
                for( size_t i = 0; i < vFSM.size() && !accepted; i++ )
                    if( vFSM[ i ].is_accepted() )
                    {
                        tokens.push_back( (*this).substr( base, (index - vFSM[i].get_word_size()) - base) );
                        tokens.push_back( (*this).substr( index - vFSM[i].get_word_size(), vFSM[i].get_word_size() ) );

                        for( size_t j = 0; j < vFSM.size(); j++ )
                            vFSM[j].init();

                        accepted = true;
                        base = index;
                    }

                if( !accepted )
                {
                    bool blackhole = true;
                    for( size_t i = 0; i < vFSM.size() && blackhole; i++ )
                        blackhole &= vFSM[i].is_hole();

                    if( blackhole )
                    {
                        for( size_t i = 0; i < vFSM.size(); i++ )
                            vFSM[i].init();
                    }
                }
            }

            if( base < index )
                tokens.push_back( (*this).substr( base, index - base) );

            return tokens;
        }

        std::deque< moon9::string > tokenize( const std::string& delimiters, int maxtokens = 0 ) const
        {
            std::deque< moon9::string > tokens;

            // Skip delimiters at beginning.
            std::string::size_type lastPos = this->find_first_not_of(delimiters, 0);
            // Find first "non-delimiter".
            std::string::size_type pos     = this->find_first_of(delimiters, lastPos);

            while (std::string::npos != pos || std::string::npos != lastPos)
            {
                // Found a token, add it to the deque.
                tokens.push_back(this->substr(lastPos, pos - lastPos));
                // Skip delimiters.  Note the "not_of"
                lastPos = this->find_first_not_of(delimiters, pos);
                // Find next "non-delimiter"
                pos = this->find_first_of(delimiters, lastPos);

                if( ! (--maxtokens) )
                {
                    if( std::string::npos != lastPos )
                        tokens.push_back( this->substr(lastPos, std::string::npos) );

                    return tokens;
                }
            }

            return tokens;
        }

        std::deque< moon9::string > tokenize_incl_separators/*split*/( const std::string& delimiters ) const
        {
            std::deque< moon9::string > tokens;
            std::string str;

            for( size_t i = 0; i < this->size(); ++i )
            {
                char c = this->operator[](i);

                if( delimiters.find_first_of( c ) == std::string::npos )
                {
                    str += c;
                }
                else
                {
                    if( str.size() )
                        tokens.push_back( str );

                    tokens.push_back( c );

                    str = "";
                }
            }

            tokens.push_back( str );
            return tokens;
        }

        moon9::string replace( const std::string &target, const std::string &replacement ) const
        {
            std::string s = *this;
            size_t found = 0;

            while( ( found = s.find( target, found ) ) != string::npos )
            {
                s.replace( found, target.length(), replacement );
                found += replacement.length();
            }

            return s;
        }

        // to do: specialize for target_t == char || replacement_t == char

        moon9::string replace_map( const std::map< std::string, std::string > &replacements ) const
        {
            moon9::string out;

            for( size_t i = 0; i < this->size(); )
            {
                bool found = false;
                size_t match_length = 0;

                std::map< std::string, std::string >::const_reverse_iterator it;
                for( it = replacements.rbegin(); !found && it != replacements.rend(); ++it )
                {
                    const std::string &target = it->first;
                    const std::string &replacement = it->second;

                    if( match_length != target.size() )
                        match_length = target.size();

                    if( this->size() - i >= target.size() )
                    if( !memcmp( &this->at(i), &target.at(0), match_length ) )
                    {
                        i += target.size();

                        out += replacement;

                        found = true;
                    }
                }

                if( !found )
                    out += this->at(i++);
            }

           return out;
        }

        moon9::string eval() const;

#if 1

        moon9::string escape() const
        {
            std::map< std::string, std::string > map;

            map[ "\t" ] = "%09";
            map[ "\r" ] = "%0A";
            map[ "\n" ] = "%0D";

            map[ " " ] = "%20";
            map[ "<" ] = "%3C";
            map[ ">" ] = "%3E";
            map[ "#" ] = "%23";
            //map[ "%" ] = "%25";
            map[ "{" ] = "%7B";
            map[ "}" ] = "%6D";
            map[ "|" ] = "%7C";
            map[ "\\" ] = "%5C";
            map[ "^" ] = "%5E";
            map[ "~" ] = "%7E";

            map[ "[" ] = "%5B";
            map[ "]" ] = "%5D";
            map[ "`" ] = "%60";
            map[ ";" ] = "%3B";
            map[ "/" ] = "%2F";
            map[ "?" ] = "%3F";
            map[ ":" ] = "%3A";
            map[ "@" ] = "%40";
            map[ "=" ] = "%3D";
            map[ "&" ] = "%26";
            map[ "$" ] = "%24";

            return replace_map( map );

            moon9::string s = this->replace( "%",  "%25" );

            s = s.replace( "\t", "%09" );
            s = s.replace( "\r", "%0A" );
            s = s.replace( "\n", "%0D" );

            s = s.replace( " ", "%20" );
            s = s.replace( "<", "%3C" );
            s = s.replace( ">", "%3E" );
            s = s.replace( "#", "%23" );
            //s = s.replace( "%", "%25" );
            s = s.replace( "{", "%7B" );
            s = s.replace( "}", "%6D" );
            s = s.replace( "|", "%7C" );
            s = s.replace( "\\", "%5C" );
            s = s.replace( "^", "%5E" );
            s = s.replace( "~", "%7E" );

            s = s.replace( "[", "%5B" );
            s = s.replace( "]", "%5D" );
            s = s.replace( "`", "%60" );
            s = s.replace( ";", "%3B" );
            s = s.replace( "/", "%2F" );
            s = s.replace( "?", "%3F" );
            s = s.replace( ":", "%3A" );
            s = s.replace( "@", "%40" );
            s = s.replace( "=", "%3D" );
            s = s.replace( "&", "%26" );
            s = s.replace( "$", "%24" );

            return s;
        }

        moon9::string unescape() const
        {
            std::map< std::string, std::string > map;

            map[ "%09" ] = "\t";
            map[ "%0A" ] = "\r";
            map[ "%0D" ] = "\n";

            map[ "%20" ] = " ";
            map[ "%3C" ] = "<";
            map[ "%3E" ] = ">";
            map[ "%23" ] = "#";
            //map[ "%25" ] = "%";
            map[ "%7B" ] = "{";
            map[ "%6D" ] = "}";
            map[ "%7C" ] = "|";
            map[ "%5C" ] = "\\";
            map[ "%5E" ] = "^";
            map[ "%7E" ] = "~";

            map[ "%5B" ] = "[";
            map[ "%5D" ] = "]";
            map[ "%60" ] = "`";
            map[ "%3B" ] = ";";
            map[ "%2F" ] = "/";
            map[ "%3F" ] = "?";
            map[ "%3A" ] = ":";
            map[ "%40" ] = "@";
            map[ "%3D" ] = "=";
            map[ "%26" ] = "&";
            map[ "%24" ] = "$";

            map[ "%25" ] = "%";

            return replace_map( map );

            moon9::string s = *this;

            s = s.replace( "%09", "\t" );
            s = s.replace( "%0A", "\r" );
            s = s.replace( "%0D", "\n" );

            s = s.replace( "%20", " " );
            s = s.replace( "%3C", "<" );
            s = s.replace( "%3E", ">" );
            s = s.replace( "%23", "#" );
            //s = s.replace( "%25", "%");
            s = s.replace( "%7B", "{" );
            s = s.replace( "%6D", "}" );
            s = s.replace( "%7C", "|" );
            s = s.replace( "%5C", "\\" );
            s = s.replace( "%5E", "^" );
            s = s.replace( "%7E", "~" );

            s = s.replace( "%5B", "[" );
            s = s.replace( "%5D", "]" );
            s = s.replace( "%60", "`" );
            s = s.replace( "%3B", ";" );
            s = s.replace( "%2F", "/" );
            s = s.replace( "%3F", "?" );
            s = s.replace( "%3A", ":" );
            s = s.replace( "%40", "@" );
            s = s.replace( "%3D", "=" );
            s = s.replace( "%26", "&" );
            s = s.replace( "%24", "$" );

            s = s.replace( "%25", "%" );

            return s;
        }

#else

        private:

        struct escape_utils
        {
        // Based on code from https://github.com/brianmario/escape_utils/blob/master/ext/escape_utils.c
        // Copyright (c) 2010 Brian Lopez - http://github.com/brianmario

        /*
        #define IS_HEX(c)   ( (c >= '0' || c <= '9') && (c >= 'A' || c <= 'F') && (c >= 'a' || c <= 'f') )
        #define NOT_HEX(c)  ( (c <  '0' || c >  '9') && (c <  'A' || c >  'Z') && (c <  'a' || c >  'z') )
        #define UNHEX(c)    ( (c >= '0' && c <= '9' ? c - '0' : c >= 'A' && c <= 'F' ? c - 'A' + 10 : c - 'a' + 10) )
        #define URI_SAFE(c) ( (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '-' || c == '.' || c == '_' || c == '~' )
        */

        #define IS_IN(c,min,max) ( c >= min && c <= max )
        #define IS_DIGIT(c)      ( IS_IN(c,'0','9') )
        #define IS_ALPHA(c)      ( IS_IN(c,'A','Z') || IS_IN(c,'a','z') )
        #define IS_HEX(c)        ( IS_IN(c,'A','F') || IS_IN(c,'a','f') || IS_DIGIT(c) )
        #define IS_ALPHADIGIT(c) ( IS_ALPHA(c) || IS_DIGIT(c) )
        #define IS_URI_SAFE(c)   ( IS_DIGIT(c) || IS_ALPHA(c) || c == '-' || c == '.' || c == '_' || c == '~' )
        #define UNHEX(c)         ( IS_DIGIT(c) ? c - '0' : c - ( c >= 'A' ? 'A' : 'a' ) + 10 )

        static size_t escape_html(char *out, const char *in, size_t in_len, bool secure = false) { /* size*=5 */
          size_t total = 0;
          char curChar;

          total = in_len;
          while (in_len) {
            curChar = *in++;

            if (curChar == '<') {
              *out++ = '&'; *out++ = 'l'; *out++ = 't'; *out++ = ';';
              total += 3;
            } else if (curChar == '>') {
              *out++ = '&'; *out++ = 'g'; *out++ = 't'; *out++ = ';';
              total += 3;
            } else if (curChar == '&') {
              *out++ = '&'; *out++ = 'a'; *out++ = 'm'; *out++ = 'p'; *out++ = ';';
              total += 4;
            } else if (curChar == '\'') {
              *out++ = '&'; *out++ = '#'; *out++ = '3'; *out++ = '9'; *out++ = ';';
              total += 4;
            } else if (curChar == '\"') {
              *out++ = '&'; *out++ = 'q'; *out++ = 'u'; *out++ = 'o'; *out++ = 't'; *out++ = ';';
              total += 5;
            } else if (secure && curChar == '/') {
              *out++ = '&'; *out++ = '#'; *out++ = '4'; *out++ = '7'; *out++ = ';';
              total += 4;
            } else {
              *out++ = curChar;
            }
            in_len--;
          }

          return total;
        }

        static size_t unescape_html(char *out, const char *in, int in_len) { /* size*=1 */
          size_t total = 0, len = in_len;
          char curChar, *start;

          start = (char *)&in[0];
          total = in_len;
          while (len) {
            curChar = *in++;
            if (curChar == '&') {
              if ((in-start)+2 <= in_len && *in == 'l' && *(in+1) == 't' && *(in+2) == ';') {
                *out++ = '<';
                total-=3;
                in+=3;
                len-=3;
              } else if ((in-start)+2 <= in_len && *in == 'g' && *(in+1) == 't' && *(in+2) == ';') {
                *out++ = '>';
                total-=3;
                in+=3;
                len-=3;
              } else if ((in-start)+3 <= in_len && *in == 'a' && *(in+1) == 'm' && *(in+2) == 'p' && *(in+3) == ';') {
                *out++ = '&';
                total-=4;
                in+=4;
                len-=4;
              } else if ((in-start)+3 <= in_len && *in == '#' && *(in+1) == '3' && *(in+2) == '9' && *(in+3) == ';') {
                *out++ = '\'';
                total-=4;
                in+=4;
                len-=4;
              } else if ((in-start)+3 <= in_len && *in == '#' && *(in+1) == '4' && *(in+2) == '7' && *(in+3) == ';') {
                *out++ = '/';
                total-=4;
                in+=4;
                len-=4;
              } else if ((in-start)+4 <= in_len && *in == 'q' && *(in+1) == 'u' && *(in+2) == 'o' && *(in+3) == 't' && *(in+4) == ';') {
                *out++ = '\"';
                total-=5;
                in+=5;
                len-=5;
              }
            } else {
              *out++ = curChar;
            }
            len--;
          }

          return total;
        }

        static size_t escape_javascript(char *out, const char *in, int in_len) { /* size*=2 */
          size_t total = 0;
          char curChar;

          total = in_len;
          while (in_len) {
            curChar = *in++;
            switch (curChar) {
            case '\\':
              *out++ = '\\'; *out++ = '\\';
              total++;
              break;
            case '<':
              *out++ = '<';
              if (*in == '/') {
                *out++ = '\\'; *out++ = '/';
                in++; in_len--;
                total++;
              }
              break;
            case '\r':
              if (*in == '\n') {
                *out++ = '\\'; *out++ = 'n';
                in++; in_len--;
              } else {
                *out++ = '\\'; *out++ = 'n';
                total++;
              }
              break;
            case '\n':
              *out++ = '\\'; *out++ = 'n';
              total++;
              break;
            case '\'':
              *out++ = '\\'; *out++ = '\'';
              total++;
              break;
            case '\"':
              *out++ = '\\'; *out++ = '\"';
              total++;
              break;
            default:
              *out++ = curChar;
              break;
            }
            in_len--;
          }

          return total;
        }

        static size_t unescape_javascript(char *out, const char *in, int in_len) { /* size*=1 */
          size_t total = 0;
          char curChar;

          total = in_len;
          while (in_len) {
            curChar = *in++;
            if (curChar == '\\') {
              if (*in == 'n') {
                *out++ = '\n';
                total--;
              } else if (*in == '\\') {
                *out++ = '\\';
                total--;
              } else if (*in == '\'') {
                *out++ = '\'';
                total--;
              } else if (*in == '\"') {
                *out++ = '\"';
                total--;
              } else if (*in == '/') {
                *out++ = '/';
                total--;
              } else {
                *out++ = curChar;
              }
              in++; in_len--;
            } else {
              *out++ = curChar;
            }
            in_len--;
          }

          return total;
        }

        static size_t escape_url(char *out, const char *in, int in_len) { /* size*=3 */
          static const char hexChars[] = "0123456789ABCDEF";
          size_t total = 0;
          char curChar, hex[2];

          total = in_len;
          while (in_len) {
            curChar = *in++;
            if (curChar == ' ') {
              *out++ = '+';
            } else if ((curChar != '_' && curChar != '.' && curChar != '-') && (!IS_ALPHADIGIT(curChar)) ) {
              hex[1] = hexChars[curChar & 0x0f];
              hex[0] = hexChars[(curChar >> 4) & 0x0f];
              *out++ = '%'; *out++ = hex[0]; *out++ = hex[1];
              total += 2;
            } else {
              *out++ = curChar;
            }
            in_len--;
          }

          return total;
        }

        static size_t unescape_url(char *out, const char *in, int in_len) { /* size*=1 */
          size_t total = 0, len = in_len;
          char curChar, *start;

          start = (char *)&in[0];
          total = in_len;
          while (len) {
            curChar = *in++;
            if (curChar == '%') {
              if ((in-start)+2 <= in_len && IS_HEX(*in) && IS_HEX(*(in+1))) {
                *out++ = (UNHEX(*in) << 4) + UNHEX(*(in+1));
                in+=2;
                total-=2;
              }
            } else if (curChar == '+') {
              *out++ = ' ';
            } else {
              *out++ = curChar;
            }
            len--;
          }

          return total;
        }

        static size_t escape_uri(char *out, const char *in, int in_len) {  /* size*=3 */
          static const char hexChars[] = "0123456789ABCDEF";
          size_t total = 0;
          char curChar, hex[2];

          total = in_len;
          while (in_len) {
            curChar = *in++;
            if (IS_URI_SAFE(curChar)) {
                *out++ = curChar;
            } else {
                hex[1] = hexChars[curChar & 0x0f];
                hex[0] = hexChars[(curChar >> 4) & 0x0f];
                *out++ = '%'; *out++ = hex[0]; *out++ = hex[1];
                total += 2;
            }
            in_len--;
          }

          return total;
        }

        static size_t unescape_uri(char *out, const char *in, int in_len) {    /* size*=1 */
          size_t total = 0, len = in_len;
          char curChar, *start;

          start = (char *)&in[0];
          total = in_len;
          while (len) {
            curChar = *in++;
            if (curChar == '%') {
              if ((in-start)+2 <= in_len && IS_HEX(*in) && IS_HEX(*(in+1))) {
                *out++ = (UNHEX(*in) << 4) + UNHEX(*(in+1));
                in+=2;
                total-=2;
              }
            } else {
              *out++ = curChar;
            }
            len--;
          }

          return total;
        }

        #undef IS_IN
        #undef IS_DIGIT
        #undef IS_ALPHA
        #undef IS_HEX
        #undef IS_ALPHADIGIT
        #undef IS_URI_SAFE
        #undef UNHEX

        };

        public:

        moon9::string escape() const
        {
            std::string s;

            s.resize( this->size() * 3 );

            s.resize( escape_utils::escape_uri( &s.at(0), this->c_str(), this->size() ) );

            return s;
        }

        moon9::string unescape() const
        {
            std::string s;

            s.resize( this->size() * 1 );

            s.resize( escape_utils::unescape_uri( &s.at(0), this->c_str(), this->size() ) );

            return s;
        }

#endif

        bool match( const std::string &pattern ) const
        {
            struct local
            {
                static bool match( const char *pattern, const char *str )
                {
                    if( *pattern=='\0' ) return !*str;
                    if( *pattern=='*' )  return match(pattern+1, str) || *str && match(pattern, str+1);
                    if( *pattern=='?' )  return *str && (*str != '.') && match(pattern+1, str+1);
                    return (*str == *pattern) && match(pattern+1, str+1);
                }
            };

            return local::match( pattern.c_str(), (*this).c_str() );
        }

        moon9::string uppercase() const
        {
            std::string s = *this;

            std::transform( s.begin(), s.end(), s.begin(), (int(*)(int)) std::toupper );

            return s;
        }

        moon9::string lowercase() const
        {
            std::string s = *this;

            std::transform( s.begin(), s.end(), s.begin(), (int(*)(int)) std::tolower );

            return s;
        }

        private:

        moon9::string strip( const std::string &chars, bool strip_left, bool strip_right ) const
        {
            std::string::size_type len = this->size(), i = 0, j = len, charslen = chars.size();

            if( charslen == 0 )
            {
                if( strip_left )
                    while( i < len && std::isspace( this->operator[]( i ) ))
                        i++;

                if( strip_right && j ) {
                    do j--; while( j >= i && std::isspace( this->operator[]( j ) ));
                    j++;
                }
            }
            else
            {
                const char *sep = chars.c_str();

                if( strip_left )
                    while( i < len && std::memchr( sep, this->operator[]( i ), charslen ))
                        i++;

                if( strip_right && j ) {
                    do j--; while( j >= i && std::memchr( sep, this->operator[]( j ), charslen ));
                    j++;
                }
            }

            if( j - i == len ) return *this;
        return this->substr( i, j - i );
        }

        public: // based on python string and pystring

        // Return a copy of the string with leading characters removed (default chars: space)
        moon9::string lstrip( const std::string &chars = std::string() ) const
        {
            return strip( chars, true, false );
        }
        moon9::string ltrim( const std::string &chars = std::string() ) const
        {
            return strip( chars, true, false );
        }

        // Return a copy of the string with trailing characters removed (default chars: space)
        moon9::string rstrip( const std::string &chars = std::string() ) const
        {
            return strip( chars, false, true );
        }
        moon9::string rtrim( const std::string &chars = std::string() ) const
        {
            return strip( chars, false, true );
        }

        // Return a copy of the string with both leading and trailing characters removed (default chars: space)
        moon9::string strip( const std::string &chars = std::string() ) const
        {
            return strip( chars, true, true );
        }
        moon9::string trim( const std::string &chars = std::string() ) const
        {
            return strip( chars, true, true );
        }

        bool starts_with( const moon9::string &prefix ) const
        {
            return this->size() >= prefix.size() ? this->substr( 0, prefix.size() ) == prefix : false;
        }

        bool starts_with( const moon9::string &prefix, bool is_case_sensitive ) const
        {
            return is_case_sensitive ? starts_with( prefix ) : uppercase().starts_with( prefix.uppercase() );
        }

        bool ends_with( const moon9::string &suffix ) const
        {
            return this->size() < suffix.size() ? false : this->substr( this->size() - suffix.size() ) == suffix;
        }

        bool ends_with( const moon9::string &suffix, bool is_case_sensitive ) const
        {
            return is_case_sensitive ? ends_with( suffix ) : uppercase().ends_with( suffix.uppercase() );
        }

        moon9::string left_of( const moon9::string &substring ) const
        {
            std::string::size_type pos = this->find( substring );
            return pos == std::string::npos ? *this : this->substr(0, pos);
        }

        moon9::string right_of( const moon9::string &substring ) const
        {
            std::string::size_type pos = this->find( substring );
            return pos == std::string::npos ? *this : this->substr(pos + 1); //, pos + substring.size());
        }

        moon9::string &operator <<( const void *generic_ptr )
        {
            *this += moon9::string( generic_ptr );
            return *this;
        }

        template <typename RETURN_TYPE>
        moon9::string &operator <<( RETURN_TYPE (*ptr_to_function)() ) { *this += moon9::string( ptr_to_function ); return *this; }
        template <typename RETURN_TYPE, typename A0>
        moon9::string &operator <<( RETURN_TYPE (*ptr_to_function)(A0) ) { *this += moon9::string( ptr_to_function ); return *this; }
        template <typename RETURN_TYPE, typename A0, typename A1>
        moon9::string &operator <<( RETURN_TYPE (*ptr_to_function)(A0,A1) ) { *this += moon9::string( ptr_to_function ); return *this; }

        template <typename CLASS_TYPE, typename RETURN_TYPE>
        moon9::string &operator <<( RETURN_TYPE (CLASS_TYPE::*ptr_to_method)() ) { *this += moon9::string( ptr_to_method ); return *this; }
        template <typename CLASS_TYPE, typename RETURN_TYPE, typename A0>
        moon9::string &operator <<( RETURN_TYPE (CLASS_TYPE::*ptr_to_method)(A0) ) { *this += moon9::string( ptr_to_method ); return *this; }
        template <typename CLASS_TYPE, typename RETURN_TYPE, typename A0, typename A1>
        moon9::string &operator <<( RETURN_TYPE (CLASS_TYPE::*ptr_to_method)(A0,A1) ) { *this += moon9::string( ptr_to_method ); return *this; }
    };

    // @todo: move me to <moon9/string/strings.h> file (good idea?)

    class strings : public std::deque< moon9::string >
    {
        public:

            strings()
            {}

            template <typename contained>
            strings( const std::deque<contained> &c )
            {
                operator=( c );
            }

            template <typename contained>
            strings &operator =( const std::deque<contained> &c )
            {
                this->resize( c.size() );

                std::copy( c.begin(), c.end(), this->begin() );

                return *this;
            }

            template <typename contained>
            strings( const std::vector<contained> &c )
            {
                operator=( c );
            }

            template <typename contained>
            strings &operator =( const std::vector<contained> &c )
            {
                this->resize( c.size() );

                std::copy( c.begin(), c.end(), this->begin() );

                return *this;
            }

            strings( const int &argc, const char **&argv )
            {
                for( int i = 0; i < argc; ++i )
                    this->push_back( argv[i] );
            }

            strings( const int &argc, char **&argv )
            {
                for( int i = 0; i < argc; ++i )
                    this->push_back( argv[i] );
            }

            template< typename T, const size_t N >
            strings( const T (&args)[N] )
            {
                this->resize( N );
                for( int n = 0; n < N; ++n )
                    (*this)[ n ] = args[ n ];
            }

            template< typename T > strings( const T &t0, const T &t1 )
            { this->resize(2); (*this)[0] = t0; (*this)[1] = t1; }
            template< typename T > strings( const T &t0, const T &t1, const T &t2 )
            { this->resize(3); (*this)[0] = t0; (*this)[1] = t1; (*this)[2] = t2; }
            template< typename T > strings( const T &t0, const T &t1, const T &t2, const T &t3 )
            { this->resize(3); (*this)[0] = t0; (*this)[1] = t1; (*this)[2] = t2; (*this)[3] = t3; }
            template< typename T > strings( const T &t0, const T &t1, const T &t2, const T &t3, const T &t4 )
            { this->resize(3); (*this)[0] = t0; (*this)[1] = t1; (*this)[2] = t2; (*this)[3] = t3; (*this)[4] = t4; }

            const moon9::string &at( const int &pos ) const
            {
                //extended behaviour
                int size = this->size();
                if( !size ) //throw std::out_of_range("moon9::strings::at() empty");
                { static std::map< const moon9::strings *, moon9::string > map; return ( ( map[ this ] = map[ this ] ) = moon9::string() ); }
                return *( this->begin() + ( pos >= 0 ? pos % size : size - 1 + ((pos+1) % size) ) );
            }

            moon9::string &at( const int &pos )
            {
                //extended behaviour
                int size = this->size();
                if( !size ) //throw std::out_of_range("moon9::strings::at() empty");
                { static std::map< const moon9::strings *, moon9::string > map; return ( ( map[ this ] = map[ this ] ) = moon9::string() ); }
                return *( this->begin() + ( pos >= 0 ? pos % size : size - 1 + ((pos+1) % size) ) );
            }

            operator std::deque<std::string>() const //faster way to do this? reinterpret_cast?
            {
                std::deque<std::string> out;
                out.resize( this->size() );
                std::copy( this->begin(), this->end(), out.begin() );
                return out;
            }

            operator std::vector<std::string>() const //faster way to do this? reinterpret_cast?
            {
                std::vector<std::string> out;
                out.resize( this->size() );
                std::copy( this->begin(), this->end(), out.begin() );
                return out;
            }

            // @todo
            // subset( 0, EOF )
            // subset( N, EOF )
            // subset( 0, -1 ) -> 0, EOF - 1
            // subset( -2, -1 ) -> EOF-2, EOF-1

            std::string str( const char *format1 = "\1\n" ) const
            {
                if( this->size() == 1 )
                    return *this->begin();

                std::string out;

                for( const_iterator it = this->begin(); it != this->end(); ++it )
                    out += moon9::string( format1, (*it) );

                return out;
            }
    };

    // @todo: LOCALE class

    // locale class reinterpret string as utf32 string

    // class locale : public moon9::string
    // {}
    // size() = moon9::string().size / 4

    // class locales : public moon9::strings
    // {}

    template<typename T>
    std::string str( const T& t, const std::string &format1, const std::string &pre = std::string(), const std::string &post = std::string() )
    {
        moon9::string out;

        for( T::const_iterator it = t.begin(), end = t.end(); it != end; ++it )
        {
            out << moon9::string( format1, *it );
        }

        return moon9::string() << pre << out << post;
    }

    template<typename T>
    std::string str1( const T& t, const std::string &format1, const std::string &pre = std::string(), const std::string &post = std::string() )
    {
        moon9::string out;

        for( T::const_iterator it = t.begin(), end = t.end(); it != end; ++it )
        {
            out << moon9::string( format1, it->first );
        }

        return moon9::string() << pre << out << post;
    }

    template<typename T>
    std::string str2( const T& t, const std::string &format1, const std::string &pre = std::string(), const std::string &post = std::string() )
    {
        moon9::string out;

        for( T::const_iterator it = t.begin(), end = t.end(); it != end; ++it )
        {
            out << moon9::string( format1, it->second );
        }

        return moon9::string() << pre << out << post;
    }

    template<typename T>
    std::string str12( const T& t, const std::string &format12, const std::string &pre = std::string(), const std::string &post = std::string() )
    {
        moon9::string out;

        for( T::const_iterator it = t.begin(), end = t.end(); it != end; ++it )
        {
            out << moon9::string( format12, it->first, it->second );
        }

        return moon9::string() << pre << out << post;
    }
}

#include <iostream>

std::ostream &operator <<( std::ostream &os, const moon9::strings &s );
