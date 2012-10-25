/*
 * Extended C++ standard string
 * Copyright (c) 2010-2012, Mario 'rlyeh' Rodriguez

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

 * - rlyeh
 */

#pragma once

#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <cctype>       // std::toupper

namespace moon9
{
    class string : public std::string
    {
        public:

        // basic constructors

        string() : std::string()
        {}

        string( const std::string &s ) : std::string( s )
        {}

        string( const char &c, size_t n ) : std::string( n, c )
        {}

        string( size_t n, const char &c ) : std::string( n, c )
        {}

        string( const char &c ) : std::string( 1, c )
        {}

        string( const char *cstr ) : std::string( cstr ? cstr : "" )
        {}

        template<size_t N>
        string( const char (&cstr)[N] ) : std::string( cstr )
        {}

        string( const bool &val ) : std::string( val ? "true" : "false" )
        {}

        // constructor sugar

        template< typename T >
        string( const T &t ) : std::string()
        {
            operator=(t);
        }

        // extended constructors; safe formatting


        template< typename T1 >
        string( const std::string &_fmt, const T1 &t1 ) : std::string()
        {
            std::string t[] = { std::string(), string(t1) };

            for( const char *fmt = _fmt.c_str(); *fmt; ++fmt )
            {
                if( *fmt == '\1' )
                    t[0] += t[1];
                else
                    t[0] += *fmt;
            }

            this->assign( t[0] );
        }

        template< typename T1, typename T2 >
        string( const std::string &_fmt, const T1 &t1, const T2 &t2 ) : std::string()
        {
            std::string t[] = { std::string(), string(t1), string(t2) };

            for( const char *fmt = _fmt.c_str(); *fmt; ++fmt )
            {
                if( *fmt == '\1' )
                    t[0] += t[1];
                else
                if( *fmt == '\2' )
                    t[0] += t[2];
                else
                    t[0] += *fmt;
            }

            this->assign( t[0] );
        }

        template< typename T1, typename T2, typename T3 >
        string( const std::string &_fmt, const T1 &t1, const T2 &t2, const T3 &t3 ) : std::string()
        {
            std::string t[] = { std::string(), string(t1), string(t2), string(t3) };

            for( const char *fmt = _fmt.c_str(); *fmt; ++fmt )
            {
                if( *fmt == '\1' )
                    t[0] += t[1];
                else
                if( *fmt == '\2' )
                    t[0] += t[2];
                else
                if( *fmt == '\3' )
                    t[0] += t[3];
                else
                    t[0] += *fmt;
            }

            this->assign( t[0] );
        }

        template< typename T1, typename T2, typename T3, typename T4 >
        string( const std::string &_fmt, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4 ) : std::string()
        {
            std::string t[] = { std::string(), string(t1), string(t2), string(t3), string(t4) };

            for( const char *fmt = _fmt.c_str(); *fmt; ++fmt )
            {
                if( *fmt == '\1' )
                    t[0] += t[1];
                else
                if( *fmt == '\2' )
                    t[0] += t[2];
                else
                if( *fmt == '\3' )
                    t[0] += t[3];
                else
                if( *fmt == '\4' )
                    t[0] += t[4];
                else
                    t[0] += *fmt;
            }

            this->assign( t[0] );
        }

        template< typename T1, typename T2, typename T3, typename T4, typename T5 >
        string( const std::string &_fmt, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5 ) : std::string()
        {
            std::string t[] = { std::string(), string(t1), string(t2), string(t3), string(t4), string(t5) };

            for( const char *fmt = _fmt.c_str(); *fmt; ++fmt )
            {
                if( *fmt == '\1' )
                    t[0] += t[1];
                else
                if( *fmt == '\2' )
                    t[0] += t[2];
                else
                if( *fmt == '\3' )
                    t[0] += t[3];
                else
                if( *fmt == '\4' )
                    t[0] += t[4];
                else
                if( *fmt == '\5' )
                    t[0] += t[5];
                else
                    t[0] += *fmt;
            }

            this->assign( t[0] );
        }

        template< typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 >
        string( const std::string &_fmt, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6 ) : std::string()
        {
            std::string t[] = { std::string(), string(t1), string(t2), string(t3), string(t4), string(t5), string(t6) };

            for( const char *fmt = _fmt.c_str(); *fmt; ++fmt )
            {
                if( *fmt == '\1' )
                    t[0] += t[1];
                else
                if( *fmt == '\2' )
                    t[0] += t[2];
                else
                if( *fmt == '\3' )
                    t[0] += t[3];
                else
                if( *fmt == '\4' )
                    t[0] += t[4];
                else
                if( *fmt == '\5' )
                    t[0] += t[5];
                else
                if( *fmt == '\6' )
                    t[0] += t[6];
                else
                    t[0] += *fmt;
            }

            this->assign( t[0] );
        }

