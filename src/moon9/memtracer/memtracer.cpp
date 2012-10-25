/*
 * Simple and lightweight memory leak detector
 * Copyright (c) 2011 Mario 'rlyeh' Rodriguez

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

 * Features:
 * - tiny
 * - clean
 * - callstack based
 * - macroless API. no new/delete macros
 * - embeddable, just include this source in your project

 * To do:
 * - support for malloc/realloc/free

 * References:
 * - http://www.codeproject.com/KB/cpp/allocator.aspx
 * - http://drdobbs.com/cpp/184403759

 * - rlyeh ~~ listening to Long Distance Calling / Metulsky Curse Revisited
 */

#include <cassert>
#include <cstddef>
#include <iostream>
#include <map>
#include <memory>
#include <new>

#include <moon9/debug/callstack.hpp>
#include <moon9/os/mutex.hpp>
#include <moon9/string/string.hpp>

#ifdef _WIN32
#   include <windows.h> // AllocConsole()
#endif

//    flag  | directives            | working?
// ---------+-----------------------+-------------
//    /MT   | _MT                   | no (crt)
//    /MTd  | _MT && _DEBUG         | no (crt)
//    /MD   | _MT && _DLL           | yes
//    /MDd  | _MT && _DLL && _DEBUG | yes
//          |                       |
#if defined( _WIN32 ) && defined( _MSC_VER )
#   if defined( _MT ) && !defined( _DLL )
#       if !defined( _DEBUG )
#           error Incompatible C Run-Time libraries compiler flag detected ( /MT ) : Use /MD instead
#       else
#           error Incompatible C Run-Time libraries compiler flag detected ( /MTd ) : Use /MDd instead
#       endif
#   endif
#endif


#ifndef kBooAllocMultiplier
/*  to increase memory requirements, and to simulate and to debug worse memory conditions */
/*  should be equal or bigger than 1                                                      */
#	define kBooAllocMultiplier 1
#endif

#ifndef kBooAlloc
/*  Behaviour is undefined if kBooAlloc() implementation uses any global symbols */
#   define kBooAlloc( x )     std::malloc( x )
#endif

#ifndef kBooFree
/*  Behaviour is undefined if kBooFree() implementation uses any global symbols */
#   define kBooFree( x )      std::free( x )
#endif

#ifndef kBooPrint
/*  Behaviour is undefined if kBooPrint() implementation uses any global symbols, like std::cout */
#   define kBooPrint( str )   fprintf( stderr, "%s", moon9::string( str ).c_str() )
#endif

#ifndef kBooAssert
#	define kBooAssert( x )    assert( x )
#endif

#ifndef kBooReportWildPointers
#   define kBooReportWildPointers 1
#endif

#ifndef kBooReportNullPointers
#   define kBooReportNullPointers 0
#endif

#ifndef kBooDefineMemoryOperators
#	define kBooDefineMemoryOperators 1
#endif

#ifndef kBooReportOnExit
#	define kBooReportOnExit 1
#endif

#ifndef kBooEnabledOnStart
#	define kBooEnabledOnStart 1
#endif

namespace moon9
{
    namespace detail
    {
        void show_report( const std::string &header, const std::string &body )
        {
            kBooPrint( header );
            kBooPrint( body );
        }

		volatile size_t timestamp_id = 0;

		size_t create_id()
		{
			static size_t id = 0;
			return ++id;
		}

        struct leak
        {
            size_t size, id;

            leak( const size_t _size = 0 ) : size( _size ), id( create_id() )
			{}
        };

		typedef std::pair< leak *, moon9::callstack * > leak_full;

        volatile bool enabled = kBooEnabledOnStart;

