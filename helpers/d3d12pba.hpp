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
        ExceptionPath({ {}, OldProtect })
    {}

    _D3D12_MAP_DESC() :
        pData(nullptr),
        Size(0),
        MappingType(_D3D12_MAPPING_INVALID),
        ExceptionPath({ {}, 0 })
    {}
};

extern std::map<SIZE_T, std::map<UINT, _D3D12_MAP_DESC>> g_D3D12AddressMappings;
extern std::recursive_mutex g_D3D12AddressMappingsMutex;

static inline bool
_guard_mapped_memory(const _D3D12_MAP_DESC& Desc, DWORD* OldProtect)
{
    BOOL vp_result = VirtualProtect(Desc.pData, Desc.Size, PAGE_READWRITE | PAGE_GUARD, OldProtect);
    assert(vp_result);
    return vp_result;
}

constexpr size_t PageSize = 4096;

static inline uintptr_t
get_offset_from_page(void* address) {
    return reinterpret_cast<uintptr_t>(address) & (PageSize - 1);
}

static inline void*
align_to_page(void* address) {
    return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(address) - get_offset_from_page(address));
}


static inline void
_flush_mapping_watch_memcpys(_D3D12_MAP_DESC& mapping)
{
    static std::vector<void*> s_addresses;

    if (!mapping.pData)
        return;

    size_t page_offset     = get_offset_from_page(mapping.pData);
    void*  aligned_address = align_to_page(mapping.pData);
    SIZE_T watch_size      = align(mapping.Size + page_offset, PageSize);
    ULONG_PTR count        = ULONG_PTR(watch_size / PageSize);
    DWORD  granularity     = 0;

    s_addresses.resize(size_t(count));

    // Find out addresses that have changed
    if (GetWriteWatch(WRITE_WATCH_FLAG_RESET, aligned_address, watch_size, s_addresses.data(), &count, &granularity) != 0)
    {
#ifndef NDEBUG
        MEMORY_BASIC_INFORMATION info;
        VirtualQuery(mapping.pData, &info, sizeof(info));
#endif
        os::log("apitrace: error: Failed to write watch - %u\n", GetLastError());
        assert(false);
        return;
    }

    uintptr_t resource_start = reinterpret_cast<uintptr_t>(mapping.pData);
    for (ULONG_PTR i = 0; i < count; i++)
    {
        uintptr_t base_address = reinterpret_cast<uintptr_t>(s_addresses[i]);

        // Combine contiguous pages into a single memcpy!
#if 1
        SIZE_T contiguous_pages = 1;
        while (i + 1 != count && reinterpret_cast<uintptr_t>(s_addresses[i + 1]) == reinterpret_cast<uintptr_t>(s_addresses[i]) + PageSize)
        {
            contiguous_pages++;
            i++;
        }

        uintptr_t start_copy_range = std::max(base_address,                               resource_start);
        uintptr_t end_copy_range   = std::min(base_address + PageSize * contiguous_pages, resource_start + mapping.Size);

        if (end_copy_range <= start_copy_range) {
            os::log("end_copy_range <= start_copy_range: %llu <= %llu\n", end_copy_range, start_copy_range);
            continue;
        }

        size_t size = size_t(end_copy_range - start_copy_range);
        assert(size <= mapping.Size);
        trace::fakeMemcpy(reinterpret_cast<void*>(start_copy_range), size);
#else
        uintptr_t start_copy_range = std::max(base_address,                               resource_start);
        uintptr_t end_copy_range   = std::min(base_address + PageSize * contiguous_pages, resource_start + mapping.Size);

        if (end_copy_range <= start_copy_range) {
            os::log("end_copy_range <= start_copy_range: %llu <= %llu\n", end_copy_range, start_copy_range);
            continue;
        }

        size_t size = size_t(end_copy_range - start_copy_range);
        trace::fakeMemcpy(start_copy_range, size);
#endif
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
_map_subresource(ID3D12Resource* pResource, UINT Subresource, void* pData)
{
    D3D12_HEAP_FLAGS flags = _get_heap_flags(pResource);

    if (!(flags & D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH))
        return;

    SIZE_T key = reinterpret_cast<SIZE_T>(pResource);
    // Assert we're page aligned... If we aren't we need to do more work here.
    // TODO(Josh) : Placed resources.
    assert(reinterpret_cast<uintptr_t>(pData) % 4096 == 0);

    // Ensure we have a resource mapping.
    auto resource_iter = g_D3D12AddressMappings.find(key);
    if (resource_iter == g_D3D12AddressMappings.end())
        g_D3D12AddressMappings.try_emplace(key);
    resource_iter = g_D3D12AddressMappings.find(key);

    auto subresource_iter = resource_iter->second.find(Subresource);
    if (subresource_iter == resource_iter->second.end())
        resource_iter->second[Subresource] = _D3D12_MAP_DESC(_D3D12_MAPPING_WRITE_WATCH, pData, _getMapSize(pResource, Subresource));
}

static inline size_t _d3d12_AllocationSize(const void *pAddress)
{
    MEMORY_BASIC_INFORMATION info;

    /* Query the base pointer */
    if (!VirtualQuery(pAddress, &info, sizeof(info)))
        return 0;

    /* Allocation base must equal address. */
    if (info.AllocationBase != pAddress)
        return 0;
    if (info.BaseAddress != info.AllocationBase)
        return 0;

    /* All pages must be committed. */
    if (info.State != MEM_COMMIT)
        return 0;

    const size_t allocation_size = info.RegionSize;

    /* All pages must have same protections, so there cannot be multiple regions for VirtualQuery. */
    if (VirtualQuery((uint8_t *)pAddress + allocation_size, &info, sizeof(info)) &&
            info.AllocationBase == pAddress)
        return 0;

    return allocation_size;
}


static inline void
_map_heap(ID3D12Heap* pResource, void* pData)
{
    SIZE_T allocation_size = _d3d12_AllocationSize(pData);
    if (!allocation_size)
        return;

    SIZE_T key = reinterpret_cast<SIZE_T>(pResource);
    // Assert we're page aligned... If we aren't we need to do more work here.
    // TODO(Josh) : Placed resources.
    assert(reinterpret_cast<uintptr_t>(pData) % 4096 == 0);

    // Ensure we have a resource mapping.
    auto resource_iter = g_D3D12AddressMappings.find(key);
    if (resource_iter == g_D3D12AddressMappings.end())
        g_D3D12AddressMappings[key] = std::map<UINT, _D3D12_MAP_DESC>();
    resource_iter = g_D3D12AddressMappings.find(key);

    auto subresource_iter = resource_iter->second.find(0);
    if (subresource_iter == resource_iter->second.end())
        resource_iter->second[0] = _D3D12_MAP_DESC(_D3D12_MAPPING_WRITE_WATCH, pData, allocation_size);
}

static inline void
_unmap_subresource(SIZE_T key, UINT subresource)
{
    auto resource = g_D3D12AddressMappings.find(key);
    if (resource == g_D3D12AddressMappings.end())
        return;

    auto iter = resource->second.find(key);
    if (iter == resource->second.end())
        return;

    // If this is going to release the mapping, then flush memcpys here.
    auto& mapping = iter->second;

    assert(_getMapSize((ID3D12Resource*)key, subresource) == mapping.Size);

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

    resource->second.erase(iter);
}

static inline void
_unmap_subresource(ID3D12Resource* pResource, UINT Subresource)
{
    D3D12_HEAP_FLAGS flags = _get_heap_flags(pResource);

    if (!(flags & D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH))
        return;

    _unmap_subresource(static_cast<SIZE_T>(reinterpret_cast<uintptr_t>(pResource)), Subresource);
}

static inline void
_fully_unmap_resource(ID3D12Resource* pResource)
{
    D3D12_HEAP_FLAGS flags = _get_heap_flags(pResource);

    if (!(flags & D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH))
        return;

    SIZE_T key = static_cast<SIZE_T>(reinterpret_cast<uintptr_t>(pResource));

    auto resource = g_D3D12AddressMappings.find(key);
    if (resource == g_D3D12AddressMappings.end())
        return;

    std::vector<UINT> subresources_to_unmap;
    for (auto& subresource : resource->second)
        subresources_to_unmap.push_back(subresource.first);
    
    for (UINT subresource : subresources_to_unmap)
        _unmap_subresource(key, subresource);

    g_D3D12AddressMappings.erase(resource);
}

static inline void
_fully_unmap_resource(ID3D12Heap* pResource)
{
    _unmap_subresource(static_cast<SIZE_T>(reinterpret_cast<uintptr_t>(pResource)), 0);
}

static inline void
_flush_mappings()
{
    for (auto& resource : g_D3D12AddressMappings)
    {
#ifndef NDEBUG
        ID3D12Resource* resource_ptr = reinterpret_cast<ID3D12Resource*>(resource.first);
        D3D12_RESOURCE_DESC desc = resource_ptr->GetDesc();
        D3D12_HEAP_PROPERTIES heap_properties;
        D3D12_HEAP_FLAGS heap_flags;
        HRESULT hr = resource_ptr->GetHeapProperties(&heap_properties, &heap_flags);
#endif

        for (auto& element : resource.second)
        {
            auto& mapping = element.second;

            assert(_getMapSize((ID3D12Resource*)resource.first, resource.first) == mapping.Size);

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

            auto lock = std::unique_lock<std::recursive_mutex>(g_D3D12AddressMappingsMutex);

            for (auto& resource : g_D3D12AddressMappings) {
                for (auto& element : resource.second)
                {
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
            }

            return EXCEPTION_CONTINUE_SEARCH;
        }

        return EXCEPTION_CONTINUE_SEARCH;
    });
}
