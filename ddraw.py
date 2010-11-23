##########################################################################
#
# Copyright 2008-2009 VMware, Inc.
# All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
##########################################################################/

"""ddraw.h"""

from windows import *

IDirectDraw = Interface("IDirectDraw", IUnknown)
IDirectDraw2 = Interface("IDirectDraw2", IUnknown)
IDirectDraw4 = Interface("IDirectDraw4", IUnknown)
IDirectDraw7 = Interface("IDirectDraw7", IUnknown)
IDirectDrawPalette = Interface("IDirectDrawPalette", IUnknown)
IDirectDrawClipper = Interface("IDirectDrawClipper", IUnknown)
IDirectDrawSurface = Interface("IDirectDrawSurface", IUnknown)
IDirectDrawSurface2 = Interface("IDirectDrawSurface2", IUnknown)
IDirectDrawSurface3 = Interface("IDirectDrawSurface3", IUnknown)
IDirectDrawSurface4 = Interface("IDirectDrawSurface4", IUnknown)
IDirectDrawSurface7 = Interface("IDirectDrawSurface7", IUnknown)
IDirectDrawColorControl = Interface("IDirectDrawColorControl", IUnknown)
IDirectDrawGammaControl = Interface("IDirectDrawGammaControl", IUnknown)

LPDIRECTDRAW = Pointer(IDirectDraw)
LPDIRECTDRAW2 = Pointer(IDirectDraw2)
LPDIRECTDRAW4 = Pointer(IDirectDraw4)
LPDIRECTDRAW7 = Pointer(IDirectDraw7)
LPDIRECTDRAWSURFACE = Pointer(IDirectDrawSurface)
LPDIRECTDRAWSURFACE2 = Pointer(IDirectDrawSurface2)
LPDIRECTDRAWSURFACE3 = Pointer(IDirectDrawSurface3)
LPDIRECTDRAWSURFACE4 = Pointer(IDirectDrawSurface4)
LPDIRECTDRAWSURFACE7 = Pointer(IDirectDrawSurface7)
LPDIRECTDRAWPALETTE = Pointer(IDirectDrawPalette)
LPDIRECTDRAWCLIPPER = Pointer(IDirectDrawClipper)
LPDIRECTDRAWCOLORCONTROL = Pointer(IDirectDrawColorControl)
LPDIRECTDRAWGAMMACONTROL = Pointer(IDirectDrawGammaControl)

DDARGB = Struct("DDARGB", [
    (BYTE, "blue"),
    (BYTE, "green"),
    (BYTE, "red"),
    (BYTE, "alpha"),
])

LPDDARGB = Pointer(DDARGB)

DDRGBA = Struct("DDRGBA", [
    (BYTE, "red"),
    (BYTE, "green"),
    (BYTE, "blue"),
    (BYTE, "alpha"),
])
LPDDRGBA = Pointer(DDRGBA)

DDCOLORKEY = Struct("DDCOLORKEY", [
    (DWORD, "dwColorSpaceLowValue"),
    (DWORD, "dwColorSpaceHighValue"),
])
LPDDCOLORKEY = Pointer(DDCOLORKEY)

DDBLTFX = Struct("DDBLTFX", [
    (DWORD, "dwSize"),
    (DWORD, "dwDDFX"),
    (DWORD, "dwROP"),
    (DWORD, "dwDDROP"),
    (DWORD, "dwRotationAngle"),
    (DWORD, "dwZBufferOpCode"),
    (DWORD, "dwZBufferLow"),
    (DWORD, "dwZBufferHigh"),
    (DWORD, "dwZBufferBaseDest"),
    (DWORD, "dwZDestConstBitDepth"),
    (DWORD, "dwZDestConst"),
    (LPDIRECTDRAWSURFACE, "lpDDSZBufferDest"),
    (DWORD, "dwZSrcConstBitDepth"),
    (DWORD, "dwZSrcConst"),
    (LPDIRECTDRAWSURFACE, "lpDDSZBufferSrc"),
    (DWORD, "dwAlphaEdgeBlendBitDepth"),
    (DWORD, "dwAlphaEdgeBlend"),
    (DWORD, "dwReserved"),
    (DWORD, "dwAlphaDestConstBitDepth"),
    (DWORD, "dwAlphaDestConst"),
    (LPDIRECTDRAWSURFACE, "lpDDSAlphaDest"),
    (DWORD, "dwAlphaSrcConstBitDepth"),
    (DWORD, "dwAlphaSrcConst"),
    (LPDIRECTDRAWSURFACE, "lpDDSAlphaSrc"),
    (DWORD, "dwFillColor"),
    (DWORD, "dwFillDepth"),
    (DWORD, "dwFillPixel"),
    (LPDIRECTDRAWSURFACE, "lpDDSPattern"),
    (DDCOLORKEY, "ddckDestColorkey"),
    (DDCOLORKEY, "ddckSrcColorkey"),
])
LPDDBLTFX = Pointer(DDBLTFX)

DDSCAPS = Struct("DDSCAPS", [
    (DWORD, "dwCaps"),
])
LPDDSCAPS = Pointer(DDSCAPS)

DDOSCAPS = Struct("DDOSCAPS", [
    (DWORD, "dwCaps"),
])
LPDDOSCAPS = Pointer(DDOSCAPS)

DDSCAPSEX = Struct("DDSCAPSEX", [
    (DWORD, "dwCaps2"),
    (DWORD, "dwCaps3"),
    (DWORD, "dwCaps4"),
])

DDSCAPS2 = Struct("DDSCAPS2", [
    (DWORD, "dwCaps"),
    (DWORD, "dwCaps2"),
    (DWORD, "dwCaps3"),
    (DWORD, "dwCaps4"),
])
LPDDSCAPS2 = Pointer(DDSCAPS2)

DDCAPS_DX1 = Struct("DDCAPS_DX1", [
    (DWORD, "dwSize"),
    (DWORD, "dwCaps"),
    (DWORD, "dwCaps2"),
    (DWORD, "dwCKeyCaps"),
    (DWORD, "dwFXCaps"),
    (DWORD, "dwFXAlphaCaps"),
    (DWORD, "dwPalCaps"),
    (DWORD, "dwSVCaps"),
    (DWORD, "dwAlphaBltConstBitDepths"),
    (DWORD, "dwAlphaBltPixelBitDepths"),
    (DWORD, "dwAlphaBltSurfaceBitDepths"),
    (DWORD, "dwAlphaOverlayConstBitDepths"),
    (DWORD, "dwAlphaOverlayPixelBitDepths"),
    (DWORD, "dwAlphaOverlaySurfaceBitDepths"),
    (DWORD, "dwZBufferBitDepths"),
    (DWORD, "dwVidMemTotal"),
    (DWORD, "dwVidMemFree"),
    (DWORD, "dwMaxVisibleOverlays"),
    (DWORD, "dwCurrVisibleOverlays"),
    (DWORD, "dwNumFourCCCodes"),
    (DWORD, "dwAlignBoundarySrc"),
    (DWORD, "dwAlignSizeSrc"),
    (DWORD, "dwAlignBoundaryDest"),
    (DWORD, "dwAlignSizeDest"),
    (DWORD, "dwAlignStrideAlign"),
    (DWORD, "dwRops[DD_ROP_SPACE]"),
    (DDSCAPS, "ddsCaps"),
    (DWORD, "dwMinOverlayStretch"),
    (DWORD, "dwMaxOverlayStretch"),
    (DWORD, "dwMinLiveVideoStretch"),
    (DWORD, "dwMaxLiveVideoStretch"),
    (DWORD, "dwMinHwCodecStretch"),
    (DWORD, "dwMaxHwCodecStretch"),
    (DWORD, "dwReserved1"),
    (DWORD, "dwReserved2"),
    (DWORD, "dwReserved3"),
])
LPDDCAPS_DX1 = Pointer(DDCAPS_DX1)

DDCAPS_DX3 = Struct("DDCAPS_DX3", [
    (DWORD, "dwSize"),
    (DWORD, "dwCaps"),
    (DWORD, "dwCaps2"),
    (DWORD, "dwCKeyCaps"),
    (DWORD, "dwFXCaps"),
    (DWORD, "dwFXAlphaCaps"),
    (DWORD, "dwPalCaps"),
    (DWORD, "dwSVCaps"),
    (DWORD, "dwAlphaBltConstBitDepths"),
    (DWORD, "dwAlphaBltPixelBitDepths"),
    (DWORD, "dwAlphaBltSurfaceBitDepths"),
    (DWORD, "dwAlphaOverlayConstBitDepths"),
    (DWORD, "dwAlphaOverlayPixelBitDepths"),
    (DWORD, "dwAlphaOverlaySurfaceBitDepths"),
    (DWORD, "dwZBufferBitDepths"),
    (DWORD, "dwVidMemTotal"),
    (DWORD, "dwVidMemFree"),
    (DWORD, "dwMaxVisibleOverlays"),
    (DWORD, "dwCurrVisibleOverlays"),
    (DWORD, "dwNumFourCCCodes"),
    (DWORD, "dwAlignBoundarySrc"),
    (DWORD, "dwAlignSizeSrc"),
    (DWORD, "dwAlignBoundaryDest"),
    (DWORD, "dwAlignSizeDest"),
    (DWORD, "dwAlignStrideAlign"),
    (DWORD, "dwRops[DD_ROP_SPACE]"),
    (DDSCAPS, "ddsCaps"),
    (DWORD, "dwMinOverlayStretch"),
    (DWORD, "dwMaxOverlayStretch"),
    (DWORD, "dwMinLiveVideoStretch"),
    (DWORD, "dwMaxLiveVideoStretch"),
    (DWORD, "dwMinHwCodecStretch"),
    (DWORD, "dwMaxHwCodecStretch"),
    (DWORD, "dwReserved1"),
    (DWORD, "dwReserved2"),
    (DWORD, "dwReserved3"),
    (DWORD, "dwSVBCaps"),
    (DWORD, "dwSVBCKeyCaps"),
    (DWORD, "dwSVBFXCaps"),
    (DWORD, "dwSVBRops[DD_ROP_SPACE]"),
    (DWORD, "dwVSBCaps"),
    (DWORD, "dwVSBCKeyCaps"),
    (DWORD, "dwVSBFXCaps"),
    (DWORD, "dwVSBRops[DD_ROP_SPACE]"),
    (DWORD, "dwSSBCaps"),
    (DWORD, "dwSSBCKeyCaps"),
    (DWORD, "dwSSBFXCaps"),
    (DWORD, "dwSSBRops[DD_ROP_SPACE]"),
    (DWORD, "dwReserved4"),
    (DWORD, "dwReserved5"),
    (DWORD, "dwReserved6"),
])
LPDDCAPS_DX3 = Pointer(DDCAPS_DX3)

