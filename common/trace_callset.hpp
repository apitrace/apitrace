/**************************************************************************
 *
 * Copyright 2012 VMware, Inc.
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
 * Representation of call sets.
 *
 * Grammar:
 *
 *     set = '@' filename
 *         | range ( ',' ? range ) *
 *
 *     range = interval ( '/' frequency )
 *
 *     interval = '*'
 *              | number
 *              | start_number '-' end_number
 *
 *     frequency = divisor
 *               | "frame"
 *               | "rendertarget" | "fbo"
 *               | "render | "draw"
 *
 */

#ifndef _TRACE_CALLSET_HPP_
#define _TRACE_CALLSET_HPP_


#include <limits>
#include <list>

#include "trace_model.hpp"
#include "trace_fast_callset.hpp"

namespace trace {

    // Aliases for call flags
    enum {
        FREQUENCY_NONE         = 0,
        FREQUENCY_FRAME        = CALL_FLAG_END_FRAME,
        FREQUENCY_RENDERTARGET = CALL_FLAG_END_FRAME | CALL_FLAG_SWAP_RENDERTARGET,
        FREQUENCY_RENDER       = CALL_FLAG_RENDER,
        FREQUENCY_ALL          = 0xffffffff
    };

    // A linear range of calls
    class CallRange
    {
    public:
        CallNo start;
        CallNo stop;
        CallNo step;
        CallFlags freq;

        CallRange(CallNo callNo) :
            start(callNo),
            stop(callNo),
            step(1),
            freq(FREQUENCY_ALL)
        {}

        CallRange(CallNo _start, CallNo _stop, CallNo _step = 1, CallFlags _freq = FREQUENCY_ALL) :
            start(_start),
            stop(_stop),
            step(_step),
            freq(_freq)
        {}

        bool
        contains(CallNo callNo, CallFlags callFlags) const {
            return callNo >= start &&
                   callNo <= stop &&
                   ((callNo - start) % step) == 0 &&
                   ((callFlags & freq) ||
                    freq == FREQUENCY_ALL);
        }
    };


    // A collection of call ranges
    class CallSet
    {
    private:
        CallRange limits;

    public:
        FastCallSet fast_call_set;

        // TODO: use binary tree to speed up lookups, (less important
        // now that we are using FastCallSet for ranges without step
        // or freq).
        typedef std::list< CallRange > RangeList;
        RangeList ranges;

        CallSet(): limits(std::numeric_limits<CallNo>::min(), std::numeric_limits<CallNo>::max()) {}

        CallSet(CallFlags freq);

        CallSet(const char *str);

        // Not empty set
        inline bool
        empty() const {
            return fast_call_set.empty() && ranges.empty();
        }

        void
        addRange(const CallRange & range) {
            if (range.start <= range.stop &&
                range.freq != FREQUENCY_NONE) {

                if (empty()) {
                    limits.start = range.start;
                    limits.stop = range.stop;
                } else {
                    if (range.start < limits.start)
                        limits.start = range.start;
                    if (range.stop > limits.stop)
                        limits.stop = range.stop;
                }

                /* Optimize by using fast_call_set whenever possible */
                if (range.step == 1 && range.freq == FREQUENCY_ALL) {
                    fast_call_set.add(range.start, range.stop);
                } else {
                    RangeList::iterator it = ranges.begin();
                    while (it != ranges.end() && it->start < range.start) {
                        ++it;
                    }

                    ranges.insert(it, range);
                }
            }
        }

        inline bool
        contains(CallNo callNo, CallFlags callFlags = FREQUENCY_ALL) const {
            if (empty()) {
                return false;
            }
            if (fast_call_set.contains(callNo))
                return true;
            RangeList::const_iterator it;
            for (it = ranges.begin(); it != ranges.end() && it->start <= callNo; ++it) {
                if (it->contains(callNo, callFlags)) {
                    return true;
                }
            }
            return false;
        }

        inline bool
        contains(const trace::Call &call) {
            return contains(call.no, call.flags);
        }

        CallNo getFirst() {
            return limits.start;
        }

        CallNo getLast() {
            return limits.stop;
        }
    };


    CallSet parse(const char *string);


} /* namespace trace */


#endif /* _TRACE_CALLSET_HPP_ */
