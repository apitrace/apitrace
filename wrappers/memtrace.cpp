/**************************************************************************
 *
 * Copyright 2014-2016 VMware, Inc.
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


#include "memtrace.hpp"

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <algorithm>

#include "crc32c.hpp"


#if \
    (defined(__i386__) && defined(__SSE2__)) /* gcc */ || \
    defined(_M_IX86) /* msvc */ || \
    defined(__x86_64__) /* gcc */ || \
    defined(_M_X64) /* msvc */ || \
    defined(_M_AMD64) /* msvc */

#  define HAVE_SSE2

// TODO: Detect and leverage SSE 4.1 and 4.2 at runtime
#  undef HAVE_SSE41
#  undef HAVE_SSE42

#endif


#if defined(HAVE_SSE42)
#  include <nmmintrin.h>
#elif defined(HAVE_SSE41)
#  include <smmintrin.h>
#elif defined(HAVE_SSE2)
#  include <emmintrin.h>
#endif


#define BLOCK_ALIGN 64
#define BLOCK_SIZE 512


template< class T >
static inline T *
lAlignPtr(T *p, uintptr_t alignment)
{
    return reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(p) & ~(alignment - 1));
}


template< class T >
static inline T *
rAlignPtr(T *p, uintptr_t alignment)
{
    return reinterpret_cast<T *>((reinterpret_cast<uintptr_t>(p) + alignment - 1) & ~(alignment - 1));
}


#ifdef HAVE_SSE2

#ifdef HAVE_SSE41
    #define mm_stream_load_si128 _mm_stream_load_si128
    #define mm_extract_epi32_0(x) _mm_extract_epi32(x, 0)
    #define mm_extract_epi32_1(x) _mm_extract_epi32(x, 1)
    #define mm_extract_epi32_2(x) _mm_extract_epi32(x, 2)
    #define mm_extract_epi32_3(x) _mm_extract_epi32(x, 3)
#else /* !HAVE_SSE41 */
    #define mm_stream_load_si128 _mm_load_si128
    #define mm_extract_epi32_0(x) _mm_cvtsi128_si32(x)
    #define mm_extract_epi32_1(x) _mm_cvtsi128_si32(_mm_shuffle_epi32(x,_MM_SHUFFLE(1,1,1,1)))
    #define mm_extract_epi32_2(x) _mm_cvtsi128_si32(_mm_shuffle_epi32(x,_MM_SHUFFLE(2,2,2,2)))
    #define mm_extract_epi32_3(x) _mm_cvtsi128_si32(_mm_shuffle_epi32(x,_MM_SHUFFLE(3,3,3,3)))
#endif /* !HAVE_SSE41 */

#ifdef HAVE_SSE42

#define mm_crc32_u32 _mm_crc32_u32

#else /* !HAVE_SSE42 */

static inline uint32_t
mm_crc32_u32(uint32_t crc, uint32_t current)
{
    uint32_t one = current ^ crc;
    crc  = crc32c_8x256_table[0][ one >> 24        ] ^
           crc32c_8x256_table[1][(one >> 16) & 0xff] ^
           crc32c_8x256_table[2][(one >>  8) & 0xff] ^
           crc32c_8x256_table[3][ one        & 0xff];
    return crc;
}

#endif /* !HAVE_SSE42 */

#endif /* HAVE_SSE2 */


