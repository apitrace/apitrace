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

// https://docs.microsoft.com/en-us/windows-hardware/drivers/gettingstarted/virtual-address-spaces
#ifdef _WIN64
#define NUM_PAGES 0x80000000U // 8TB
#else
#define NUM_PAGES 0xc0000U    // 3GB
#endif

#define NUM_PAGES_32 ((NUM_PAGES + 31U)/32U)

template< typename T >
inline uintptr_t pageFromAddr(T addr)
{
    return uintptr_t(addr) / PAGE_SIZE;
}

volatile uint32_t *g_PageBitMap = nullptr;

static inline void
setPageBit(uintptr_t page) {
    assert(g_PageBitMap);
    assert(page < NUM_PAGES);
#if 0
    g_PageBitMap[page / 32] |= 1U << (page % 32);
#else
    _InterlockedOr(reinterpret_cast<volatile long *>(&g_PageBitMap[page / 32]), 1U << (page % 32));
#endif
}
static inline void
unsetPageBit(uintptr_t page) {
    assert(g_PageBitMap);
    assert(page < NUM_PAGES);
#if 0
    g_PageBitMap[page / 32] &= ~(1U << (page % 32));
#else
    _InterlockedAnd(reinterpret_cast<volatile long *>(&g_PageBitMap[page / 32]), ~(1U << (page % 32)));
#endif
}
static inline bool
getPageBit(uintptr_t page) {
    assert(g_PageBitMap);
    assert(page < NUM_PAGES);
    return g_PageBitMap[page / 32] & (1U << (page % 32));
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

        if (page < NUM_PAGES &&
            getPageBit(page)) {
            DWORD flOldProtect;

            BOOL bRet = VirtualProtect(reinterpret_cast<LPVOID>(page * PAGE_SIZE),
                                       PAGE_SIZE, PAGE_READWRITE, &flOldProtect);
            assert(bRet);
            os::log("memtrace: unprotect 0x%" PRIxPTR ", flOldProtect=0x%lx\n", page * PAGE_SIZE, flOldProtect);

            unsetPageBit(page);
            return EXCEPTION_CONTINUE_EXECUTION;
        } else {
            os::log("VectoredHandler: unknown page at 0x%" PRIxPTR "\n",
                    uintptr_t(pExceptionRecord->ExceptionInformation[1]));
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

    if (!g_PageBitMap) {
        g_PageBitMap = static_cast<uint32_t *>(VirtualAlloc(nullptr, NUM_PAGES_32 * sizeof(uint32_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
        assert(g_PageBitMap != nullptr);
    }

    if (!handler) {
      AddVectoredExceptionHandler(1, VectoredHandler);
      handler = true;
    }

#if 1
    os::log("memtrace: %p-%p\n", _ptr, (uint8_t *)_ptr + _size);
#endif

    startPage = pageFromAddr(_ptr);
    nPages = ((uintptr_t)_ptr + _size + PAGE_SIZE - 1)/PAGE_SIZE - (uintptr_t)_ptr/PAGE_SIZE;
    for (size_t i = 0; i < nPages; ++i) {
        uintptr_t page = startPage + i;
        assert(page < NUM_PAGES);

        // TODO: Write 32bits at a time.
        setPageBit(page);
    }


    LPVOID lpAddress = reinterpret_cast<LPVOID>(startPage * PAGE_SIZE);

#if 1
    MEMORY_BASIC_INFORMATION mbi;

    if (VirtualQuery(lpAddress, &mbi, sizeof mbi) != sizeof mbi) {
        os::log("error: VirtualQuery failed\n");
        return;
    }

#endif
    DWORD flNewProtect = (mbi.Protect & (PAGE_GUARD | PAGE_NOCACHE | PAGE_WRITECOMBINE))
                       | PAGE_READONLY;

#if 1
    os::log("memtrace: protect 0x%" PRIxPTR "-0x%" PRIxPTR ", mbiProtect=0x%lx\n",
            startPage * PAGE_SIZE, (startPage + nPages)*PAGE_SIZE, mbi.Protect);
#endif

    BOOL bRet = VirtualProtect(lpAddress, nPages * PAGE_SIZE, flNewProtect, &flOldProtect);
    if (!bRet) {
        DWORD dwLastError = GetLastError();
        os::log("error: VirtualProtect failed with error 0x%lx, mbiProtect=0x%lx\n",
                dwLastError, mbi.Protect);
        flOldProtect = mbi.Protect;
    }
}


void MemoryShadow::update(Callback callback) const
{
    uintptr_t realStart = ~uintptr_t(0);
    uintptr_t realStop  = 0;

    for (size_t i = 0; i < nPages; ++i) {
        uintptr_t page = startPage + i;

        assert(page < NUM_PAGES);
        if (getPageBit(page)) {
            unsetPageBit(page);
        } else {
            realStart = std::min(realStart, page * PAGE_SIZE);
            realStop  = std::max(realStop,  (page + 1) * PAGE_SIZE);
        }
    }

    os::log("memtrace: unprotect 0x%" PRIxPTR "-0x%" PRIxPTR "\n",
            startPage * PAGE_SIZE, (startPage + nPages)*PAGE_SIZE);

    DWORD flPrevProtect;
    BOOL bRet = VirtualProtect(reinterpret_cast<LPVOID>(startPage * PAGE_SIZE), nPages * PAGE_SIZE, flOldProtect, &flPrevProtect);
    assert(bRet);

    realStart = std::max(realStart, uintptr_t(realPtr));
    realStop  = std::min(realStop,  uintptr_t(realPtr + size));

    // Update the rest
    if (realStart < realStop) {
        callback((const void *)realStart, realStop - realStart);
    }
}


#endif // _WIN32
