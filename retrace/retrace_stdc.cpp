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


static void
retrace_memcpy(trace::Call &call)
{
    retrace::Range destRange;
    retrace::toRange(call.arg(0), destRange);

    retrace::Range srcRange;
    retrace::toRange(call.arg(1), srcRange);

    size_t n = call.arg(2).toUInt();

    if (!destRange.ptr || !srcRange.ptr || !n) {
        return;
    }

    // We don't support sources with swizzled pitches
    assert(srcRange.dims == 0);

    assert(destRange.dims <= 2);
    if (destRange.dims == 2 &&
        destRange.tracePitch != destRange.realPitch) {
        int srcPitch = destRange.tracePitch;
        int destPitch = destRange.realPitch;

        if (0) {
            retrace::warning(call) << "swizzling pitch from " << srcPitch << " to " << destPitch << "\n";
        }

        int destOffset = 0;
        int srcOffset = 0;
        int width = std::min(destPitch, srcPitch);
        while (destOffset + width <= destRange.len &&
               srcOffset + width <= srcRange.len) {
            memcpy((char *)destRange.ptr + destOffset,
                   (char *)srcRange.ptr + srcOffset, width);
            destOffset += destPitch;
            srcOffset += srcPitch;
        }

        if (destOffset < destRange.len &&
            srcOffset < srcRange.len) {
            width = std::min(destRange.len - destOffset, srcRange.len - srcOffset);
            memcpy((char *)destRange.ptr + destOffset,
                   (char *)srcRange.ptr + srcOffset, width);
        }

        return;
    }


    if (n > destRange.len) {
        retrace::warning(call) << "dest buffer overflow of " << n - destRange.len << " bytes\n";
    }

    if (n > srcRange.len) {
        retrace::warning(call) << "src buffer overflow of " << n - srcRange.len << " bytes\n";
    }

    n = std::min(n, destRange.len);
    n = std::min(n, srcRange.len);

    memcpy(destRange.ptr, srcRange.ptr, n);
}


const retrace::Entry retrace::stdc_callbacks[] = {
    {"malloc", &retrace_malloc},
    {"memcpy", &retrace_memcpy},
    {NULL, NULL}
};
