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

#include <d3d12.h>
#include <map>
#include <stdexcept>

class D3D12_HEAP_ALLOCATOR
{
private:
    typedef struct _HEAP_INFO
    {
        void* Address;
        SIZE_T Bytes;
    } HEAP_INFO;

public:
    D3D12_HEAP_ALLOCATOR()
    {}

    inline void* Register(const void* From, SIZE_T Bytes)
    {
        void* To = VirtualAlloc(nullptr, Bytes, MEM_COMMIT, PAGE_READWRITE);
        if (To == nullptr)
        {
            throw std::runtime_error("Unable to allocate desired memory");
        }

        m_Heaps.emplace(From, HEAP_INFO{
            To,
            Bytes,
        });

        return To;
    }

    inline void Unregister(void* From)
    {
        // TODO(damcclos):
        throw std::runtime_error("Not implemented");
    }

    inline void* Resolve(D3D12_GPU_VIRTUAL_ADDRESS Address)
    {
        // TODO(damcclos):
        throw std::runtime_error("Not implemented");
    }

private:
    std::map<const void*, HEAP_INFO> m_Heaps;
};

class D3D12_ADDRESS_RESOLVER
{
private:
    typedef struct _MEMORY_INFO
    {
        D3D12_GPU_VIRTUAL_ADDRESS Base;
        D3D12_GPU_VIRTUAL_ADDRESS End;

        D3D12_RESOURCE_ALLOCATION_INFO AllocationInfo;
    } MEMORY_INFO;

    typedef struct _ADDRESS_INFO
    {
        MEMORY_INFO From;
        MEMORY_INFO To;
    } ADDRESS_INFO;

public:
    D3D12_ADDRESS_RESOLVER()
    {}

    inline void Register(D3D12_GPU_VIRTUAL_ADDRESS From, D3D12_RESOURCE_ALLOCATION_INFO FromAllocationInfo, D3D12_GPU_VIRTUAL_ADDRESS To, D3D12_RESOURCE_ALLOCATION_INFO ToAllocationInfo)
    {
        m_Addresses.emplace(From, ADDRESS_INFO{
            MEMORY_INFO{
                From,
                From + FromAllocationInfo.SizeInBytes,
                FromAllocationInfo
            },
            MEMORY_INFO{
                To,
                To + ToAllocationInfo.SizeInBytes,
                ToAllocationInfo
            },
        });
    }

    inline void Unregister(D3D12_GPU_VIRTUAL_ADDRESS From)
    {
        m_Addresses.erase(From);
    }

    inline D3D12_GPU_VIRTUAL_ADDRESS Resolve(D3D12_GPU_VIRTUAL_ADDRESS Address)
    {
        if (Address != 0) {
            // upper_bound returns (desired element) + 1
            auto AddressIterator = (--m_Addresses.upper_bound(Address));
            if (AddressIterator == m_Addresses.end())
            {
                throw std::runtime_error("Unable to find the desired address");
            }

            // Verify the handle is within the found heap
            auto& AddressMap = AddressIterator->second;
            if (!(Address >= AddressMap.From.Base && Address < AddressMap.From.End))
            {
                throw std::runtime_error("Unable to find the desired address");
            }

            return AddressMap.To.Base + (Address - AddressMap.From.Base);
        }

        return Address;
    }

private:
    std::map<D3D12_GPU_VIRTUAL_ADDRESS, ADDRESS_INFO> m_Addresses;
};

class D3D12_DESCRIPTOR_RESOLVER
{
private:
    typedef struct _DESCRIPTOR_INFO
    {
        SIZE_T Base;
        SIZE_T End;

        UINT IncrementSize;
    } DESCRIPTOR_INFO;

    typedef struct _DESCRIPTOR_HEAP_INFO
    {
        DESCRIPTOR_INFO From;
        DESCRIPTOR_INFO To;

        UINT Count;
    } DESCRIPTOR_HEAP_INFO;

public:
    D3D12_DESCRIPTOR_RESOLVER()
    {}

    inline void Register(D3D12_CPU_DESCRIPTOR_HANDLE From, UINT FromIncrementSize, D3D12_CPU_DESCRIPTOR_HANDLE To, UINT ToIncrementSize, UINT Count)
    {
        Register(m_CPUHeaps, From.ptr, FromIncrementSize, To.ptr, ToIncrementSize, Count);
    }

    inline void Register(D3D12_GPU_DESCRIPTOR_HANDLE From, UINT FromIncrementSize, D3D12_GPU_DESCRIPTOR_HANDLE To, UINT ToIncrementSize, UINT Count)
    {
        Register(m_GPUHeaps, From.ptr, FromIncrementSize, To.ptr, ToIncrementSize, Count);
    }

    inline void Unregister(D3D12_CPU_DESCRIPTOR_HANDLE From)
    {
        Unregister(m_CPUHeaps, From.ptr);
    }

    inline void Unregister(D3D12_GPU_DESCRIPTOR_HANDLE From)
    {
        Unregister(m_GPUHeaps, From.ptr);
    }

    inline D3D12_CPU_DESCRIPTOR_HANDLE Resolve(D3D12_CPU_DESCRIPTOR_HANDLE Handle)
    {
        return D3D12_CPU_DESCRIPTOR_HANDLE{
            Resolve(m_CPUHeaps, Handle.ptr)
        };
    }

    inline D3D12_GPU_DESCRIPTOR_HANDLE Resolve(D3D12_GPU_DESCRIPTOR_HANDLE Handle)
    {
        return D3D12_GPU_DESCRIPTOR_HANDLE{
            Resolve(m_GPUHeaps, Handle.ptr)
        };
    }

private:
    inline void Register(std::map<SIZE_T, DESCRIPTOR_HEAP_INFO>& Heaps, SIZE_T From, UINT FromIncrementSize, SIZE_T To, UINT ToIncrementSize, UINT Count)
    {
        Heaps.emplace(From, DESCRIPTOR_HEAP_INFO{
            DESCRIPTOR_INFO{
                From,
                From + FromIncrementSize * Count,
                FromIncrementSize
            },
            DESCRIPTOR_INFO{
                To,
                To + ToIncrementSize * Count,
                ToIncrementSize
            },
            Count
        });
    }

    inline void Unregister(std::map<SIZE_T, DESCRIPTOR_HEAP_INFO>& Heaps, SIZE_T From)
    {
        Heaps.erase(From);
    }

    inline SIZE_T Resolve(const std::map<SIZE_T, DESCRIPTOR_HEAP_INFO>& Heaps, SIZE_T Handle)
    {
        if (Handle == 0)
            return Handle;

        // upper_bound returns (desired element) + 1
        auto HeapIterator = (--Heaps.upper_bound(Handle));
        if (HeapIterator == Heaps.end())
        {
            throw std::runtime_error("Unable to find the desired handle");
        }

        // Verify the handle is within the found heap
        auto& Heap = HeapIterator->second;
        if (!(Handle >= Heap.From.Base && Handle < Heap.From.End))
        {
            throw std::runtime_error("Unable to find the desired handle");
        }

        return Heap.To.Base + ((Handle - Heap.From.Base) / Heap.From.IncrementSize) * Heap.To.IncrementSize;
    }

    std::map<SIZE_T, DESCRIPTOR_HEAP_INFO> m_CPUHeaps;
    std::map<SIZE_T, DESCRIPTOR_HEAP_INFO> m_GPUHeaps;
};
