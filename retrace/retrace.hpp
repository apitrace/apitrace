/**************************************************************************
 *
 * Copyright 2011-2012 Jose Fonseca
 * Copyright (C) 2013 Intel Corporation. All rights reversed.
 * Author: Shuang He <shuang.he@intel.com>
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

#pragma once

#include <assert.h>
#include <string.h>

#include <list>
#include <map>
#include <ostream>

#ifdef _WIN32
#include <windows.h>
#endif

#include "trace_model.hpp"
#include "trace_parser.hpp"
#include "trace_profiler.hpp"
#include "trace_dump.hpp"

#include "scoped_allocator.hpp"


namespace image {
    class Image;
}

class StateWriter;


namespace retrace {


extern trace::Parser parser;
extern trace::Profiler profiler;


class ScopedAllocator : public ::ScopedAllocator
{
public:
    /**
     * Allocate an array with the same dimensions as the specified value.
     */
    inline void *
    allocArray(const trace::Value *value, size_t elemSize) {
        const trace::Array *array = value->toArray();
        if (array) {
            size_t numElems = array->size();
            size_t size = numElems * elemSize;
            void *ptr = ::ScopedAllocator::alloc(size);
            if (ptr) {
                memset(ptr, 0, size);
            }
            return ptr;
        }
        const trace::Null *null = value->toNull();
        if (null) {
            return NULL;
        }
        assert(0);
        return NULL;
    }

    /**
     * XXX: We must not compute sizeof(T) inside the function body! d3d8.h and
     * d3d9.h have declarations of D3DPRESENT_PARAMETERS and D3DVOLUME_DESC
     * structures with different size sizes.  Multiple specializations of these
     * will be produced (on debug builds, as on release builds the whole body
     * is inlined.), and the linker will pick up one, leading to wrong results
     * if the smallest specialization is picked.
     */
    template< class T >
    inline T *
    allocArray(const trace::Value *value, size_t sizeof_T = sizeof(T)) {
        return static_cast<T *>(allocArray(value, sizeof_T));
    }

};


/**
 * Output verbosity when retracing files.
 */
extern int verbosity;

/**
 * Debugging checks.
 */
extern unsigned debug;

/**
 * Whether to force windowed. Recommeded, as there is no guarantee that the
 * original display mode is available.
 */
extern bool forceWindowed;

/**
 * Add profiling data to the dump when retracing.
 */
extern unsigned curPass;
extern unsigned numPasses;
extern bool profilingWithBackends;
extern char* profilingCallsMetricsString;
extern char* profilingFramesMetricsString;
extern char* profilingDrawCallsMetricsString;
extern bool profilingListMetrics;
extern bool profilingNumPasses;

extern bool profiling;
extern bool profilingCpuTimes;
extern bool profilingGpuTimes;
extern bool profilingPixelsDrawn;
extern bool profilingMemoryUsage;

/**
 * State dumping.
 */
extern bool dumpingState;
extern bool dumpingSnapshots;


enum Driver {
    DRIVER_DEFAULT,
    DRIVER_HARDWARE, // force hardware
    DRIVER_SOFTWARE,
    DRIVER_REFERENCE,
    DRIVER_NULL,
    DRIVER_MODULE,
};

extern Driver driver;
extern const char *driverModule;

extern bool doubleBuffer;
extern unsigned samples;

extern unsigned frameNo;
extern unsigned callNo;

extern trace::DumpFlags dumpFlags;

std::ostream &warning(trace::Call &call);

#ifdef _WIN32
void failed(trace::Call &call, HRESULT hr);
#endif

void checkMismatch(trace::Call &call, const char *expr, trace::Value *traceValue, long actualValue);

void ignore(trace::Call &call);
void unsupported(trace::Call &call);


typedef void (*Callback)(trace::Call &call);

struct Entry {
    const char *name;
    Callback callback;
};


struct stringComparer {
  bool operator() (const char *a, const  char *b) const {
    return strcmp(a, b) < 0;
  }
};


extern const Entry stdc_callbacks[];


class Retracer
{
    typedef std::map<const char *, Callback, stringComparer> Map;
    Map map;

    std::vector<Callback> callbacks;

public:
    Retracer() {
        addCallbacks(stdc_callbacks);
    }

    virtual ~Retracer() {}

    void addCallback(const Entry *entry);
    void addCallbacks(const Entry *entries);

    void retrace(trace::Call &call);
};


class Dumper
{
public:
    virtual image::Image *
    getSnapshot(void) = 0;

    virtual bool
    canDump(void) = 0;

    virtual void
    dumpState(StateWriter &) = 0;
};


extern Dumper *dumper;


void
setFeatureLevel(const char *featureLevel);

void
setUp(void);

void
addCallbacks(retrace::Retracer &retracer);

void
frameComplete(trace::Call &call);


/**
 * Flush rendering (called when switching threads).
 */
void
flushRendering(void);

/**
 * Finish rendering (called before exiting.)
 */
void
finishRendering(void);

void
waitForInput(void);

void
cleanUp(void);


} /* namespace retrace */