DDCAPS_DX5 = Struct("DDCAPS_DX5", [
    (DWORD, "dwSize"),
    (DWORD, "dwCaps"),
    (DWORD, "dwCaps2"),
    (DWORD, "dwCKeyCaps"),
    (DWORD, "dwFXCaps"),
    (DWORD, "dwFXAlphaCaps"),
    (DWORD, "dwPalCaps"),
    (DWORD, "dwSVCaps"),
    (DWORD, "dwAlphaBltConstBitDepths"),
    (DWORD, "dwAlphaBltPixelBitDepths"),
    (DWORD, "dwAlphaBltSurfaceBitDepths"),
    (DWORD, "dwAlphaOverlayConstBitDepths"),
    (DWORD, "dwAlphaOverlayPixelBitDepths"),
    (DWORD, "dwAlphaOverlaySurfaceBitDepths"),
    (DWORD, "dwZBufferBitDepths"),
    (DWORD, "dwVidMemTotal"),
    (DWORD, "dwVidMemFree"),
    (DWORD, "dwMaxVisibleOverlays"),
    (DWORD, "dwCurrVisibleOverlays"),
    (DWORD, "dwNumFourCCCodes"),
    (DWORD, "dwAlignBoundarySrc"),
    (DWORD, "dwAlignSizeSrc"),
    (DWORD, "dwAlignBoundaryDest"),
    (DWORD, "dwAlignSizeDest"),
    (DWORD, "dwAlignStrideAlign"),
    (DWORD, "dwRops[DD_ROP_SPACE]"),
    (DDSCAPS, "ddsCaps"),
    (DWORD, "dwMinOverlayStretch"),
    (DWORD, "dwMaxOverlayStretch"),
    (DWORD, "dwMinLiveVideoStretch"),
    (DWORD, "dwMaxLiveVideoStretch"),
    (DWORD, "dwMinHwCodecStretch"),
    (DWORD, "dwMaxHwCodecStretch"),
    (DWORD, "dwReserved1"),
    (DWORD, "dwReserved2"),
    (DWORD, "dwReserved3"),
    (DWORD, "dwSVBCaps"),
    (DWORD, "dwSVBCKeyCaps"),
    (DWORD, "dwSVBFXCaps"),
    (DWORD, "dwSVBRops[DD_ROP_SPACE]"),
    (DWORD, "dwVSBCaps"),
    (DWORD, "dwVSBCKeyCaps"),
    (DWORD, "dwVSBFXCaps"),
    (DWORD, "dwVSBRops[DD_ROP_SPACE]"),
    (DWORD, "dwSSBCaps"),
    (DWORD, "dwSSBCKeyCaps"),
    (DWORD, "dwSSBFXCaps"),
    (DWORD, "dwSSBRops[DD_ROP_SPACE]"),
    (DWORD, "dwMaxVideoPorts"),
    (DWORD, "dwCurrVideoPorts"),
    (DWORD, "dwSVBCaps2"),
    (DWORD, "dwNLVBCaps"),
    (DWORD, "dwNLVBCaps2"),
    (DWORD, "dwNLVBCKeyCaps"),
    (DWORD, "dwNLVBFXCaps"),
    (DWORD, "dwNLVBRops[DD_ROP_SPACE]"),
])
LPDDCAPS_DX5 = Pointer(DDCAPS_DX5)

DDCAPS_DX6 = Struct("DDCAPS_DX6", [
    (DWORD, "dwSize"),
    (DWORD, "dwCaps"),
    (DWORD, "dwCaps2"),
    (DWORD, "dwCKeyCaps"),
    (DWORD, "dwFXCaps"),
    (DWORD, "dwFXAlphaCaps"),
    (DWORD, "dwPalCaps"),
    (DWORD, "dwSVCaps"),
    (DWORD, "dwAlphaBltConstBitDepths"),
    (DWORD, "dwAlphaBltPixelBitDepths"),
    (DWORD, "dwAlphaBltSurfaceBitDepths"),
    (DWORD, "dwAlphaOverlayConstBitDepths"),
    (DWORD, "dwAlphaOverlayPixelBitDepths"),
    (DWORD, "dwAlphaOverlaySurfaceBitDepths"),
    (DWORD, "dwZBufferBitDepths"),
    (DWORD, "dwVidMemTotal"),
    (DWORD, "dwVidMemFree"),
    (DWORD, "dwMaxVisibleOverlays"),
    (DWORD, "dwCurrVisibleOverlays"),
    (DWORD, "dwNumFourCCCodes"),
    (DWORD, "dwAlignBoundarySrc"),
    (DWORD, "dwAlignSizeSrc"),
    (DWORD, "dwAlignBoundaryDest"),
    (DWORD, "dwAlignSizeDest"),
    (DWORD, "dwAlignStrideAlign"),
    (DWORD, "dwRops[DD_ROP_SPACE]"),
    (DDSCAPS, "ddsOldCaps"),
    (DWORD, "dwMinOverlayStretch"),
    (DWORD, "dwMaxOverlayStretch"),
    (DWORD, "dwMinLiveVideoStretch"),
    (DWORD, "dwMaxLiveVideoStretch"),
    (DWORD, "dwMinHwCodecStretch"),
    (DWORD, "dwMaxHwCodecStretch"),
    (DWORD, "dwReserved1"),
    (DWORD, "dwReserved2"),
    (DWORD, "dwReserved3"),
    (DWORD, "dwSVBCaps"),
    (DWORD, "dwSVBCKeyCaps"),
    (DWORD, "dwSVBFXCaps"),
    (DWORD, "dwSVBRops[DD_ROP_SPACE]"),
    (DWORD, "dwVSBCaps"),
    (DWORD, "dwVSBCKeyCaps"),
    (DWORD, "dwVSBFXCaps"),
    (DWORD, "dwVSBRops[DD_ROP_SPACE]"),
    (DWORD, "dwSSBCaps"),
    (DWORD, "dwSSBCKeyCaps"),
    (DWORD, "dwSSBFXCaps"),
    (DWORD, "dwSSBRops[DD_ROP_SPACE]"),
    (DWORD, "dwMaxVideoPorts"),
    (DWORD, "dwCurrVideoPorts"),
    (DWORD, "dwSVBCaps2"),
    (DWORD, "dwNLVBCaps"),
    (DWORD, "dwNLVBCaps2"),
    (DWORD, "dwNLVBCKeyCaps"),
    (DWORD, "dwNLVBFXCaps"),
    (DWORD, "dwNLVBRops[DD_ROP_SPACE]"),
    (DDSCAPS2, "ddsCaps"),
])
LPDDCAPS_DX6 = Pointer(DDCAPS_DX6)

DDCAPS_DX7 = Struct("DDCAPS_DX7", [
    (DWORD, "dwSize"),
    (DWORD, "dwCaps"),
    (DWORD, "dwCaps2"),
    (DWORD, "dwCKeyCaps"),
    (DWORD, "dwFXCaps"),
    (DWORD, "dwFXAlphaCaps"),
    (DWORD, "dwPalCaps"),
    (DWORD, "dwSVCaps"),
    (DWORD, "dwAlphaBltConstBitDepths"),
    (DWORD, "dwAlphaBltPixelBitDepths"),
    (DWORD, "dwAlphaBltSurfaceBitDepths"),
    (DWORD, "dwAlphaOverlayConstBitDepths"),
    (DWORD, "dwAlphaOverlayPixelBitDepths"),
    (DWORD, "dwAlphaOverlaySurfaceBitDepths"),
    (DWORD, "dwZBufferBitDepths"),
    (DWORD, "dwVidMemTotal"),
    (DWORD, "dwVidMemFree"),
    (DWORD, "dwMaxVisibleOverlays"),
    (DWORD, "dwCurrVisibleOverlays"),
    (DWORD, "dwNumFourCCCodes"),
    (DWORD, "dwAlignBoundarySrc"),
    (DWORD, "dwAlignSizeSrc"),
    (DWORD, "dwAlignBoundaryDest"),
    (DWORD, "dwAlignSizeDest"),
    (DWORD, "dwAlignStrideAlign"),
    (DWORD, "dwRops[DD_ROP_SPACE]"),
    (DDSCAPS, "ddsOldCaps"),
    (DWORD, "dwMinOverlayStretch"),
    (DWORD, "dwMaxOverlayStretch"),
    (DWORD, "dwMinLiveVideoStretch"),
    (DWORD, "dwMaxLiveVideoStretch"),
    (DWORD, "dwMinHwCodecStretch"),
    (DWORD, "dwMaxHwCodecStretch"),
    (DWORD, "dwReserved1"),
    (DWORD, "dwReserved2"),
    (DWORD, "dwReserved3"),
    (DWORD, "dwSVBCaps"),
    (DWORD, "dwSVBCKeyCaps"),
    (DWORD, "dwSVBFXCaps"),
    (DWORD, "dwSVBRops[DD_ROP_SPACE]"),
    (DWORD, "dwVSBCaps"),
    (DWORD, "dwVSBCKeyCaps"),
    (DWORD, "dwVSBFXCaps"),
    (DWORD, "dwVSBRops[DD_ROP_SPACE]"),
    (DWORD, "dwSSBCaps"),
    (DWORD, "dwSSBCKeyCaps"),
    (DWORD, "dwSSBFXCaps"),
    (DWORD, "dwSSBRops[DD_ROP_SPACE]"),
    (DWORD, "dwMaxVideoPorts"),
    (DWORD, "dwCurrVideoPorts"),
    (DWORD, "dwSVBCaps2"),
    (DWORD, "dwNLVBCaps"),
    (DWORD, "dwNLVBCaps2"),
    (DWORD, "dwNLVBCKeyCaps"),
    (DWORD, "dwNLVBFXCaps"),
    (DWORD, "dwNLVBRops[DD_ROP_SPACE]"),
    (DDSCAPS2, "ddsCaps"),
])
LPDDCAPS_DX7 = Pointer(DDCAPS_DX7)
DDCAPS = DDCAPS_DX7
LPDDCAPS = Pointer(DDCAPS)

DDPIXELFORMAT = Struct("DDPIXELFORMAT", [
    (DWORD, "dwSize"),
    (DWORD, "dwFlags"),
    (DWORD, "dwFourCC"),
    (DWORD, "dwRGBBitCount"),
    (DWORD, "dwYUVBitCount"),
    (DWORD, "dwZBufferBitDepth"),
    (DWORD, "dwAlphaBitDepth"),
    (DWORD, "dwLuminanceBitCount"),
    (DWORD, "dwBumpBitCount"),
    (DWORD, "dwPrivateFormatBitCount"),
    (DWORD, "dwRBitMask"),
    (DWORD, "dwYBitMask"),
    (DWORD, "dwStencilBitDepth"),
    (DWORD, "dwLuminanceBitMask"),
    (DWORD, "dwBumpDuBitMask"),
    (DWORD, "dwOperations"),
    (DWORD, "dwGBitMask"),
    (DWORD, "dwUBitMask"),
    (DWORD, "dwZBitMask"),
    (DWORD, "dwBumpDvBitMask"),
    (WORD, "MultiSampleCaps.wFlipMSTypes"),
    (WORD, "MultiSampleCaps.wBltMSTypes"),
    (DWORD, "dwBBitMask"),
    (DWORD, "dwVBitMask"),
    (DWORD, "dwStencilBitMask"),
    (DWORD, "dwBumpLuminanceBitMask"),
    (DWORD, "dwRGBAlphaBitMask"),
    (DWORD, "dwYUVAlphaBitMask"),
    (DWORD, "dwLuminanceAlphaBitMask"),
    (DWORD, "dwRGBZBitMask"),
    (DWORD, "dwYUVZBitMask"),
])
LPDDPIXELFORMAT = Pointer(DDPIXELFORMAT)

DDOVERLAYFX = Struct("DDOVERLAYFX", [
    (DWORD, "dwSize"),
    (DWORD, "dwAlphaEdgeBlendBitDepth"),
    (DWORD, "dwAlphaEdgeBlend"),
    (DWORD, "dwReserved"),
    (DWORD, "dwAlphaDestConstBitDepth"),
    (DWORD, "dwAlphaDestConst"),
    (LPDIRECTDRAWSURFACE, "lpDDSAlphaDest"),
    (DWORD, "dwAlphaSrcConstBitDepth"),
    (DWORD, "dwAlphaSrcConst"),
    (LPDIRECTDRAWSURFACE, "lpDDSAlphaSrc"),
    (DDCOLORKEY, "dckDestColorkey"),
    (DDCOLORKEY, "dckSrcColorkey"),
    (DWORD, "dwDDFX"),
    (DWORD, "dwFlags"),
])
LPDDOVERLAYFX = Pointer(DDOVERLAYFX)

