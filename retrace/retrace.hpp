/**************************************************************************
 *
 * Copyright 2011-2012 Jose Fonseca
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
#include <stdint.h>

#include <list>
#include <map>
#include <ostream>

#include "trace_model.hpp"
#include "trace_parser.hpp"


namespace retrace {


extern trace::Parser parser;


/**
 * Handle map.
 *
 * It is just like a regular std::map<T, T> container, but lookups of missing
 * keys return the key instead of default constructor.
 *
 * This is necessary for several GL named objects, where one can either request
 * the implementation to generate an unique name, or pick a value never used
 * before.
 *
 * XXX: In some cases, instead of returning the key, it would make more sense
 * to return an unused data value (e.g., container count).
 */
template <class T>
class map
{
private:
    typedef std::map<T, T> base_type;
    base_type base;

public:

    T & operator[] (const T &key) {
        typename base_type::iterator it;
        it = base.find(key);
        if (it == base.end()) {
            return (base[key] = key);
        }
        return it->second;
    }
    
    const T & operator[] (const T &key) const {
        typename base_type::const_iterator it;
        it = base.find(key);
        if (it == base.end()) {
            return (base[key] = key);
        }
        return it->second;
    }
};


/**
 * Similar to alloca(), but implemented with malloc.
 */
class ScopedAllocator
{
private:
    uintptr_t next;

public:
    ScopedAllocator() :
        next(0) {
    }

    inline void *
    alloc(size_t size) {
        /* Always return valid address, even when size is zero */
        size = std::max(size, sizeof(uintptr_t));

        uintptr_t * buf = static_cast<uintptr_t *>(malloc(sizeof(uintptr_t) + size));
        if (!buf) {
            return NULL;
        }

        *buf = next;
        next = reinterpret_cast<uintptr_t>(buf);
        assert((next & 1) == 0);

        return static_cast<void *>(&buf[1]);
    }

    template< class T >
    inline T *
    alloc(size_t n = 1) {
        return static_cast<T *>(alloc(sizeof(T) * n));
    }

    /**
     * Allocate an array with the same dimensions as the specified value.
     */
    template< class T >
    inline T *
    alloc(const trace::Value *value) {
        const trace::Array *array = dynamic_cast<const trace::Array *>(value);
        if (array) {
            return alloc<T>(array->size());
        }
        const trace::Null *null = dynamic_cast<const trace::Null *>(value);
        if (null) {
            return NULL;
        }
        assert(0);
        return NULL;
    }

    /**
     * Prevent this pointer from being automatically freed.
     */
    template< class T >
    inline void
    bind(T *ptr) {
        if (ptr) {
            reinterpret_cast<uintptr_t *>(ptr)[-1] |= 1;
        }
    }

    inline
    ~ScopedAllocator() {
        while (next) {
            uintptr_t temp = *reinterpret_cast<uintptr_t *>(next);

            bool bind = temp & 1;
            temp &= ~1;

            if (!bind) {
                free(reinterpret_cast<void *>(next));
            }

            next = temp;
        }
    }
};


void
addRegion(unsigned long long address, void *buffer, unsigned long long size);

void
delRegionByPointer(void *ptr);

void *
toPointer(trace::Value &value, bool bind = false);


/**
 * Output verbosity when retracing files.
 */
extern int verbosity;

/**
 * Avoid expensive checks when benchmarking.
 */
extern bool benchmark;

/**
 * Add profiling data to the dump when retracing.
 */
extern bool profiling;


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


} /* namespace retrace */

#endif /* _RETRACE_HPP_ */
