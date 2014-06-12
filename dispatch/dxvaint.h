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


#include <dxva2api.h>


#ifndef DXVA2_E_NOT_INITIALIZED
#define DXVA2_E_NOT_INITIALIZED ((HRESULT)0x80041000L)
#endif

#ifndef DXVA2_E_NEW_VIDEO_DEVICE
#define DXVA2_E_NEW_VIDEO_DEVICE ((HRESULT)0x80041001L)
#endif

#ifndef DXVA2_E_VIDEO_DEVICE_LOCKED
#define DXVA2_E_VIDEO_DEVICE_LOCKED ((HRESULT)0x80041002L)
#endif

#ifndef DXVA2_E_NOT_AVAILABLE
#define DXVA2_E_NOT_AVAILABLE ((HRESULT)0x80041003L)
#endif


typedef struct _DXVA2_DECODEBUFFERDESC {
    IDirect3DSurface9 *pRenderTarget;
    DWORD CompressedBufferType;
    DWORD BufferIndex;
    DWORD DataOffset;
    DWORD DataSize;
    DWORD FirstMBaddress;
    DWORD NumMBsInBuffer;
    DWORD Width;
    DWORD Height;
    DWORD Stride;
    DWORD ReservedBits;
    PVOID pCipherCounter;
} DXVA2_DECODEBUFFERDESC;

typedef struct _DXVA2_DECODEEXECUTE {
    UINT NumCompBuffers;
    DXVA2_DECODEBUFFERDESC *pCompressedBuffers;
} DXVA2_DECODEEXECUTE;

typedef struct _DXVA2_VIDEOSAMPLE {
  REFERENCE_TIME           Start;
  REFERENCE_TIME           End;
  DXVA2_ExtendedFormat   SampleFormat;
  DWORD SampleFlags;
  IDirect3DSurface9*   SrcSurface;
  RECT                     SrcRect;
  RECT                     DstRect;
  DXVA2_AYUVSample8      Pal[16];
  DXVA2_Fixed32          PlanarAlpha;
} DXVA2_VIDEOSAMPLE;

/**
 * See also:
 * - D3DDDIARG_VIDEOPROCESSBLT
 * - DXVA2_VideoProcessBltParams
 */
typedef struct _DXVA2_VIDEOPROCESSBLT {
    REFERENCE_TIME               TargetFrame;
    RECT                         TargetRect;
    SIZE                         ConstrictionSize;
    DWORD                        StreamingFlags;
    DXVA2_AYUVSample16         BackgroundColor;
    DXVA2_ExtendedFormat       DestFormat;
    DWORD DestFlags;
    DXVA2_ProcAmpValues        ProcAmpValues;
    DXVA2_Fixed32              Alpha;
    DXVA2_FilterValues         NoiseFilterLuma;
    DXVA2_FilterValues         NoiseFilterChroma;
    DXVA2_FilterValues         DetailFilterLuma;
    DXVA2_FilterValues         DetailFilterChroma;
    DXVA2_VIDEOSAMPLE          *pSrcSurfaces;
    UINT                         NumSrcSurfaces;
}  DXVA2_VIDEOPROCESSBLT;

typedef struct _DXVA2_DECODEBUFFERINFO DXVA2_DECODEBUFFERINFO; /* XXX */

typedef struct _DXVA2_EXTENSIONEXECUTE DXVA2_EXTENSIONEXECUTE; /* XXX */


static const GUID IID_IDirect3DDecodeDevice9 = {0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}; /* XXX */
static const GUID IID_IDirect3DVideoProcessDevice9 = {0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} /* XXX */;
static const GUID IID_IDirect3DDXVAExtensionDevice9 = {0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}; /* XXX */
static const GUID IID_IDirect3DDxva2Container9 = {0x126D0349,0x4787,0x4AA6,{0x8E,0x1B,0x40,0xC1,0x77,0xC6,0x0A,0x01}};


typedef struct DECLSPEC_ALIGN(16) _DXVA2_PVP_KEY128
{
    BYTE Data[16];
} DXVA2_PVP_KEY128;

typedef struct _DXVA2_PVP_SETKEY
{
    DXVA2_PVP_KEY128  ContentKey;
} DXVA2_PVP_SETKEY;

class IDirect3DDecodeDevice9 : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE DecodeBeginFrame(DXVA2_PVP_SETKEY *pPVPSetKey) = 0;
    virtual HRESULT STDMETHODCALLTYPE DecodeEndFrame(HANDLE *pHandleComplete) = 0;
    virtual HRESULT STDMETHODCALLTYPE DecodeSetRenderTarget(IDirect3DSurface9 *pRenderTarget) = 0;
    virtual HRESULT STDMETHODCALLTYPE DecodeExecute(DXVA2_DECODEEXECUTE /* const */ *pExecuteParams) = 0;
};


