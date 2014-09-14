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

#include <dxgi1_3.h>


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
struct IWarpPrivateAPI: public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE WarpEscape(void *pData) = 0;
};

struct DXGI_SCROLL_RECT {
    RECT rc1;
    RECT rc2;
};

struct DXGI_FRAME_STATISTICS_DWM;
struct DXGI_MULTIPLANE_OVERLAY_CAPS;
struct DXGI_CHECK_MULTIPLANEOVERLAYSUPPORT_PLANE_INFO;
struct DXGI_PRESENT_MULTIPLANE_OVERLAY;


DEFINE_GUID(IID_IDXGISwapChainDWM1,0x8F0F30DB,0x446A,0x4282,0x9F,0xA2,0x28,0xF7,0x5C,0x1F,0x39,0xAD);
struct IDXGISwapChainDWM1: public IDXGISwapChainDWM
{
    virtual HRESULT STDMETHODCALLTYPE Present1(UINT SyncInterval, UINT Flags, UINT NumRects, const RECT *pRects, UINT NumScrollRects, const DXGI_SCROLL_RECT *pScrollRects) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLogicalSurfaceHandle(UINT64 *pHandle) = 0;
    virtual HRESULT STDMETHODCALLTYPE CheckDirectFlipSupport(UINT CheckDirectFlipFlags, IDXGIResource *pResource, BOOL *pSupported) = 0;
    virtual HRESULT STDMETHODCALLTYPE Present2(UINT SyncInterval, UINT Flags, UINT NumRects, const RECT *pRects, UINT NumScrollRects, const DXGI_SCROLL_RECT *pScrollRects, IDXGIResource *pResource) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCompositionSurface(HANDLE *pHandle) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFrameStatisticsDWM(DXGI_FRAME_STATISTICS_DWM *pStats) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMultiplaneOverlayCaps(DXGI_MULTIPLANE_OVERLAY_CAPS *pCaps) = 0;
    virtual HRESULT STDMETHODCALLTYPE CheckMultiplaneOverlaySupport(UINT Arg1, const DXGI_CHECK_MULTIPLANEOVERLAYSUPPORT_PLANE_INFO *pInfo, BOOL *pSupported) = 0;
    virtual HRESULT STDMETHODCALLTYPE PresentMultiplaneOverlay(UINT Arg1, UINT Arg2, UINT Arg3, const DXGI_PRESENT_MULTIPLANE_OVERLAY *pArg4) = 0;
    virtual HRESULT STDMETHODCALLTYPE CheckPresentDurationSupport(UINT Arg1, UINT *pArg2, UINT *pArg3) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPrivateFrameDuration(UINT Duration) = 0;
};


DEFINE_GUID(IID_IDXGIFactoryDWM2,0x1DDD77AA,0x9A4A,0x4CC8,0x9E,0x55,0x98,0xC1,0x96,0xBA,0xFC,0x8F);
struct IDXGIFactoryDWM2: public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE CreateSwapChainDWM(IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGIOutput *pOutput, IDXGISwapChainDWM **ppSwapChain) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateSwapChainDDA(IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC1 *pDesc, IDXGIOutput *pOutput, IDXGISwapChainDWM1 **ppSwapChain) = 0;
};


struct DXGI_OUTPUT_DWM_DESC {
    BOOL bUnknown;
};


DEFINE_GUID(IID_IDXGIOutputDWM,0x6F66A9A0,0xBECE,0x4EE8,0xB1,0x1B,0x99,0x0E,0xB3,0x8E,0xD9,0x76);
struct IDXGIOutputDWM: public IUnknown
{
    virtual BOOL STDMETHODCALLTYPE HasDDAClient(void) = 0;
    virtual void STDMETHODCALLTYPE GetDesc(DXGI_OUTPUT_DWM_DESC *pDesc) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindClosestMatchingModeFromDesktop(const DXGI_MODE_DESC1 *pModeToMatch, DXGI_MODE_DESC1 *pClosestMatch, IUnknown *pConcernedDevice) = 0;
    virtual HRESULT STDMETHODCALLTYPE WaitForVBlankOrObjects(UINT NumHandles, const HANDLE *pHandles) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFrameStatisticsDWM(DXGI_FRAME_STATISTICS_DWM *pStats) = 0;
};


struct DXGI_INTERNAL_TRACKED_FENCE;


DEFINE_GUID(IID_IDXGIDeviceDWM,0xC0C5FDA1,0x8899,0x4194,0xB1,0xF6,0x54,0x3C,0x6D,0x2E,0xD3,0x14);
struct IDXGIDeviceDWM: public IUnknown
{
    // cf. D3DKMTOpenSyncObjectFromNtHandle
    virtual HRESULT STDMETHODCALLTYPE OpenFenceByHandle(HANDLE hFence, DXGI_INTERNAL_TRACKED_FENCE **ppFence) = 0;
    // ??
    virtual HRESULT STDMETHODCALLTYPE ReleaseResources(UINT NumResources, IDXGIResource * const *pResources) = 0;
    // cf. D3DKMTDestroySynchronizationObject
    virtual HRESULT STDMETHODCALLTYPE CloseFence(DXGI_INTERNAL_TRACKED_FENCE *pFence) = 0;
    // cf. D3DKMTPinDirectFlipResources
    virtual HRESULT STDMETHODCALLTYPE PinResources(UINT NumResources, IDXGIResource * const *pResources) = 0;
    // cf. D3DKMTUnpinDirectFlipResources
    virtual HRESULT STDMETHODCALLTYPE UnpinResources(UINT NumResources, IDXGIResource * const *pResources) = 0;
};


DEFINE_GUID(IID_CDXGIAdapter,0x712BD56D,0x86FF,0x4B71,0x91,0xE1,0xC1,0x3B,0x27,0x4F,0xF2,0xA2);
struct CDXGIAdapter: public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE OpenKernelHandle(void **ppHandle) = 0;
    virtual HRESULT STDMETHODCALLTYPE CloseKernelHandle(void *pHandle) = 0;
};


#include <d3d11.h>


DEFINE_GUID(IID_ID3D11PartnerDevice,0x07F857E7,0x077F,0x4119,0x86,0xB9,0xE7,0xAE,0x71,0x04,0xCF,0x33);
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
    virtual HRESULT STDMETHODCALLTYPE CreateCompositionBuffer(UINT, UINT, DXGI_FORMAT, D3D11_USAGE, UINT, UINT, REFIID, HANDLE *, void * *) = 0;
    virtual HRESULT STDMETHODCALLTYPE PresentCompositionBuffers(HANDLE, IUnknown * const *, UINT) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetGuardRect(ID3D11Texture2D *pTexture2D, BOOL *pEmpty, RECT *pRect) = 0;
};


DEFINE_GUID(IID_ID2DPrivateInfo,0xB79CC8DA,0x337F,0x400F,0xB0,0x9D,0xB2,0xED,0xF8,0xA8,0x4E,0x47);
struct ID2DPrivateInfo: public IUnknown
{
    virtual UINT STDMETHODCALLTYPE ConservativeFlushCount(void) = 0;
};
