/**************************************************************************
 *
 * Copyright 2017 VMware, Inc
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

#include "process_name.hpp"

#include <assert.h>

#if defined(__linux__)
#include <features.h>  // __GLIBC__
#endif

#include <iostream>


#if defined(__GLIBC__)

#include <dlfcn.h>
#include <string.h>


// TODO: Intercept readlink("/proc/self/exe")

void
setProcessName(const char *processName)
{
    char **p__progname_full = (char **)dlsym(RTLD_DEFAULT, "__progname_full");
    if (p__progname_full == nullptr) {
        std::cerr << "error: failed to get address of __progname_full symbol" << std::endl;
        return;
    }

    char *progname_full = strdup(processName);
    assert(progname_full != nullptr);

    *p__progname_full = progname_full;

    char **p__progname = (char **)dlsym(RTLD_DEFAULT, "__progname");
    if (p__progname == nullptr) {
        std::cerr << "error: failed to get address of __progname symbol" << std::endl;
        return;
    }

    char *progname = strrchr(progname_full, '/');
    if (progname == nullptr) {
        progname = progname_full;
    } else {
        progname += 1;
    }

    *p__progname = progname;
}


#else

void
setProcessName(const char *processName)
{
}

#endif
