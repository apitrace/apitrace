/**************************************************************************
 *
 * Copyright 2014-2019 VMware, Inc.
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


#include "memtrace.hpp"

#ifdef _WIN32

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <algorithm>

#include <windows.h>

#include "os.hpp"


#define PAGE_SIZE 4096U

#define NUM_PAGES (uintptr_t(2)*1024*1024*1024 / PAGE_SIZE - 1)
#define NUM_PAGES_32 ((NUM_PAGES + 31)/32)

template< typename T >
inline uintptr_t pageFromAddr(T addr)
{
    return uintptr_t(addr) & ~uintptr_t(PAGE_SIZE - 1);
}

// http://web.archive.org/web/20071223173210/http://www.concentric.net/~Ttwang/tech/inthash.htm
static uint32_t
_pageHash(uint64_t page)
{
    assert((page % PAGE_SIZE) == 0);
    uint64_t key = page /* / PAGE_SIZE */;

    key = (~key) + (key << 18); // key = (key << 18) - key - 1;
    key = key ^ (key >> 31);
    key = key * 21; // key = (key + (key << 2)) + (key << 4);
    key = key ^ (key >> 11);
    key = key + (key << 6);
    key = key ^ (key >> 22);
    return (key % NUM_PAGES);
}

static uintptr_t _pageHashTable[NUM_PAGES];
static uint32_t
addPage(uintptr_t page)
{
    uint32_t hash = _pageHash(page);
    while (true) {
        uintptr_t entry = _pageHashTable[hash];
        if (entry == page) {
            return hash;
        }
        if (entry == 0) {
            _pageHashTable[hash] = page;
            return hash;
        }
        hash = (hash + 1) % NUM_PAGES;
    }
}

static bool
getPage(uintptr_t page, uint32_t &hash)
{
    hash = _pageHash(page);
    while (true) {
        uintptr_t entry = _pageHashTable[hash];
        if (entry == page) {
            return true;
        }
        if (entry == 0) {
            return false;
        }
        hash = (hash + 1) % NUM_PAGES;
    }
}


uint32_t dirtyPages[NUM_PAGES_32];
static inline void
setPageDirty(uint32_t hash) {
    assert(hash < NUM_PAGES);
    dirtyPages[hash / 32] |= 1U << (hash % 32);
}
static inline void
setPageClean(uint32_t hash) {
    assert(hash < NUM_PAGES);
    dirtyPages[hash / 32] &= ~(1U << (hash % 32));
}
static inline bool
isPageDirty(uint32_t hash) {
    assert(hash < NUM_PAGES);
    return dirtyPages[hash / 32] & (1U << (hash % 32));
}


static bool handler = false;

static LONG CALLBACK
VectoredHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
    PEXCEPTION_RECORD pExceptionRecord = pExceptionInfo->ExceptionRecord;
    DWORD ExceptionCode = pExceptionRecord->ExceptionCode;

#if 0
    os::log("VectoredHandler: Code=%lx, nparams=%lu, param0=0x%" PRIxPTR ", param1=0x%" PRIxPTR "\n",
            ExceptionCode,
            pExceptionRecord->NumberParameters,
            pExceptionRecord->ExceptionInformation[0],
            pExceptionRecord->ExceptionInformation[1]);
#endif

    if (ExceptionCode == EXCEPTION_ACCESS_VIOLATION &&
        pExceptionRecord->NumberParameters >= 2 &&
        pExceptionRecord->ExceptionInformation[0] == 1) { // writing
        uintptr_t page = pageFromAddr(pExceptionRecord->ExceptionInformation[1]);
        uint32_t hash ;
        if (getPage(page, hash)) {
            DWORD flOldProtect;
            BOOL bRet = VirtualProtect(reinterpret_cast<LPVOID>(page), PAGE_SIZE, PAGE_READWRITE, &flOldProtect);
            assert(bRet);
            setPageDirty(hash);
            return EXCEPTION_CONTINUE_EXECUTION;
        } else {
            os::log("VectoredHandler: unknown page at 0x%" PRIxPTR "\n",
                    page);
        }
    }

    return EXCEPTION_CONTINUE_SEARCH;
}


void MemoryShadow::zero(void *_ptr, size_t _size)
{
}

void MemoryShadow::cover(void *_ptr, size_t _size, bool _discard)
{
    assert(_ptr);

    size = _size;
    realPtr = (const uint8_t *)_ptr;

    if (!handler) {
      AddVectoredExceptionHandler(1, VectoredHandler);
    }

    startPage = pageFromAddr(_ptr);
    nPages = ((uintptr_t)_ptr + _size + PAGE_SIZE - 1)/PAGE_SIZE - (uintptr_t)_ptr/PAGE_SIZE;
    for (size_t i = 0; i < nPages; ++i) {
        uintptr_t page = startPage + i*PAGE_SIZE;
        addPage(page);
    }

    BOOL bRet = VirtualProtect(reinterpret_cast<LPVOID>(startPage), nPages * PAGE_SIZE, PAGE_READONLY, &flOldProtect);
    assert(bRet);
}


void MemoryShadow::update(Callback callback) const
{
    uintptr_t realStart = ~uintptr_t(0);
    uintptr_t realStop  = 0;

    for (size_t i = 0; i < nPages; ++i) {
        uintptr_t page = startPage + i*PAGE_SIZE;
        uint32_t hash;
        if (getPage(page, hash)) {
            if (isPageDirty(hash)) {
                realStart = std::min(realStart, page);
                realStop  = std::max(realStop,  page + PAGE_SIZE);
                setPageClean(hash);
            }
        } else {
            assert(0);
        }
    }

    DWORD flPrevProtect;
    BOOL bRet = VirtualProtect(reinterpret_cast<LPVOID>(startPage), nPages * PAGE_SIZE, flOldProtect, &flPrevProtect);
    assert(bRet);

    realStart = std::max(realStart, uintptr_t(realPtr));
    realStop  = std::min(realStop,  uintptr_t(realPtr + size));

    // Update the rest
    if (realStart < realStop) {
        callback((const void *)realStart, realStop - realStart);
    }
}


#endif // _WIN32
