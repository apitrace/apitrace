/**************************************************************************
 *
 * Copyright 2012 VMware, Inc.
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


#pragma once

#include <windows.h>

#include "winsdk_compat.h"

#include <dxgi1_6.h>


DEFINE_GUID(IID_IDXGISwapChainDWM,0xF69F223B,0x45D3,0x4AA0,0x98,0xC8,0xC4,0x0C,0x2B,0x23,0x10,0x29);
struct IDXGISwapChainDWM: public IDXGIDeviceSubObject
{
    virtual HRESULT STDMETHODCALLTYPE Present(UINT SyncInterval, UINT Flags) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetBuffer(UINT Buffer, REFIID riid, void **ppSurface) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDesc(DXGI_SWAP_CHAIN_DESC *pDesc) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResizeBuffers(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResizeTarget(const DXGI_MODE_DESC *pNewTargetParameters) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetContainingOutput(IDXGIOutput **ppOutput) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFrameStatistics(DXGI_FRAME_STATISTICS *pStats) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLastPresentCount(UINT *pLastPresentCount) = 0;
};

DEFINE_GUID(IID_IDXGIFactoryDWM,0x713F394E,0x92CA,0x47E7,0xAB,0x81,0x11,0x59,0xC2,0x79,0x1E,0x54);
struct IDXGIFactoryDWM: public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE CreateSwapChain(IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGIOutput *pOutput, IDXGISwapChainDWM **ppSwapChain) = 0;
};


DEFINE_GUID(IID_IWarpPrivateAPI,0xF13EBCD1,0x672C,0x4F8B,0xA6,0x31,0x95,0x39,0xCA,0x74,0x8D,0x71);


struct IUseCounted: public IUnknown
{
    virtual ULONG STDMETHODCALLTYPE UCAddUse(void) = 0;
    virtual ULONG STDMETHODCALLTYPE UCReleaseUse(void) = 0;
    virtual void STDMETHODCALLTYPE UCBreakCyclicReferences(void) = 0;
    virtual void STDMETHODCALLTYPE UCEstablishCyclicReferences(void) = 0;
    virtual void STDMETHODCALLTYPE UCDestroy(void) = 0;
};

struct ILayeredUseCounted: public IUseCounted
{
#if 0
    virtual HRESULT STDMETHODCALLTYPE LUCCompleteLayerConstruction(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE LUCBeginLayerDestruction(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE LUCDeferDeletion(int) = 0;
    virtual HRESULT STDMETHODCALLTYPE LUCDestroyStatus(E_LUC_Destroy eStatus) = 0;
    virtual HRESULT STDMETHODCALLTYPE LUCGetInternalIdentity(void const *pLayeredChildArgs, UINT uiArgSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE LUCGetInternalIdentity(void **ppInternalIdentity) = 0;
    virtual HRESULT STDMETHODCALLTYPE LUCGetChildType(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE LUCCompleteInitialization(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE LUCPrepareForDestruction(void) = 0;
#endif
};

struct ID3D11LayeredUseCounted: public ILayeredUseCounted
{
};

struct IUseCounted2: public IUseCounted
{
    virtual HRESULT STDMETHODCALLTYPE UCQueryInterface(UINT flags, REFIID riid, void **ppvObj) = 0;
};


struct ID3D10Buffer;

struct ID3D10DebugTest: public IUnknown
{
    virtual void STDMETHODCALLTYPE PreventFurtherExecutionOnError(BOOL Enable) = 0;
    virtual void STDMETHODCALLTYPE VSSetConstantBuffer14(ID3D10Buffer *pCB14) = 0;
    virtual void STDMETHODCALLTYPE GSSetConstantBuffer14(ID3D10Buffer *pCB14) = 0;
    virtual void STDMETHODCALLTYPE PSSetConstantBuffer14(ID3D10Buffer *pCB14) = 0;
};

struct ID3D11Buffer;

struct ID3D11DebugTest: public IUnknown
{
    virtual void STDMETHODCALLTYPE PreventFurtherExecutionOnError(BOOL Enable) = 0;
    virtual void STDMETHODCALLTYPE VSSetConstantBuffer14(ID3D11Buffer *pCB14) = 0;
    virtual void STDMETHODCALLTYPE HSSetConstantBuffer14(ID3D11Buffer *pCB14) = 0;
    virtual void STDMETHODCALLTYPE DSSetConstantBuffer14(ID3D11Buffer *pCB14) = 0;
    virtual void STDMETHODCALLTYPE GSSetConstantBuffer14(ID3D11Buffer *pCB14) = 0;
    virtual void STDMETHODCALLTYPE PSSetConstantBuffer14(ID3D11Buffer *pCB14) = 0;
    virtual void STDMETHODCALLTYPE CSSetConstantBuffer14(ID3D11Buffer *pCB14) = 0;
};


struct IDXGIDeviceXAML: public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE SetInProcessGPUPriority(INT Priority) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetInProcessGPUPriority(INT *pPriority) = 0;
};


#include <d3d11_2.h>

static const GUID IID_CID3D11Forwarder = {0xa05c8c37,0xd2c6,0x4732,{0xb3,0xa0,0x9c,0xe0,0xb0,0xdc,0x9a,0xe6}};
struct CID3D11Forwarder: public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE CreateBuffer(D3D11_BUFFER_DESC const *,D3D11_SUBRESOURCE_DATA const *,ID3D11Buffer * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateTexture1D(D3D11_TEXTURE1D_DESC const *,D3D11_SUBRESOURCE_DATA const *,ID3D11Texture1D * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateTexture2D(D3D11_TEXTURE2D_DESC const *,D3D11_SUBRESOURCE_DATA const *,ID3D11Texture2D * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateTexture3D(D3D11_TEXTURE3D_DESC const *,D3D11_SUBRESOURCE_DATA const *,ID3D11Texture3D * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateShaderResourceView(ID3D11Resource *,D3D11_SHADER_RESOURCE_VIEW_DESC const *,ID3D11ShaderResourceView * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateUnorderedAccessView(ID3D11Resource *,D3D11_UNORDERED_ACCESS_VIEW_DESC const *,ID3D11UnorderedAccessView * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateRenderTargetView(ID3D11Resource *,D3D11_RENDER_TARGET_VIEW_DESC const *,ID3D11RenderTargetView * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilView(ID3D11Resource *,D3D11_DEPTH_STENCIL_VIEW_DESC const *,ID3D11DepthStencilView * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateInputLayout(D3D11_INPUT_ELEMENT_DESC const *,UINT,void const *,SIZE_T,ID3D11InputLayout * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateVertexShader(void const *,SIZE_T,ID3D11ClassLinkage *,ID3D11VertexShader * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateGeometryShader(void const *,SIZE_T,ID3D11ClassLinkage *,ID3D11GeometryShader * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateGeometryShaderWithStreamOutput(void const *,SIZE_T,D3D11_SO_DECLARATION_ENTRY const *,UINT,UINT const *,UINT,UINT,ID3D11ClassLinkage *,ID3D11GeometryShader * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreatePixelShader(void const *,SIZE_T,ID3D11ClassLinkage *,ID3D11PixelShader * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateHullShader(void const *,SIZE_T,ID3D11ClassLinkage *,ID3D11HullShader * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateDomainShader(void const *,SIZE_T,ID3D11ClassLinkage *,ID3D11DomainShader * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateComputeShader(void const *,SIZE_T,ID3D11ClassLinkage *,ID3D11ComputeShader * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateClassLinkage(ID3D11ClassLinkage * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateBlendState(D3D11_BLEND_DESC const *,ID3D11BlendState * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilState(D3D11_DEPTH_STENCIL_DESC const *,ID3D11DepthStencilState * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateRasterizerState(D3D11_RASTERIZER_DESC const *,ID3D11RasterizerState * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateSamplerState(D3D11_SAMPLER_DESC const *,ID3D11SamplerState * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateQuery(D3D11_QUERY_DESC const *,ID3D11Query * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreatePredicate(D3D11_QUERY_DESC const *,ID3D11Predicate * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateCounter(D3D11_COUNTER_DESC const *,ID3D11Counter * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateDeferredContext(UINT,ID3D11DeviceContext * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE OpenSharedResource(void *,REFGUID,void * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CheckFormatSupport(DXGI_FORMAT,UINT *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CheckMultisampleQualityLevels(DXGI_FORMAT,UINT,UINT *) = 0;
    virtual void STDMETHODCALLTYPE CheckCounterInfo(D3D11_COUNTER_INFO *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CheckCounter(D3D11_COUNTER_DESC const *,D3D11_COUNTER_TYPE *,UINT *,char *,UINT *,char *,UINT *,char *,UINT *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CheckFeatureSupport(D3D11_FEATURE,void *,UINT) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID,UINT *,void *) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID,UINT,void const *) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID,IUnknown const *) = 0;
    virtual UINT STDMETHODCALLTYPE GetCreationFlags(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceRemovedReason(void) = 0;
    virtual void STDMETHODCALLTYPE GetImmediateContext(ID3D11DeviceContext * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetExceptionMode(UINT) = 0;
    virtual UINT STDMETHODCALLTYPE GetExceptionMode(void) = 0;
    virtual void STDMETHODCALLTYPE GetImmediateContext1(ID3D11DeviceContext1 * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateDeferredContext1(UINT,ID3D11DeviceContext1 * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateBlendState1(D3D11_BLEND_DESC1 const *,ID3D11BlendState1 * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateRasterizerState1(D3D11_RASTERIZER_DESC1 const *,ID3D11RasterizerState1 * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateDeviceContextState(UINT,D3D_FEATURE_LEVEL const *,UINT,UINT,REFGUID,D3D_FEATURE_LEVEL *,ID3DDeviceContextState * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE OpenSharedResource1(void *,REFGUID,void * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE OpenSharedResourceByName(LPCWSTR,DWORD,REFGUID,void * *) = 0;
    virtual void STDMETHODCALLTYPE GetImmediateContext2(ID3D11DeviceContext2 * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateDeferredContext2(UINT,ID3D11DeviceContext2 * *) = 0;
    virtual void STDMETHODCALLTYPE GetResourceTiling(ID3D11Resource *,UINT *,D3D11_PACKED_MIP_DESC *,D3D11_TILE_SHAPE *,UINT *,UINT,D3D11_SUBRESOURCE_TILING *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CheckMultisampleQualityLevels1(DXGI_FORMAT,UINT,UINT,UINT *) = 0;
#if 0
    virtual HRESULT STDMETHODCALLTYPE CreateTexture2D1(D3D11_TEXTURE2D_DESC1 const *,D3D11_SUBRESOURCE_DATA const *,ID3D11Texture2D1 * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateTexture3D1(D3D11_TEXTURE3D_DESC1 const *,D3D11_SUBRESOURCE_DATA const *,ID3D11Texture3D1 * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateRasterizerState2(D3D11_RASTERIZER_DESC2 const *,ID3D11RasterizerState2 * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateShaderResourceView1(ID3D11Resource *,D3D11_SHADER_RESOURCE_VIEW_DESC1 const *,ID3D11ShaderResourceView1 * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateUnorderedAccessView1(ID3D11Resource *,D3D11_UNORDERED_ACCESS_VIEW_DESC1 const *,ID3D11UnorderedAccessView1 * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateRenderTargetView1(ID3D11Resource *,D3D11_RENDER_TARGET_VIEW_DESC1 const *,ID3D11RenderTargetView1 * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateQuery1(D3D11_QUERY_DESC1 const *,ID3D11Query1 * *) = 0;
    virtual void STDMETHODCALLTYPE GetImmediateContext3(ID3D11DeviceContext3 * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateDeferredContext3(UINT,ID3D11DeviceContext3 * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE WriteToSubresource(ID3D11Resource *,UINT,D3D11_BOX const *,void const *,UINT,UINT) = 0;
    virtual HRESULT STDMETHODCALLTYPE ReadFromSubresource(void *,UINT,UINT,ID3D11Resource *,UINT,D3D11_BOX const *) = 0;
    virtual D3D_FEATURE_LEVEL STDMETHODCALLTYPE GetD3DFeatureLevel(void) = 0;
#endif
};

struct ID3D11PartnerDevice: public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE BeginGuardRectangleSupport(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE EndGuardRectangleSupport(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateGuardableTexture2D(const D3D11_TEXTURE2D_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture2D **ppTexture2D) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetGuardRect(ID3D11Texture2D *pTexture2D, RECT Rect) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEmptyGuardRect(ID3D11Texture2D *pTexture2D) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetUnguarded(ID3D11Texture2D *pTexture2D) = 0;
    virtual HRESULT STDMETHODCALLTYPE OfferResourcesInternal(UINT NumResources, IDXGIResource * const * ppResources, DXGI_OFFER_RESOURCE_PRIORITY Priority) = 0;
    virtual HRESULT STDMETHODCALLTYPE ReclaimResourcesInternal(UINT NumResources, IDXGIResource * const * ppResources, BOOL *pDiscarded) = 0;
    virtual UINT STDMETHODCALLTYPE GetPartnerCaps(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateCompositionBuffer(UINT, UINT, DXGI_FORMAT, D3D11_USAGE, UINT, UINT, REFIID, void * *, void * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE PresentCompositionBuffers(void *, IUnknown * const *, UINT) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetGuardRect(ID3D11Texture2D *pTexture2D, BOOL *pEmpty, RECT *pRect) = 0;
};


struct ID2DPrivateInfo: public IUnknown
{
    virtual UINT STDMETHODCALLTYPE ConservativeFlushCount(void) = 0;
};
