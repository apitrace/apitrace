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

#include "os_time.hpp"
#include "retrace.hpp"

#ifdef _WIN32
#include <dxerr.h>
#endif


namespace retrace {


trace::DumpFlags dumpFlags = trace::DUMP_FLAG_THREAD_IDS;


static bool call_dumped = false;


static void dumpCall(trace::Call &call) {
    if (verbosity >= 0 && !call_dumped) {
        trace::dump(call, std::cout, dumpFlags);
        std::cout.flush();
        call_dumped = true;
    }
}


std::ostream &warning(trace::Call &call) {
    dumpCall(call);

    std::cerr << call.no << ": ";
    std::cerr << "warning: ";

    return std::cerr;
}


#ifdef _WIN32
void
failed(trace::Call &call, HRESULT hr)
{
    std::ostream &os = warning(call);

    os << "failed with 0x" << std::hex << hr << std::dec;

    LPCSTR lpszErrorString = DXGetErrorStringA(hr);
    assert(lpszErrorString);
    os << " (" << lpszErrorString << "): ";

    char szErrorDesc[512];
    DXGetErrorDescriptionA(hr, szErrorDesc, sizeof szErrorDesc);
    os << szErrorDesc;

    os << "\n";
}
#endif /* _WIN32 */


void
checkMismatch(trace::Call &call, const char *expr, trace::Value *traceValue, long actualValue)
{
    assert(traceValue);
    long traceIntValue = traceValue->toSInt();
    if (traceIntValue == actualValue) {
        return;
    }

    std::ostream &os = warning(call);
    os << "mismatch in " << expr << ": expected " << traceIntValue << " but got " << actualValue << "\n";
}


void ignore(trace::Call &call) {
    (void)call;
}

void unsupported(trace::Call &call) {
    warning(call) << "unsupported " << call.name() << " call\n";
}

inline void Retracer::addCallback(const Entry *entry) {
    assert(entry->name);
    assert(entry->callback);
    map[entry->name] = entry->callback;
}


void Retracer::addCallbacks(const Entry *entries) {
    while (entries->name && entries->callback) {
        addCallback(entries++);
    }
}


void Retracer::retrace(trace::Call &call) {
    call_dumped = false;

    Callback callback = 0;

    trace::Id id = call.sig->id;
    if (id >= callbacks.size()) {
        callbacks.resize(id + 1);
        callback = 0;
    } else {
        callback = callbacks[id];
    }

    if (!callback) {
        Map::const_iterator it = map.find(call.name());
        if (it == map.end()) {
            callback = &unsupported;
        } else {
            callback = it->second;
        }
        callbacks[id] = callback;
    }

    assert(callback);
    assert(callbacks[id] == callback);

    if (verbosity >= 1) {
        if (verbosity >= 2 ||
            (!(call.flags & trace::CALL_FLAG_VERBOSE) &&
             callback != &ignore)) {
            dumpCall(call);
        }
    }

    callback(call);
}


} /* namespace retrace */