DDBLTBATCH = Struct("DDBLTBATCH", [
    (LPRECT, "lprDest"),
    (LPDIRECTDRAWSURFACE, "lpDDSSrc"),
    (LPRECT, "lprSrc"),
    (DWORD, "dwFlags"),
    (LPDDBLTFX, "lpDDBltFx"),
])
LPDDBLTBATCH = Pointer(DDBLTBATCH)

DDGAMMARAMP = Struct("DDGAMMARAMP", [
    (WORD, "red[256]"),
    (WORD, "green[256]"),
    (WORD, "blue[256]"),
])
LPDDGAMMARAMP = Pointer(DDGAMMARAMP)

DDDEVICEIDENTIFIER = Struct("DDDEVICEIDENTIFIER", [
    (String, "szDriver"),
    (String, "szDescription"),
    (LARGE_INTEGER, "liDriverVersion"),
    (DWORD, "dwVendorId"),
    (DWORD, "dwDeviceId"),
    (DWORD, "dwSubSysId"),
    (DWORD, "dwRevision"),
    (GUID, "guidDeviceIdentifier"),
])
LPDDDEVICEIDENTIFIER = Pointer(DDDEVICEIDENTIFIER)

DDDEVICEIDENTIFIER2 = Struct("DDDEVICEIDENTIFIER2", [
    (String, "szDriver[MAX_DDDEVICEID_STRING]"),
    (String, "szDescription[MAX_DDDEVICEID_STRING]"),
    (LARGE_INTEGER, "liDriverVersion"),
    (DWORD, "dwVendorId"),
    (DWORD, "dwDeviceId"),
    (DWORD, "dwSubSysId"),
    (DWORD, "dwRevision"),
    (GUID, "guidDeviceIdentifier"),
    (DWORD, "dwWHQLLevel"),
])
LPDDDEVICEIDENTIFIER2 = Pointer(DDDEVICEIDENTIFIER2)

LPCLIPPERCALLBACK = FunctionPointer("DWORD", "LPCLIPPERCALLBACK", [(LPDIRECTDRAWCLIPPER, "lpDDClipper"), (HWND, "hWnd"), (DWORD, "code"), (LPVOID, "lpContext")])
LPSURFACESTREAMINGCALLBACK = FunctionPointer("DWORD", "LPSURFACESTREAMINGCALLBACK", [DWORD])

DDSURFACEDESC = Struct("DDSURFACEDESC", [
    (DWORD, "dwSize"),
    (DWORD, "dwFlags"),
    (DWORD, "dwHeight"),
    (DWORD, "dwWidth"),
    (LONG, "lPitch"),
    (DWORD, "dwLinearSize"),
    (DWORD, "dwBackBufferCount"),
    (DWORD, "dwMipMapCount"),
    (DWORD, "dwZBufferBitDepth"),
    (DWORD, "dwRefreshRate"),
    (DWORD, "dwAlphaBitDepth"),
    (DWORD, "dwReserved"),
    (LPVOID, "lpSurface"),
    (DDCOLORKEY, "ddckCKDestOverlay"),
    (DDCOLORKEY, "ddckCKDestBlt"),
    (DDCOLORKEY, "ddckCKSrcOverlay"),
    (DDCOLORKEY, "ddckCKSrcBlt"),
    (DDPIXELFORMAT, "ddpfPixelFormat"),
    (DDSCAPS, "ddsCaps"),
])
LPDDSURFACEDESC = Pointer(DDSURFACEDESC)

DDSURFACEDESC2 = Struct("DDSURFACEDESC2", [
    (DWORD, "dwSize"),
    (DWORD, "dwFlags"),
    (DWORD, "dwHeight"),
    (DWORD, "dwWidth"),
    (LONG, "lPitch"),
    (DWORD, "dwLinearSize"),
    (DWORD, "dwBackBufferCount"),
    (DWORD, "dwDepth"),
    (DWORD, "dwMipMapCount"),
    (DWORD, "dwRefreshRate"),
    (DWORD, "dwSrcVBHandle"),
    (DWORD, "dwAlphaBitDepth"),
    (DWORD, "dwReserved"),
    (LPVOID, "lpSurface"),
    (DDCOLORKEY, "ddckCKDestOverlay"),
    (DWORD, "dwEmptyFaceColor"),
    (DDCOLORKEY, "ddckCKDestBlt"),
    (DDCOLORKEY, "ddckCKSrcOverlay"),
    (DDCOLORKEY, "ddckCKSrcBlt"),
    (DDPIXELFORMAT, "ddpfPixelFormat"),
    (DWORD, "dwFVF"),
    (DDSCAPS2, "ddsCaps"),
    (DWORD, "dwTextureStage"),
])
LPDDSURFACEDESC2 = Pointer(DDSURFACEDESC2)

DDSD = Flags(DWORD, [
    "DDSD_CAPS",
    "DDSD_HEIGHT",
    "DDSD_WIDTH",
    "DDSD_PITCH",
    "DDSD_BACKBUFFERCOUNT",
    "DDSD_ZBUFFERBITDEPTH",
    "DDSD_ALPHABITDEPTH",
    "DDSD_LPSURFACE",
    "DDSD_PIXELFORMAT",
    "DDSD_CKDESTOVERLAY",
    "DDSD_CKDESTBLT",
    "DDSD_CKSRCOVERLAY",
    "DDSD_CKSRCBLT",
    "DDSD_MIPMAPCOUNT",
    "DDSD_REFRESHRATE",
    "DDSD_LINEARSIZE",
    "DDSD_TEXTURESTAGE",
    "DDSD_FVF",
    "DDSD_SRCVBHANDLE",
    "DDSD_DEPTH",
    "DDSD_ALL",
])

DDOPTSURFACEDESC = Struct("DDOPTSURFACEDESC", [
    (DWORD, "dwSize"),
    (DWORD, "dwFlags"),
    (DDSCAPS2, "ddSCaps"),
    (DDOSCAPS, "ddOSCaps"),
    (GUID, "guid"),
    (DWORD, "dwCompressionRatio"),
])

DDSD = Flags(DWORD, [
    "DDOSD_GUID",
    "DDOSD_COMPRESSION_RATIO",
    "DDOSD_SCAPS",
    "DDOSD_OSCAPS",
    "DDOSD_ALL",
])

DDOSDCAPS = Flags(DWORD, [
    "DDOSDCAPS_OPTCOMPRESSED",
    "DDOSDCAPS_OPTREORDERED",
    "DDOSDCAPS_MONOLITHICMIPMAP",
    "DDOSDCAPS_VALIDSCAPS",
    "DDOSDCAPS_VALIDOSCAPS",
])

DDCOLORCONTROL = Struct("DDCOLORCONTROL", [
    (DWORD, "dwSize"),
    (DWORD, "dwFlags"),
    (LONG, "lBrightness"),
    (LONG, "lContrast"),
    (LONG, "lHue"),
    (LONG, "lSaturation"),
    (LONG, "lSharpness"),
    (LONG, "lGamma"),
    (LONG, "lColorEnable"),
    (DWORD, "dwReserved1"),
])
LPDDCOLORCONTROL = Pointer(DDCOLORCONTROL)

DDCOLOR = Flags(DWORD, [
    "DDCOLOR_BRIGHTNESS",
    "DDCOLOR_CONTRAST",
    "DDCOLOR_HUE",
    "DDCOLOR_SATURATION",
    "DDCOLOR_SHARPNESS",
    "DDCOLOR_GAMMA",
    "DDCOLOR_COLORENABLE",
])

DDSCAPS = Flags(DWORD, [
    "DDSCAPS_RESERVED1",
    "DDSCAPS_ALPHA",
    "DDSCAPS_BACKBUFFER",
    "DDSCAPS_COMPLEX",
    "DDSCAPS_FLIP",
    "DDSCAPS_FRONTBUFFER",
    "DDSCAPS_OFFSCREENPLAIN",
    "DDSCAPS_OVERLAY",
    "DDSCAPS_PALETTE",
    "DDSCAPS_PRIMARYSURFACE",
    "DDSCAPS_RESERVED3",
    "DDSCAPS_PRIMARYSURFACELEFT",
    "DDSCAPS_SYSTEMMEMORY",
    "DDSCAPS_TEXTURE",
    "DDSCAPS_3DDEVICE",
    "DDSCAPS_VIDEOMEMORY",
    "DDSCAPS_VISIBLE",
    "DDSCAPS_WRITEONLY",
    "DDSCAPS_ZBUFFER",
    "DDSCAPS_OWNDC",
    "DDSCAPS_LIVEVIDEO",
    "DDSCAPS_HWCODEC",
    "DDSCAPS_MODEX",
    "DDSCAPS_MIPMAP",
    "DDSCAPS_RESERVED2",
    "DDSCAPS_ALLOCONLOAD",
    "DDSCAPS_VIDEOPORT",
    "DDSCAPS_LOCALVIDMEM",
    "DDSCAPS_NONLOCALVIDMEM",
    "DDSCAPS_STANDARDVGAMODE",
    "DDSCAPS_OPTIMIZED",
])
DDCAPS2 = Flags(DWORD, [
    "DDSCAPS2_RESERVED4",
    "DDSCAPS2_HARDWAREDEINTERLACE",
    "DDSCAPS2_HINTDYNAMIC",
    "DDSCAPS2_HINTSTATIC",
    "DDSCAPS2_TEXTUREMANAGE",
    "DDSCAPS2_RESERVED1",
    "DDSCAPS2_RESERVED2",
    "DDSCAPS2_OPAQUE",
    "DDSCAPS2_HINTANTIALIASING",
    "DDSCAPS2_CUBEMAP",
    "DDSCAPS2_CUBEMAP_ALLFACES",
    "DDSCAPS2_CUBEMAP_POSITIVEX",
    "DDSCAPS2_CUBEMAP_NEGATIVEX",
    "DDSCAPS2_CUBEMAP_POSITIVEY",
    "DDSCAPS2_CUBEMAP_NEGATIVEY",
    "DDSCAPS2_CUBEMAP_POSITIVEZ",
    "DDSCAPS2_CUBEMAP_NEGATIVEZ",
    "DDSCAPS2_MIPMAPSUBLEVEL",
    "DDSCAPS2_D3DTEXTUREMANAGE",
    "DDSCAPS2_DONOTPERSIST",
    "DDSCAPS2_STEREOSURFACELEFT",
    "DDSCAPS2_VOLUME",
    "DDSCAPS2_NOTUSERLOCKABLE",
    "DDSCAPS2_POINTS",
    "DDSCAPS2_RTPATCHES",
    "DDSCAPS2_NPATCHES",
    "DDSCAPS2_RESERVED3",
    "DDSCAPS2_DISCARDBACKBUFFER",
    "DDSCAPS2_ENABLEALPHACHANNEL",
    "DDSCAPS2_EXTENDEDFORMATPRIMARY",
    "DDSCAPS2_ADDITIONALPRIMARY",
])
DDSCAPS3 = Flags(DWORD, [
    "DDSCAPS3_MULTISAMPLE_MASK",
    "DDSCAPS3_MULTISAMPLE_QUALITY_MASK",
    "DDSCAPS3_MULTISAMPLE_QUALITY_SHIFT",
    "DDSCAPS3_RESERVED1",
    "DDSCAPS3_RESERVED2",
    "DDSCAPS3_LIGHTWEIGHTMIPMAP",
    "DDSCAPS3_AUTOGENMIPMAP",
    "DDSCAPS3_DMAP",
    "DDSCAPS3_CREATESHAREDRESOURCE",
    "DDSCAPS3_READONLYRESOURCE",
    "DDSCAPS3_OPENSHAREDRESOURCE",
])

