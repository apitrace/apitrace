/**************************************************************************
 *
 * Copyright 2015 VMware, Inc
 * All Rights Reserved.
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
 *
 **************************************************************************/


#pragma once


#include <stdint.h>
#include <stdlib.h>


namespace ubjson {


// http://ubjson.org/type-reference/
enum Marker {
    MARKER_EOF            = -1,
    MARKER_NULL           = 'Z',
    MARKER_NOOP           = 'N',
    MARKER_TRUE           = 'T',
    MARKER_FALSE          = 'F',
    MARKER_INT8           = 'i',
    MARKER_UINT8          = 'U',
    MARKER_INT16          = 'I',
    MARKER_INT32          = 'l',
    MARKER_INT64          = 'L',
    MARKER_FLOAT32        = 'd',
    MARKER_FLOAT64        = 'D',
    MARKER_HIGH_PRECISION = 'H',
    MARKER_CHAR           = 'C',
    MARKER_STRING         = 'S',
    MARKER_ARRAY_BEGIN    = '[',
    MARKER_ARRAY_END      = ']',
    MARKER_OBJECT_BEGIN   = '{',
    MARKER_OBJECT_END     = '}',
    MARKER_TYPE           = '$',
    MARKER_COUNT          = '#',
};


inline uint16_t
bigEndian16(uint16_t x) {
#ifdef HAVE_BIGENDIAN
    return x;
#elif defined(_MSC_VER)
    return _byteswap_ushort(x);
#elif defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) >= 408
    return __builtin_bswap16(x);
#else
    return (x << 8) | (x >> 8);
#endif
}

inline uint32_t
bigEndian32(uint32_t x) {
#ifdef HAVE_BIGENDIAN
    return x;
#elif defined(_MSC_VER)
    return _byteswap_ulong(x);
#else
    return __builtin_bswap32(x);
#endif
}

inline uint64_t
bigEndian64(uint64_t x) {
#ifdef HAVE_BIGENDIAN
    return x;
#elif defined(_MSC_VER)
    return _byteswap_uint64(x);
#else
    return __builtin_bswap64(x);
#endif
}


union Float32 {
    float f;
    uint32_t i;
};


union Float64 {
    double f;
    uint64_t i;
};


}
