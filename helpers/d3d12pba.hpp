/**************************************************************************
 *
 * Copyright 2020 Joshua Ashton for Valve Software
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
 * AUTHORS,
 * AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **************************************************************************/


/*
 * Auxiliary functions to compute the size of array/blob arguments.
 */

#pragma once

#include <assert.h>

#include <algorithm>

/**
 * Information about a faked VA
 */
struct _HEAP_VA_DESC
{
    void*  pAllocatedVABase;
    SIZE_T FakeVABase;

    _HEAP_VA_DESC() :
        pAllocatedVABase(nullptr),
        FakeVABase(0)
    {}
};

struct _D3D12_PAGE_RANGE
{
    DWORD PageStart;
    DWORD PageEnd;
};

struct _D3D12_MAP_DESC
{
    void* pData;
    SIZE_T Size;
    std::vector<_D3D12_PAGE_RANGE> DirtyPages;
    DWORD OldProtect;

    _D3D12_MAP_DESC() :
        pData(nullptr),
        OldProtect(0),
        Size(Size)
    {}

};

template<typename T, typename U = T>
constexpr T align(T what, U to) {
    return (what + to - 1) & ~(to - 1);
}

static inline _HEAP_VA_DESC
_allocateFakeVA(const SIZE_T FakeSize, const SIZE_T FakeAlignment)
{
    _HEAP_VA_DESC Desc;

    Desc.pAllocatedVABase = VirtualAlloc(nullptr, FakeSize << 1ull, MEM_RESERVE, PAGE_NOACCESS);
    SIZE_T FakeVARegion = static_cast<SIZE_T>(reinterpret_cast<uintptr_t>(Desc.pAllocatedVABase));
    Desc.FakeVABase = align(FakeVARegion, FakeAlignment);

    return Desc;
}

extern std::map<SIZE_T, _D3D12_MAP_DESC> g_D3D12AddressMappings;

static inline bool
_guard_mapped_memory(const _D3D12_MAP_DESC& Desc, DWORD* OldProtect)
{
    BOOL vp_result = VirtualProtect(Desc.pData, Desc.Size, PAGE_READWRITE | PAGE_GUARD, OldProtect);
    assert(vp_result);
    return vp_result;
}

static inline void
_flush_mapping_memcpys(_D3D12_MAP_DESC& mapping)
{
    if (mapping.DirtyPages.empty())
        return;

    std::sort(mapping.DirtyPages.begin(), mapping.DirtyPages.end(),
        [](_D3D12_PAGE_RANGE a, _D3D12_PAGE_RANGE b) {
            return a.PageStart < b.PageStart;
        });

    size_t index = 0;
    for (size_t i = 1; i < mapping.DirtyPages.size(); i++)
    {
        // If this is not first Interval and overlaps  
        // with the previous one  
        if (mapping.DirtyPages[index].PageEnd >= mapping.DirtyPages[i].PageStart)
        {
            // Merge previous and current Intervals  
            mapping.DirtyPages[index].PageEnd   = std::max(mapping.DirtyPages[index].PageEnd,   mapping.DirtyPages[i].PageEnd);
            mapping.DirtyPages[index].PageStart = std::min(mapping.DirtyPages[index].PageStart, mapping.DirtyPages[i].PageStart);
        }
        else {
            index++;
            mapping.DirtyPages[index] = mapping.DirtyPages[i];
        }
    }

    for (size_t i = 0; i <= index; i++)
    {
        constexpr DWORD page_size = 4096;

        uintptr_t resource_start = reinterpret_cast<uintptr_t>(mapping.pData);
        uintptr_t page_start     = resource_start + mapping.DirtyPages[i].PageStart * page_size;
        void* start = reinterpret_cast<void*>(page_start);
        size_t size = (mapping.DirtyPages[i].PageEnd - mapping.DirtyPages[i].PageStart) * page_size;
        size = std::min(page_start + size, resource_start + mapping.Size) - page_start;

        trace::fakeMemcpy(start, size);
    }
}

static inline void
_unmap_resource(ID3D12Resource* pResource)
{
    SIZE_T key = static_cast<SIZE_T>(reinterpret_cast<uintptr_t>(pResource));
    auto& mapping = g_D3D12AddressMappings[key];
    
    _flush_mapping_memcpys(mapping);

    g_D3D12AddressMappings.erase(key);

    // No need to re-apply page guard given we are now unmapping...
}

static inline void
_flush_mappings()
{
    for (auto& mapping : g_D3D12AddressMappings)
    {
        _flush_mapping_memcpys(mapping.second);
        mapping.second.DirtyPages.clear();

        [[maybe_unused]] DWORD old_protection;
        // Re-apply page guard given we came from ExecuteCommandLists
        bool result = _guard_mapped_memory(mapping.second, &old_protection);
        assert(result);
    }
}

static inline void
_setup_seh()
{
    PVOID OPTHandler = AddVectoredExceptionHandler(1, [](EXCEPTION_POINTERS* pException) -> LONG {
        static thread_local bool      s_LastWasWrite;
        static thread_local uintptr_t s_LastAddress;

        if (pException->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION)
        {
            s_LastWasWrite = static_cast<bool>     (pException->ExceptionRecord->ExceptionInformation[0]);
            s_LastAddress  = static_cast<uintptr_t>(pException->ExceptionRecord->ExceptionInformation[1]);

            // Set trap flag to raise EXCEPTION_SINGLE_STEP (run exactly ONE instruction, this one!)
            pException->ContextRecord->EFlags |= 0x100;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
        else if (pException->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP)
        {
            for (auto& mapping : g_D3D12AddressMappings) {
                uintptr_t mapping_base = reinterpret_cast<uintptr_t>(mapping.second.pData);
                uintptr_t mapping_end  = mapping_base + mapping.second.Size;
                if (s_LastAddress >= mapping_base && s_LastAddress < mapping_end) {
                    constexpr DWORD page_size = 4096;

                    uintptr_t offset = s_LastAddress - mapping_base;
                    DWORD page_start = DWORD(offset / page_size);
                    DWORD page_end   = (offset + 64) / page_size != page_start ? page_start + 2 : page_start + 1;

                    mapping.second.DirtyPages.push_back({ page_start, page_end });

                    return EXCEPTION_CONTINUE_EXECUTION;
                }
            }
        }

        return EXCEPTION_CONTINUE_SEARCH;
    });
}
