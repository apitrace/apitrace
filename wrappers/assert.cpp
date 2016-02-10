/**************************************************************************
 *
 * Copyright 2015 VMware, Inc.
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
 * It's convenient to just use the standard assert() macro, but on Windows we
 * want to ensure the messages go to somewhere we can actually see.
 */

#if defined(_WIN32) && !defined(NDEBUG)


#include <assert.h>

#include "os.hpp"


extern "C" void
_assert(const char *_Message, const char *_File, unsigned _Line)
{
    os::log("Assertion failed: %s, file %s, line %u\n", _Message, _File, _Line);
    os::abort();
}


extern "C" void
_wassert(const wchar_t * _Message, const wchar_t *_File, unsigned _Line)
{
    os::log("Assertion failed: %S, file %S, line %u\n", _Message, _File, _Line);
    os::abort();
}


// Hack to force this module to be linked on MinGW
#ifdef __GNUC__
__attribute__ ((constructor))
static void
force_link(void)
{}
#endif /* __GNUC__ */


#endif /* _WIN32 && !NDEBUG */
