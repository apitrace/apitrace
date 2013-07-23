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
 * Comon definitions for all tracers.
 */


#ifndef _TRACE_HPP_
#define _TRACE_HPP_

/* Unless specified at build-time with -DTRACE_ENABLED_CHECK=0, auto-detect */

#ifndef TRACE_ENABLED_CHECK
# ifdef ANDROID
#  define TRACE_ENABLED_CHECK 1
# else
#  define TRACE_ENABLED_CHECK 0
# endif
#endif

namespace trace {


#if TRACE_ENABLED_CHECK

bool isTracingEnabled(void);

#else /* !TRACE_ENABLED_CHECK */

static inline bool
isTracingEnabled(void) {
    return true;
}

#endif /* !TRACE_ENABLED_CHECK */


} /* namespace trace */


#endif /* _TRACE_HPP_ */
