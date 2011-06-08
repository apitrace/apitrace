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


#include <string.h>
#include <iostream>

#include "retrace.hpp"


namespace retrace {


int verbosity = 0;


void ignore(Trace::Call &call) {
    (void)call;
}

void retrace_unknown(Trace::Call &call) {
    if (verbosity >= 0) {
        std::cerr << call.no << ": warning: unknown call " << call.name() << "\n";
    }
}

void dispatch(Trace::Call &call, const Entry *entries, unsigned num_entries)
{
    /* TODO: do a bisection instead of a linear search */

    const char *name = call.name();
    for (unsigned i = 0; i < num_entries; ++i) {
        if (strcmp(name, entries[i].name) == 0) {
            entries[i].callback(call);
            return;
        }
    }

    retrace_unknown(call);
}


} /* namespace retrace */
