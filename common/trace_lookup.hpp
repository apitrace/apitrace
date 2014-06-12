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

/*
 * Helper code for function name indexed lookup tables.
 */

#ifndef _TRACE_LOOKUP_HPP_
#define _TRACE_LOOKUP_HPP_


#include <assert.h>
#include <string.h>

#include <algorithm>
#include <iostream>


namespace trace {


/**
 * Generic type for (name, value) pairs.
 */
template< class T >
struct Entry
{
    const char *name;
    T value;
};


/**
 * Function object which compare entries by name.
 */
template< class T >
struct EntryCompare {
    inline bool
    operator() (const Entry<T> & a, const Entry<T> & b) const {
        return strcmp(a.name, b.name) < 0;
    }
};


/**
 * Lookup the entry with the given name, .
 *
 * The entry table must be sorted alphabetically (the same rules used by
 * strcmp).
 */
template< class T, std::size_t n >
inline const T &
entryLookup(const char *name, const Entry<T> (& entries)[n], const T & default_)
{
    typedef const Entry<T> * ConstIterator;

    ConstIterator first = &entries[0];
    ConstIterator last = &entries[n];

    assert(first != last);

    Entry<T> reference;
    reference.name = name;

    EntryCompare<T> compare;

#ifndef NDEBUG
    for (ConstIterator it = first; it != last; ++it) {
        ConstIterator next = it + 1;
        if (next != last && !compare(*it, *next)) {
            std::cerr << "error: " << it->name << " and " << next->name << " not properly sorted\n";
            assert(0);
        }
    }
#endif
    
    first = std::lower_bound(first, last, reference, compare);
  
    if (first == last || compare(reference, *first)) {
        return default_;
    }

    return first->value;
}


} /* namespace trace */

#endif /* _TRACE_LOOKUP_HPP_ */
