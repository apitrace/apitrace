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

/*
 * Simple OS time measurement abstraction.
 */

#ifndef _OS_TIME_HPP_
#define _OS_TIME_HPP_


#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <time.h>
#else
#include <sys/time.h>
#endif


namespace os {

    // OS dependent time frequency
#if defined(_WIN32)
    // runtime variable on Windows
    extern long long timeFrequency;
#elif defined(__linux__)
    // nanoseconds
    static const long long timeFrequency = 1000000000;
#else
    // microseconds on
    static const long long timeFrequency = 1000000;
#endif

    // Time from an unknown base in a unit determined by timeFrequency
    inline long long
    getTime(void) {
#if defined(_WIN32)
        if (!timeFrequency) {
            LARGE_INTEGER frequency;
            QueryPerformanceFrequency(&frequency);
            timeFrequency = frequency.QuadPart;
        }
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        return counter.QuadPart;
#elif defined(__linux__)
        struct timespec tp;
        if (clock_gettime(CLOCK_REALTIME, &tp) == -1) {
            return 0;
        }
        return tp.tv_sec * 1000000000LL + tp.tv_nsec;
#else
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec * 1000000LL + tv.tv_usec;
#endif
    }


} /* namespace os */

#endif /* _OS_TIME_HPP_ */