DDCAPS = Flags(DWORD, [
    "DDCAPS_3D",
    "DDCAPS_ALIGNBOUNDARYDEST",
    "DDCAPS_ALIGNSIZEDEST",
    "DDCAPS_ALIGNBOUNDARYSRC",
    "DDCAPS_ALIGNSIZESRC",
    "DDCAPS_ALIGNSTRIDE",
    "DDCAPS_BLT",
    "DDCAPS_BLTQUEUE",
    "DDCAPS_BLTFOURCC",
    "DDCAPS_BLTSTRETCH",
    "DDCAPS_GDI",
    "DDCAPS_OVERLAY",
    "DDCAPS_OVERLAYCANTCLIP",
    "DDCAPS_OVERLAYFOURCC",
    "DDCAPS_OVERLAYSTRETCH",
    "DDCAPS_PALETTE",
    "DDCAPS_PALETTEVSYNC",
    "DDCAPS_READSCANLINE",
    "DDCAPS_RESERVED1",
    "DDCAPS_VBI",
    "DDCAPS_ZBLTS",
    "DDCAPS_ZOVERLAYS",
    "DDCAPS_COLORKEY",
    "DDCAPS_ALPHA",
    "DDCAPS_COLORKEYHWASSIST",
    "DDCAPS_NOHARDWARE",
    "DDCAPS_BLTCOLORFILL",
    "DDCAPS_BANKSWITCHED",
    "DDCAPS_BLTDEPTHFILL",
    "DDCAPS_CANCLIP",
    "DDCAPS_CANCLIPSTRETCHED",
    "DDCAPS_CANBLTSYSMEM",
])

DDCAPS2 = Flags(DWORD, [
    "DDCAPS2_CERTIFIED",
    "DDCAPS2_NO2DDURING3DSCENE",
    "DDCAPS2_VIDEOPORT",
    "DDCAPS2_AUTOFLIPOVERLAY",
    "DDCAPS2_CANBOBINTERLEAVED",
    "DDCAPS2_CANBOBNONINTERLEAVED",
    "DDCAPS2_COLORCONTROLOVERLAY",
    "DDCAPS2_COLORCONTROLPRIMARY",
    "DDCAPS2_CANDROPZ16BIT",
    "DDCAPS2_NONLOCALVIDMEM",
    "DDCAPS2_NONLOCALVIDMEMCAPS",
    "DDCAPS2_NOPAGELOCKREQUIRED",
    "DDCAPS2_WIDESURFACES",
    "DDCAPS2_CANFLIPODDEVEN",
    "DDCAPS2_CANBOBHARDWARE",
    "DDCAPS2_COPYFOURCC",
    "DDCAPS2_PRIMARYGAMMA",
    "DDCAPS2_CANRENDERWINDOWED",
    "DDCAPS2_CANCALIBRATEGAMMA",
    "DDCAPS2_FLIPINTERVAL",
    "DDCAPS2_FLIPNOVSYNC",
    "DDCAPS2_CANMANAGETEXTURE",
    "DDCAPS2_TEXMANINNONLOCALVIDMEM",
    "DDCAPS2_STEREO",
    "DDCAPS2_SYSTONONLOCAL_AS_SYSTOLOCAL",
    "DDCAPS2_RESERVED1",
    "DDCAPS2_CANMANAGERESOURCE",
    "DDCAPS2_DYNAMICTEXTURES",
    "DDCAPS2_CANAUTOGENMIPMAP",
    "DDCAPS2_CANSHARERESOURCE",
])

DDFXALPHACAPS = Flags(DWORD, [
    "DDFXALPHACAPS_BLTALPHAEDGEBLEND",
    "DDFXALPHACAPS_BLTALPHAPIXELS",
    "DDFXALPHACAPS_BLTALPHAPIXELSNEG",
    "DDFXALPHACAPS_BLTALPHASURFACES",
    "DDFXALPHACAPS_BLTALPHASURFACESNEG",
    "DDFXALPHACAPS_OVERLAYALPHAEDGEBLEND",
    "DDFXALPHACAPS_OVERLAYALPHAPIXELS",
    "DDFXALPHACAPS_OVERLAYALPHAPIXELSNEG",
    "DDFXALPHACAPS_OVERLAYALPHASURFACES",
    "DDFXALPHACAPS_OVERLAYALPHASURFACESNEG",
])

DDFXCAPS = Flags(DWORD, [
    "DDFXCAPS_BLTARITHSTRETCHY",
    "DDFXCAPS_BLTARITHSTRETCHYN",
    "DDFXCAPS_BLTMIRRORLEFTRIGHT",
    "DDFXCAPS_BLTMIRRORUPDOWN",
    "DDFXCAPS_BLTROTATION",
    "DDFXCAPS_BLTROTATION90",
    "DDFXCAPS_BLTSHRINKX",
    "DDFXCAPS_BLTSHRINKXN",
    "DDFXCAPS_BLTSHRINKY",
    "DDFXCAPS_BLTSHRINKYN",
    "DDFXCAPS_BLTSTRETCHX",
    "DDFXCAPS_BLTSTRETCHXN",
    "DDFXCAPS_BLTSTRETCHY",
    "DDFXCAPS_BLTSTRETCHYN",
    "DDFXCAPS_OVERLAYARITHSTRETCHY",
    "DDFXCAPS_OVERLAYARITHSTRETCHYN",
    "DDFXCAPS_OVERLAYSHRINKX",
    "DDFXCAPS_OVERLAYSHRINKXN",
    "DDFXCAPS_OVERLAYSHRINKY",
    "DDFXCAPS_OVERLAYSHRINKYN",
    "DDFXCAPS_OVERLAYSTRETCHX",
    "DDFXCAPS_OVERLAYSTRETCHXN",
    "DDFXCAPS_OVERLAYSTRETCHY",
    "DDFXCAPS_OVERLAYSTRETCHYN",
    "DDFXCAPS_OVERLAYMIRRORLEFTRIGHT",
    "DDFXCAPS_OVERLAYMIRRORUPDOWN",
    "DDFXCAPS_OVERLAYDEINTERLACE",
    "DDFXCAPS_BLTALPHA",
    "DDFXCAPS_BLTFILTER",
    "DDFXCAPS_OVERLAYALPHA",
    "DDFXCAPS_OVERLAYFILTER",
])

DDSVCAPS = Flags(DWORD, [
    "DDSVCAPS_RESERVED1",
    "DDSVCAPS_RESERVED2",
    "DDSVCAPS_RESERVED3",
    "DDSVCAPS_RESERVED4",
    "DDSVCAPS_STEREOSEQUENTIAL",
])

DDPCAPS = Flags(DWORD, [
    "DDPCAPS_4BIT",
    "DDPCAPS_8BITENTRIES",
    "DDPCAPS_8BIT",
    "DDPCAPS_INITIALIZE",
    "DDPCAPS_PRIMARYSURFACE",
    "DDPCAPS_PRIMARYSURFACELEFT",
    "DDPCAPS_ALLOW256",
    "DDPCAPS_VSYNC",
    "DDPCAPS_1BIT",
    "DDPCAPS_2BIT",
    "DDPCAPS_ALPHA",
])

DDSPD = Flags(DWORD, [
    "DDSPD_IUNKNOWNPOINTER",
    "DDSPD_VOLATILE",
])

DDBD = Flags(DWORD, [
    "DDBD_1",
    "DDBD_2",
    "DDBD_4",
    "DDBD_8",
    "DDBD_16",
    "DDBD_24",
    "DDBD_32",
])

DDCKEY = Flags(DWORD, [
    "DDCKEY_COLORSPACE",
    "DDCKEY_DESTBLT",
    "DDCKEY_DESTOVERLAY",
    "DDCKEY_SRCBLT",
    "DDCKEY_SRCOVERLAY",
])

DDCKEYCAPS = Flags(DWORD, [
    "DDCKEYCAPS_DESTBLT",
    "DDCKEYCAPS_DESTBLTCLRSPACE",
    "DDCKEYCAPS_DESTBLTCLRSPACEYUV",
    "DDCKEYCAPS_DESTBLTYUV",
    "DDCKEYCAPS_DESTOVERLAY",
    "DDCKEYCAPS_DESTOVERLAYCLRSPACE",
    "DDCKEYCAPS_DESTOVERLAYCLRSPACEYUV",
    "DDCKEYCAPS_DESTOVERLAYONEACTIVE",
    "DDCKEYCAPS_DESTOVERLAYYUV",
    "DDCKEYCAPS_SRCBLT",
    "DDCKEYCAPS_SRCBLTCLRSPACE",
    "DDCKEYCAPS_SRCBLTCLRSPACEYUV",
    "DDCKEYCAPS_SRCBLTYUV",
    "DDCKEYCAPS_SRCOVERLAY",
    "DDCKEYCAPS_SRCOVERLAYCLRSPACE",
    "DDCKEYCAPS_SRCOVERLAYCLRSPACEYUV",
    "DDCKEYCAPS_SRCOVERLAYONEACTIVE",
    "DDCKEYCAPS_SRCOVERLAYYUV",
    "DDCKEYCAPS_NOCOSTOVERLAY",
])

DDPF = Flags(DWORD, [
    "DDPF_ALPHAPIXELS",
    "DDPF_ALPHA",
    "DDPF_FOURCC",
    "DDPF_PALETTEINDEXED4",
    "DDPF_PALETTEINDEXEDTO8",
    "DDPF_PALETTEINDEXED8",
    "DDPF_RGB",
    "DDPF_COMPRESSED",
    "DDPF_RGBTOYUV",
    "DDPF_YUV",
    "DDPF_ZBUFFER",
    "DDPF_PALETTEINDEXED1",
    "DDPF_PALETTEINDEXED2",
    "DDPF_ZPIXELS",
    "DDPF_STENCILBUFFER",
    "DDPF_ALPHAPREMULT",
    "DDPF_LUMINANCE",
    "DDPF_BUMPLUMINANCE",
    "DDPF_BUMPDUDV",
])

DDENUMSURFACES = Flags(DWORD, [
    "DDENUMSURFACES_ALL",
    "DDENUMSURFACES_MATCH",
    "DDENUMSURFACES_NOMATCH",
    "DDENUMSURFACES_CANBECREATED",
    "DDENUMSURFACES_DOESEXIST",
])

DDSDM = Flags(DWORD, [
    "DDSDM_STANDARDVGAMODE",
])

DDEDM = Flags(DWORD, [
    "DDEDM_REFRESHRATES",
    "DDEDM_STANDARDVGAMODES",
])

DDSCL = Flags(DWORD, [
    "DDSCL_FULLSCREEN",
    "DDSCL_ALLOWREBOOT",
    "DDSCL_NOWINDOWCHANGES",
    "DDSCL_NORMAL",
    "DDSCL_EXCLUSIVE",
    "DDSCL_ALLOWMODEX",
    "DDSCL_SETFOCUSWINDOW",
    "DDSCL_SETDEVICEWINDOW",
    "DDSCL_CREATEDEVICEWINDOW",
    "DDSCL_MULTITHREADED",
    "DDSCL_FPUSETUP",
    "DDSCL_FPUPRESERVE",
])

