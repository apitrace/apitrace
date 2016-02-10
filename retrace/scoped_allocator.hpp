/**************************************************************************
 *
 * Copyright 2011-2012 Jose Fonseca
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


#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <algorithm>

/**
 * Similar to alloca(), but implemented with malloc.
 */
class ScopedAllocator
{
private:
    uintptr_t next;

public:
    inline
    ScopedAllocator() :
        next(0) {
    }

    inline void *
    alloc(size_t size) {
        /* Always return valid address, even when size is zero */
        size = std::max(size, sizeof(uintptr_t));

        uintptr_t * buf = static_cast<uintptr_t *>(malloc(sizeof(uintptr_t) + size));
        if (!buf) {
            return NULL;
        }

        *buf = next;
        next = reinterpret_cast<uintptr_t>(buf);
        assert((next & 1) == 0);

        return static_cast<void *>(&buf[1]);
    }
    
    /* XXX: See comment in retrace::ScopedAllocator::allocArray template. */
    template< class T >
    inline T *
    alloc(size_t size = 1, size_t sizeof_T = sizeof(T)) {
        return static_cast<T *>(alloc(sizeof_T * size));
    }

    /**
     * Prevent this pointer from being automatically freed.
     */
    template< class T >
    inline void
    bind(T *ptr) {
        if (ptr) {
            reinterpret_cast<uintptr_t *>(ptr)[-1] |= 1;
        }
    }

    inline
    ~ScopedAllocator() {
        while (next) {
            uintptr_t temp = *reinterpret_cast<uintptr_t *>(next);

            bool bind = temp & 1;
            temp &= ~1;

            if (!bind) {
                free(reinterpret_cast<void *>(next));
            }

            next = temp;
        }
    }
};


