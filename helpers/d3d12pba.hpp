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

#pragma once

#include <assert.h>

#include <algorithm>
#include <mutex>

enum _D3D12_MAPPING_TYPE
{
    _D3D12_MAPPING_INVALID,
    _D3D12_MAPPING_WRITE_WATCH, // For normal resources, heaps - the fast path
    _D3D12_MAPPING_EXCEPTION,   // For imported user heaps from pointer/files - the slow path
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
    _D3D12_MAPPING_TYPE MappingType;
    uint32_t RefCount;
 
    struct
    {
        // Dirty pages for exception tracking.
        std::vector<_D3D12_PAGE_RANGE> DirtyPages;
        DWORD OldProtect;
    } ExceptionPath;

    _D3D12_MAP_DESC(_D3D12_MAPPING_TYPE Type, void* pData, SIZE_T Size, DWORD OldProtect = 0) :
        pData(pData),
        Size(Size),
        MappingType(Type),
        RefCount(1),
        ExceptionPath({ {}, OldProtect })
    {}

    _D3D12_MAP_DESC() :
        pData(nullptr),
        Size(0),
        MappingType(_D3D12_MAPPING_INVALID),
        RefCount(1),
        ExceptionPath({ {}, 0 })
    {}
};

extern std::map<SIZE_T, _D3D12_MAP_DESC> g_D3D12AddressMappings;
extern std::mutex g_D3D12AddressMappingsMutex;

static inline bool
_guard_mapped_memory(const _D3D12_MAP_DESC& Desc, DWORD* OldProtect)
{
    BOOL vp_result = VirtualProtect(Desc.pData, Desc.Size, PAGE_READWRITE | PAGE_GUARD, OldProtect);
    assert(vp_result);
    return vp_result;
}

static inline void
_flush_mapping_watch_memcpys(_D3D12_MAP_DESC& mapping)
{
    static std::vector<uintptr_t> s_addresses;

    constexpr size_t PageSize = 4096;
    size_t watch_size = align(mapping.Size, PageSize);
    size_t address_count = watch_size / PageSize;
    s_addresses.resize(address_count);

    DWORD granularity = DWORD(PageSize);
    ULONG_PTR count = ULONG_PTR(address_count);

    // Find out addresses that have changed
    if (GetWriteWatch(WRITE_WATCH_FLAG_RESET, mapping.pData, watch_size, reinterpret_cast<void**>(s_addresses.data()), &count, &granularity) != 0)
    {
#ifndef NDEBUG
        MEMORY_BASIC_INFORMATION info;
        VirtualQuery(mapping.pData, &info, sizeof(info));
#endif
        os::log("apitrace: error: Failed to write watch - %u\n", GetLastError());
        assert(false);
        return;
    }

    for (ULONG_PTR i = 0; i < count; i++)
    {
        uintptr_t base_address = s_addresses[i];

        // Combine contiguous pages into a single memcpy!
        ULONG_PTR contiguous_pages = 1;
        while (i + 1 != count && s_addresses[i + 1] == s_addresses[i] + PageSize)
        {
            contiguous_pages++;
            i++;
        }

        // Clip to this resource's region
        uintptr_t resource_start = reinterpret_cast<uintptr_t>(mapping.pData);
        size_t size = contiguous_pages * PageSize;
        size = std::min<size_t>(base_address + size, resource_start + mapping.Size) - base_address;

        trace::fakeMemcpy(reinterpret_cast<void*>(base_address), size);
    }
}

static inline void
_reset_writewatch(_D3D12_MAP_DESC& mapping)
{
    constexpr size_t PageSize = 4096;
    size_t watch_size = align(mapping.Size, PageSize);

    if (ResetWriteWatch(mapping.pData, watch_size) != 0)
    {
        os::log("apitrace: Failed to reset write watch\n");
        assert(false);
        return;
    }
}

static inline void
_flush_mapping_exception_memcpys(_D3D12_MAP_DESC& mapping)
{
    auto& dirtyPages = mapping.ExceptionPath.DirtyPages;

    if (dirtyPages.empty())
        return;

    std::sort(dirtyPages.begin(), dirtyPages.end(),
        [](_D3D12_PAGE_RANGE a, _D3D12_PAGE_RANGE b) {
            return a.PageStart < b.PageStart;
        });

    size_t index = 0;
    for (size_t i = 1; i < dirtyPages.size(); i++)
    {
        // If this is not first Interval and overlaps  
        // with the previous one  
        if (dirtyPages[index].PageEnd >= dirtyPages[i].PageStart)
        {
            // Merge previous and current Intervals  
            dirtyPages[index].PageEnd   = std::max(dirtyPages[index].PageEnd,   dirtyPages[i].PageEnd);
            dirtyPages[index].PageStart = std::min(dirtyPages[index].PageStart, dirtyPages[i].PageStart);
        }
        else {
            index++;
            dirtyPages[index] = dirtyPages[i];
        }
    }

    for (size_t i = 0; i <= index; i++)
    {
        constexpr DWORD page_size = 4096;

        uintptr_t resource_start = reinterpret_cast<uintptr_t>(mapping.pData);
        uintptr_t page_start     = resource_start + dirtyPages[i].PageStart * page_size;
        void* start = reinterpret_cast<void*>(page_start);
        size_t size = (dirtyPages[i].PageEnd - dirtyPages[i].PageStart) * page_size;
        size = std::min<size_t>(page_start + size, resource_start + mapping.Size) - page_start;

        trace::fakeMemcpy(start, size);
    }
}