DDBLT = Flags(DWORD, [
    "DDBLT_ALPHADEST",
    "DDBLT_ALPHADESTCONSTOVERRIDE",
    "DDBLT_ALPHADESTNEG",
    "DDBLT_ALPHADESTSURFACEOVERRIDE",
    "DDBLT_ALPHAEDGEBLEND",
    "DDBLT_ALPHASRC",
    "DDBLT_ALPHASRCCONSTOVERRIDE",
    "DDBLT_ALPHASRCNEG",
    "DDBLT_ALPHASRCSURFACEOVERRIDE",
    "DDBLT_ASYNC",
    "DDBLT_COLORFILL",
    "DDBLT_DDFX",
    "DDBLT_DDROPS",
    "DDBLT_KEYDEST",
    "DDBLT_KEYDESTOVERRIDE",
    "DDBLT_KEYSRC",
    "DDBLT_KEYSRCOVERRIDE",
    "DDBLT_ROP",
    "DDBLT_ROTATIONANGLE",
    "DDBLT_ZBUFFER",
    "DDBLT_ZBUFFERDESTCONSTOVERRIDE",
    "DDBLT_ZBUFFERDESTOVERRIDE",
    "DDBLT_ZBUFFERSRCCONSTOVERRIDE",
    "DDBLT_ZBUFFERSRCOVERRIDE",
    "DDBLT_WAIT",
    "DDBLT_DEPTHFILL",
    "DDBLT_DONOTWAIT",
    "DDBLT_PRESENTATION",
    "DDBLT_LAST_PRESENTATION",
    "DDBLT_EXTENDED_FLAGS",
    "DDBLT_EXTENDED_LINEAR_CONTENT",
])

DDBLTFAST = Flags(DWORD, [
    "DDBLTFAST_NOCOLORKEY",
    "DDBLTFAST_SRCCOLORKEY",
    "DDBLTFAST_DESTCOLORKEY",
    "DDBLTFAST_WAIT",
    "DDBLTFAST_DONOTWAIT",
])

DDFLIP = Flags(DWORD, [
    "DDFLIP_WAIT",
    "DDFLIP_EVEN",
    "DDFLIP_ODD",
    "DDFLIP_NOVSYNC",
    "DDFLIP_INTERVAL2",
    "DDFLIP_INTERVAL3",
    "DDFLIP_INTERVAL4",
    "DDFLIP_STEREO",
    "DDFLIP_DONOTWAIT",
])

DDOVER = Flags(DWORD, [
    "DDOVER_ALPHADEST",
    "DDOVER_ALPHADESTCONSTOVERRIDE",
    "DDOVER_ALPHADESTNEG",
    "DDOVER_ALPHADESTSURFACEOVERRIDE",
    "DDOVER_ALPHAEDGEBLEND",
    "DDOVER_ALPHASRC",
    "DDOVER_ALPHASRCCONSTOVERRIDE",
    "DDOVER_ALPHASRCNEG",
    "DDOVER_ALPHASRCSURFACEOVERRIDE",
    "DDOVER_HIDE",
    "DDOVER_KEYDEST",
    "DDOVER_KEYDESTOVERRIDE",
    "DDOVER_KEYSRC",
    "DDOVER_KEYSRCOVERRIDE",
    "DDOVER_SHOW",
    "DDOVER_ADDDIRTYRECT",
    "DDOVER_REFRESHDIRTYRECTS",
    "DDOVER_REFRESHALL",
    "DDOVER_DDFX",
    "DDOVER_AUTOFLIP",
    "DDOVER_BOB",
    "DDOVER_OVERRIDEBOBWEAVE",
    "DDOVER_INTERLEAVED",
    "DDOVER_BOBHARDWARE",
    "DDOVER_ARGBSCALEFACTORS",
    "DDOVER_DEGRADEARGBSCALING",
])

DDSETSURFACEDESC = Flags(DWORD, [
    #"DDSETSURFACEDESC_RECREATEDC",
    #"DDSETSURFACEDESC_PRESERVEDC",
])

DDLOCK = Flags(DWORD, [
    "DDLOCK_SURFACEMEMORYPTR",
    "DDLOCK_WAIT",
    "DDLOCK_EVENT",
    "DDLOCK_READONLY",
    "DDLOCK_WRITEONLY",
    "DDLOCK_NOSYSLOCK",
    "DDLOCK_NOOVERWRITE",
    "DDLOCK_DISCARDCONTENTS",
    "DDLOCK_OKTOSWAP",
    "DDLOCK_DONOTWAIT",
    "DDLOCK_HASVOLUMETEXTUREBOXRECT",
    "DDLOCK_NODIRTYUPDATE",
])

DDBLTFX = Flags(DWORD, [
    "DDBLTFX_ARITHSTRETCHY",
    "DDBLTFX_MIRRORLEFTRIGHT",
    "DDBLTFX_MIRRORUPDOWN",
    "DDBLTFX_NOTEARING",
    "DDBLTFX_ROTATE180",
    "DDBLTFX_ROTATE270",
    "DDBLTFX_ROTATE90",
    "DDBLTFX_ZBUFFERRANGE",
    "DDBLTFX_ZBUFFERBASEDEST",
])

DDOVERFX = Flags(DWORD, [
    "DDOVERFX_ARITHSTRETCHY",
    "DDOVERFX_MIRRORLEFTRIGHT",
    "DDOVERFX_MIRRORUPDOWN",
    "DDOVERFX_DEINTERLACE",
])

DDWAITVB = Flags(DWORD, [
    "DDWAITVB_BLOCKBEGIN",
    "DDWAITVB_BLOCKBEGINEVENT",
    "DDWAITVB_BLOCKEND",
])

DDGFS = Flags(DWORD, [
    "DDGFS_CANFLIP",
    "DDGFS_ISFLIPDONE",
])

DDGBS = Flags(DWORD, [
    "DDGBS_CANBLT",
    "DDGBS_ISBLTDONE",
])

DDENUMOVERLAYZ = Flags(DWORD, [
    "DDENUMOVERLAYZ_BACKTOFRONT",
    "DDENUMOVERLAYZ_FRONTTOBACK",
])

DDOVERZ = Flags(DWORD, [
    "DDOVERZ_SENDTOFRONT",
    "DDOVERZ_SENDTOBACK",
    "DDOVERZ_MOVEFORWARD",
    "DDOVERZ_MOVEBACKWARD",
    "DDOVERZ_INSERTINFRONTOF",
    "DDOVERZ_INSERTINBACKOF",
])

DDSGR = Flags(DWORD, [
    "DDSGR_CALIBRATE",
])

DDSMT = Flags(DWORD, [
    "DDSMT_ISTESTREQUIRED",
])

DDEM = Flags(DWORD, [
    "DDEM_MODEPASSED",
    "DDEM_MODEFAILED",
])

HRESULT = Enum("HRESULT", [
    "DD_OK",
    "DD_FALSE",
    "DDERR_ALREADYINITIALIZED",
    "DDERR_CANNOTATTACHSURFACE",
    "DDERR_CANNOTDETACHSURFACE",
    "DDERR_CURRENTLYNOTAVAIL",
    "DDERR_EXCEPTION",
    "DDERR_GENERIC",
    "DDERR_HEIGHTALIGN",
    "DDERR_INCOMPATIBLEPRIMARY",
    "DDERR_INVALIDCAPS",
    "DDERR_INVALIDCLIPLIST",
    "DDERR_INVALIDMODE",
    "DDERR_INVALIDOBJECT",
    "DDERR_INVALIDPARAMS",
    "DDERR_INVALIDPIXELFORMAT",
    "DDERR_INVALIDRECT",
    "DDERR_LOCKEDSURFACES",
    "DDERR_NO3D",
    "DDERR_NOALPHAHW",
    "DDERR_NOSTEREOHARDWARE",
    "DDERR_NOSURFACELEFT",
    "DDERR_NOCLIPLIST",
    "DDERR_NOCOLORCONVHW",
    "DDERR_NOCOOPERATIVELEVELSET",
    "DDERR_NOCOLORKEY",
    "DDERR_NOCOLORKEYHW",
    "DDERR_NODIRECTDRAWSUPPORT",
    "DDERR_NOEXCLUSIVEMODE",
    "DDERR_NOFLIPHW",
    "DDERR_NOGDI",
    "DDERR_NOMIRRORHW",
    "DDERR_NOTFOUND",
    "DDERR_NOOVERLAYHW",
    "DDERR_OVERLAPPINGRECTS",
    "DDERR_NORASTEROPHW",
    "DDERR_NOROTATIONHW",
    "DDERR_NOSTRETCHHW",
    "DDERR_NOT4BITCOLOR",
    "DDERR_NOT4BITCOLORINDEX",
    "DDERR_NOT8BITCOLOR",
    "DDERR_NOTEXTUREHW",
    "DDERR_NOVSYNCHW",
    "DDERR_NOZBUFFERHW",
    "DDERR_NOZOVERLAYHW",
    "DDERR_OUTOFCAPS",
    "DDERR_OUTOFMEMORY",
    "DDERR_OUTOFVIDEOMEMORY",
    "DDERR_OVERLAYCANTCLIP",
    "DDERR_OVERLAYCOLORKEYONLYONEACTIVE",
    "DDERR_PALETTEBUSY",
    "DDERR_COLORKEYNOTSET",
    "DDERR_SURFACEALREADYATTACHED",
    "DDERR_SURFACEALREADYDEPENDENT",
    "DDERR_SURFACEBUSY",
    "DDERR_CANTLOCKSURFACE",
    "DDERR_SURFACEISOBSCURED",
    "DDERR_SURFACELOST",
    "DDERR_SURFACENOTATTACHED",
    "DDERR_TOOBIGHEIGHT",
    "DDERR_TOOBIGSIZE",
    "DDERR_TOOBIGWIDTH",
    "DDERR_UNSUPPORTED",
    "DDERR_UNSUPPORTEDFORMAT",
    "DDERR_UNSUPPORTEDMASK",
    "DDERR_INVALIDSTREAM",
    "DDERR_VERTICALBLANKINPROGRESS",
    "DDERR_WASSTILLDRAWING",
    "DDERR_DDSCAPSCOMPLEXREQUIRED",
    "DDERR_XALIGN",
    "DDERR_INVALIDDIRECTDRAWGUID",
    "DDERR_DIRECTDRAWALREADYCREATED",
    "DDERR_NODIRECTDRAWHW",
    "DDERR_PRIMARYSURFACEALREADYEXISTS",
    "DDERR_NOEMULATION",
    "DDERR_REGIONTOOSMALL",
    "DDERR_CLIPPERISUSINGHWND",
    "DDERR_NOCLIPPERATTACHED",
    "DDERR_NOHWND",
    "DDERR_HWNDSUBCLASSED",
    "DDERR_HWNDALREADYSET",
    "DDERR_NOPALETTEATTACHED",
    "DDERR_NOPALETTEHW",
    "DDERR_BLTFASTCANTCLIP",
    "DDERR_NOBLTHW",
    "DDERR_NODDROPSHW",
    "DDERR_OVERLAYNOTVISIBLE",
    "DDERR_NOOVERLAYDEST",
    "DDERR_INVALIDPOSITION",
    "DDERR_NOTAOVERLAYSURFACE",
    "DDERR_EXCLUSIVEMODEALREADYSET",
    "DDERR_NOTFLIPPABLE",
    "DDERR_CANTDUPLICATE",
    "DDERR_NOTLOCKED",
    "DDERR_CANTCREATEDC",
    "DDERR_NODC",
    "DDERR_WRONGMODE",
    "DDERR_IMPLICITLYCREATED",
    "DDERR_NOTPALETTIZED",
    "DDERR_UNSUPPORTEDMODE",
    "DDERR_NOMIPMAPHW",
    "DDERR_INVALIDSURFACETYPE",
    "DDERR_NOOPTIMIZEHW",
    "DDERR_NOTLOADED",
    "DDERR_NOFOCUSWINDOW",
    "DDERR_NOTONMIPMAPSUBLEVEL",
    "DDERR_DCALREADYCREATED",
    "DDERR_NONONLOCALVIDMEM",
    "DDERR_CANTPAGELOCK",
    "DDERR_CANTPAGEUNLOCK",
    "DDERR_NOTPAGELOCKED",
    "DDERR_MOREDATA",
    "DDERR_EXPIRED",
    "DDERR_TESTFINISHED",
    "DDERR_NEWMODE",
    "DDERR_D3DNOTINITIALIZED",
    "DDERR_VIDEONOTACTIVE",
    "DDERR_NOMONITORINFORMATION",
    "DDERR_NODRIVERSUPPORT",
    "DDERR_DEVICEDOESNTOWNSURFACE",
    "DDERR_NOTINITIALIZED",
])

