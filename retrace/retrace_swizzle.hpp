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

#pragma once


#include <map>

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
    typedef typename base_type::const_iterator const_iterator;

    const_iterator end(void) const {
        return base.end();
    }

    const_iterator find(const T & key) const {
        return base.find(key);
    }

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

    /*
     * Handle situations where the application declares an array like
     *
     *   uniform vec4 myMatrix[4];
     *
     * then calls glGetUniformLocation(..., "myMatrix") and then infer the slot
     * numbers rather than explicitly calling glGetUniformLocation(...,
     * "myMatrix[0]"), etc.
     */
    T lookupUniformLocation(const T &key) {
        typename base_type::const_iterator it;
        it = base.upper_bound(key);
        if (it != base.begin()) {
            --it;
        } else {
            return (base[key] = key);
        }
        T t = it->second + (key - it->first);
        return t;
    }
};


void
addRegion(trace::Call &call, unsigned long long address, void *buffer, unsigned long long size);

void
delRegionByPointer(void *ptr);

void
toRange(trace::Value &value, void * & ptr, size_t & len);

void *
toPointer(trace::Value &value, bool bind = false);


void
addObj(trace::Call &call, trace::Value &value, void *obj);

void
delObj(trace::Value &value);

void *
toObjPointer(trace::Call &call, trace::Value &value);

template< class T >
inline T *
asObjPointer(trace::Call &call, trace::Value &value) {
    return static_cast<T *>(toObjPointer(call, value));
}


} /* namespace retrace */


