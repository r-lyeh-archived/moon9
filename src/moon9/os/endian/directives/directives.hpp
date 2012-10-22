/*
 * Compiler/target directives and related macros

 * Copyright (c) 2010 Mario 'rlyeh' Rodriguez
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

 * Notes:
 * - Directives taken from http://predef.sourceforge.net/preos.html#sec25 and
 *   http://www.boost.org/doc/libs/1_35_0/boost/detail/endian.hpp (thanks!)

 * To do:
 * - Add:
 *   DEBUG                     = UNOPTIMIZED +  DEBUG SYMBOL
 *   DEBUG_OPT                 =   OPTIMIZED +  DEBUG SYMBOL
 *   RELEASE_UNOPT             = UNOPTIMIZED + NDEBUG SYMBOL
 *   RELEASE                   =   OPTIMIZED + NDEBUG SYMBOL
 *   (code should rely only on DEBUG/NDEBUG symbols)

 * - rlyeh
 */

#pragma once

// useful? {

#if defined( NDEBUG ) || defined( _NDEBUG )
#   define kMoon9CompilerRelease  1
#else
#   define kMoon9CompilerDebug    1
#endif

// }

// Compiler detection

#ifdef _MSC_VER
#   if _MSC_VER >= 1700
#      define kMoon9CompilerMsvc 2011
#      define kMoon9CompilerCxx11   1
#   elif _MSC_VER >= 1600
#      define kMoon9CompilerMsvc 2010
#      define kMoon9CompilerCxx11   0
#   elif _MSC_VER >= 1500
#      define kMoon9CompilerMsvc 2008
#      define kMoon9CompilerCxx11   0
#   elif _MSC_VER >= 1400
#      define kMoon9CompilerMsvc 2005
#      define kMoon9CompilerCxx11   0
#   else
#      define kMoon9CompilerMsvc 2003 //or older...
#      define kMoon9CompilerCxx11   0
#   endif
#endif

#ifdef __GNUC__
#   define kMoon9CompilerGcc     (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#   if kMoon9CompilerGcc >= 40600 // >= 4.6.0
#       define kMoon9CompilerCxx11  1
#   else
#       define kMoon9CompilerCxx11  0
#   endif
#endif

#if defined(kMoon9CompilerCxx11) && kMoon9CompilerCxx11 == 0
#   undef kMoon9CompilerCxx11
#endif

#if !defined(kMoon9CompilerCxx11)
#   if __cplusplus > 199711
#       define kMoon9CompilerCxx11  1
#   else
#       define kMoon9CompilerCxx11  0
#   endif
#endif

// Macro to force inlining

#ifndef kMoon9CompilerForceInline
#   if defined( kMoon9CompilerMsvc )
#       define kMoon9CompilerForceInline( ... ) __forceinline __VA_ARGS__
#   endif
#   if defined( kMoon9CompilerGcc ) || defined( kMoon9CompilerMw )
#       define kMoon9CompilerForceInline( ... ) __inline__ __VA_ARGS__
#   endif
#   if !defined( kMoon9CompilerForceInline )
#       define kMoon9CompilerForceInline( ... ) inline __VA_ARGS__
#   endif
#endif

// Macro to prevent inlining

#ifndef kMoon9CompilerNoInline
#   if defined( kMoon9CompilerMsvc )
#       define kMoon9CompilerNoInline( ... ) __declspec(noinline) __VA_ARGS__
#   endif
#   if defined( kMoon9CompilerGcc )
#       define kMoon9CompilerNoInline( ... ) __VA_ARGS__ __attribute__ ((noinline))
#   endif
#   if !defined( kMoon9CompilerNoInline )
#       define kMoon9CompilerNoInline( ... ) __VA_ARGS__
#   endif
#endif


// Weak symbols. Usage:
// kMoon9CompilerWeakSymbol( void custom_function( int elems, char *str ) )
// { /*...*/ }

#ifndef kMoon9CompilerWeakSymbol
#   if defined( kMoon9CompilerMsvc )
#       define kMoon9CompilerWeakSymbol( ... ) __declspec(noinline) __inline __VA_ARGS__
#   endif
#   if defined( kMoon9CompilerGcc )
#       define kMoon9CompilerWeakSymbol( ... ) __VA_ARGS__ __attribute__ ((weak))
#   endif
#   if !defined( kMoon9CompilerWeakSymbol )
#       define kMoon9CompilerWeakSymbol( ... ) __VA_ARGS__
#   endif
#endif

