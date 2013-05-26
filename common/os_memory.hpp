/**************************************************************************
 *
 * Copyright (C) 2013 Intel Corporation. All rights reversed.
 * Author: Shuang He <shuang.he@intel.com>
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

#ifndef _OS_MEMORY_HPP_
#define _OS_MEMORY_HPP_

#ifdef HAVE_READPROC_H
#include <proc/readproc.h>

namespace os {
    inline long long
    getVsize(void) {
        proc_t proc;
        look_up_our_self(&proc);
        return proc.vsize;
    }

    inline long long
    getRss(void) {
        proc_t proc;
        look_up_our_self(&proc);
        return proc.rss;
    }
} /* namespace os */

#else
namespace os {
    inline long long
    getVsize(void) {
        return 0;
    }

    inline long long
    getRss(void) {
        return 0;
    }
} /* namespace os */
#endif

#endif /* _OS_MEMORY_HPP_ */
