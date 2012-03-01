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


#include <assert.h>

#include <string.h>

#include "glproc.hpp"



#include "trace_parser.hpp"
#include "retrace.hpp"


namespace retrace {

struct Region
{
    void *buffer;
    unsigned long long size;
};

typedef std::map<unsigned long long, Region> RegionMap;
static RegionMap regionMap;


static inline bool
contains(RegionMap::iterator &it, unsigned long long address) {
    return it->first <= address && (it->first + it->second.size) > address;
}


static inline bool
intersects(RegionMap::iterator &it, unsigned long long start, unsigned long long size) {
    unsigned long it_start = it->first;
    unsigned long it_stop  = it->first + it->second.size;
    unsigned long stop = start + size;
    return it_start < stop && start < it_stop;
}


// Iterator to the first region that contains the address, or the first after
static RegionMap::iterator
lowerBound(unsigned long long address) {
    RegionMap::iterator it = regionMap.lower_bound(address);

    while (it != regionMap.begin()) {
        RegionMap::iterator pred = it;
        --pred;
        if (contains(pred, address)) {
            it = pred;
        } else {
            break;
        }
    }

    return it;
}

// Iterator to the first region that starts after the address
static RegionMap::iterator
upperBound(unsigned long long address) {
    RegionMap::iterator it = regionMap.upper_bound(address);

    return it;
}

void
addRegion(unsigned long long address, void *buffer, unsigned long long size)
{
    if (!address) {
        // Ignore NULL pointer
        assert(!buffer);
        return;
    }

#ifndef NDEBUG
    RegionMap::iterator start = lowerBound(address);
    RegionMap::iterator stop = upperBound(address + size);
    if (0) {
        // Forget all regions that intersect this new one.
        regionMap.erase(start, stop);
    } else {
        for (RegionMap::iterator it = start; it != stop; ++it) {
            std::cerr << std::hex << "warning: "
                "region 0x" << address << "-0x" << (address + size) << " "
                "intersects existing region 0x" << it->first << "-0x" << (it->first + it->second.size) << "\n" << std::dec;
            assert(intersects(it, address, size));
        }
    }
#endif

    assert(buffer);

    Region region;
    region.buffer = buffer;
    region.size = size;

    regionMap[address] = region;
}

static RegionMap::iterator
lookupRegion(unsigned long long address) {
    RegionMap::iterator it = regionMap.lower_bound(address);

    if (it == regionMap.end() ||
        it->first > address) {
        if (it == regionMap.begin()) {
            return regionMap.end();
        } else {
            --it;
        }
    }

    assert(contains(it, address));
    return it;
}

void
delRegion(unsigned long long address) {
    RegionMap::iterator it = lookupRegion(address);
    if (it != regionMap.end()) {
        regionMap.erase(it);
    } else {
        assert(0);
    }
}


void
delRegionByPointer(void *ptr) {
    for (RegionMap::iterator it = regionMap.begin(); it != regionMap.end(); ++it) {
        if (it->second.buffer == ptr) {
            regionMap.erase(it);
            return;
        }
    }
    assert(0);
}

void *
lookupAddress(unsigned long long address) {
    RegionMap::iterator it = lookupRegion(address);
    if (it != regionMap.end()) {
        unsigned long long offset = address - it->first;
        assert(offset < it->second.size);
        return (char *)it->second.buffer + offset;
    }

    if (address >= 0x00400000) {
        std::cerr << "warning: could not translate address 0x" << std::hex << address << std::dec << "\n";
    }

    return (void *)(uintptr_t)address;
}


class Translator : protected trace::Visitor
{
protected:
    bool bind;

    void *result;

    void visit(trace::Null *) {
        result = NULL;
    }

    void visit(trace::Blob *blob) {
        result = blob->toPointer(bind);
    }

    void visit(trace::Pointer *p) {
        result = lookupAddress(p->value);
    }

public:
    Translator(bool _bind) :
        bind(_bind),
        result(NULL)
    {}

    void * operator() (trace::Value *node) {
        _visit(node);
        return result;
    }
};


void *
toPointer(trace::Value &value, bool bind) {
    return Translator(bind) (&value);
}


static void retrace_malloc(trace::Call &call) {
    size_t size = call.arg(0).toUInt();
    unsigned long long address = call.ret->toUIntPtr();

    if (!address) {
        return;
    }

    void *buffer = malloc(size);
    if (!buffer) {
        std::cerr << "error: failed to allocated " << size << " bytes.";
        return;
    }

    addRegion(address, buffer, size);
}


static void retrace_memcpy(trace::Call &call) {
    void * dest = toPointer(call.arg(0));
    void * src  = toPointer(call.arg(1));
    size_t n    = call.arg(2).toUInt();

    if (!dest || !src || !n) {
        return;
    }

    memcpy(dest, src, n);
}


const retrace::Entry stdc_callbacks[] = {
    {"malloc", &retrace_malloc},
    {"memcpy", &retrace_memcpy},
    {NULL, NULL}
};


} /* retrace */