LPDDENUMMODESCALLBACK     = FunctionPointer(HRESULT, "LPDDENUMMODESCALLBACK", [LPDDSURFACEDESC, LPVOID])
LPDDENUMMODESCALLBACK2    = FunctionPointer(HRESULT, "LPDDENUMMODESCALLBACK2", [LPDDSURFACEDESC2, LPVOID])
LPDDENUMSURFACESCALLBACK  = FunctionPointer(HRESULT, "LPDDENUMSURFACESCALLBACK", [LPDIRECTDRAWSURFACE, LPDDSURFACEDESC, LPVOID])
LPDDENUMSURFACESCALLBACK2 = FunctionPointer(HRESULT, "LPDDENUMSURFACESCALLBACK2", [LPDIRECTDRAWSURFACE4, LPDDSURFACEDESC2, LPVOID])
LPDDENUMSURFACESCALLBACK7 = FunctionPointer(HRESULT, "LPDDENUMSURFACESCALLBACK7", [LPDIRECTDRAWSURFACE7, LPDDSURFACEDESC2, LPVOID])

IDirectDraw.methods += [
    Method(HRESULT, "Compact", []),
    Method(HRESULT, "CreateClipper", [DWORD, Pointer(LPDIRECTDRAWCLIPPER) , Pointer(IUnknown) ]),
    Method(HRESULT, "CreatePalette", [DWORD, LPPALETTEENTRY, Pointer(LPDIRECTDRAWPALETTE) , Pointer(IUnknown) ]),
    Method(HRESULT, "CreateSurface", [LPDDSURFACEDESC, Pointer(LPDIRECTDRAWSURFACE) , Pointer(IUnknown) ]),
    Method(HRESULT, "DuplicateSurface", [ LPDIRECTDRAWSURFACE, Pointer(LPDIRECTDRAWSURFACE) ]),
    Method(HRESULT, "EnumDisplayModes", [ DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK ]),
    Method(HRESULT, "EnumSurfaces", [DWORD, LPDDSURFACEDESC, LPVOID,LPDDENUMSURFACESCALLBACK ]),
    Method(HRESULT, "FlipToGDISurface", []),
    Method(HRESULT, "GetCaps", [ LPDDCAPS, LPDDCAPS]),
    Method(HRESULT, "GetDisplayMode", [ LPDDSURFACEDESC]),
    Method(HRESULT, "GetFourCCCodes", [LPDWORD, LPDWORD ]),
    Method(HRESULT, "GetGDISurface", [Pointer(LPDIRECTDRAWSURFACE) ]),
    Method(HRESULT, "GetMonitorFrequency", [LPDWORD]),
    Method(HRESULT, "GetScanLine", [LPDWORD]),
    Method(HRESULT, "GetVerticalBlankStatus", [LPBOOL ]),
    Method(HRESULT, "Initialize", [Pointer(GUID) ]),
    Method(HRESULT, "RestoreDisplayMode", []),
    Method(HRESULT, "SetCooperativeLevel", [HWND, DWORD]),
    Method(HRESULT, "SetDisplayMode", [DWORD, DWORD,DWORD]),
    Method(HRESULT, "WaitForVerticalBlank", [DWORD, HANDLE ]),
]

IDirectDraw2.methods += [
    Method(HRESULT, "Compact", []),
    Method(HRESULT, "CreateClipper", [DWORD, Pointer(LPDIRECTDRAWCLIPPER) , Pointer(IUnknown) ]),
    Method(HRESULT, "CreatePalette", [DWORD, LPPALETTEENTRY, Pointer(LPDIRECTDRAWPALETTE) , Pointer(IUnknown) ]),
    Method(HRESULT, "CreateSurface", [LPDDSURFACEDESC, Pointer(LPDIRECTDRAWSURFACE) , Pointer(IUnknown) ]),
    Method(HRESULT, "DuplicateSurface", [ LPDIRECTDRAWSURFACE, Pointer(LPDIRECTDRAWSURFACE) ]),
    Method(HRESULT, "EnumDisplayModes", [ DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK ]),
    Method(HRESULT, "EnumSurfaces", [DWORD, LPDDSURFACEDESC, LPVOID,LPDDENUMSURFACESCALLBACK ]),
    Method(HRESULT, "FlipToGDISurface", []),
    Method(HRESULT, "GetCaps", [ LPDDCAPS, LPDDCAPS]),
    Method(HRESULT, "GetDisplayMode", [ LPDDSURFACEDESC]),
    Method(HRESULT, "GetFourCCCodes", [LPDWORD, LPDWORD ]),
    Method(HRESULT, "GetGDISurface", [Pointer(LPDIRECTDRAWSURFACE) ]),
    Method(HRESULT, "GetMonitorFrequency", [LPDWORD]),
    Method(HRESULT, "GetScanLine", [LPDWORD]),
    Method(HRESULT, "GetVerticalBlankStatus", [LPBOOL ]),
    Method(HRESULT, "Initialize", [Pointer(GUID) ]),
    Method(HRESULT, "RestoreDisplayMode", []),
    Method(HRESULT, "SetCooperativeLevel", [HWND, DWORD]),
    Method(HRESULT, "SetDisplayMode", [DWORD, DWORD,DWORD, DWORD, DWORD]),
    Method(HRESULT, "WaitForVerticalBlank", [DWORD, HANDLE ]),
    Method(HRESULT, "GetAvailableVidMem", [LPDDSCAPS, LPDWORD, LPDWORD]),
]

IDirectDraw4.methods += [
    Method(HRESULT, "Compact", []),
    Method(HRESULT, "CreateClipper", [DWORD, Pointer(LPDIRECTDRAWCLIPPER) , Pointer(IUnknown) ]),
    Method(HRESULT, "CreatePalette", [DWORD, LPPALETTEENTRY, Pointer(LPDIRECTDRAWPALETTE) , Pointer(IUnknown) ]),
    Method(HRESULT, "CreateSurface", [LPDDSURFACEDESC2, Pointer(LPDIRECTDRAWSURFACE4) , Pointer(IUnknown) ]),
    Method(HRESULT, "DuplicateSurface", [ LPDIRECTDRAWSURFACE4, Pointer(LPDIRECTDRAWSURFACE4) ]),
    Method(HRESULT, "EnumDisplayModes", [ DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMMODESCALLBACK2 ]),
    Method(HRESULT, "EnumSurfaces", [DWORD, LPDDSURFACEDESC2, LPVOID,LPDDENUMSURFACESCALLBACK2 ]),
    Method(HRESULT, "FlipToGDISurface", []),
    Method(HRESULT, "GetCaps", [ LPDDCAPS, LPDDCAPS]),
    Method(HRESULT, "GetDisplayMode", [ LPDDSURFACEDESC2]),
    Method(HRESULT, "GetFourCCCodes", [LPDWORD, LPDWORD ]),
    Method(HRESULT, "GetGDISurface", [Pointer(LPDIRECTDRAWSURFACE4) ]),
    Method(HRESULT, "GetMonitorFrequency", [LPDWORD]),
    Method(HRESULT, "GetScanLine", [LPDWORD]),
    Method(HRESULT, "GetVerticalBlankStatus", [LPBOOL ]),
    Method(HRESULT, "Initialize", [Pointer(GUID) ]),
    Method(HRESULT, "RestoreDisplayMode", []),
    Method(HRESULT, "SetCooperativeLevel", [HWND, DWORD]),
    Method(HRESULT, "SetDisplayMode", [DWORD, DWORD,DWORD, DWORD, DWORD]),
    Method(HRESULT, "WaitForVerticalBlank", [DWORD, HANDLE ]),
    Method(HRESULT, "GetAvailableVidMem", [LPDDSCAPS2, LPDWORD, LPDWORD]),
    Method(HRESULT, "GetSurfaceFromDC", [HDC, Pointer(LPDIRECTDRAWSURFACE4)]),
    Method(HRESULT, "RestoreAllSurfaces", []),
    Method(HRESULT, "TestCooperativeLevel", []),
    Method(HRESULT, "GetDeviceIdentifier", [LPDDDEVICEIDENTIFIER, DWORD ]),
]

IDirectDraw7.methods += [
    Method(HRESULT, "Compact", []),
    Method(HRESULT, "CreateClipper", [DWORD, Pointer(LPDIRECTDRAWCLIPPER) , Pointer(IUnknown) ]),
    Method(HRESULT, "CreatePalette", [DWORD, LPPALETTEENTRY, Pointer(LPDIRECTDRAWPALETTE) , Pointer(IUnknown) ]),
    Method(HRESULT, "CreateSurface", [LPDDSURFACEDESC2, Pointer(LPDIRECTDRAWSURFACE7) , Pointer(IUnknown) ]),
    Method(HRESULT, "DuplicateSurface", [ LPDIRECTDRAWSURFACE7, Pointer(LPDIRECTDRAWSURFACE7) ]),
    Method(HRESULT, "EnumDisplayModes", [ DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMMODESCALLBACK2 ]),
    Method(HRESULT, "EnumSurfaces", [DWORD, LPDDSURFACEDESC2, LPVOID,LPDDENUMSURFACESCALLBACK7 ]),
    Method(HRESULT, "FlipToGDISurface", []),
    Method(HRESULT, "GetCaps", [ LPDDCAPS, LPDDCAPS]),
    Method(HRESULT, "GetDisplayMode", [ LPDDSURFACEDESC2]),
    Method(HRESULT, "GetFourCCCodes", [LPDWORD, LPDWORD ]),
    Method(HRESULT, "GetGDISurface", [Pointer(LPDIRECTDRAWSURFACE7) ]),
    Method(HRESULT, "GetMonitorFrequency", [LPDWORD]),
    Method(HRESULT, "GetScanLine", [LPDWORD]),
    Method(HRESULT, "GetVerticalBlankStatus", [LPBOOL ]),
    Method(HRESULT, "Initialize", [Pointer(GUID) ]),
    Method(HRESULT, "RestoreDisplayMode", []),
    Method(HRESULT, "SetCooperativeLevel", [HWND, DWORD]),
    Method(HRESULT, "SetDisplayMode", [DWORD, DWORD,DWORD, DWORD, DWORD]),
    Method(HRESULT, "WaitForVerticalBlank", [DWORD, HANDLE ]),
    Method(HRESULT, "GetAvailableVidMem", [LPDDSCAPS2, LPDWORD, LPDWORD]),
    Method(HRESULT, "GetSurfaceFromDC", [HDC, Pointer(LPDIRECTDRAWSURFACE7)]),
    Method(HRESULT, "RestoreAllSurfaces", []),
    Method(HRESULT, "TestCooperativeLevel", []),
    Method(HRESULT, "GetDeviceIdentifier", [LPDDDEVICEIDENTIFIER2, DWORD ]),
    Method(HRESULT, "StartModeTest", [LPSIZE, DWORD, DWORD ]),
    Method(HRESULT, "EvaluateMode", [DWORD, Pointer(DWORD) ]),
]

