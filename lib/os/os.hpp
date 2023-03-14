/**************************************************************************
 *
 * Copyright 2010 VMware, Inc.
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
 * Simple OS abstraction layer.
 */

#pragma once

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#ifndef strcasecmp
#define strcasecmp stricmp
#endif
#endif /* !_WIN32 */

namespace os {

void log(const char *format, ...)
#if defined(__MINGW32__)
    __attribute__ ((format (__MINGW_PRINTF_FORMAT, 1, 2)))
#elif  defined(__GNUC__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

#if defined _WIN32 || defined __CYGWIN__
  /* We always use .def files on windows for now */
  #if 0
  #define PUBLIC __declspec(dllexport)
  #else
  #define PUBLIC
  #endif
  #define PRIVATE
#else
  #define PUBLIC __attribute__ ((visibility("default")))
  #define PRIVATE __attribute__ ((visibility("hidden")))
#endif

/**
 * Exit immediately.
 *
 * This should be called only from the wrappers, when there is no safe way of
 * failing gracefully.
 */
[[noreturn]]
void
abort(void);

void
breakpoint(void);


void setExceptionCallback(void (*callback)(void));
void resetExceptionCallback(void);

/**
 * Returns a pseudo-random integer in the range 0 to RAND_MAX.
 */
static inline int
random(void) {
#ifdef _WIN32
    return ::rand();
#else
    return ::random();
#endif
}

} /* namespace os */