// Fix stupid vc++ warnings

#ifdef kMoon9CompilerMsvc
#   ifndef _CRT_SECURE_NO_WARNINGS
#       define _CRT_SECURE_NO_WARNINGS 1
#		pragma warning(disable:4996)
#		pragma warning(disable:4244)	// remove me!
#   endif
#endif

// Workaround to fix missing __func__ / __FUNCTION__ / __PRETTY_FUNCTION__ directives in a few compilers

#if defined( kMoon9CompilerMsvc )
#   define  __PRETTY_FUNCTION__ __FUNCTION__
#   define  __func__ __FUNCTION__
#endif

#if defined( kMoon9CompilerGcc )
#endif

#if !defined( kMoon9CompilerMsvc ) && !defined( kMoon9CompilerGcc )
#   define  __PRETTY_FUNCTION__ __func__
#   define  __FUNCTION__ __func__
#endif

// if non C99 compiler, redefine'em as "??"
//#   define  __PRETTY_FUNCTION__ "??"
//#   define  __FUNCTION__ "??"
//#   define  __func__ "??"


// Workaround to fix missing __COUNTER__ directive in a few compilers

#if ( __COUNTER__ == __COUNTER__ )
#   define __COUNTER__ __LINE__
#endif


#ifndef kMoon9Target
#   if defined( _WIN32 ) || defined( _WIN64 ) || defined( __WIN32__ ) || defined (__TOS_WIN__) || defined( __WINDOWS__ )
#       define kMoon9TargetWindows    1
#       define kMoon9Target           "Windows"
#   endif
#   if defined( __linux__ ) || defined( __linux ) || defined( linux ) || defined( __CYGWIN__ )
#       define kMoon9TargetLinux      1
#       define kMoon9Target           "Linux"
#   endif
#   if defined( macintosh ) || defined( Macintosh ) || ( defined( __APPLE__  ) && defined( __MACH__ ) )
#       define kMoon9TargetApple      1
#       define kMoon9Target           "Apple"
#   endif
#endif

#ifndef kMoon9Target
#   define kMoon9TargetUnknown        1
#   define kMoon9Target               "??"
#   error Cannot determinate target platform
#endif

#if !defined( kMoon9TargetLittleEndian ) && !defined( kMoon9TargetMiddleEndian ) && !defined( kMoon9TargetBigEndian )
#   if defined (__GLIBC__)
#     include <endian.h>
#     if    (__BYTE_ORDER == __BIG_ENDIAN)
#         define kMoon9TargetBigEndian
#     elif  (__BYTE_ORDER == __LITTLE_ENDIAN)
#         define kMoon9TargetLittleEndian
#     elif  (__BYTE_ORDER == __PDP_ENDIAN)
#         define kMoon9TargetMiddleEndian
#     endif
#   else
#     if defined(_BIG_ENDIAN) \
        || defined(__sparc) || defined(__sparc__) \
        || defined(_POWER) || defined(__powerpc__) \
        || defined(__ppc__) || defined(__hpux) \
        || defined(_MIPSEB) || defined(_POWER) \
        || defined(__s390__)
#          define kMoon9TargetBigEndian
#     elif defined(_LITTLE_ENDIAN) \
        || defined(__i386__) || defined(__alpha__) \
        || defined(__ia64) || defined(__ia64__) \
        || defined(_M_IX86) || defined(_M_IA64) \
        || defined(_M_ALPHA) || defined(__amd64) \
        || defined(__amd64__) || defined(_M_AMD64) \
        || defined(__x86_64) || defined(__x86_64__) \
        || defined(_M_X64)
#          define kMoon9TargetLittleEndian
#     elif (__BYTE_ORDER == __PDP_ENDIAN)
#          define kMoon9TargetMiddleEndian
#     endif
#   endif
#endif

#if !defined( kMoon9TargetLittleEndian ) && !defined( kMoon9TargetMiddleEndian ) && !defined( kMoon9TargetBigEndian )
#   error Cannot determinate target endianness
#endif

#ifdef kMoon9TargetLittleEndian
#   define kMoon9TargetByteOrder 1234
#endif

#ifdef kMoon9TargetMiddleEndian   //If this is middle endian, how do i store nuxi endianness?
#   define kMoon9TargetByteOrder 2143
#endif

#ifdef kMoon9TargetBigEndian
#   define kMoon9TargetByteOrder 4321
#endif
