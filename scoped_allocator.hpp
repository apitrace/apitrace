/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
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


#ifndef _SCOPED_ALLOCATOR_HPP_
#define _SCOPED_ALLOCATOR_HPP_


#include <stdlib.h>


class scoped_allocator
{
private:
    void *next;

public:
    scoped_allocator() : next(NULL) {
    }

    inline void *
    alloc(size_t size) {
        if (!size) {
            return NULL;
        }

        void * * buf = (void * *)malloc(sizeof(void *) + size);
        if (!buf) {
            return NULL;
        }

        *buf = next;
        next = buf;

        return &buf[1];
    }

    template< class T >
    inline void operator() (T * &ptr, size_t n = 1) {
        ptr = (T *)alloc(sizeof(T) * n);
    }

    inline ~scoped_allocator() {
        while (next) {
            void *temp = *(void **)next;
            free(next);
            next = temp;
        }
    }
};


#endif
