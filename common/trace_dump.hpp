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
 * Human-readible dumping.
 */

#ifndef _TRACE_DUMP_HPP_
#define _TRACE_DUMP_HPP_


#include <iostream>

#include "trace_model.hpp"


namespace trace {


typedef unsigned DumpFlags;

enum {
    DUMP_FLAG_NO_COLOR                 = (1 << 0),
    DUMP_FLAG_NO_ARG_NAMES             = (1 << 1),
    DUMP_FLAG_NO_CALL_NO               = (1 << 2),
};


void dump(Value *value, std::ostream &os, DumpFlags flags = 0);


inline std::ostream & operator <<(std::ostream &os, Value *value) {
    if (value) {
        dump(value, os);
    }
    return os;
}


void dump(Call &call, std::ostream &os, DumpFlags flags = 0);


inline std::ostream & operator <<(std::ostream &os, Call &call) {
    dump(call, os);
    return os;
}


} /* namespace trace */

#endif /* _TRACE_DUMP_HPP_ */
