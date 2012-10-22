/*
 * portable pods header (hopefully)
 * -rlyeh
 */

#pragma once

#if defined(_MSC_VER) && _MSC_VER < 1600
#	include <msinttypes/stdint.h>
#else
#	include <stdint.h>
#endif

namespace moon9
{
    typedef  int8_t    int8;
    typedef uint8_t   uint8;
    typedef uint64_t  bits8;

    typedef  int16_t  int16;
    typedef uint16_t uint16;
    typedef uint64_t bits16;

    typedef  int32_t  int32;
    typedef uint32_t uint32;
    typedef uint64_t bits32;

    typedef  int64_t  int64;
    typedef uint64_t uint64;
    typedef uint64_t bits64;

    typedef  intmax_t ssize;
    typedef uintmax_t  size;
}
