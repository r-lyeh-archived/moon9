/*
 * OS header

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

 * - rlyeh
 */

#pragma once

#ifdef _WIN32
#   if defined(min) || defined(max)
//#       error Error: min or max is #defined, probably in <Windows.h>.  Put #define NOMINMAX before including Windows.h to suppress windows global min,max macros.
#   endif
#   ifndef NOMINMAX
#       define NOMINMAX
#   endif
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif
#   ifndef WIN32_EXTRA_LEAN
#       define WIN32_EXTRA_LEAN
#   endif
#   include <winsock2.h>
#   include <windows.h>
#   if defined(min)
#     undef min
#   endif
#   if defined(max)
#     undef max
#   endif
#   include <ctime>
#   include <sys/stat.h>

//#endif
//#ifdef __linux__
//#   include <X11/keysym.h>

#else

#   include <pthread.h>
#   include <sys/signal.h>
#   include <signal.h>

#   include <sys/time.h>
#   include <sys/stat.h>

#   include <errno.h>
#   include <string.h>   //strerror_r

#endif
