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

#ifndef _TRIM_CALLSET_HPP_
#define _TRIM_CALLSET_HPP_

namespace trim {

/* A set of call numbers.
 *
 * This is designed as a more efficient replacement for
 * std::set<unsigned> which is used heavily within the trim code's
 * TraceAnalyzer. This is quite similar to the trace::CallSet with the
 * following differences:
 *
 *   Simplifications:
 *
 *	* This supports only the addition of a single call number, not
 *	  the addition of a range.
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
 *        merging two ranges). This keeps the length of the list down
 *        when succesively adding individual calls that could be
 *        efficiently expressed with a range.
 *
 * It would not be impossible to extend this code to support the
 * missing features of trace::CallSet, (though the 'step' and 'freq'
 * features would prevent some range-extending and merging
 * optimizations in some cases). Currently, trace::CallSet is not used
 * in any performance-critical areas, so it may not be important to
 * provide the performance imrpovements to it.
 */

typedef unsigned CallNo;

class CallRange {
public:
    CallNo first;
    CallNo last;
    int level;
    CallRange **next;

    CallRange(CallNo call_no, int level);

    bool contains(CallNo call_no);
};

class CallSet {
public:
    CallRange head;
    int max_level;

    CallSet();

    void add(CallNo call_no);

    bool contains(CallNo call_no);
};

} /* namespace trim */

#endif /* _TRIM_CALLSET_HPP_ */