class IDirect3DVideoProcessDevice9 : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE VideoProcessBeginFrame(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE VideoProcessEndFrame(HANDLE *pHandleComplete) = 0;
    virtual HRESULT STDMETHODCALLTYPE VideoProcessSetRenderTarget(IDirect3DSurface9 *pRenderTarget) = 0;
    virtual HRESULT STDMETHODCALLTYPE VideoProcessBlt(DXVA2_VIDEOPROCESSBLT /* const */ *pData) = 0;
};


class IDirect3DDXVAExtensionDevice9 : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE ExtensionExecute(DXVA2_EXTENSIONEXECUTE *pData) = 0;
};


class IDirect3DDxva2Container9 : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE CreateSurface(UINT Width, UINT Height, UINT BackBuffers, D3DFORMAT Format, D3DPOOL Pool, DWORD Usage, DWORD DxvaType, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle) = 0;
    virtual HRESULT STDMETHODCALLTYPE VidToSysBlt(IDirect3DSurface9 *pSourceSurface, RECT *pSourceRect, IDirect3DSurface9 *pDestSurface, RECT *pDestRect) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDecodeGuidCount(UINT *pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDecodeGuids(UINT Count, GUID *pGuids) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDecodeRenderTargetFormatCount(REFGUID Guid, UINT *pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDecodeRenderTargets(REFGUID Guid, UINT Count, D3DFORMAT *pFormats) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDecodeCompressedBufferCount(REFGUID Guid, const DXVA2_VideoDesc *pVideoDesc, UINT *pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDecodeCompressedBuffers(REFGUID Guid, const DXVA2_VideoDesc *pVideoDesc, UINT Count, DXVA2_DECODEBUFFERINFO *pBufferInfo) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDecodeConfigurationCount(REFGUID Guid, const DXVA2_VideoDesc *pVideoDesc, UINT *pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDecodeConfigurations(REFGUID Guid, const DXVA2_VideoDesc *pVideoDesc, UINT Count, DXVA2_ConfigPictureDecode *pConfigs) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateDecodeDevice(REFGUID Guid, const DXVA2_VideoDesc *pVideoDesc, DXVA2_ConfigPictureDecode const *pConfig, IDirect3DSurface9 * *ppDecoderRenderTargets, UINT NumSurfaces, IDirect3DDecodeDevice9 ** ppDecode) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVideoProcessorDeviceGuidCount(const DXVA2_VideoDesc *pVideoDesc, UINT *pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVideoProcessorDeviceGuids(const DXVA2_VideoDesc *pVideoDesc, UINT Count, GUID *pGuids) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVideoProcessorCaps(REFGUID Guid, const DXVA2_VideoDesc *pVideoDesc, D3DFORMAT Format, DXVA2_VideoProcessorCaps *pCaps) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcAmpRange(REFGUID Guid, const DXVA2_VideoDesc *pVideoDesc, D3DFORMAT Format, UINT ProcAmpCap, DXVA2_ValueRange *pRange) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFilterPropertyRange(REFGUID Guid, const DXVA2_VideoDesc *pVideoDesc, D3DFORMAT Format, UINT FilterSetting, DXVA2_ValueRange *pRange) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVideoProcessorRenderTargetCount(REFGUID Guid, const DXVA2_VideoDesc *pVideoDesc, UINT *pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVideoProcessorRenderTargets(REFGUID Guid, const DXVA2_VideoDesc *pVideoDesc, UINT Count, D3DFORMAT *pFormats) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVideoProcessorSubStreamFormatCount(REFGUID Guid, const DXVA2_VideoDesc *pVideoDesc, D3DFORMAT Format, UINT *pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVideoProcessorSubStreamFormats(REFGUID Guid, const DXVA2_VideoDesc *pVideoDesc, D3DFORMAT Format, UINT Count, D3DFORMAT *pFormats) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateVideoProcessDevice(REFGUID Guid, const DXVA2_VideoDesc *pVideoDesc, D3DFORMAT Format, UINT, IDirect3DVideoProcessDevice9 * * ppVideoProcessDevice) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetExtensionGuidCount(DWORD Extension, UINT *pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetExtensionGuids(DWORD Extension, UINT Count, GUID *pGuids) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetExtensionCaps(REFGUID Guid, UINT, void *, UINT, void *, UINT) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateExtensionDevice(REFGUID Guid, void *, UINT, IDirect3DDXVAExtensionDevice9 ** ppExtension) = 0;
};

