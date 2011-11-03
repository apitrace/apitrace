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

#include <string.h>

#include <list>
#include <map>
#include <ostream>

#include "trace_model.hpp"


namespace retrace {


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
    void *next;

public:
    ScopedAllocator() :
        next(NULL) {
    }

    inline void *
    alloc(size_t size) {
        if (!size) {
            return NULL;
        }

        void * * buf = static_cast<void **>(malloc(sizeof(void *) + size));
        if (!buf) {
            return NULL;
        }

        *buf = next;
        next = buf;

        return &buf[1];
    }

    template< class T >
    inline T *
    alloc(size_t n = 1) {
        return static_cast<T *>(alloc(sizeof(T) * n));
    }

    inline
    ~ScopedAllocator() {
        while (next) {
            void *temp = *static_cast<void **>(next);
            free(next);
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