        void *trace( void *ptr, const size_t &size )
        {
            //* Implementation details:
            //* - mutex is used to discard any coming recursive allocation call
            //* - mutex is constructed thru std::malloc+placement-new to avoid infinite recursion

            static volatile bool initialized = false;
            static moon9::mutex *mutex = 0;

            if( !ptr )
                return ptr;

            if( !mutex )
            {
				//mutex = (moon9::mutex *) std::malloc( sizeof(moon9::mutex) ); //kBooAlloc( sizeof(moon9::mutex) );
				static char placement[ sizeof(moon9::mutex) ]; mutex = (moon9::mutex *)placement;
                new (mutex) moon9::mutex();    // recursion safe; we don't track placement-news
                initialized = true;
            }

            if( !initialized )
                return ptr;

            if( !enabled )
                return ptr;

            if( mutex->is_locked_by_me() )
                return ptr;

            mutex->lock();

            {
				static
				class container : public std::map< const void *, leak_full, std::less< const void * > > //, moon9::detail::malloc_allocator< std::pair< const void *, leak * > > >
				{
					public:

					container()
					{}

					~container()
					{
						enabled = false;

#						if kBooReportOnExit
							_report();
#						endif

						_clear();
					}

					void _clear()
					{
						for( iterator it = this->begin(), end = this->end(); it != end; ++it )
						{
							if( it->second.first )
								delete it->second.first, it->second.first = 0;

							if( it->second.second )
								delete it->second.second, it->second.second = 0;
						}

						this->clear();
					}

					void _report() const
					{
#						ifdef _WIN32
							AllocConsole();
#						endif

						// this should happen at the very end of a program (even *after* static memory unallocation)
						// @todo: avoid using any global object like std::cout/cerr (because some implementations like "cl /MT" will crash)

						kBooPrint( moon9::string("<moon9/memtracer/memtracer.cpp> says: Generating report. Please wait...\n") );

						moon9::string header, body;

						std::map< size_t, std::string > sort_by_id;

						size_t ibegin = 0, iend = this->size(), percent = ~0, current = 0, wasted = 0, n_leak = 0;
						for( const_iterator it = this->begin(), end = this->end(); it != end; ++it )
						{
							const void *the_address = it->first;
							leak *the_leak = it->second.first;
							moon9::callstack *the_callstack = it->second.second;

							if( the_leak && the_callstack /* && it->second->size != ~0 */ )
							{
								if( the_leak->id >= moon9::detail::timestamp_id )
								{
									++n_leak;
									wasted += the_leak->size;

									moon9::string  line( "Leak \1 bytes [\2]\r\n", the_leak->size, the_address );
									moon9::strings lines = moon9::string( the_callstack->str("\2\n", 2) ).tokenize("\n");

									for( size_t i = 0; i < lines.size(); ++i )
										line << moon9::string( "\t\t\1\r\n", lines[i] );

									sort_by_id.insert( std::pair< size_t, std::string >( the_leak->id, line ) );
								}
							}

							if( percent != current )
								kBooPrint( moon9::string("\r<moon9/memtracer/memtracer.cpp> says: Inspecting \1/\2 backtraces (\3%)", ibegin, iend, percent = current ) );

							current = (size_t)( ++ibegin * 100.0 / iend );
						}

						kBooPrint( moon9::string("\r<moon9/memtracer/memtracer.cpp> says: Inspecting \1/\1 backtraces (100%)", iend ) );

						double leaks_pct = this->size() ? n_leak * 100.0 / this->size() : 0.0;
						std::string score = "perfect!";
						if( leaks_pct >  0.00 ) score = "good";
						if( leaks_pct >  1.25 ) score = "excellent";
						if( leaks_pct >  2.50 ) score = "poor";
						if( leaks_pct >  5.00 ) score = "mediocre";
						if( leaks_pct > 10.00 ) score = "lame";

						kBooPrint( moon9::string("\n<moon9/memtracer/memtracer.cpp> says: Done ('\1' score)\n", score ) );

						// body report (linux & macosx will display windows line endings right)

						header = moon9::string("<moon9/memtracer/memtracer.cpp> says: Report: \1, \2 leaks found; \3 bytes wasted ('\4' score)\r\n", !n_leak ? "ok" : "error", n_leak, wasted, score );

						n_leak = 0;
						for( std::map< size_t, std::string >::const_iterator it = sort_by_id.begin(), end = sort_by_id.end(); it != end; ++it )
							body << moon9::string("\t\1) ", ++n_leak ) << it->second;

						sort_by_id.clear();

						show_report( header, body );
					}
				}
				map;

                container::iterator it = map.find( ptr );
                bool found = ( map.find( ptr ) != map.end() );

                if( size == ~0 )
                {
                    if( found )
                    {
						//map[ ptr ]->~leak();
						//kBooFree( map[ ptr ] );

						if( map[ ptr ].first )
							delete map[ ptr ].first, map[ ptr ].first = 0;

						if( map[ ptr ].second )
							delete map[ ptr ].second, map[ ptr ].second = 0;
                    }
                    else
                    {
                        // 1st) wild/null pointer deallocation found; warn user

#						if kBooReportWildPointers
							if( ptr )
								show_report( moon9::string( "<moon9/memtracer/memtracer.cpp> says: Error, wild pointer deallocation.\r\n" ), moon9::callstack().str("\t\1\r\n", 4) );
#						endif
#						if kBooReportNullPointers
							// unreachable code
							if( !ptr )
								show_report( moon9::string( "<moon9/memtracer/memtracer.cpp> says: Error, null pointer deallocation.\r\n" ), moon9::callstack().str("\t\1\r\n", 4) );
#						endif

                        // 2nd) normalize ptr for further deallocation (deallocating null pointers is ok)
                        ptr = 0;
                    }
                }
                else
				if( size == (~0) - 1 )
				{
					map._report();
				}
                else
				if( size == (~0) - 2 )
				{
					map._clear();
					timestamp_id = detail::create_id();
				}
				else
                {
                    if( found /* && map[ ptr ] */ )
                    {
                        // 1st) double pointer allocation (why?); warn user
						// kBooFree( map[ ptr ] );
                    }

                    // create a leak and (re)insert it into map

                    map[ ptr ] = map[ ptr ];
					map[ ptr ] = std::make_pair< leak *, moon9::callstack * >( new leak( size ), new moon9::callstack() );
                }

                mutex->unlock();
            }

            return ptr;
        }
    }

