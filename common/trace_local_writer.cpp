/**************************************************************************
 *
 * Copyright 2007-2011 VMware, Inc.
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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zlib.h>

#include <algorithm>
#include <list>

#include "os.hpp"
#include "range.hpp"
#include "trace_file.hpp"
#include "trace_writer.hpp"
#include "trace_format.hpp"


namespace Trace {


static void exceptionCallback(void)
{
    localWriter.flush();
}


LocalWriter::LocalWriter() :
    acquired(0)
{
    // Install the signal handlers as early as possible, to prevent
    // interfering with the application's signal handling.
    OS::SetExceptionCallback(exceptionCallback);
}

LocalWriter::~LocalWriter()
{
    OS::ResetExceptionCallback();
}

void
LocalWriter::open(void) {

    static unsigned dwCounter = 0;

    const char *szExtension = "trace";
    char szFileName[PATH_MAX];
    const char *lpFileName;

    lpFileName = getenv("TRACE_FILE");
    if (lpFileName) {
        strncpy(szFileName, lpFileName, PATH_MAX);
    }
    else {
        char szProcessName[PATH_MAX];
        char szCurrentDir[PATH_MAX];
        OS::GetProcessName(szProcessName, PATH_MAX);
        OS::GetCurrentDir(szCurrentDir, PATH_MAX);

        for (;;) {
            FILE *file;

            if (dwCounter)
                snprintf(szFileName, PATH_MAX, "%s%c%s.%u.%s", szCurrentDir, PATH_SEP, szProcessName, dwCounter, szExtension);
            else
                snprintf(szFileName, PATH_MAX, "%s%c%s.%s", szCurrentDir, PATH_SEP, szProcessName, szExtension);

            file = fopen(szFileName, "rb");
            if (file == NULL)
                break;

            fclose(file);

            ++dwCounter;
        }
    }

    OS::DebugMessage("apitrace: tracing to %s\n", szFileName);

    Writer::open(szFileName);

#if 0
    // For debugging the exception handler
    *((int *)0) = 0;
#endif
}

unsigned LocalWriter::beginEnter(const FunctionSig *sig) {
    OS::AcquireMutex();
    ++acquired;

    if (!m_file->isOpened()) {
        open();
    }

    return Writer::beginEnter(sig);
}

void LocalWriter::endEnter(void) {
    Writer::endEnter();
    --acquired;
    OS::ReleaseMutex();
}

void LocalWriter::beginLeave(unsigned call) {
    OS::AcquireMutex();
    ++acquired;
    Writer::beginLeave(call);
}

void LocalWriter::endLeave(void) {
    Writer::endLeave();
    --acquired;
    OS::ReleaseMutex();
}

void LocalWriter::flush(void) {
    /*
     * Do nothing if the mutex is already acquired (e.g., if a segfault happen
     * while writing the file) to prevent dead-lock.
     */

    if (!acquired) {
        OS::AcquireMutex();
        if (m_file->isOpened()) {
            OS::DebugMessage("apitrace: flushing trace due to an exception\n");
            m_file->flush();
        }
        OS::ReleaseMutex();
    }
}

struct RangeInfo : public range::range<size_t>
{
    unsigned long crc;
};

typedef std::list<RangeInfo> RangeInfoList;

struct RegionInfo
{
    size_t size;
    const char *start;
    RangeInfoList ranges;
};

typedef std::list<RegionInfo> RegionInfoList;

static RegionInfoList regionInfoList;


static RegionInfo * lookupRegionInfo(Writer &writer, const void *ptr) {
    OS::MemoryInfo info;

    if (!OS::queryVirtualAddress(ptr, &info)) {
        OS::DebugMessage("apitrace: warning: failed to query virtual address %p\n", ptr);
        return NULL;
    }

    if (0) {
        OS::DebugMessage("apitrace: %p => %p..%p\n", ptr, info.start, info.stop);
    }

    for (RegionInfoList::iterator it = regionInfoList.begin(); it != regionInfoList.end(); ) {
        const char *start = it->start;
        const char *stop  = start + it->size;
        if (info.start < stop && info.stop > start) {
            // Intersect
            if (info.start == start && info.stop == stop) {
                return &*it;
            } else {
                OS::DebugMessage("apitrace: warning: range %p-%p changed to %p-%p\n", start, stop, info.start, info.stop);
                // XXX: Emit a realloc
                it = regionInfoList.erase(it);
            }
        } else {
            ++it;
        }
    }

    RegionInfo regionInfo;
    regionInfo.size = (const char*)info.stop - (const char*)info.start;
    regionInfo.start = (const char *)info.start;

    unsigned __call = writer.beginEnter(&malloc_sig);
    writer.beginArg(0);
    writer.writeUInt(regionInfo.size);
    writer.endArg();
    writer.endEnter();
    writer.beginLeave(__call);
    writer.beginReturn();
    writer.writeOpaque(regionInfo.start);
    writer.endReturn();
    writer.endLeave();

    return &*regionInfoList.insert(regionInfoList.end(), regionInfo);
}