IDirectDrawPalette.methods += [
    Method(HRESULT, "GetCaps", [LPDWORD]),
    Method(HRESULT, "GetEntries", [DWORD,DWORD,DWORD,LPPALETTEENTRY]),
    Method(HRESULT, "Initialize", [LPDIRECTDRAW, DWORD, LPPALETTEENTRY]),
    Method(HRESULT, "SetEntries", [DWORD,DWORD,DWORD,LPPALETTEENTRY]),
]

IDirectDrawClipper.methods += [
    Method(HRESULT, "GetClipList", [LPRECT, LPRGNDATA, LPDWORD]),
    Method(HRESULT, "GetHWnd", [Pointer(HWND) ]),
    Method(HRESULT, "Initialize", [LPDIRECTDRAW, DWORD]),
    Method(HRESULT, "IsClipListChanged", [Pointer(BOOL) ]),
    Method(HRESULT, "SetClipList", [LPRGNDATA,DWORD]),
    Method(HRESULT, "SetHWnd", [DWORD, HWND ]),
]


IDirectDrawSurface.methods += [
    Method(HRESULT, "AddAttachedSurface", [LPDIRECTDRAWSURFACE]),
    Method(HRESULT, "AddOverlayDirtyRect", [LPRECT]),
    Method(HRESULT, "Blt", [LPRECT,LPDIRECTDRAWSURFACE, LPRECT,DWORD, LPDDBLTFX]),
    Method(HRESULT, "BltBatch", [LPDDBLTBATCH, DWORD, DWORD ]),
    Method(HRESULT, "BltFast", [DWORD,DWORD,LPDIRECTDRAWSURFACE, LPRECT,DWORD]),
    Method(HRESULT, "DeleteAttachedSurface", [DWORD,LPDIRECTDRAWSURFACE]),
    Method(HRESULT, "EnumAttachedSurfaces", [LPVOID,LPDDENUMSURFACESCALLBACK]),
    Method(HRESULT, "EnumOverlayZOrders", [DWORD,LPVOID,LPDDENUMSURFACESCALLBACK]),
    Method(HRESULT, "Flip", [LPDIRECTDRAWSURFACE, DWORD]),
    Method(HRESULT, "GetAttachedSurface", [LPDDSCAPS, Pointer(LPDIRECTDRAWSURFACE) ]),
    Method(HRESULT, "GetBltStatus", [DWORD]),
    Method(HRESULT, "GetCaps", [LPDDSCAPS]),
    Method(HRESULT, "GetClipper", [Pointer(LPDIRECTDRAWCLIPPER) ]),
    Method(HRESULT, "GetColorKey", [DWORD, LPDDCOLORKEY]),
    Method(HRESULT, "GetDC", [Pointer(HDC) ]),
    Method(HRESULT, "GetFlipStatus", [DWORD]),
    Method(HRESULT, "GetOverlayPosition", [LPLONG, LPLONG ]),
    Method(HRESULT, "GetPalette", [Pointer(LPDIRECTDRAWPALETTE) ]),
    Method(HRESULT, "GetPixelFormat", [LPDDPIXELFORMAT]),
    Method(HRESULT, "GetSurfaceDesc", [LPDDSURFACEDESC]),
    Method(HRESULT, "Initialize", [LPDIRECTDRAW, LPDDSURFACEDESC]),
    Method(HRESULT, "IsLost", []),
    Method(HRESULT, "Lock", [LPRECT,LPDDSURFACEDESC,DWORD,HANDLE]),
    Method(HRESULT, "ReleaseDC", [HDC]),
    Method(HRESULT, "Restore", []),
    Method(HRESULT, "SetClipper", [LPDIRECTDRAWCLIPPER]),
    Method(HRESULT, "SetColorKey", [DWORD, LPDDCOLORKEY]),
    Method(HRESULT, "SetOverlayPosition", [LONG, LONG ]),
    Method(HRESULT, "SetPalette", [LPDIRECTDRAWPALETTE]),
    Method(HRESULT, "Unlock", [LPVOID]),
    Method(HRESULT, "UpdateOverlay", [LPRECT, LPDIRECTDRAWSURFACE,LPRECT,DWORD, LPDDOVERLAYFX]),
    Method(HRESULT, "UpdateOverlayDisplay", [DWORD]),
    Method(HRESULT, "UpdateOverlayZOrder", [DWORD, LPDIRECTDRAWSURFACE]),
]

IDirectDrawSurface2.methods += [
    Method(HRESULT, "AddAttachedSurface", [LPDIRECTDRAWSURFACE2]),
    Method(HRESULT, "AddOverlayDirtyRect", [LPRECT]),
    Method(HRESULT, "Blt", [LPRECT,LPDIRECTDRAWSURFACE2, LPRECT,DWORD, LPDDBLTFX]),
    Method(HRESULT, "BltBatch", [LPDDBLTBATCH, DWORD, DWORD ]),
    Method(HRESULT, "BltFast", [DWORD,DWORD,LPDIRECTDRAWSURFACE2, LPRECT,DWORD]),
    Method(HRESULT, "DeleteAttachedSurface", [DWORD,LPDIRECTDRAWSURFACE2]),
    Method(HRESULT, "EnumAttachedSurfaces", [LPVOID,LPDDENUMSURFACESCALLBACK]),
    Method(HRESULT, "EnumOverlayZOrders", [DWORD,LPVOID,LPDDENUMSURFACESCALLBACK]),
    Method(HRESULT, "Flip", [LPDIRECTDRAWSURFACE2, DWORD]),
    Method(HRESULT, "GetAttachedSurface", [LPDDSCAPS, Pointer(LPDIRECTDRAWSURFACE2) ]),
    Method(HRESULT, "GetBltStatus", [DWORD]),
    Method(HRESULT, "GetCaps", [LPDDSCAPS]),
    Method(HRESULT, "GetClipper", [Pointer(LPDIRECTDRAWCLIPPER) ]),
    Method(HRESULT, "GetColorKey", [DWORD, LPDDCOLORKEY]),
    Method(HRESULT, "GetDC", [Pointer(HDC) ]),
    Method(HRESULT, "GetFlipStatus", [DWORD]),
    Method(HRESULT, "GetOverlayPosition", [LPLONG, LPLONG ]),
    Method(HRESULT, "GetPalette", [Pointer(LPDIRECTDRAWPALETTE) ]),
    Method(HRESULT, "GetPixelFormat", [LPDDPIXELFORMAT]),
    Method(HRESULT, "GetSurfaceDesc", [LPDDSURFACEDESC]),
    Method(HRESULT, "Initialize", [LPDIRECTDRAW, LPDDSURFACEDESC]),
    Method(HRESULT, "IsLost", []),
    Method(HRESULT, "Lock", [LPRECT,LPDDSURFACEDESC,DWORD,HANDLE]),
    Method(HRESULT, "ReleaseDC", [HDC]),
    Method(HRESULT, "Restore", []),
    Method(HRESULT, "SetClipper", [LPDIRECTDRAWCLIPPER]),
    Method(HRESULT, "SetColorKey", [DWORD, LPDDCOLORKEY]),
    Method(HRESULT, "SetOverlayPosition", [LONG, LONG ]),
    Method(HRESULT, "SetPalette", [LPDIRECTDRAWPALETTE]),
    Method(HRESULT, "Unlock", [LPVOID]),
    Method(HRESULT, "UpdateOverlay", [LPRECT, LPDIRECTDRAWSURFACE2,LPRECT,DWORD, LPDDOVERLAYFX]),
    Method(HRESULT, "UpdateOverlayDisplay", [DWORD]),
    Method(HRESULT, "UpdateOverlayZOrder", [DWORD, LPDIRECTDRAWSURFACE2]),
    Method(HRESULT, "GetDDInterface", [Pointer(LPVOID) ]),
    Method(HRESULT, "PageLock", [DWORD]),
    Method(HRESULT, "PageUnlock", [DWORD]),
]

IDirectDrawSurface3.methods += [
    Method(HRESULT, "AddAttachedSurface", [LPDIRECTDRAWSURFACE3]),
    Method(HRESULT, "AddOverlayDirtyRect", [LPRECT]),
    Method(HRESULT, "Blt", [LPRECT,LPDIRECTDRAWSURFACE3, LPRECT,DWORD, LPDDBLTFX]),
    Method(HRESULT, "BltBatch", [LPDDBLTBATCH, DWORD, DWORD ]),
    Method(HRESULT, "BltFast", [DWORD,DWORD,LPDIRECTDRAWSURFACE3, LPRECT,DWORD]),
    Method(HRESULT, "DeleteAttachedSurface", [DWORD,LPDIRECTDRAWSURFACE3]),
    Method(HRESULT, "EnumAttachedSurfaces", [LPVOID,LPDDENUMSURFACESCALLBACK]),
    Method(HRESULT, "EnumOverlayZOrders", [DWORD,LPVOID,LPDDENUMSURFACESCALLBACK]),
    Method(HRESULT, "Flip", [LPDIRECTDRAWSURFACE3, DWORD]),
    Method(HRESULT, "GetAttachedSurface", [LPDDSCAPS, Pointer(LPDIRECTDRAWSURFACE3) ]),
    Method(HRESULT, "GetBltStatus", [DWORD]),
    Method(HRESULT, "GetCaps", [LPDDSCAPS]),
    Method(HRESULT, "GetClipper", [Pointer(LPDIRECTDRAWCLIPPER) ]),
    Method(HRESULT, "GetColorKey", [DWORD, LPDDCOLORKEY]),
    Method(HRESULT, "GetDC", [Pointer(HDC) ]),
    Method(HRESULT, "GetFlipStatus", [DWORD]),
    Method(HRESULT, "GetOverlayPosition", [LPLONG, LPLONG ]),
    Method(HRESULT, "GetPalette", [Pointer(LPDIRECTDRAWPALETTE) ]),
    Method(HRESULT, "GetPixelFormat", [LPDDPIXELFORMAT]),
    Method(HRESULT, "GetSurfaceDesc", [LPDDSURFACEDESC]),
    Method(HRESULT, "Initialize", [LPDIRECTDRAW, LPDDSURFACEDESC]),
    Method(HRESULT, "IsLost", []),
    Method(HRESULT, "Lock", [LPRECT,LPDDSURFACEDESC,DWORD,HANDLE]),
    Method(HRESULT, "ReleaseDC", [HDC]),
    Method(HRESULT, "Restore", []),
    Method(HRESULT, "SetClipper", [LPDIRECTDRAWCLIPPER]),
    Method(HRESULT, "SetColorKey", [DWORD, LPDDCOLORKEY]),
    Method(HRESULT, "SetOverlayPosition", [LONG, LONG ]),
    Method(HRESULT, "SetPalette", [LPDIRECTDRAWPALETTE]),
    Method(HRESULT, "Unlock", [LPVOID]),
    Method(HRESULT, "UpdateOverlay", [LPRECT, LPDIRECTDRAWSURFACE3,LPRECT,DWORD, LPDDOVERLAYFX]),
    Method(HRESULT, "UpdateOverlayDisplay", [DWORD]),
    Method(HRESULT, "UpdateOverlayZOrder", [DWORD, LPDIRECTDRAWSURFACE3]),
    Method(HRESULT, "GetDDInterface", [Pointer(LPVOID) ]),
    Method(HRESULT, "PageLock", [DWORD]),
    Method(HRESULT, "PageUnlock", [DWORD]),
    Method(HRESULT, "SetSurfaceDesc", [LPDDSURFACEDESC, DWORD]),
]

