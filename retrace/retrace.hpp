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

#ifndef _RETRACE_HPP_
#define _RETRACE_HPP_

#include <assert.h>
#include <string.h>

#include <list>
#include <map>
#include <ostream>

#include "trace_model.hpp"
#include "trace_parser.hpp"
#include "trace_profiler.hpp"

#include "scoped_allocator.hpp"


namespace image {
    class Image;
}


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
    alloc(const trace::Value *value, size_t size) {
        const trace::Array *array = dynamic_cast<const trace::Array *>(value);
        if (array) {
            return ::ScopedAllocator::alloc(array->size() * size);
        }
        const trace::Null *null = dynamic_cast<const trace::Null *>(value);
        if (null) {
            return NULL;
        }
        assert(0);
        return NULL;
    }

};


/**
 * Output verbosity when retracing files.
 */
extern int verbosity;

/**
 * Debugging checks.
 */
extern bool debug;

/**
 * Add profiling data to the dump when retracing.
 */
extern bool profiling;
extern bool profilingCpuTimes;
extern bool profilingGpuTimes;
extern bool profilingPixelsDrawn;
extern bool profilingMemoryUsage;

/**
 * State dumping.
 */
extern bool dumpingState;


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
extern bool coreProfile;

extern unsigned frameNo;
extern unsigned callNo;


std::ostream &warning(trace::Call &call);


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
    getSnapshot(void) {
        return NULL;
    }

    virtual bool
    dumpState(std::ostream &os) {
        return false;
    }
};


extern Dumper *dumper;


void
setUp(void);

void
addCallbacks(retrace::Retracer &retracer);

void
frameComplete(trace::Call &call);



void
flushRendering(void);

void
waitForInput(void);

void
cleanUp(void);


} /* namespace retrace */

#endif /* _RETRACE_HPP_ */
