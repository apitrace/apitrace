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

#include <array>
#include <atomic>

template <typename T>
class _D3D12_ADDRESS_SLAB_RESOLVER
{
public:
    static constexpr size_t SlabCount = 64 * 1024;

    _D3D12_ADDRESS_SLAB_RESOLVER()
        : m_slabs()
    {}

    void RegisterSlab(const T& base, const T& value)
    {
        m_slabs[GetIdx(base)] = value;
    }

    T LookupSlab(const T& value)
    {
        if (value == 0)
            return 0;

        return m_slabs[GetIdx(value)] + (value & 0xFFFFFFFFull);
    }

    inline size_t GetIdx(const T& value) { return (value >> 32ull) - 1ull; }

private:
    std::array<T, SlabCount> m_slabs;
};

template <typename T>
class _D3D12_ADDRESS_SLAB_TRACER
{
public:
    _D3D12_ADDRESS_SLAB_TRACER()
        : m_resolver()
        , m_count({ 0 })
    {}

    T RegisterSlab(const T& value)
    {
        const uint32_t idx = ++m_count;
        
        const T ptr = static_cast<T>(idx) << 32ull;
        m_resolver.RegisterSlab(ptr, value);
        return ptr;
    }

    T LookupSlab(const T& ptr)
    {
        return m_resolver.LookupSlab(ptr);
    }
private:
    _D3D12_ADDRESS_SLAB_RESOLVER<T> m_resolver;
    std::atomic<uint32_t> m_count;
};

constexpr UINT _DescriptorIncrementSize = 64;

struct _D3D12_DESCRIPTOR_INFO
{
    D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle;

    UINT                        IncrementSize;
};

class _D3D12_DESCRIPTOR_RESOLVER
{
public:
    static constexpr size_t SlabCount = 64 * 1024;

    _D3D12_DESCRIPTOR_RESOLVER()
        : m_slabs()
    {}

    inline void RegisterSlab(UINT64 Base, _D3D12_DESCRIPTOR_INFO Info)
    {
        m_slabs[GetIdx(Base)] = Info;
    }

    inline void RegisterSlabPartialCPU(UINT64 Base, D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle)
    {
        m_slabs[GetIdx(Base)].CPUHandle = CPUHandle;
    }

    inline void RegisterSlabPartialGPU(UINT64 Base, D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle)
    {
        m_slabs[GetIdx(Base)].GPUHandle = GPUHandle;
    }

    inline void RegisterSlabPartialIncrement(UINT64 Base, UINT IncrementSize)
    {
        m_slabs[GetIdx(Base)].IncrementSize = IncrementSize;
    }

    inline D3D12_CPU_DESCRIPTOR_HANDLE LookupCPUDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE Handle)
    {
        if (Handle.ptr == 0)
            return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };

        _D3D12_DESCRIPTOR_INFO info = m_slabs[GetIdx(Handle.ptr)];
        UINT offset = static_cast<UINT>(Handle.ptr & 0xFFFFFFFFull);
        offset = (offset / _DescriptorIncrementSize) * info.IncrementSize;
        assert(info.CPUHandle.ptr);
        return D3D12_CPU_DESCRIPTOR_HANDLE { info.CPUHandle.ptr + offset };
    }

    inline D3D12_GPU_DESCRIPTOR_HANDLE LookupGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE Handle)
    {
        if (Handle.ptr == 0)
            return D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };

        _D3D12_DESCRIPTOR_INFO info = m_slabs[GetIdx(Handle.ptr)];
        UINT offset = static_cast<UINT>(Handle.ptr & 0xFFFFFFFFull);
        offset = (offset / _DescriptorIncrementSize) * info.IncrementSize;
        assert(info.GPUHandle.ptr);
        return D3D12_GPU_DESCRIPTOR_HANDLE{ info.GPUHandle.ptr + offset };
    }

    inline size_t GetIdx(UINT64 value) { return (value >> 32ull) - 1ull; }

private:
    std::array<_D3D12_DESCRIPTOR_INFO, SlabCount> m_slabs;
};

class _D3D12_DESCRIPTOR_TRACER
{
public:
    _D3D12_DESCRIPTOR_TRACER()
        : m_resolver()
        , m_count({ 0 })
    {}

    inline UINT64 RegisterSlab(D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle, D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle, UINT IncrementSize)
    {
        const _D3D12_DESCRIPTOR_INFO info = { CPUHandle, GPUHandle, IncrementSize };

        const uint32_t idx = ++m_count;
        const UINT64 ptr = static_cast<UINT64>(idx) << 32ull;

        m_resolver.RegisterSlab(ptr, info);
        return ptr;
    }

    inline D3D12_CPU_DESCRIPTOR_HANDLE LookupCPUDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE Handle)
    {
        return m_resolver.LookupCPUDescriptorHandle(Handle);
    }

    inline D3D12_GPU_DESCRIPTOR_HANDLE LookupGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE Handle)
    {
        return m_resolver.LookupGPUDescriptorHandle(Handle);
    }

private:
    _D3D12_DESCRIPTOR_RESOLVER m_resolver;
    std::atomic<uint32_t> m_count;
};
