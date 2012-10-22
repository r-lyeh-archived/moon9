/*
 * Simple name demangler

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

 * - rlyeh
 */

#include <string>

#if defined(_WIN32)
#	ifdef kMoon9ReturnDemangled
#		undef kMoon9ReturnDemangled
#	endif
#	include <Windows.h>
#   include <Dbghelp.h>
#   pragma comment(lib, "dbghelp.lib")
#   define kMoon9ReturnDemangled( name ) { char demangled[1024]; \
    return (UnDecorateSymbolName(name.c_str(), demangled, sizeof( demangled ), UNDNAME_COMPLETE)) ? demangled : name; }
#endif

#if defined(__GNUC__) // && defined(HAVE_CXA_DEMANGLE)
#	ifdef kMoon9ReturnDemangled
#		undef kMoon9ReturnDemangled
#	endif
#   include <cxxabi.h>
#   define kMoon9ReturnDemangled( name ) { char demangled[1024]; size_t sz = sizeof(demangled); int status; \
    abi::__cxa_demangle(name.c_str(), demangled, &sz, &status); return !status ? demangled : name; }
#endif

#if defined(__linux__)
#	ifdef kMoon9ReturnDemangled
#		undef kMoon9ReturnDemangled
#	endif
#	include <cstdio>
#   define kMoon9ReturnDemangled( name ) { \
	FILE *fp = popen( (std::string("echo -n \"") + name + std::string("\" | c++filt" )).c_str(), "r" ); \
	if (!fp) { return name; } \
	char demangled[1024]; char *line_p = fgets(demangled, sizeof(demangled), fp); pclose(fp); \
	return line_p; }
#endif

#ifndef kMoon9ReturnDemangled
#   define kMoon9ReturnDemangled( name ) return name;
#endif

std::string demangle( const std::string &name )
{
    kMoon9ReturnDemangled( name );
}

#undef kMoon9ReturnDemangled