IDirectDrawSurface4.methods += [
    Method(HRESULT, "AddAttachedSurface", [LPDIRECTDRAWSURFACE4]),
    Method(HRESULT, "AddOverlayDirtyRect", [LPRECT]),
    Method(HRESULT, "Blt", [LPRECT,LPDIRECTDRAWSURFACE4, LPRECT,DWORD, LPDDBLTFX]),
    Method(HRESULT, "BltBatch", [LPDDBLTBATCH, DWORD, DWORD ]),
    Method(HRESULT, "BltFast", [DWORD,DWORD,LPDIRECTDRAWSURFACE4, LPRECT,DWORD]),
    Method(HRESULT, "DeleteAttachedSurface", [DWORD,LPDIRECTDRAWSURFACE4]),
    Method(HRESULT, "EnumAttachedSurfaces", [LPVOID,LPDDENUMSURFACESCALLBACK2]),
    Method(HRESULT, "EnumOverlayZOrders", [DWORD,LPVOID,LPDDENUMSURFACESCALLBACK2]),
    Method(HRESULT, "Flip", [LPDIRECTDRAWSURFACE4, DWORD]),
    Method(HRESULT, "GetAttachedSurface", [LPDDSCAPS2, Pointer(LPDIRECTDRAWSURFACE4) ]),
    Method(HRESULT, "GetBltStatus", [DWORD]),
    Method(HRESULT, "GetCaps", [LPDDSCAPS2]),
    Method(HRESULT, "GetClipper", [Pointer(LPDIRECTDRAWCLIPPER) ]),
    Method(HRESULT, "GetColorKey", [DWORD, LPDDCOLORKEY]),
    Method(HRESULT, "GetDC", [Pointer(HDC) ]),
    Method(HRESULT, "GetFlipStatus", [DWORD]),
    Method(HRESULT, "GetOverlayPosition", [LPLONG, LPLONG ]),
    Method(HRESULT, "GetPalette", [Pointer(LPDIRECTDRAWPALETTE) ]),
    Method(HRESULT, "GetPixelFormat", [LPDDPIXELFORMAT]),
    Method(HRESULT, "GetSurfaceDesc", [LPDDSURFACEDESC2]),
    Method(HRESULT, "Initialize", [LPDIRECTDRAW, LPDDSURFACEDESC2]),
    Method(HRESULT, "IsLost", []),
    Method(HRESULT, "Lock", [LPRECT,LPDDSURFACEDESC2,DWORD,HANDLE]),
    Method(HRESULT, "ReleaseDC", [HDC]),
    Method(HRESULT, "Restore", []),
    Method(HRESULT, "SetClipper", [LPDIRECTDRAWCLIPPER]),
    Method(HRESULT, "SetColorKey", [DWORD, LPDDCOLORKEY]),
    Method(HRESULT, "SetOverlayPosition", [LONG, LONG ]),
    Method(HRESULT, "SetPalette", [LPDIRECTDRAWPALETTE]),
    Method(HRESULT, "Unlock", [LPRECT]),
    Method(HRESULT, "UpdateOverlay", [LPRECT, LPDIRECTDRAWSURFACE4,LPRECT,DWORD, LPDDOVERLAYFX]),
    Method(HRESULT, "UpdateOverlayDisplay", [DWORD]),
    Method(HRESULT, "UpdateOverlayZOrder", [DWORD, LPDIRECTDRAWSURFACE4]),
    Method(HRESULT, "GetDDInterface", [Pointer(LPVOID) ]),
    Method(HRESULT, "PageLock", [DWORD]),
    Method(HRESULT, "PageUnlock", [DWORD]),
    Method(HRESULT, "SetSurfaceDesc", [LPDDSURFACEDESC2, DWORD]),
    Method(HRESULT, "SetPrivateData", [REFGUID, LPVOID, DWORD, DWORD]),
    Method(HRESULT, "GetPrivateData", [REFGUID, LPVOID, LPDWORD]),
    Method(HRESULT, "FreePrivateData", [REFGUID]),
    Method(HRESULT, "GetUniquenessValue", [LPDWORD]),
    Method(HRESULT, "ChangeUniquenessValue", []),
]

IDirectDrawSurface7.methods += [
    Method(HRESULT, "AddAttachedSurface", [LPDIRECTDRAWSURFACE7]),
    Method(HRESULT, "AddOverlayDirtyRect", [LPRECT]),
    Method(HRESULT, "Blt", [LPRECT,LPDIRECTDRAWSURFACE7, LPRECT,DWORD, LPDDBLTFX]),
    Method(HRESULT, "BltBatch", [LPDDBLTBATCH, DWORD, DWORD ]),
    Method(HRESULT, "BltFast", [DWORD,DWORD,LPDIRECTDRAWSURFACE7, LPRECT,DWORD]),
    Method(HRESULT, "DeleteAttachedSurface", [DWORD,LPDIRECTDRAWSURFACE7]),
    Method(HRESULT, "EnumAttachedSurfaces", [LPVOID,LPDDENUMSURFACESCALLBACK7]),
    Method(HRESULT, "EnumOverlayZOrders", [DWORD,LPVOID,LPDDENUMSURFACESCALLBACK7]),
    Method(HRESULT, "Flip", [LPDIRECTDRAWSURFACE7, DWORD]),
    Method(HRESULT, "GetAttachedSurface", [LPDDSCAPS2, Pointer(LPDIRECTDRAWSURFACE7) ]),
    Method(HRESULT, "GetBltStatus", [DWORD]),
    Method(HRESULT, "GetCaps", [LPDDSCAPS2]),
    Method(HRESULT, "GetClipper", [Pointer(LPDIRECTDRAWCLIPPER) ]),
    Method(HRESULT, "GetColorKey", [DWORD, LPDDCOLORKEY]),
    Method(HRESULT, "GetDC", [Pointer(HDC) ]),
    Method(HRESULT, "GetFlipStatus", [DWORD]),
    Method(HRESULT, "GetOverlayPosition", [LPLONG, LPLONG ]),
    Method(HRESULT, "GetPalette", [Pointer(LPDIRECTDRAWPALETTE) ]),
    Method(HRESULT, "GetPixelFormat", [LPDDPIXELFORMAT]),
    Method(HRESULT, "GetSurfaceDesc", [LPDDSURFACEDESC2]),
    Method(HRESULT, "Initialize", [LPDIRECTDRAW, LPDDSURFACEDESC2]),
    Method(HRESULT, "IsLost", []),
    Method(HRESULT, "Lock", [LPRECT,LPDDSURFACEDESC2,DWORD,HANDLE]),
    Method(HRESULT, "ReleaseDC", [HDC]),
    Method(HRESULT, "Restore", []),
    Method(HRESULT, "SetClipper", [LPDIRECTDRAWCLIPPER]),
    Method(HRESULT, "SetColorKey", [DWORD, LPDDCOLORKEY]),
    Method(HRESULT, "SetOverlayPosition", [LONG, LONG ]),
    Method(HRESULT, "SetPalette", [LPDIRECTDRAWPALETTE]),
    Method(HRESULT, "Unlock", [LPRECT]),
    Method(HRESULT, "UpdateOverlay", [LPRECT, LPDIRECTDRAWSURFACE7,LPRECT,DWORD, LPDDOVERLAYFX]),
    Method(HRESULT, "UpdateOverlayDisplay", [DWORD]),
    Method(HRESULT, "UpdateOverlayZOrder", [DWORD, LPDIRECTDRAWSURFACE7]),
    Method(HRESULT, "GetDDInterface", [Pointer(LPVOID) ]),
    Method(HRESULT, "PageLock", [DWORD]),
    Method(HRESULT, "PageUnlock", [DWORD]),
    Method(HRESULT, "SetSurfaceDesc", [LPDDSURFACEDESC2, DWORD]),
    Method(HRESULT, "SetPrivateData", [REFGUID, LPVOID, DWORD, DWORD]),
    Method(HRESULT, "GetPrivateData", [REFGUID, LPVOID, LPDWORD]),
    Method(HRESULT, "FreePrivateData", [REFGUID]),
    Method(HRESULT, "GetUniquenessValue", [LPDWORD]),
    Method(HRESULT, "ChangeUniquenessValue", []),
    Method(HRESULT, "SetPriority", [DWORD]),
    Method(HRESULT, "GetPriority", [LPDWORD]),
    Method(HRESULT, "SetLOD", [DWORD]),
    Method(HRESULT, "GetLOD", [LPDWORD]),
]

IDirectDrawColorControl.methods += [
    Method(HRESULT, "GetColorControls", [LPDDCOLORCONTROL]),
    Method(HRESULT, "SetColorControls", [LPDDCOLORCONTROL]),
]

IDirectDrawGammaControl.methods += [
    Method(HRESULT, "GetGammaRamp", [DWORD, LPDDGAMMARAMP]),
    Method(HRESULT, "SetGammaRamp", [DWORD, LPDDGAMMARAMP]),
]

LPDDENUMCALLBACKA   = FunctionPointer(BOOL, "LPDDENUMCALLBACKA", [Pointer(GUID), LPSTR, LPSTR, LPVOID])
LPDDENUMCALLBACKW   = FunctionPointer(BOOL, "LPDDENUMCALLBACKW", [Pointer(GUID), LPWSTR, LPWSTR, LPVOID])
LPDDENUMCALLBACKEXA = FunctionPointer(BOOL, "LPDDENUMCALLBACKEXA", [Pointer(GUID), LPSTR, LPSTR, LPVOID, HMONITOR])
LPDDENUMCALLBACKEXW = FunctionPointer(BOOL, "LPDDENUMCALLBACKEXW", [Pointer(GUID), LPWSTR, LPWSTR, LPVOID, HMONITOR])

DDENUM = Flags(DWORD, [
    "DDENUM_ATTACHEDSECONDARYDEVICES",
    "DDENUM_DETACHEDSECONDARYDEVICES",
    "DDENUM_NONDISPLAYDEVICES",
])

DDCREATE = Flags(DWORD, [
    "DDCREATE_HARDWAREONLY",
    "DDCREATE_EMULATIONONLY",
])

ddraw = Dll("ddraw")
ddraw.functions += [
    StdFunction(HRESULT, "DirectDrawEnumerateW", [(LPDDENUMCALLBACKW, "lpCallback"), (LPVOID, "lpContext")]),
    StdFunction(HRESULT, "DirectDrawEnumerateA", [(LPDDENUMCALLBACKA, "lpCallback"), (LPVOID, "lpContext")]),
    StdFunction(HRESULT, "DirectDrawEnumerateExW", [(LPDDENUMCALLBACKEXW, "lpCallback"), (LPVOID, "lpContext"), (DDENUM, "dwFlags")]),
    StdFunction(HRESULT, "DirectDrawEnumerateExA", [(LPDDENUMCALLBACKEXA, "lpCallback"), (LPVOID, "lpContext"), (DDENUM, "dwFlags")]),
    StdFunction(HRESULT, "DirectDrawCreate", [(Pointer(GUID), "lpGUID"), Out(Pointer(LPDIRECTDRAW), "lplpDD"), (Pointer(IUnknown), "pUnkOuter")]),
    StdFunction(HRESULT, "DirectDrawCreateEx", [(Pointer(GUID), "lpGuid"), Out(Pointer(LPVOID), "lplpDD"), (REFIID, "iid"), (Pointer(IUnknown), "pUnkOuter")]),
    StdFunction(HRESULT, "DirectDrawCreateClipper", [(DWORD, "dwFlags"), Out(Pointer(LPDIRECTDRAWCLIPPER), "lplpDDClipper"), (Pointer(IUnknown), "pUnkOuter")]),
]

if __name__ == '__main__':
    print '#include <windows.h>'
    print '#include <tchar.h>'
    print '#include <ddraw.h>'
    print
    print '#include "log.hpp"'
    print
    wrap()
