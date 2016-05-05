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

#pragma once

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

class FastCallRangePtr;

class FastCallRange {
public:
    CallNo first;
    CallNo last;
    int level;
    std::vector<FastCallRangePtr> next;

    // (NOTE: Initalize ref_counter to 0 in all constructors)
    FastCallRange(CallNo first, CallNo last, int level);

    bool contains(CallNo call_no) const;

private:
    friend class FastCallRangePtr;
    size_t ref_counter;
    // ref_counter must be initialized to 0 by all constructors
    // ref_counter is the number of FastCallRangePtr objects that point at this
};

class FastCallRangePtr {
public:
    FastCallRange* operator-> () { return  this->ptr; }
    FastCallRange& operator * () { return *this->ptr; }
    FastCallRange* operator ()() { return  this->ptr; } // get pointer


    FastCallRangePtr () : ptr(0) {}
    FastCallRangePtr(FastCallRange* _ptr) : ptr(_ptr)
                                 { if (this->ptr) ++this->ptr->ref_counter; }

   ~FastCallRangePtr()           { if (this->ptr)
                                       if (--this->ptr->ref_counter == 0)
                                           delete this->ptr;
                                 }

    FastCallRangePtr(FastCallRangePtr const& _ptr) : ptr(_ptr.ptr)
                                 { if (this->ptr) ++this->ptr->ref_counter; }

    FastCallRangePtr& operator= (FastCallRangePtr const& new_ptr)
        {   // DO NOT CHANGE THE ORDER OF THESE STATEMENTS!
            // (This order properly handles self-assignment)
            // (This order also properly handles recursion, e.g.,
            //  if a FastCallRange contains FastCallRangePtrs)
            FastCallRange* const old_ptr = this->ptr;
            this->ptr = new_ptr.ptr;
            if (this->ptr)
                ++this->ptr->ref_counter;
            if (old_ptr) {
                if (--old_ptr->ref_counter == 0)
                    delete old_ptr;
            }
            return *this;
        }
private:
    FastCallRange* ptr;
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

