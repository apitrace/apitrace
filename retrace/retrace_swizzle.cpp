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


#include <assert.h>

#include <string.h>

#include "retrace.hpp"
#include "retrace_swizzle.hpp"


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

#ifndef NDEBUG
    if (it != regionMap.end()) {
        assert(contains(it, address) || it->first > address);
    }
#endif

    return it;
}

// Iterator to the first region that starts after the address
static RegionMap::iterator
upperBound(unsigned long long address) {
    RegionMap::iterator it = regionMap.upper_bound(address);

#ifndef NDEBUG
    if (it != regionMap.end()) {
        assert(it->first >= address);
    }
#endif

    return it;
}

void
addRegion(unsigned long long address, void *buffer, unsigned long long size)
{
    if (retrace::verbosity >= 2) {
        std::cout
            << "region "
            << std::hex
            << "0x" << address << "-0x" << (address + size)
            << " -> "
            << "0x" << (uintptr_t)buffer << "-0x" << ((uintptr_t)buffer + size)
            << std::dec
            << "\n";
    }

    if (!address) {
        // Ignore NULL pointer
        assert(!buffer);
        return;
    }

#ifndef NDEBUG
    RegionMap::iterator start = lowerBound(address);
    RegionMap::iterator stop = upperBound(address + size - 1);
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
        void *addr = (char *)it->second.buffer + offset;

        if (retrace::verbosity >= 2) {
            std::cout
                << "region "
                << std::hex
                << "0x" << address
                << " <- "
                << "0x" << (uintptr_t)addr
                << std::dec
                << "\n";
        }

        return addr;
    }

    if (retrace::debug && address >= 64 * 1024 * 1024) {
        /* Likely not an offset, but an address that should had been swizzled */
        std::cerr << "warning: passing high address 0x" << std::hex << address << std::dec << " as uintptr_t\n";
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



static std::map<unsigned long long, void *> _obj_map;

void
addObj(trace::Call &call, trace::Value &value, void *obj) {
    unsigned long long address = value.toUIntPtr();

    if (!address) {
        if (obj) {
            warning(call) << "unexpected non-null object\n";
        }
        return;
    }

    if (!obj) {
        warning(call) << "got null for object 0x" << std::hex << address << std::dec << "\n";
    }

    _obj_map[address] = obj;
    
    if (retrace::verbosity >= 2) {
        std::cout << std::hex << "obj 0x" << address << " -> 0x" << size_t(obj) << std::dec << "\n";
    }
}

void
delObj(trace::Value &value) {
    unsigned long long address = value.toUIntPtr();
    _obj_map.erase(address);
    if (retrace::verbosity >= 2) {
        std::cout << std::hex << "obj 0x" << address << " del\n";
    }
}

void *
toObjPointer(trace::Call &call, trace::Value &value) {
    unsigned long long address = value.toUIntPtr();

    void *obj;
    if (address) {
        obj = _obj_map[address];
        if (!obj) {
            warning(call) << "unknown object 0x" << std::hex << address << std::dec << "\n";
        }
    } else {
        obj = NULL;
    }

    if (retrace::verbosity >= 2) {
        std::cout << std::hex << "obj 0x" << address << " <- 0x" << size_t(obj) << std::dec << "\n";
    }

    return obj;
}


} /* retrace */