        template< typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7 >
        string( const std::string &_fmt, const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7 ) : std::string()
        {
            std::string t[] = { std::string(), string(t1), string(t2), string(t3), string(t4), string(t5), string(t6), string(t7) };

            for( const char *fmt = _fmt.c_str(); *fmt; ++fmt )
            {
                if( *fmt == '\1' )
                    t[0] += t[1];
                else
                if( *fmt == '\2' )
                    t[0] += t[2];
                else
                if( *fmt == '\3' )
                    t[0] += t[3];
                else
                if( *fmt == '\4' )
                    t[0] += t[4];
                else
                if( *fmt == '\5' )
                    t[0] += t[5];
                else
                if( *fmt == '\6' )
                    t[0] += t[6];
                else
                if( *fmt == '\7' )
                    t[0] += t[7];
                else
                    t[0] += *fmt;
            }

            this->assign( t[0] );
        }

        // conversion

        template< typename T >
        T as() const
        {
            T t;
            std::stringstream ss( *this );
            return ss >> t ? t : (T)(as<bool>());
        }

        template<>
        bool as() const
        {
            return this->size() && (*this) != "0" && (*this) != "false";
        }

        template<>
        char as() const
        {
            return (char)(as<int>());
        }

        template<>
        signed char as() const
        {
            return (signed char)(as<int>());
        }

        template<>
        unsigned char as() const
        {
            return (unsigned char)(as<int>());
        }

        template<>
        const char *as() const
        {
            return this->c_str();
        }

        template<>
        std::string as() const
        {
            return *this;
        }

        template< typename T >
        operator T() const
        {
            return as<T>();
        }

        // chaining operators

        template< typename T >
        string &operator +=( const T &t )
        {
            this->assign( this->str() + string(t) );
            return *this;
        }

        string &operator +=( std::ostream& ( *pf )(std::ostream&))
        {
            return *pf == static_cast<std::ostream& ( * )(std::ostream&)>( std::endl ) ? (*this) += "\n", *this : *this;
        }

        template <typename T>
        string &operator <<( const T &t )
        {
            this->assign( this->str() + string(t) );
            return *this;
        }

        string &operator <<( std::ostream& ( *pf )(std::ostream&))
        {
            return *pf == static_cast<std::ostream& ( * )(std::ostream&)>( std::endl ) ? (*this) += "\n", *this : *this;
        }

        // assignment sugars

        template< typename T >
        string &operator=( const T &t )
        {
            std::stringstream ss;
            ss.precision(20);
            if( ss << /* std::boolalpha << */ t )
                this->assign( ss.str() );
            return *this;
        }

        template<>
        string &operator=( const bool &t )
        {
            this->assign( t ? "true" : "false" );
            return *this;
        }

        template<>
        string &operator=( const long double &t )
        {
            std::stringstream ss;
            ss.precision(90);
            if( ss << /* std::boolalpha << */ t )
                this->assign( ss.str() );
            return *this;
        }

        string &operator=( const char *t )
        {
            this->assign( t ? t : std::string() );
            return *this;
        }

        // comparison sugars

        operator const bool() const
        {
            return as<bool>();
        }

        // extra methods

        const char &at( const int &pos ) const
        {
            //classic behaviour: "hello"[5] = undefined, "hello"[-1] = undefined
            //return this->std::string::at( pos );

            //extended behaviour: "hello"[5] = h, "hello"[-1] = o,
            int size = this->size();
            if( !size ) //throw std::out_of_range("string::at() empty string");
            { static std::map< const string *, char > map; return ( ( map[ this ] = map[ this ] ) = '\0' ); }
            return this->data()[ pos >= 0 ? pos % size : size - 1 + ((pos+1) % size) ];
        }