static inline D3D12_HEAP_FLAGS
_get_heap_flags(ID3D12Resource* pResource)
{
    D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE;
    if (FAILED(pResource->GetHeapProperties(nullptr, &flags)))
    {
        // This is probably a reserved resource...
        // These cannot be mapped to lets say FLAG_NONE!
        return D3D12_HEAP_FLAG_NONE;
    }

    return flags;
}

static inline void
_map_resource(ID3D12Resource* pResource, void* pData)
{
    D3D12_HEAP_FLAGS flags = _get_heap_flags(pResource);

    if (!(flags & D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH))
        return;

    SIZE_T key = reinterpret_cast<SIZE_T>(pResource);
    // Assert we're page aligned... If we aren't we need to do more work here.
    // TODO(Josh) : Placed resources.
    assert(reinterpret_cast<uintptr_t>(pData) % 4096 == 0);
    auto iter = g_D3D12AddressMappings.find(key);
    if (iter != g_D3D12AddressMappings.end())
        iter->second.RefCount++;
    else
        g_D3D12AddressMappings.try_emplace(key, _D3D12_MAPPING_WRITE_WATCH, pData, _getMapSize(pResource));
}

static inline void
_unmap_resource(ID3D12Resource* pResource)
{
    D3D12_HEAP_FLAGS flags = _get_heap_flags(pResource);

    if (!(flags & D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH))
        return;

    SIZE_T key = static_cast<SIZE_T>(reinterpret_cast<uintptr_t>(pResource));
    auto iter = g_D3D12AddressMappings.find(key);
    if (iter == g_D3D12AddressMappings.end())
        return;

    auto& mapping = iter->second;
    
    if (mapping.MappingType == _D3D12_MAPPING_WRITE_WATCH)
    {
        // WriteWatch mappings.
        _flush_mapping_watch_memcpys(mapping);

        // TODO(Josh): Is this the right thing to do for aliased resources??? :<<<<<<
        // I think this won't work for buffers
        // Not enabling this for now... It'll probably work everywhere
        // But what about the funny edge case with
        // [buffer1       ]
        // [buffer2       XXXXXX]
        // and buffer 1 gets unmapped
        // but XXXXX is in buffer 1's page boundaries but
        // then it's unmapped after that watch will be destroyed
        // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
        // THIS API SUCKS!!!!!!!!!!!!!!!!!!!!!!
        //_reset_writewatch(mapping);
    }
    else if (mapping.MappingType == _D3D12_MAPPING_EXCEPTION)
    {
        // Exception mappings.
        _flush_mapping_exception_memcpys(mapping);

        // No need to re-apply the guard as we are unmapping now.
    }
    else
    {
        os::log("apitrace: Unhandled mapping type\n");
        assert(false);
    }

    if (--iter->second.RefCount == 0)
        g_D3D12AddressMappings.erase(iter);
}

static inline void
_flush_mappings()
{
    for (auto& element : g_D3D12AddressMappings)
    {
        auto& mapping = element.second;

        if (mapping.MappingType == _D3D12_MAPPING_WRITE_WATCH)
        {
            // WriteWatch mappings.
            _flush_mapping_watch_memcpys(mapping);
        }
        else if (mapping.MappingType == _D3D12_MAPPING_EXCEPTION)
        {
            // Exception mappings.
            _flush_mapping_exception_memcpys(mapping);
            mapping.ExceptionPath.DirtyPages.clear();

            DWORD old_protection;
            // Re-apply page guard given we came from ExecuteCommandLists
            bool result = _guard_mapped_memory(mapping, &old_protection);
            os::log("apitrace: Failed to guard memory\n");
            assert(result);
        }
        else
        {
            os::log("apitrace: Unhandled mapping type\n");
            assert(false);
        }
    }

    // Reset writewatches after to deal with aliased resources
    // (and placed resources that may not be aligned to pages?)
    /*for (auto& element : g_D3D12AddressMappings)
    {
        auto& mapping = element.second;

        if (mapping.MappingType == _D3D12_MAPPING_WRITE_WATCH)
        {
            _reset_writewatch(mapping);
        }
    }*/
}

static inline void
_setup_seh()
{
    PVOID OPTHandler = AddVectoredExceptionHandler(1, [](EXCEPTION_POINTERS* pException) -> LONG {
        if (pException->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION)
        {
            bool wasWrite     = static_cast<bool>     (pException->ExceptionRecord->ExceptionInformation[0]);
            uintptr_t address = static_cast<uintptr_t>(pException->ExceptionRecord->ExceptionInformation[1]);

            auto lock = std::unique_lock<std::mutex>(g_D3D12AddressMappingsMutex);

            for (auto& element : g_D3D12AddressMappings) {
                auto& mapping = element.second;
                uintptr_t mapping_base = reinterpret_cast<uintptr_t>(mapping.pData);
                uintptr_t mapping_end = mapping_base + mapping.Size;
                if (address >= mapping_base && address < mapping_end) {
                    constexpr DWORD page_size = 4096;

                    uintptr_t offset = address - mapping_base;
                    DWORD page_start = DWORD(offset / page_size);
                    DWORD page_end = (offset + 64) / page_size != page_start ? page_start + 2 : page_start + 1;

                    mapping.ExceptionPath.DirtyPages.push_back({ page_start, page_end });

                    return EXCEPTION_CONTINUE_EXECUTION;
                }
            }

            return EXCEPTION_CONTINUE_SEARCH;
        }

        return EXCEPTION_CONTINUE_SEARCH;
    });
}
