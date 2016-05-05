/**************************************************************************
 *
 * Copyright 2015 VMware, Inc.
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


#include "guids.hpp"

#include <assert.h>
#include <inttypes.h>
#include <string.h>

#include <algorithm>

#include <os_string.hpp>


struct GuidEntry
{
    const char *name;
    GUID guid;
};


static const GuidEntry
guidEntries[] = {

#define GUID_ENTRY(l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8,name) \
    { #name, { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } } },

#include "guids_entries.h"

};


inline bool
guidCompare(const GUID &g1, const GUID &g2)
{
    int ret;
    if (g1.Data1 == g2.Data1) {
        ret = g1.Data2 - g2.Data2;
        if (ret == 0) {
            ret = g1.Data3 - g2.Data3;
            if (ret == 0) {
                ret = memcmp(g1.Data4, g2.Data4, sizeof g1.Data4);
            }
        }
    } else {
        ret = g1.Data1 > g2.Data1 ? 1 : -1;
    }
    if (&g1 == &g2) {
        assert(ret == 0);
    }

    return ret < 0;
}

inline bool
guidEntryCompare(const GuidEntry &e1, const GUID &g2)
{
    const GUID &g1 = e1.guid;
    return guidCompare(g1, g2);
}


const char *
getGuidName(const GUID & guid)
{
    const size_t entrySize = sizeof guidEntries[0];
    const size_t numEntries = sizeof guidEntries / entrySize;

    // Verify GUID entries are sorted
#ifndef NDEBUG
    static bool checked = false;
    if (!checked) {
        for (size_t i = 0; i < numEntries - 1; ++i) {
            assert(guidEntryCompare(guidEntries[i], guidEntries[i + 1].guid));
        }
        assert(guidEntryCompare(guidEntries[0], guidEntries[numEntries - 1].guid));
        checked = true;
    }
#endif

    // Binary search
    const GuidEntry *first = guidEntries;
    const GuidEntry *last = first + numEntries;
    const GuidEntry *entry;
    entry = std::lower_bound(first, last, guid, guidEntryCompare);
    if (entry != last &&
        !guidCompare(guid, entry->guid)) {
        return entry->name;
    }

    // Generic case
    // See https://en.wikipedia.org/wiki/Globally_unique_identifier#Text_encoding
    static char name[] = "uuid(00000000-0000-0000-0000-000000000000)";
    const size_t len = sizeof name - 1;
    int written;
    written = snprintf(name,
                       sizeof name,
                       "uuid(%08" PRIx32 "-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)",
                       (uint32_t)guid.Data1,
                       guid.Data2, guid.Data3,
                       guid.Data4[0], guid.Data4[1],
                       guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    assert(written == len);
    (void)written;
    name[len] = '\0';

    return name;
}

