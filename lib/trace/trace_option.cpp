/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
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

#include "trace_option.hpp"

#include <string.h>
#include <iostream>
#include <stdlib.h>

namespace trace {

bool
boolOption(const char *option, bool default_) {
    if (!option) {
        return default_;
    }
    if (strcmp(option, "0") == 0 ||
        strcmp(option, "no") == 0 ||
        strcmp(option, "false") == 0) {
        return false;
    }
    if (strcmp(option, "1") == 0 ||
        strcmp(option, "yes") == 0 ||
        strcmp(option, "true") == 0) {
        return true;
    }
    std::cerr << "error: unexpected bool " << option << "\n";
    return default_;
}

int
intOption(const char *option, int default_) {
    if (!option) {
        return default_;
    }
    return atoi(option);
}

} /* namespace trace */
