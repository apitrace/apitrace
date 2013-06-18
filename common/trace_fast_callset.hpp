/*********************************************************************
 *
 * Copyright 2012 Intel Corporation
 * Copyright 2012 VMware, Inc.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *********************************************************************/

#ifndef _TRACE_FAST_CALLSET_HPP_
#define _TRACE_FAST_CALLSET_HPP_

#include "trace_model.hpp"

namespace trace {

/* A set of call numbers.
 *
 * This was originally designed as a more efficient replacement for
 * std::set<unsigned> which was used heavily within the trim code's
 * TraceAnalyzer. This is quite similar to the trace::CallSet with the
 * following differences:
 *
 *   Simplifications:
 *
 *	* There is no support here for the 'step' and 'freq' features
 *	  supported by trace::CallSet.
 *
 *   Sophistications:
 *
 *	* This callset is implemented with a skip list for
 *	  (probabilistically) logarithmic lookup times for
 *	  out-of-order lookups.
 *
 *	* This callset optimizes the addition of new calls which are
 *        within or adjacent to existing ranges, (by either doing
 *        nothing, expanding the limits of an existing range, or also
 *        merging two or more ranges).
 *
 * It would not be impossible to extend this code to support the
 * missing features of trace::CallSet, (though the 'step' and 'freq'
 * features would prevent some range-extending and merging
 * optimizations in some cases).
 */

class FastCallRange {
public:
    CallNo first;
    CallNo last;
    int level;
    FastCallRange **next;

    FastCallRange(CallNo first, CallNo last, int level);

    bool contains(CallNo call_no) const;
};

class FastCallSet {
public:
    FastCallRange head;
    int max_level;

    FastCallSet();

    bool empty(void) const;

    void add(CallNo first, CallNo last);

    void add(CallNo call_no);

    bool contains(CallNo call_no) const;
};

} /* namespace trace */

#endif /* _TRACE_FAST_CALLSET_HPP_ */