static const char *memcpy_args[3] = {"dest", "src", "n"};
const FunctionSig memcpy_sig = {0, "memcpy", 3, memcpy_args};

static const char *malloc_args[1] = {"size"};
const FunctionSig malloc_sig = {1, "malloc", 1, malloc_args};

static const char *free_args[1] = {"ptr"};
const FunctionSig free_sig = {2, "free", 1, free_args};

static const char *realloc_args[2] = {"ptr", "size"};
const FunctionSig realloc_sig = {3, "realloc", 2, realloc_args};


/**
 * Ensure this region is updated, emitting the necessary memcpy calls.
 */
void LocalWriter::updateRegion(const void *ptr, size_t size) {
    if (!ptr) {
        return;
    }

    OS::AcquireMutex();

    RegionInfo * regionInfo = lookupRegionInfo(*this, ptr);

    if (!regionInfo || !size) {
        OS::ReleaseMutex();
        return;
    }

    if ((char *)ptr + size > regionInfo->start + regionInfo->size) {
        OS::DebugMessage("apitrace: warning: range %p-%p exceeds region %p-%p\n", 
                         ptr, (char *)ptr + size,
                         regionInfo->start, regionInfo->start + regionInfo->size);

    }

    size_t start = (const char *)ptr - regionInfo->start;

#if 0

    // Simply emit one memcpy for the whole range
    unsigned __call = Writer::beginEnter(&memcpy_sig);
    Writer::beginArg(0);
    Writer::writeOpaque(ptr);
    Writer::endArg();
    Writer::beginArg(1);
    Writer::writeBlob(ptr, size);
    Writer::endArg();
    Writer::beginArg(2);
    Writer::writeUInt(size);
    Writer::endArg();
    Writer::endEnter();
    Writer::beginLeave(__call);
    Writer::endLeave();

#else

    size_t stop = start + size;

    // The range for what needs to be updated
    range::range<size_t> update(start, stop);

    // The set of ranges which need to be copied, i.e., the update range minus
    // everything that did not change since last time.
    range::range_set<size_t> copy(update);

    if (0) {
        OS::DebugMessage("  %p..%p\n", ptr, (const char *)ptr + size);
    }

    // Go through the all ranges that intersect the update range, checking for redundancy,
    for (RangeInfoList::iterator it = regionInfo->ranges.begin(); it != regionInfo->ranges.end(); ) {
        if (it->intersects(update)) {
            const Bytef *p = (const Bytef *)regionInfo->start + it->start;
            size_t length = it->stop - it->start;

            unsigned long crc = crc32(0L, Z_NULL, 0);
            crc = crc32(crc, p, length);

            if (0) {
                OS::DebugMessage("    %p+0x%04lx: %08lx %s %08lx\n",
                    p, (unsigned long)length,
                    it->crc, it->crc == crc ? "==" : "->", crc);
            }

            if (it->crc == crc) {
                // This range did not change, so no need to copy it.
                copy.sub(*it);
                ++it;
            } else {
                // This range did change, so remove all info about it.
                it = regionInfo->ranges.erase(it);
            }
        } else {
            ++it;
        }
    }

    // Emit fake memcpy calls for each range that needs to be copied
    for (range::range_set<size_t>::const_iterator it = copy.begin(); it != copy.end(); ++it) {
        const Bytef *p = (const Bytef *)regionInfo->start + it->start;
        size_t length = it->stop - it->start;

        unsigned __call = Writer::beginEnter(&memcpy_sig);
        Writer::beginArg(0);
        Writer::writeOpaque(p);
        Writer::endArg();
        Writer::beginArg(1);
        Writer::writeBlob(p, length);
        Writer::endArg();
        Writer::beginArg(2);
        Writer::writeUInt(length);
        Writer::endArg();
        Writer::endEnter();
        Writer::beginLeave(__call);
        Writer::endLeave();

        // Note down this range's CRC for future redundancy checks.
        RangeInfo r;
        r.start = it->start;
        r.stop = it->stop;
        r.crc = crc32(0L, Z_NULL, 0);
        r.crc = crc32(r.crc, p, length);
        regionInfo->ranges.push_front(r);
    }

#endif

    OS::ReleaseMutex();
}


LocalWriter localWriter;


} /* namespace Trace */

