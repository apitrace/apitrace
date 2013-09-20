/**************************************************************************
 *
 * Copyright 2013 Samsung
 * Contributed by Eugene Velesevich
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

#ifndef _OS_BACKTRACE_HPP_
#define _OS_BACKTRACE_HPP_

#include <vector>

#include "trace_model.hpp"

namespace os {

using trace::RawStackFrame;


#if defined(ANDROID) || defined(__ELF__)

std::vector<RawStackFrame> get_backtrace();
bool backtrace_is_needed(const char* fname);

#else

static inline std::vector<RawStackFrame> get_backtrace() {
    return std::vector<RawStackFrame>();
}

static inline bool backtrace_is_needed(const char*) {
    return false;
}

#endif

#if defined(__ELF__)

void dump_backtrace();

#else

static inline void dump_backtrace() {}

#endif

} /* namespace os */

#endif
