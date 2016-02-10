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


#include <assert.h>
#include <string.h>

#include <iostream>
#include <algorithm>

#include "retrace.hpp"
#include "retrace_swizzle.hpp"


static void retrace_malloc(trace::Call &call) {
    size_t size = call.arg(0).toUInt();
    unsigned long long address = call.ret->toUIntPtr();

    if (!address) {
        return;
    }

    void *buffer = malloc(size);
    if (!buffer) {
        std::cerr << "error: failed to allocate " << size << " bytes.";
        return;
    }

    retrace::addRegion(call, address, buffer, size);
}


static void retrace_memcpy(trace::Call &call) {
    void * destPtr;
    size_t destLen;
    retrace::toRange(call.arg(0), destPtr, destLen);

    void * srcPtr;
    size_t srcLen;
    retrace::toRange(call.arg(1), srcPtr, srcLen);

    size_t n = call.arg(2).toUInt();

    if (!destPtr || !srcPtr || !n) {
        return;
    }

    if (n > destLen) {
        retrace::warning(call) << "dest buffer overflow of " << n - destLen << " bytes\n";
    }

    if (n > srcLen) {
        retrace::warning(call) << "src buffer overflow of " << n - srcLen << " bytes\n";
    }

    n = std::min(n, destLen);
    n = std::min(n, srcLen);

    memcpy(destPtr, srcPtr, n);
}


const retrace::Entry retrace::stdc_callbacks[] = {
    {"malloc", &retrace_malloc},
    {"memcpy", &retrace_memcpy},
    {NULL, NULL}
};
