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
#include <Windows.h>
#else
#include <errno.h>
#include <string.h> //strerror_r
#endif

#include <string>
#include <sstream>

namespace moon9
{

#ifdef _WIN32

   std::string get_os_error()
   {
         // Retrieve the system error message for the last-error code

         LPVOID lpMsgBuf;
         DWORD dw = GetLastError();

         std::stringstream ss;

         if( FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, NULL )
            )
         {
            std::string out( (const char *)lpMsgBuf );

            //out.pop_back(); //remove trailing '\n'
            //out.pop_back(); //remove trailing '\r'
            out.erase( out.end() - 1 );
            out.erase( out.end() - 1 );

            LocalFree( lpMsgBuf );

            ss << out << " (" << dw << ')';
         }
         else ss << dw;

         return ss.str();
   }

#else

   std::string get_os_error()
   {
         //      //no thread safe:
         //      return strerror( errno );

         char translated[256] = "";

         if( errno && strerror_r( errno, translated, 256 ) )
         {
            switch( errno )
            {
               case EINVAL: return "<moon9/os/error.hpp> says: invalid errno";
               case ERANGE: return "<moon9/os/error.hpp> says: insufficent buffer";
               default:     return "<moon9/os/error.hpp> says: ?";
            }
         }

         return translated;
   }

#endif

}