        char &at( const int &pos )
        {
            //classic behaviour: "hello"[5] = undefined, "hello"[-1] = undefined
            //return this->std::string::at( pos );

            //extended behaviour: "hello"[5] = h, "hello"[-1] = o,
            int size = this->size();
            if( !size ) // std::out_of_range("string::at() empty string");
            { static std::map< const string *, char > map; return ( ( map[ this ] = map[ this ] ) = '\0' ); }
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

        // tools

        std::string str() const
        {
            return *this;
        }

        bool matches( const std::string &pattern ) const
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

        string uppercase() const
        {
            std::string s = *this;

            std::transform( s.begin(), s.end(), s.begin(), (int(*)(int)) std::toupper );

            return s;
        }

        string lowercase() const
        {
            std::string s = *this;

            std::transform( s.begin(), s.end(), s.begin(), (int(*)(int)) std::tolower );

            return s;
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

        string left_of( const std::string &substring ) const
        {
            string::size_type pos = this->find( substring );
            return pos == std::string::npos ? *this : this->substr(0, pos);
        }

        string right_of( const std::string &substring ) const
        {
            std::string::size_type pos = this->find( substring );
            return pos == std::string::npos ? *this : this->substr(pos + 1);
        }

        string replace( const std::string &target, const std::string &replacement ) const
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
        string replace_map( const std::map< std::string, std::string > &replacements ) const
        {
            string out;

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

        private:

        string strip( const std::string &chars, bool strip_left, bool strip_right ) const
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
        string lstrip( const std::string &chars = std::string() ) const
        {
            return strip( chars, true, false );
        }
        string ltrim( const std::string &chars = std::string() ) const
        {
            return strip( chars, true, false );
        }

        // Return a copy of the string with trailing characters removed (default chars: space)
        string rstrip( const std::string &chars = std::string() ) const
        {
            return strip( chars, false, true );
        }
        string rtrim( const std::string &chars = std::string() ) const
        {
            return strip( chars, false, true );
        }

        // Return a copy of the string with both leading and trailing characters removed (default chars: space)
        string strip( const std::string &chars = std::string() ) const
        {
            return strip( chars, true, true );
        }
        string trim( const std::string &chars = std::string() ) const
        {
            return strip( chars, true, true );
        }

        bool starts_with( const std::string &prefix ) const
        {
            return this->size() >= prefix.size() ? this->substr( 0, prefix.size() ) == prefix : false;
        }

        bool starts_with( const std::string &prefix, bool is_case_sensitive ) const
        {
            return is_case_sensitive ? starts_with( prefix ) : this->uppercase().starts_with( string(prefix).uppercase() );
        }

        bool ends_with( const std::string &suffix ) const
        {
            return this->size() < suffix.size() ? false : this->substr( this->size() - suffix.size() ) == suffix;
        }

        bool ends_with( const std::string &suffix, bool is_case_sensitive ) const
        {
            return is_case_sensitive ? ends_with( suffix ) : this->uppercase().ends_with( string(suffix).uppercase() );
        }

        // legacy

        string eval() const;

        std::deque< string > parse( const std::vector< std::string >& delimiters ) const
        {
            struct tokenizer // iago gayoso's tokenizer
            {
                size_t index;
                bool hole, escape;
                std::string fsm, word;

                tokenizer( const std::string& str = std::string() ) : index(0), hole(false), escape(false), fsm(str)
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

            std::vector< tokenizer > vFSM;
            std::deque< string > tokens;

            size_t base = 0, index = 0, size = (*this).size();

            for( size_t i = 0; i < delimiters.size(); i++ )
                vFSM.push_back( tokenizer( delimiters[i] ) );

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

        std::deque< string > tokenize( const std::string &chars ) const
        {
            std::string map( 256, '\0' );

            for( auto it = chars.begin(), end = chars.end(); it != end; ++it )
                map[ *it ] = '\1';

            std::deque<string> tokens;

            tokens.push_back( std::string() );

            for( int i = 0, end = this->size(); i < end; ++i )
            {
                char c = at(i);

                if( map.at(c) )
                    tokens.push_back( std::string() );
                else
                    tokens.back().push_back( c );
            }

            return tokens;
        }

        std::deque< string > split( const std::string& delimiters ) const // tokenize_incl_separators
        {
            std::deque< string > tokens;
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
    };

    class strings : public std::deque< string >
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

            const string &at( const int &pos ) const
            {
                //extended behaviour
                int size = this->size();
                if( !size ) //throw std::out_of_range("strings::at() empty");
                { static std::map< const strings *, string > map; return ( ( map[ this ] = map[ this ] ) = string() ); }
                return *( this->begin() + ( pos >= 0 ? pos % size : size - 1 + ((pos+1) % size) ) );
            }

            string &at( const int &pos )
            {
                //extended behaviour
                int size = this->size();
                if( !size ) //throw std::out_of_range("strings::at() empty");
                { static std::map< const strings *, string > map; return ( ( map[ this ] = map[ this ] ) = string() ); }
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
                    out += string( format1, (*it) );

                return out;
            }
    };
}

#include <iostream>

std::ostream &operator <<( std::ostream &os, const moon9::strings &s );
