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

template<typename T, typename J>
T _bitcast(const J& src) {
    T dst;
    std::memcpy(&dst, &src, sizeof(T));
    return dst;
}

template <typename T, typename I>
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
        if (_bitcast<I>(value) == 0)
            return { 0 };

        return { _bitcast<I>(m_slabs[GetIdx(value)]) + (_bitcast<I>(value) & 0xFFFFFFFFull) };
    }

    inline size_t GetIdx(const T& value) { return (_bitcast<I>(value) >> 32ull) - 1ull; }

private:
    std::array<T, SlabCount> m_slabs;
};

template <typename T, typename I>
class _D3D12_ADDRESS_SLAB_TRACER
{
public:
    _D3D12_ADDRESS_SLAB_TRACER()
        : m_resolver()
        , m_count({ 0 })
    {}

    T RegisterSlab(const T& value)
    {
        uint32_t idx = ++m_count;
        
        auto ptr = static_cast<I>(idx) << 32ull;
        m_resolver.RegisterSlab(T{ ptr }, value);

        return T{ ptr };
    }

    T LookupSlab(const T& ptr)
    {
        return m_resolver.LookupSlab(ptr);
    }
private:
    _D3D12_ADDRESS_SLAB_RESOLVER<T, I> m_resolver;
    std::atomic<uint32_t> m_count;
};
