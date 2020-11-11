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
#include <atomic>

#include "com_ptr.hpp"

class _D3D12FakePipelineLibrary : public ID3D12PipelineLibrary1
{
public:
    _D3D12FakePipelineLibrary(ID3D12Device* pDevice)
    {
        HRESULT hr = pDevice->QueryInterface(__uuidof(ID3D12Device2), reinterpret_cast<void**>(&m_pDevice));
        assert(hr == S_OK);
    }

    virtual ~_D3D12FakePipelineLibrary()
    {

    }

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(
        REFIID riid,
        void **ppvObject)
    {
        if (!ppvObject)
            return E_POINTER;

        if (riid == IID_ID3D12PipelineLibrary1 ||
            riid == IID_ID3D12PipelineLibrary ||
            riid == IID_ID3D12DeviceChild ||
            riid == IID_ID3D12Object ||
            riid == IID_IUnknown)
        {
            *ppvObject = reinterpret_cast<void*>(this);
            return S_OK;
        }

        return E_NOINTERFACE;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return ++m_refcount;
    }

    virtual ULONG STDMETHODCALLTYPE Release(void)
    {
        ULONG refcount = --m_refcount;
        if (refcount == 0)
            delete this;
        return refcount;
    }

    virtual HRESULT STDMETHODCALLTYPE GetPrivateData( 
        REFGUID guid,
        UINT *pDataSize,
        void *pData)
    {
        return S_OK;
    }
        
    virtual HRESULT STDMETHODCALLTYPE SetPrivateData( 
        REFGUID guid,
        UINT DataSize,
        const void *pData)
    {
        return S_OK;
    }
        
    virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface( 
        REFGUID guid,
        const IUnknown *pData)
    {
        return S_OK;
    }
        
    virtual HRESULT STDMETHODCALLTYPE SetName(
        LPCWSTR Name)
    {
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE GetDevice(
        REFIID riid,
        void **ppvDevice)
    {
        return m_pDevice->QueryInterface(riid, ppvDevice);
    }

    virtual HRESULT STDMETHODCALLTYPE StorePipeline(
        LPCWSTR pName,
        ID3D12PipelineState *pPipeline)
    {
        return S_OK;
    }
        
    virtual HRESULT STDMETHODCALLTYPE LoadGraphicsPipeline(
        LPCWSTR pName,
        const D3D12_GRAPHICS_PIPELINE_STATE_DESC *pDesc,
        REFIID riid,
        void **ppPipelineState)
    {
        return m_pDevice->CreateGraphicsPipelineState(pDesc, riid, ppPipelineState);
    }
        
    virtual HRESULT STDMETHODCALLTYPE LoadComputePipeline(
        LPCWSTR pName,
        const D3D12_COMPUTE_PIPELINE_STATE_DESC *pDesc,
        REFIID riid,
        void **ppPipelineState)
    {
        return m_pDevice->CreateComputePipelineState(pDesc, riid, ppPipelineState);
    }
        
    virtual SIZE_T STDMETHODCALLTYPE GetSerializedSize(void)
    {
        return 0;
    }
        
    virtual HRESULT STDMETHODCALLTYPE Serialize(
        void *pData,
        SIZE_T DataSizeInBytes)
    {
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE LoadPipeline( 
        LPCWSTR pName,
        const D3D12_PIPELINE_STATE_STREAM_DESC *pDesc,
        REFIID riid,
        void **ppPipelineState)
    {
        return m_pDevice->CreatePipelineState(pDesc, riid, ppPipelineState);
    }

private:
    com_ptr<ID3D12Device2> m_pDevice;
    std::atomic<ULONG> m_refcount;
};