	namespace memtracer
	{
		void invalidate()
		{
			char unused;
			detail::trace( &unused, (~0) - 2 );
		}
		void report()
		{
			char unused;
			detail::trace( &unused, (~0) - 1 );
		}
		void watch( const void *ptr, size_t size )
		{
			moon9::detail::trace( (void *)ptr, size );
		}
		void forget( const void *ptr )
		{
			moon9::detail::trace( (void *)ptr, ~0 );
		}
		bool is_enabled()
		{
			return moon9::detail::enabled;
		}
		void enable( bool on )
		{
			moon9::detail::enabled = on;
		}
	}
}

#if kBooDefineMemoryOperators

//* Custom memory operators

void *operator new( size_t size, const std::nothrow_t &t ) throw()
{
    void *ptr = moon9::detail::trace( kBooAlloc( size*kBooAllocMultiplier ), size );

	kBooAssert(ptr);
	return ptr;
}

void *operator new[]( size_t size, const std::nothrow_t &t ) throw()
{
    void *ptr = moon9::detail::trace( kBooAlloc( size*kBooAllocMultiplier ), size );

	kBooAssert(ptr);
	return ptr;
}

void operator delete( void *ptr, const std::nothrow_t &t ) throw()
{
    if( ptr ) kBooFree( moon9::detail::trace( ptr, ~0 ) );
}

void operator delete[]( void *ptr, const std::nothrow_t &t ) throw()
{
    if( ptr ) kBooFree( moon9::detail::trace( ptr, ~0 ) );
}

//* Custom memory operators (w/ exceptions)

void *operator new( size_t size ) //throw(std::bad_alloc)
{
    void *ptr = kBooAlloc( size*kBooAllocMultiplier );

    if( !ptr )
        throw std::bad_alloc();

	kBooAssert(ptr);
    return moon9::detail::trace( ptr, size );
}

void *operator new[]( size_t size ) //throw(std::bad_alloc)
{
    void *ptr = kBooAlloc( size*kBooAllocMultiplier );

    if( !ptr )
        throw std::bad_alloc();

	kBooAssert(ptr);
    return moon9::detail::trace( ptr, size );
}

void operator delete( void *ptr ) throw()
{
    if( ptr ) kBooFree( moon9::detail::trace( ptr, ~0 ) );
}

void operator delete[]( void *ptr ) throw()
{
    if( ptr ) kBooFree( moon9::detail::trace( ptr, ~0 ) );
}

#endif

#undef kBooAllocMultiplier
#undef kBooAlloc
#undef kBooFree
#undef kBooPrint
#undef kBooAssert

#undef kBooDefineMemoryOperators
#undef kBooReportWildPointers
#undef kBooReportNullPointers
#undef kBooReportOnExit
#undef kBooEnabledOnStart

