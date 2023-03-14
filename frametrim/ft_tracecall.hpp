/*********************************************************************
 *
 * Copyright 2020 Collabora Ltd
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

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <bitset>

namespace frametrim {

enum ECallFlags {
    tc_required,
    tc_persistent_mapping,
    tc_skip_record_in_fbo,
    tc_last
};

class CallSet;

class TraceCall {
public:
    using Pointer = std::shared_ptr<TraceCall>;

    TraceCall(const trace::Call& call, const std::string& name);

    TraceCall(const trace::Call& call, unsigned nparam_sel);

    TraceCall(const trace::Call& call);

    unsigned callNo() const { return m_trace_call_no;};
    const std::string& name() const { return m_name;}
    const std::string& nameWithParams() const { return m_name_with_params;}
private:

    static std::string nameWithParamsel(const trace::Call& call, unsigned nsel);

    unsigned m_trace_call_no;
    std::string m_name;
    std::string m_name_with_params;
};
using PTraceCall = TraceCall::Pointer;

using StateCallMap=std::unordered_map<std::string, PTraceCall>;

inline PTraceCall trace2call(const trace::Call& call) {
    return std::make_shared<TraceCall>(call);
}

struct CallHash {
    std::size_t operator () (const PTraceCall& call) const {
        return std::hash<unsigned>{}(call->callNo());
    }
};

class CallSet {
public:
    using Pointer = std::shared_ptr<CallSet>;

    using const_iterator = std::unordered_set<PTraceCall, CallHash>::const_iterator;

    void insert(PTraceCall call);
    void clear();
    bool empty() const;
    size_t size() const {return m_calls.size(); }
    const_iterator begin() const;
    const_iterator end() const;
private:
    std::unordered_set<PTraceCall, CallHash> m_calls;
};
using PCallSet = std::shared_ptr<CallSet>;

}