uint32_t
hashBlock(const void *p)
{
    assert(lAlignPtr(p, BLOCK_ALIGN) == p);

    uint32_t crc;

#ifdef HAVE_SSE2
    crc = 0;

    __m128i *q = (__m128i *)(void *)p;

    crc = ~crc;

    for (unsigned c = BLOCK_SIZE / (4 * sizeof *q); c; --c) {
        __m128i m0 = mm_stream_load_si128(q++);
        __m128i m1 = mm_stream_load_si128(q++);
        __m128i m2 = mm_stream_load_si128(q++);
        __m128i m3 = mm_stream_load_si128(q++);

        crc = mm_crc32_u32(crc, mm_extract_epi32_0(m0));
        crc = mm_crc32_u32(crc, mm_extract_epi32_1(m0));
        crc = mm_crc32_u32(crc, mm_extract_epi32_2(m0));
        crc = mm_crc32_u32(crc, mm_extract_epi32_3(m0));

        crc = mm_crc32_u32(crc, mm_extract_epi32_0(m1));
        crc = mm_crc32_u32(crc, mm_extract_epi32_1(m1));
        crc = mm_crc32_u32(crc, mm_extract_epi32_2(m1));
        crc = mm_crc32_u32(crc, mm_extract_epi32_3(m1));

        crc = mm_crc32_u32(crc, mm_extract_epi32_0(m2));
        crc = mm_crc32_u32(crc, mm_extract_epi32_1(m2));
        crc = mm_crc32_u32(crc, mm_extract_epi32_2(m2));
        crc = mm_crc32_u32(crc, mm_extract_epi32_3(m2));

        crc = mm_crc32_u32(crc, mm_extract_epi32_0(m3));
        crc = mm_crc32_u32(crc, mm_extract_epi32_1(m3));
        crc = mm_crc32_u32(crc, mm_extract_epi32_2(m3));
        crc = mm_crc32_u32(crc, mm_extract_epi32_3(m3));
    }

    crc = ~crc;

#else /* !HAVE_SSE2 */

    crc = crc32c_8bytes(p, BLOCK_SIZE);

#endif

    return crc;
}


// We must reset the data on discard, otherwise the old data could match just
// by chance.
//
// XXX: if the appplication writes 0xCDCDCDCD at the start or the end of the
// buffer range, we'll fail to detect.  The only way to be 100% sure things
// won't fall through would be to setup memory traps.
void MemoryShadow::zero(void *_ptr, size_t _size)
{
    memset(_ptr, 0xCD, _size);
}


void MemoryShadow::cover(void *_ptr, size_t _size, bool _discard)
{
    assert(_ptr);

    const uint8_t *ptr = static_cast<const uint8_t *>(_ptr);
    const uint8_t *basePtr = lAlignPtr(ptr, BLOCK_ALIGN);

    if (_size != size) {
        static_assert(BLOCK_SIZE % BLOCK_ALIGN == 0, "inconsistent block align/size");
        nBlocks = (ptr + _size - basePtr + BLOCK_SIZE - 1)/BLOCK_SIZE;

        hashPtr = (uint32_t *)realloc(hashPtr, nBlocks * sizeof *hashPtr);
        size = _size;
    }

    realPtr = ptr;

    if (_discard) {
        zero(_ptr, size);
    }

    const uint8_t *blockPtr = basePtr;
    if (_discard) {
        hashPtr[0] = hashBlock(blockPtr);
        for (size_t i = 1; i < nBlocks; ++i) {
            hashPtr[i] = hashPtr[0];
        }
    } else {
        for (size_t i = 0; i < nBlocks; ++i) {
            hashPtr[i] = hashBlock(blockPtr);
            blockPtr += BLOCK_SIZE;
        }
    }
}


void MemoryShadow::update(Callback callback) const
{
    const uint8_t *realStart   = realPtr   + size;
    const uint8_t *realStop    = realPtr;

    const uint8_t *blockPtr = lAlignPtr(realPtr, BLOCK_ALIGN);
    for (size_t i = 0; i < nBlocks; ++i) {
        uint32_t crc = hashBlock(blockPtr);
        if (crc != hashPtr[i]) {
            realStart = std::min(realStart, blockPtr);
            realStop  = std::max(realStop,  blockPtr + BLOCK_SIZE);
        }
        blockPtr += BLOCK_SIZE;
    }

    realStart = std::max(realStart, realPtr);
    realStop  = std::min(realStop,  realPtr + size);

    // Update the rest
    if (realStart < realStop) {
        callback(realStart, realStop - realStart);
    }
}
