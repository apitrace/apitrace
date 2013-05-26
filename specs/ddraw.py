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

from winapi import *

DirectDrawOptSurfaceDescFlags = Flags(DWORD, [
    "DDOSD_GUID",
    "DDOSD_COMPRESSION_RATIO",
    "DDOSD_SCAPS",
    "DDOSD_OSCAPS",
    "DDOSD_ALL",
])

DirectDrawOptSurfaceDescCapsFlags = Flags(DWORD, [
    "DDOSDCAPS_OPTCOMPRESSED",
    "DDOSDCAPS_OPTREORDERED",
    "DDOSDCAPS_MONOLITHICMIPMAP",
])

DirectDrawGetDeviceIdentifierFlags = Flags(DWORD, [
    "DDGDI_GETHOSTIDENTIFIER",
])

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

LPUNKNOWN = ObjPointer(IUnknown)
LPDIRECTDRAW = ObjPointer(IDirectDraw)
LPDIRECTDRAW2 = ObjPointer(IDirectDraw2)
LPDIRECTDRAW4 = ObjPointer(IDirectDraw4)
LPDIRECTDRAW7 = ObjPointer(IDirectDraw7)
LPDIRECTDRAWSURFACE = ObjPointer(IDirectDrawSurface)
LPDIRECTDRAWSURFACE2 = ObjPointer(IDirectDrawSurface2)
LPDIRECTDRAWSURFACE3 = ObjPointer(IDirectDrawSurface3)
LPDIRECTDRAWSURFACE4 = ObjPointer(IDirectDrawSurface4)
LPDIRECTDRAWSURFACE7 = ObjPointer(IDirectDrawSurface7)
LPDIRECTDRAWPALETTE = ObjPointer(IDirectDrawPalette)
LPDIRECTDRAWCLIPPER = ObjPointer(IDirectDrawClipper)
LPDIRECTDRAWCOLORCONTROL = ObjPointer(IDirectDrawColorControl)
LPDIRECTDRAWGAMMACONTROL = ObjPointer(IDirectDrawGammaControl)

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
    (Array(DWORD, "DD_ROP_SPACE"), "dwRops"),
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
    (Array(DWORD, "DD_ROP_SPACE"), "dwRops"),
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
    (Array(DWORD, "DD_ROP_SPACE"), "dwSVBRops"),
    (DWORD, "dwVSBCaps"),
    (DWORD, "dwVSBCKeyCaps"),
    (DWORD, "dwVSBFXCaps"),
    (Array(DWORD, "DD_ROP_SPACE"), "dwVSBRops"),
    (DWORD, "dwSSBCaps"),
    (DWORD, "dwSSBCKeyCaps"),
    (DWORD, "dwSSBFXCaps"),
    (Array(DWORD, "DD_ROP_SPACE"), "dwSSBRops"),
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
    (Array(DWORD, "DD_ROP_SPACE"), "dwRops"),
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
    (Array(DWORD, "DD_ROP_SPACE"), "dwSVBRops"),
    (DWORD, "dwVSBCaps"),
    (DWORD, "dwVSBCKeyCaps"),
    (DWORD, "dwVSBFXCaps"),
    (Array(DWORD, "DD_ROP_SPACE"), "dwVSBRops"),
    (DWORD, "dwSSBCaps"),
    (DWORD, "dwSSBCKeyCaps"),
    (DWORD, "dwSSBFXCaps"),
    (Array(DWORD, "DD_ROP_SPACE"), "dwSSBRops"),
    (DWORD, "dwMaxVideoPorts"),
    (DWORD, "dwCurrVideoPorts"),
    (DWORD, "dwSVBCaps2"),
    (DWORD, "dwNLVBCaps"),
    (DWORD, "dwNLVBCaps2"),
    (DWORD, "dwNLVBCKeyCaps"),
    (DWORD, "dwNLVBFXCaps"),
    (Array(DWORD, "DD_ROP_SPACE"), "dwNLVBRops"),
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
    (Array(DWORD, "DD_ROP_SPACE"), "dwRops"),
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
    (Array(DWORD, "DD_ROP_SPACE"), "dwSVBRops"),
    (DWORD, "dwVSBCaps"),
    (DWORD, "dwVSBCKeyCaps"),
    (DWORD, "dwVSBFXCaps"),
    (Array(DWORD, "DD_ROP_SPACE"), "dwVSBRops"),
    (DWORD, "dwSSBCaps"),
    (DWORD, "dwSSBCKeyCaps"),
    (DWORD, "dwSSBFXCaps"),
    (Array(DWORD, "DD_ROP_SPACE"), "dwSSBRops"),
    (DWORD, "dwMaxVideoPorts"),
    (DWORD, "dwCurrVideoPorts"),
    (DWORD, "dwSVBCaps2"),
    (DWORD, "dwNLVBCaps"),
    (DWORD, "dwNLVBCaps2"),
    (DWORD, "dwNLVBCKeyCaps"),
    (DWORD, "dwNLVBFXCaps"),
    (Array(DWORD, "DD_ROP_SPACE"), "dwNLVBRops"),
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
    (Array(DWORD, "DD_ROP_SPACE"), "dwRops"),
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
    (Array(DWORD, "DD_ROP_SPACE"), "dwSVBRops"),
    (DWORD, "dwVSBCaps"),
    (DWORD, "dwVSBCKeyCaps"),
    (DWORD, "dwVSBFXCaps"),
    (Array(DWORD, "DD_ROP_SPACE"), "dwVSBRops"),
    (DWORD, "dwSSBCaps"),
    (DWORD, "dwSSBCKeyCaps"),
    (DWORD, "dwSSBFXCaps"),
    (Array(DWORD, "DD_ROP_SPACE"), "dwSSBRops"),
    (DWORD, "dwMaxVideoPorts"),
    (DWORD, "dwCurrVideoPorts"),
    (DWORD, "dwSVBCaps2"),
    (DWORD, "dwNLVBCaps"),
    (DWORD, "dwNLVBCaps2"),
    (DWORD, "dwNLVBCKeyCaps"),
    (DWORD, "dwNLVBFXCaps"),
    (Array(DWORD, "DD_ROP_SPACE"), "dwNLVBRops"),
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
    (Struct(None, [
        (WORD, "wFlipMSTypes"),
        (WORD, "wBltMSTypes"),
    ]), "MultiSampleCaps"),
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
    (Array(WORD, 256), "red"),
    (Array(WORD, 256), "green"),
    (Array(WORD, 256), "blue"),
])
LPDDGAMMARAMP = Pointer(DDGAMMARAMP)

DDDEVICEIDENTIFIER = Struct("DDDEVICEIDENTIFIER", [
    (CString, "szDriver"),
    (CString, "szDescription"),
    (LARGE_INTEGER, "liDriverVersion"),
    (DWORD, "dwVendorId"),
    (DWORD, "dwDeviceId"),
    (DWORD, "dwSubSysId"),
    (DWORD, "dwRevision"),
    (GUID, "guidDeviceIdentifier"),
])
LPDDDEVICEIDENTIFIER = Pointer(DDDEVICEIDENTIFIER)

DDDEVICEIDENTIFIER2 = Struct("DDDEVICEIDENTIFIER2", [
    (CString, "szDriver"),
    (CString, "szDescription"),
    (LARGE_INTEGER, "liDriverVersion"),
    (DWORD, "dwVendorId"),
    (DWORD, "dwDeviceId"),
    (DWORD, "dwSubSysId"),
    (DWORD, "dwRevision"),
    (GUID, "guidDeviceIdentifier"),
    (DWORD, "dwWHQLLevel"),
])
LPDDDEVICEIDENTIFIER2 = Pointer(DDDEVICEIDENTIFIER2)

LPCLIPPERCALLBACK = FunctionPointer(DWORD, "LPCLIPPERCALLBACK", [(LPDIRECTDRAWCLIPPER, "lpDDClipper"), (HWND, "hWnd"), (DWORD, "code"), (LPVOID, "lpContext")])
LPSURFACESTREAMINGCALLBACK = FunctionPointer(DWORD, "LPSURFACESTREAMINGCALLBACK", [DWORD])

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

DirectDrawSurfaceDescFlags = Flags(DWORD, [
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

DirectDrawEnumerateExFlags = Flags(DWORD, [
    "DDENUM_ATTACHEDSECONDARYDEVICES",
    "DDENUM_DETACHEDSECONDARYDEVICES",
    "DDENUM_NONDISPLAYDEVICES",
])

DirectDrawCreateFlags = FakeEnum(DWORD, [
    "DDCREATE_HARDWAREONLY",
    "DDCREATE_EMULATIONONLY",
])

DirectDrawColorControlFlags = Flags(DWORD, [
    "DDCOLOR_BRIGHTNESS",
    "DDCOLOR_CONTRAST",
    "DDCOLOR_HUE",
    "DDCOLOR_SATURATION",
    "DDCOLOR_SHARPNESS",
    "DDCOLOR_GAMMA",
    "DDCOLOR_COLORENABLE",
])

DirectDrawCapsFlags = Flags(DWORD, [
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

DirectDrawCapabilityFlags2 = Flags(DWORD, [
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

DirectDrawDriverCapsFlags = Flags(DWORD, [
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

DirectDrawDriverCapsFlags2 = Flags(DWORD, [
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

DirectDrawFxAlphaCapsFlags = Flags(DWORD, [
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

DirectDrawFxCapsFlags = Flags(DWORD, [
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

DirectDrawStereoViewCapsFlags = Flags(DWORD, [
    "DDSVCAPS_RESERVED1",
    "DDSVCAPS_RESERVED2",
    "DDSVCAPS_RESERVED3",
    "DDSVCAPS_RESERVED4",
    "DDSVCAPS_STEREOSEQUENTIAL",
])

DirectDrawPaletteCapsFlags = Flags(DWORD, [
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

DirectDrawSurfaceSetPrivateDataConstants = FakeEnum(DWORD, [
    "DDSPD_IUNKNOWNPOINTER",
    "DDSPD_VOLATILE",
])

DirectDrawSurfaceSetPaletteConstants = FakeEnum(DWORD, [
    "DDBD_1",
    "DDBD_2",
    "DDBD_4",
    "DDBD_8",
    "DDBD_16",
    "DDBD_24",
    "DDBD_32",
])

DirectDrawSurfaceSetGetColorKeyFlags = Flags(DWORD, [
    "DDCKEY_COLORSPACE",
    "DDCKEY_DESTBLT",
    "DDCKEY_DESTOVERLAY",
    "DDCKEY_SRCBLT",
    "DDCKEY_SRCOVERLAY",
])

DirectDrawColorKeyCapsFlags = Flags(DWORD, [
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

DirectDrawPixelFormatFlags = Flags(DWORD, [
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

DirectDrawEnumSurfacesFlags = Flags(DWORD, [
    "DDENUMSURFACES_ALL",
    "DDENUMSURFACES_MATCH",
    "DDENUMSURFACES_NOMATCH",
    "DDENUMSURFACES_CANBECREATED",
    "DDENUMSURFACES_DOESEXIST",
])

DirectDrawSetDisplayModeFlags = Flags(DWORD, [
    "DDSDM_STANDARDVGAMODE",
])

DirectDrawEnumDisplayModesFlags = Flags(DWORD, [
    "DDEDM_REFRESHRATES",
    "DDEDM_STANDARDVGAMODES",
])

DirectDrawSetCooperativeLevelFlags = Flags(DWORD, [
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

DirectDrawBltFlags = Flags(DWORD, [
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

DirectDrawBltFastFlags = Flags(DWORD, [
    "DDBLTFAST_NOCOLORKEY",
    "DDBLTFAST_SRCCOLORKEY",
    "DDBLTFAST_DESTCOLORKEY",
    "DDBLTFAST_WAIT",
    "DDBLTFAST_DONOTWAIT",
])

DirectDrawFlipFlags = Flags(DWORD, [
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

DirectDrawSurfaceOverlayFlags = Flags(DWORD, [
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

DirectDrawSurfaceLockFlags = Flags(DWORD, [
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

DirectDrawSurfaceBltFxFlags = Flags(DWORD, [
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

DirectDrawOverlayFxFlags = Flags(DWORD, [
    "DDOVERFX_ARITHSTRETCHY",
    "DDOVERFX_MIRRORLEFTRIGHT",
    "DDOVERFX_MIRRORUPDOWN",
    "DDOVERFX_DEINTERLACE",
])

DirectDrawWaitForVerticalBlankFlags = Flags(DWORD, [
    "DDWAITVB_BLOCKBEGIN",
    "DDWAITVB_BLOCKBEGINEVENT",
    "DDWAITVB_BLOCKEND",
])

DirectDrawGetFlipStatusFlags = Flags(DWORD, [
    "DDGFS_CANFLIP",
    "DDGFS_ISFLIPDONE",
])

DirectDrawGetBltStatusFlags = Flags(DWORD, [
    "DDGBS_CANBLT",
    "DDGBS_ISBLTDONE",
])

DirectDrawEnumOverlayZOrderFlags = Flags(DWORD, [
    "DDENUMOVERLAYZ_BACKTOFRONT",
    "DDENUMOVERLAYZ_FRONTTOBACK",
])

DirectDrawUpdateOverlayZOrderFlags = Flags(DWORD, [
    "DDOVERZ_SENDTOFRONT",
    "DDOVERZ_SENDTOBACK",
    "DDOVERZ_MOVEFORWARD",
    "DDOVERZ_MOVEBACKWARD",
    "DDOVERZ_INSERTINFRONTOF",
    "DDOVERZ_INSERTINBACKOF",
])

DirectDrawSetGammaRampFlags = Flags(DWORD, [
    "DDSGR_CALIBRATE",
])

DirectDrawStartModeTestFlags = Flags(DWORD, [
    "DDSMT_ISTESTREQUIRED",
])

DirectDrawEvaluateModeFlags = Flags(DWORD, [
    "DDEM_MODEPASSED",
    "DDEM_MODEFAILED",
])

DDRESULT = MAKE_HRESULT(ok = "DD_OK", false = "DD_FALSE", errors = [
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
    StdMethod(DDRESULT, "Compact", []),
    StdMethod(DDRESULT, "CreateClipper", [(DWORD, "dwFlags"), Out(Pointer(LPDIRECTDRAWCLIPPER), "lplpDDClipper"), (LPUNKNOWN, "pUnkOuter")]),
    StdMethod(DDRESULT, "CreatePalette", [(DirectDrawPaletteCapsFlags, "dwFlags"), (LPPALETTEENTRY, "lpDDColorArray"), Out(Pointer(LPDIRECTDRAWPALETTE), "lplpDDPalette"), (ObjPointer(IUnknown), "pUnkOuter")]),
    StdMethod(DDRESULT, "CreateSurface", [(LPDDSURFACEDESC, "lpDDSurfaceDesc"), Out(Pointer(LPDIRECTDRAWSURFACE), "lplpDDSurface"), (LPUNKNOWN, "pUnkOuter")]),
    StdMethod(DDRESULT, "DuplicateSurface", [(LPDIRECTDRAWSURFACE, "lpDDSurface"), Out(Pointer(LPDIRECTDRAWSURFACE), "lplpDupDDSurface")]),
    StdMethod(DDRESULT, "EnumDisplayModes", [(DirectDrawEnumDisplayModesFlags, "dwFlags"), (LPDDSURFACEDESC, "lpDDSurfaceDesc"), (LPVOID, "lpContext"), (LPDDENUMMODESCALLBACK, "lpEnumModesCallback")]),
    StdMethod(DDRESULT, "EnumSurfaces", [(DirectDrawEnumSurfacesFlags, "dwFlags"), (LPDDSURFACEDESC, "lpDDSurfaceDesc"), (LPVOID, "lpContext"), (LPDDENUMSURFACESCALLBACK, "lpEnumSurfacesCallback")]),
    StdMethod(DDRESULT, "FlipToGDISurface", []),
    StdMethod(DDRESULT, "GetCaps", [Out(LPDDCAPS, "lpDDDriverCaps"), Out(LPDDCAPS, "lpDDHELCaps")]),
    StdMethod(DDRESULT, "GetDisplayMode", [Out(LPDDSURFACEDESC, "lpDDSurfaceDesc")]),
    StdMethod(DDRESULT, "GetFourCCCodes", [Out(LPDWORD, "lpNumCodes"), Out(LPDWORD, "lpCodes")]),
    StdMethod(DDRESULT, "GetGDISurface", [Out(Pointer(LPDIRECTDRAWSURFACE), "lplpGDIDDSSurface")]),
    StdMethod(DDRESULT, "GetMonitorFrequency", [Out(LPDWORD, "lpdwFrequency")]),
    StdMethod(DDRESULT, "GetScanLine", [Out(LPDWORD, "lpdwScanLine")]),
    StdMethod(DDRESULT, "GetVerticalBlankStatus", [Out(LPBOOL, "lpbIsInVB")]),
    StdMethod(DDRESULT, "Initialize", [(Pointer(GUID), "lpGUID")]),
    StdMethod(DDRESULT, "RestoreDisplayMode", []),
    StdMethod(DDRESULT, "SetCooperativeLevel", [(HWND, "hWnd"), (DirectDrawSetCooperativeLevelFlags, "dwFlags")]),
    StdMethod(DDRESULT, "SetDisplayMode", [(DWORD, "dwWidth"), (DWORD, "dwHeight"), (DWORD, "dwBPP")]),
    StdMethod(DDRESULT, "WaitForVerticalBlank", [(DirectDrawWaitForVerticalBlankFlags, "dwFlags"), (HANDLE, "hEvent")]),
]

IDirectDraw2.methods += [
    StdMethod(DDRESULT, "Compact", []),
    StdMethod(DDRESULT, "CreateClipper", [(DWORD, "dwFlags"), Out(Pointer(LPDIRECTDRAWCLIPPER), "lplpDDClipper"), (LPUNKNOWN, "pUnkOuter")]),
    StdMethod(DDRESULT, "CreatePalette", [(DirectDrawPaletteCapsFlags, "dwFlags"), (LPPALETTEENTRY, "lpDDColorArray"), Out(Pointer(LPDIRECTDRAWPALETTE), "lplpDDPalette"), (LPUNKNOWN, "pUnkOuter")]),
    StdMethod(DDRESULT, "CreateSurface", [(LPDDSURFACEDESC, "lpDDSurfaceDesc"), Out(Pointer(LPDIRECTDRAWSURFACE), "lplpDDSurface"), (LPUNKNOWN, "pUnkOuter")]),
    StdMethod(DDRESULT, "DuplicateSurface", [(LPDIRECTDRAWSURFACE, "lpDDSurface"), Out(Pointer(LPDIRECTDRAWSURFACE), "lplpDupDDSurface")]),
    StdMethod(DDRESULT, "EnumDisplayModes", [(DirectDrawEnumDisplayModesFlags, "dwFlags"), (LPDDSURFACEDESC, "lpDDSurfaceDesc"), (LPVOID, "lpContext"), (LPDDENUMMODESCALLBACK, "lpEnumModesCallback")]),
    StdMethod(DDRESULT, "EnumSurfaces", [(DirectDrawEnumSurfacesFlags, "dwFlags"), (LPDDSURFACEDESC, "lpDDSurfaceDesc"), (LPVOID, "lpContext"), (LPDDENUMSURFACESCALLBACK, "lpEnumSurfacesCallback")]),
    StdMethod(DDRESULT, "FlipToGDISurface", []),
    StdMethod(DDRESULT, "GetCaps", [Out(LPDDCAPS, "lpDDDriverCaps"), Out(LPDDCAPS, "lpDDHELCaps")]),
    StdMethod(DDRESULT, "GetDisplayMode", [Out(LPDDSURFACEDESC, "lpDDSurfaceDesc")]),
    StdMethod(DDRESULT, "GetFourCCCodes", [Out(LPDWORD, "lpNumCodes"), Out(LPDWORD, "lpCodes")]),
    StdMethod(DDRESULT, "GetGDISurface", [Out(Pointer(LPDIRECTDRAWSURFACE), "lplpGDIDDSSurface")]),
    StdMethod(DDRESULT, "GetMonitorFrequency", [Out(LPDWORD, "lpdwFrequency")]),
    StdMethod(DDRESULT, "GetScanLine", [Out(LPDWORD, "lpdwScanLine")]),
    StdMethod(DDRESULT, "GetVerticalBlankStatus", [Out(LPBOOL, "lpbIsInVB")]),
    StdMethod(DDRESULT, "Initialize", [(Pointer(GUID), "lpGUID")]),
    StdMethod(DDRESULT, "RestoreDisplayMode", []),
    StdMethod(DDRESULT, "SetCooperativeLevel", [(HWND, "hWnd"), (DirectDrawSetCooperativeLevelFlags, "dwFlags")]),
    StdMethod(DDRESULT, "SetDisplayMode", [(DWORD, "dwWidth"), (DWORD, "dwHeight"), (DWORD, "dwBPP"), (DWORD, "dwRefreshRate"), (DirectDrawSetDisplayModeFlags, "dwFlags")]),
    StdMethod(DDRESULT, "WaitForVerticalBlank", [(DirectDrawWaitForVerticalBlankFlags, "dwFlags"), (HANDLE, "hEvent")]),
    StdMethod(DDRESULT, "GetAvailableVidMem", [(LPDDSCAPS, "lpDDSCaps"), Out(LPDWORD, "lpdwTotal"), Out(LPDWORD, "lpdwFree")]),
]

IDirectDraw4.methods += [
    StdMethod(DDRESULT, "Compact", []),
    StdMethod(DDRESULT, "CreateClipper", [(DWORD, "dwFlags"), Out(Pointer(LPDIRECTDRAWCLIPPER), "lplpDDClipper"), (LPUNKNOWN, "pUnkOuter")]),
    StdMethod(DDRESULT, "CreatePalette", [(DirectDrawPaletteCapsFlags, "dwFlags"), (LPPALETTEENTRY, "lpDDColorArray"), Out(Pointer(LPDIRECTDRAWPALETTE), "lplpDDPalette"), (ObjPointer(IUnknown), "pUnkOuter")]),
    StdMethod(DDRESULT, "CreateSurface", [(LPDDSURFACEDESC2, "lpDDSurfaceDesc"), Out(Pointer(LPDIRECTDRAWSURFACE4), "lplpDDSurface"), (LPUNKNOWN, "pUnkOuter")]),
    StdMethod(DDRESULT, "DuplicateSurface", [(LPDIRECTDRAWSURFACE4, "lpDDSurface"), Out(Pointer(LPDIRECTDRAWSURFACE4), "lplpDupDDSurface")]),
    StdMethod(DDRESULT, "EnumDisplayModes", [(DirectDrawEnumDisplayModesFlags, "dwFlags"), (LPDDSURFACEDESC2, "lpDDSurfaceDesc"), (LPVOID, "lpContext"), (LPDDENUMMODESCALLBACK2, "lpEnumModesCallback")]),
    StdMethod(DDRESULT, "EnumSurfaces", [(DirectDrawEnumSurfacesFlags, "dwFlags"), (LPDDSURFACEDESC2, "lpDDSurfaceDesc"), (LPVOID, "lpContext"), (LPDDENUMSURFACESCALLBACK2, "lpEnumSurfacesCallback")]),
    StdMethod(DDRESULT, "FlipToGDISurface", []),
    StdMethod(DDRESULT, "GetCaps", [Out(LPDDCAPS, "lpDDDriverCaps"), Out(LPDDCAPS, "lpDDHELCaps")]),
    StdMethod(DDRESULT, "GetDisplayMode", [Out(LPDDSURFACEDESC2, "lpDDSurfaceDesc")]),
    StdMethod(DDRESULT, "GetFourCCCodes", [Out(LPDWORD, "lpNumCodes"), Out(LPDWORD, "lpCodes")]),
    StdMethod(DDRESULT, "GetGDISurface", [Out(Pointer(LPDIRECTDRAWSURFACE4), "lplpGDIDDSSurface")]),
    StdMethod(DDRESULT, "GetMonitorFrequency", [Out(LPDWORD, "lpdwFrequency")]),
    StdMethod(DDRESULT, "GetScanLine", [Out(LPDWORD, "lpdwScanLine")]),
    StdMethod(DDRESULT, "GetVerticalBlankStatus", [Out(LPBOOL, "lpbIsInVB")]),
    StdMethod(DDRESULT, "Initialize", [(Pointer(GUID), "lpGUID")]),
    StdMethod(DDRESULT, "RestoreDisplayMode", []),
    StdMethod(DDRESULT, "SetCooperativeLevel", [(HWND, "hWnd"), (DirectDrawSetCooperativeLevelFlags, "dwFlags")]),
    StdMethod(DDRESULT, "SetDisplayMode", [(DWORD, "dwWidth"), (DWORD, "dwHeight"), (DWORD, "dwBPP"), (DWORD, "dwRefreshRate"), (DirectDrawSetDisplayModeFlags, "dwFlags")]),
    StdMethod(DDRESULT, "WaitForVerticalBlank", [(DirectDrawWaitForVerticalBlankFlags, "dwFlags"), (HANDLE, "hEvent")]),
    StdMethod(DDRESULT, "GetAvailableVidMem", [(LPDDSCAPS2, "lpDDSCaps"), Out(LPDWORD, "lpdwTotal"), Out(LPDWORD, "lpdwFree")]),
    StdMethod(DDRESULT, "GetSurfaceFromDC", [(HDC, "hdc"), Out(Pointer(LPDIRECTDRAWSURFACE4), "lpDDSurface")]),
    StdMethod(DDRESULT, "RestoreAllSurfaces", []),
    StdMethod(DDRESULT, "TestCooperativeLevel", []),
    StdMethod(DDRESULT, "GetDeviceIdentifier", [Out(LPDDDEVICEIDENTIFIER, "lpDDDI"), (DirectDrawGetDeviceIdentifierFlags, "dwFlags")]),
]

IDirectDraw7.methods += [
    StdMethod(DDRESULT, "Compact", []),
    StdMethod(DDRESULT, "CreateClipper", [(DWORD, "dwFlags"), Out(Pointer(LPDIRECTDRAWCLIPPER), "lplpDDClipper"), (LPUNKNOWN, "pUnkOuter")]),
    StdMethod(DDRESULT, "CreatePalette", [(DirectDrawPaletteCapsFlags, "dwFlags"), (LPPALETTEENTRY, "lpDDColorArray"), Out(Pointer(LPDIRECTDRAWPALETTE), "lplpDDPalette"), (LPUNKNOWN, "pUnkOuter")]),
    StdMethod(DDRESULT, "CreateSurface", [(LPDDSURFACEDESC2, "lpDDSurfaceDesc"), Out(Pointer(LPDIRECTDRAWSURFACE7), "lplpDDSurface"), (LPUNKNOWN, "pUnkOuter")]),
    StdMethod(DDRESULT, "DuplicateSurface", [(LPDIRECTDRAWSURFACE7, "lpDDSurface"), Out(Pointer(LPDIRECTDRAWSURFACE7), "lplpDupDDSurface")]),
    StdMethod(DDRESULT, "EnumDisplayModes", [(DirectDrawEnumDisplayModesFlags, "dwFlags"), (LPDDSURFACEDESC2, "lpDDSurfaceDesc"), (LPVOID, "lpContext"), (LPDDENUMMODESCALLBACK2, "lpEnumModesCallback")]),
    StdMethod(DDRESULT, "EnumSurfaces", [(DirectDrawEnumSurfacesFlags, "dwFlags"), (LPDDSURFACEDESC2, "lpDDSurfaceDesc"), (LPVOID, "lpContext"), (LPDDENUMSURFACESCALLBACK7, "lpEnumSurfacesCallback")]),
    StdMethod(DDRESULT, "FlipToGDISurface", []),
    StdMethod(DDRESULT, "GetCaps", [Out(LPDDCAPS, "lpDDDriverCaps"), Out(LPDDCAPS, "lpDDHELCaps")]),
    StdMethod(DDRESULT, "GetDisplayMode", [Out(LPDDSURFACEDESC2, "lpDDSurfaceDesc")]),
    StdMethod(DDRESULT, "GetFourCCCodes", [Out(LPDWORD, "lpNumCodes"), Out(LPDWORD, "lpCodes")]),
    StdMethod(DDRESULT, "GetGDISurface", [Out(Pointer(LPDIRECTDRAWSURFACE7), "lplpGDIDDSSurface")]),
    StdMethod(DDRESULT, "GetMonitorFrequency", [Out(LPDWORD, "lpdwFrequency")]),
    StdMethod(DDRESULT, "GetScanLine", [Out(LPDWORD, "lpdwScanLine")]),
    StdMethod(DDRESULT, "GetVerticalBlankStatus", [Out(LPBOOL, "lpbIsInVB")]),
    StdMethod(DDRESULT, "Initialize", [(Pointer(GUID), "lpGUID")]),
    StdMethod(DDRESULT, "RestoreDisplayMode", []),
    StdMethod(DDRESULT, "SetCooperativeLevel", [(HWND, "hWnd"), (DirectDrawSetCooperativeLevelFlags, "dwFlags")]),
    StdMethod(DDRESULT, "SetDisplayMode", [(DWORD, "dwWidth"), (DWORD, "dwHeight"), (DWORD, "dwBPP"), (DWORD, "dwRefreshRate"), (DirectDrawSetDisplayModeFlags, "dwFlags")]),
    StdMethod(DDRESULT, "WaitForVerticalBlank", [(DirectDrawWaitForVerticalBlankFlags, "dwFlags"), (HANDLE, "hEvent")]),
    StdMethod(DDRESULT, "GetAvailableVidMem", [(LPDDSCAPS2, "lpDDSCaps"), Out(LPDWORD, "lpdwTotal"), Out(LPDWORD, "lpdwFree")]),
    StdMethod(DDRESULT, "GetSurfaceFromDC", [(HDC, "hdc"), Out(Pointer(LPDIRECTDRAWSURFACE7), "lpDDSurface")]),
    StdMethod(DDRESULT, "RestoreAllSurfaces", []),
    StdMethod(DDRESULT, "TestCooperativeLevel", []),
    StdMethod(DDRESULT, "GetDeviceIdentifier", [Out(LPDDDEVICEIDENTIFIER2, "lpDDDI"), (DirectDrawGetDeviceIdentifierFlags, "dwFlags")]),
    StdMethod(DDRESULT, "StartModeTest", [(LPSIZE, "lpModesToTest"), (DWORD, "dwNumEntries"), (DirectDrawStartModeTestFlags, "dwFlags")]),
    StdMethod(DDRESULT, "EvaluateMode", [(DirectDrawEvaluateModeFlags, "dwFlags"), Out(Pointer(DWORD), "pSecondsUntilTimeout")]),
]

IDirectDrawPalette.methods += [
    StdMethod(DDRESULT, "GetCaps", [Out(Pointer(DirectDrawPaletteCapsFlags), "lpdwCaps")]),
    StdMethod(DDRESULT, "GetEntries", [(DWORD, "dwFlags"), (DWORD, "dwBase"), (DWORD, "dwNumEntries"), Out(LPPALETTEENTRY, "lpEntries")]),
    StdMethod(DDRESULT, "Initialize", [(LPDIRECTDRAW, "lpDD"), (DWORD, "dwFlags"), (LPPALETTEENTRY, "lpDDColorTable")]),
    StdMethod(DDRESULT, "SetEntries", [(DWORD, "dwFlags"), (DWORD, "dwStartingEntry"), (DWORD, "dwCount"), (LPPALETTEENTRY, "lpEntries")]),
]

IDirectDrawClipper.methods += [
    StdMethod(DDRESULT, "GetClipList", [(LPRECT, "lpRect"), (LPRGNDATA, "lpClipList"), Out(LPDWORD, "lpdwSize")]),
    StdMethod(DDRESULT, "GetHWnd", [Out(Pointer(HWND), "hWnd")]),
    StdMethod(DDRESULT, "Initialize", [(LPDIRECTDRAW, "lpDD"), (DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "IsClipListChanged", [Out(Pointer(BOOL), "lpbChanged")]),
    StdMethod(DDRESULT, "SetClipList", [(LPRGNDATA, "lpClipList"), (DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "SetHWnd", [(DWORD, "dwFlags"), (HWND, "hWnd")]),
]


IDirectDrawSurface.methods += [
    StdMethod(DDRESULT, "AddAttachedSurface", [(LPDIRECTDRAWSURFACE, "lpDDSAttachedSurface")]),
    StdMethod(DDRESULT, "AddOverlayDirtyRect", [(LPRECT, "lpRect")]),
    StdMethod(DDRESULT, "Blt", [(LPRECT, "lpDestRect"), (LPDIRECTDRAWSURFACE, "lpDDSrcSurface"), (LPRECT, "lpSrcRect"), (DirectDrawBltFlags, "dwFlags"), (LPDDBLTFX, "lpDDBltFx")]),
    StdMethod(DDRESULT, "BltBatch", [(LPDDBLTBATCH, "lpDDBltBatch"), (DWORD, "dwCount"), (DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "BltFast", [(DWORD, "dwX"), (DWORD, "dwY"), (LPDIRECTDRAWSURFACE, "lpDDSrcSurface"), (LPRECT, "lpSrcRect"), (DWORD, "dwTrans")]),
    StdMethod(DDRESULT, "DeleteAttachedSurface", [(DWORD, "dwFlags"), (LPDIRECTDRAWSURFACE, "lpDDSurface")]),
    StdMethod(DDRESULT, "EnumAttachedSurfaces", [(LPVOID, "lpContext"), (LPDDENUMSURFACESCALLBACK, "lpEnumSurfacesCallback")]),
    StdMethod(DDRESULT, "EnumOverlayZOrders", [(DirectDrawEnumOverlayZOrderFlags, "dwFlags"), (LPVOID, "lpContext"), (LPDDENUMSURFACESCALLBACK, "lpfnCallback")]),
    StdMethod(DDRESULT, "Flip", [(LPDIRECTDRAWSURFACE, "lpDDSurfaceTargetOverride"), (DirectDrawFlipFlags, "dwFlags")]),
    StdMethod(DDRESULT, "GetAttachedSurface", [(LPDDSCAPS, "lpDDSCaps"), Out(Pointer(LPDIRECTDRAWSURFACE), "lplpDDAttachedSurface")]),
    StdMethod(DDRESULT, "GetBltStatus", [(DirectDrawGetBltStatusFlags, "dwFlags")]),
    StdMethod(DDRESULT, "GetCaps", [Out(LPDDSCAPS, "lpDDSCaps")]),
    StdMethod(DDRESULT, "GetClipper", [Out(Pointer(LPDIRECTDRAWCLIPPER), "lplpDDClipper")]),
    StdMethod(DDRESULT, "GetColorKey", [(DirectDrawSurfaceSetGetColorKeyFlags, "dwFlags"), Out(LPDDCOLORKEY, "lpDDColorKey")]),
    StdMethod(DDRESULT, "GetDC", [Out(Pointer(HDC), "phDC")]),
    StdMethod(DDRESULT, "GetFlipStatus", [(DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "GetOverlayPosition", [Out(LPLONG, "lplX"), Out(LPLONG, "lplY")]),
    StdMethod(DDRESULT, "GetPalette", [Out(Pointer(LPDIRECTDRAWPALETTE), "lplpDDPalette")]),
    StdMethod(DDRESULT, "GetPixelFormat", [Out(LPDDPIXELFORMAT, "lpDDPixelFormat")]),
    StdMethod(DDRESULT, "GetSurfaceDesc", [Out(LPDDSURFACEDESC, "lpDDSurfaceDesc")]),
    StdMethod(DDRESULT, "Initialize", [(LPDIRECTDRAW, "lpDD"), (LPDDSURFACEDESC, "lpDDSurfaceDesc")]),
    StdMethod(DDRESULT, "IsLost", []),
    StdMethod(DDRESULT, "Lock", [(LPRECT, "lpDestRect"), (LPDDSURFACEDESC, "lpDDSurfaceDesc"), (DirectDrawSurfaceLockFlags, "dwFlags"), (HANDLE, "hEvent")]),
    StdMethod(DDRESULT, "ReleaseDC", [(HDC, "hDC")]),
    StdMethod(DDRESULT, "Restore", []),
    StdMethod(DDRESULT, "SetClipper", [(LPDIRECTDRAWCLIPPER, "lpDDClipper")]),
    StdMethod(DDRESULT, "SetColorKey", [(DirectDrawSurfaceSetGetColorKeyFlags, "dwFlags"), Out(LPDDCOLORKEY, "lpDDColorKey")]),
    StdMethod(DDRESULT, "SetOverlayPosition", [(LONG, "lX"), (LONG, "lY")]),
    StdMethod(DDRESULT, "SetPalette", [(LPDIRECTDRAWPALETTE, "lpDDPalette")]),
    StdMethod(DDRESULT, "Unlock", [(LPVOID, "lp")]),
    StdMethod(DDRESULT, "UpdateOverlay", [(LPRECT, "lpSrcRect"), (LPDIRECTDRAWSURFACE, "lpDDDestSurface"), (LPRECT, "lpDestRect"), (DirectDrawSurfaceOverlayFlags, "dwFlags"), (LPDDOVERLAYFX, "lpDDOverlayFx")]),
    StdMethod(DDRESULT, "UpdateOverlayDisplay", [(DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "UpdateOverlayZOrder", [(DirectDrawUpdateOverlayZOrderFlags, "dwFlags"), (LPDIRECTDRAWSURFACE, "lpDDSReference")]),
]

IDirectDrawSurface2.methods += [
    StdMethod(DDRESULT, "AddAttachedSurface", [(LPDIRECTDRAWSURFACE2, "lpDDSAttachedSurface")]),
    StdMethod(DDRESULT, "AddOverlayDirtyRect", [(LPRECT, "lpRect")]),
    StdMethod(DDRESULT, "Blt", [(LPRECT, "lpDestRect"), (LPDIRECTDRAWSURFACE2, "lpDDSrcSurface"), (LPRECT, "lpSrcRect"), (DirectDrawBltFlags, "dwFlags"), (LPDDBLTFX, "lpDDBltFx")]),
    StdMethod(DDRESULT, "BltBatch", [(LPDDBLTBATCH, "lpDDBltBatch"), (DWORD, "dwCount"), (DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "BltFast", [(DWORD, "dwX"), (DWORD, "dwY"), (LPDIRECTDRAWSURFACE2, "lpDDSrcSurface"), (LPRECT, "lpSrcRect"), (DWORD, "dwTrans")]),
    StdMethod(DDRESULT, "DeleteAttachedSurface", [(DWORD, "dwFlags"), (LPDIRECTDRAWSURFACE2, "lpDDSurface")]),
    StdMethod(DDRESULT, "EnumAttachedSurfaces", [(LPVOID, "lpContext"), (LPDDENUMSURFACESCALLBACK, "lpEnumSurfacesCallback")]),
    StdMethod(DDRESULT, "EnumOverlayZOrders", [(DirectDrawEnumOverlayZOrderFlags, "dwFlags"), (LPVOID, "lpContext"), (LPDDENUMSURFACESCALLBACK, "lpfnCallback")]),
    StdMethod(DDRESULT, "Flip", [(LPDIRECTDRAWSURFACE2, "lpDDSurfaceTargetOverride"), (DirectDrawFlipFlags, "dwFlags")]),
    StdMethod(DDRESULT, "GetAttachedSurface", [(LPDDSCAPS, "lpDDSCaps"), Out(Pointer(LPDIRECTDRAWSURFACE2), "lplpDDAttachedSurface")]),
    StdMethod(DDRESULT, "GetBltStatus", [(DirectDrawGetBltStatusFlags, "dwFlags")]),
    StdMethod(DDRESULT, "GetCaps", [Out(LPDDSCAPS, "lpDDSCaps")]),
    StdMethod(DDRESULT, "GetClipper", [Out(Pointer(LPDIRECTDRAWCLIPPER), "lplpDDClipper")]),
    StdMethod(DDRESULT, "GetColorKey", [(DirectDrawSurfaceSetGetColorKeyFlags, "dwFlags"), Out(LPDDCOLORKEY, "lpDDColorKey")]),
    StdMethod(DDRESULT, "GetDC", [Out(Pointer(HDC), "phDC")]),
    StdMethod(DDRESULT, "GetFlipStatus", [(DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "GetOverlayPosition", [Out(LPLONG, "lplX"), Out(LPLONG, "lplY")]),
    StdMethod(DDRESULT, "GetPalette", [Out(Pointer(LPDIRECTDRAWPALETTE), "lplpDDPalette")]),
    StdMethod(DDRESULT, "GetPixelFormat", [Out(LPDDPIXELFORMAT, "lpDDPixelFormat")]),
    StdMethod(DDRESULT, "GetSurfaceDesc", [Out(LPDDSURFACEDESC, "lpDDSurfaceDesc")]),
    StdMethod(DDRESULT, "Initialize", [(LPDIRECTDRAW, "lpDD"), (LPDDSURFACEDESC, "lpDDSurfaceDesc")]),
    StdMethod(DDRESULT, "IsLost", []),
    StdMethod(DDRESULT, "Lock", [(LPRECT, "lpDestRect"), (LPDDSURFACEDESC, "lpDDSurfaceDesc"), (DirectDrawSurfaceLockFlags, "dwFlags"), (HANDLE, "hEvent")]),
    StdMethod(DDRESULT, "ReleaseDC", [(HDC, "hDC")]),
    StdMethod(DDRESULT, "Restore", []),
    StdMethod(DDRESULT, "SetClipper", [(LPDIRECTDRAWCLIPPER, "lpDDClipper")]),
    StdMethod(DDRESULT, "SetColorKey", [(DirectDrawSurfaceSetGetColorKeyFlags, "dwFlags"), Out(LPDDCOLORKEY, "lpDDColorKey")]),
    StdMethod(DDRESULT, "SetOverlayPosition", [(LONG, "lX"), (LONG, "lY")]),
    StdMethod(DDRESULT, "SetPalette", [(LPDIRECTDRAWPALETTE, "lpDDPalette")]),
    StdMethod(DDRESULT, "Unlock", [(LPVOID, "lp")]),
    StdMethod(DDRESULT, "UpdateOverlay", [(LPRECT, "lpSrcRect"), (LPDIRECTDRAWSURFACE2, "lpDDDestSurface"), (LPRECT, "lpDestRect"), (DirectDrawSurfaceOverlayFlags, "dwFlags"), (LPDDOVERLAYFX, "lpDDOverlayFx")]),
    StdMethod(DDRESULT, "UpdateOverlayDisplay", [(DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "UpdateOverlayZOrder", [(DirectDrawUpdateOverlayZOrderFlags, "dwFlags"), (LPDIRECTDRAWSURFACE2, "lpDDSReference")]),
    StdMethod(DDRESULT, "GetDDInterface", [Out(LPUNKNOWN, "lplpDD")]),
    StdMethod(DDRESULT, "PageLock", [(DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "PageUnlock", [(DWORD, "dwFlags")]),
]

IDirectDrawSurface3.methods += [
    StdMethod(DDRESULT, "AddAttachedSurface", [(LPDIRECTDRAWSURFACE3, "lpDDSAttachedSurface")]),
    StdMethod(DDRESULT, "AddOverlayDirtyRect", [(LPRECT, "lpRect")]),
    StdMethod(DDRESULT, "Blt", [(LPRECT, "lpDestRect"), (LPDIRECTDRAWSURFACE3, "lpDDSrcSurface"), (LPRECT, "lpSrcRect"), (DirectDrawBltFlags, "dwFlags"), (LPDDBLTFX, "lpDDBltFx")]),
    StdMethod(DDRESULT, "BltBatch", [(LPDDBLTBATCH, "lpDDBltBatch"), (DWORD, "dwCount"), (DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "BltFast", [(DWORD, "dwX"), (DWORD, "dwY"), (LPDIRECTDRAWSURFACE3, "lpDDSrcSurface"), (LPRECT, "lpSrcRect"), (DWORD, "dwTrans")]),
    StdMethod(DDRESULT, "DeleteAttachedSurface", [(DWORD, "dwFlags"), (LPDIRECTDRAWSURFACE3, "lpDDSurface")]),
    StdMethod(DDRESULT, "EnumAttachedSurfaces", [(LPVOID, "lpContext"), (LPDDENUMSURFACESCALLBACK, "lpEnumSurfacesCallback")]),
    StdMethod(DDRESULT, "EnumOverlayZOrders", [(DirectDrawEnumOverlayZOrderFlags, "dwFlags"), (LPVOID, "lpContext"), (LPDDENUMSURFACESCALLBACK, "lpfnCallback")]),
    StdMethod(DDRESULT, "Flip", [(LPDIRECTDRAWSURFACE3, "lpDDSurfaceTargetOverride"), (DirectDrawFlipFlags, "dwFlags")]),
    StdMethod(DDRESULT, "GetAttachedSurface", [(LPDDSCAPS, "lpDDSCaps"), Out(Pointer(LPDIRECTDRAWSURFACE3), "lplpDDAttachedSurface")]),
    StdMethod(DDRESULT, "GetBltStatus", [(DirectDrawGetBltStatusFlags, "dwFlags")]),
    StdMethod(DDRESULT, "GetCaps", [Out(LPDDSCAPS, "lpDDSCaps")]),
    StdMethod(DDRESULT, "GetClipper", [Out(Pointer(LPDIRECTDRAWCLIPPER), "lplpDDClipper")]),
    StdMethod(DDRESULT, "GetColorKey", [(DirectDrawSurfaceSetGetColorKeyFlags, "dwFlags"), Out(LPDDCOLORKEY, "lpDDColorKey")]),
    StdMethod(DDRESULT, "GetDC", [Out(Pointer(HDC), "phDC")]),
    StdMethod(DDRESULT, "GetFlipStatus", [(DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "GetOverlayPosition", [Out(LPLONG, "lplX"), Out(LPLONG, "lplY")]),
    StdMethod(DDRESULT, "GetPalette", [Out(Pointer(LPDIRECTDRAWPALETTE), "lplpDDPalette")]),
    StdMethod(DDRESULT, "GetPixelFormat", [Out(LPDDPIXELFORMAT, "lpDDPixelFormat")]),
    StdMethod(DDRESULT, "GetSurfaceDesc", [Out(LPDDSURFACEDESC, "lpDDSurfaceDesc")]),
    StdMethod(DDRESULT, "Initialize", [(LPDIRECTDRAW, "lpDD"), (LPDDSURFACEDESC, "lpDDSurfaceDesc")]),
    StdMethod(DDRESULT, "IsLost", []),
    StdMethod(DDRESULT, "Lock", [(LPRECT, "lpDestRect"), (LPDDSURFACEDESC, "lpDDSurfaceDesc"), (DirectDrawSurfaceLockFlags, "dwFlags"), (HANDLE, "hEvent")]),
    StdMethod(DDRESULT, "ReleaseDC", [(HDC, "hDC")]),
    StdMethod(DDRESULT, "Restore", []),
    StdMethod(DDRESULT, "SetClipper", [(LPDIRECTDRAWCLIPPER, "lpDDClipper")]),
    StdMethod(DDRESULT, "SetColorKey", [(DirectDrawSurfaceSetGetColorKeyFlags, "dwFlags"), Out(LPDDCOLORKEY, "lpDDColorKey")]),
    StdMethod(DDRESULT, "SetOverlayPosition", [(LONG, "lX"), (LONG, "lY")]),
    StdMethod(DDRESULT, "SetPalette", [(LPDIRECTDRAWPALETTE, "lpDDPalette")]),
    StdMethod(DDRESULT, "Unlock", [(LPVOID, "lp")]),
    StdMethod(DDRESULT, "UpdateOverlay", [(LPRECT, "lpSrcRect"), (LPDIRECTDRAWSURFACE3, "lpDDDestSurface"), (LPRECT, "lpDestRect"), (DirectDrawSurfaceOverlayFlags, "dwFlags"), (LPDDOVERLAYFX, "lpDDOverlayFx")]),
    StdMethod(DDRESULT, "UpdateOverlayDisplay", [(DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "UpdateOverlayZOrder", [(DirectDrawUpdateOverlayZOrderFlags, "dwFlags"), (LPDIRECTDRAWSURFACE3, "lpDDSReference")]),
    StdMethod(DDRESULT, "GetDDInterface", [Out(LPUNKNOWN, "lplpDD")]),
    StdMethod(DDRESULT, "PageLock", [(DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "PageUnlock", [(DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "SetSurfaceDesc", [(LPDDSURFACEDESC, "lpDDSD"), (DWORD, "dwFlags")]),
]

IDirectDrawSurface4.methods += [
    StdMethod(DDRESULT, "AddAttachedSurface", [(LPDIRECTDRAWSURFACE4, "lpDDSAttachedSurface")]),
    StdMethod(DDRESULT, "AddOverlayDirtyRect", [(LPRECT, "lpRect")]),
    StdMethod(DDRESULT, "Blt", [(LPRECT, "lpDestRect"), (LPDIRECTDRAWSURFACE4, "lpDDSrcSurface"), (LPRECT, "lpSrcRect"), (DirectDrawBltFlags, "dwFlags"), (LPDDBLTFX, "lpDDBltFx")]),
    StdMethod(DDRESULT, "BltBatch", [(LPDDBLTBATCH, "lpDDBltBatch"), (DWORD, "dwCount"), (DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "BltFast", [(DWORD, "dwX"), (DWORD, "dwY"), (LPDIRECTDRAWSURFACE4, "lpDDSrcSurface"), (LPRECT, "lpSrcRect"), (DWORD, "dwTrans")]),
    StdMethod(DDRESULT, "DeleteAttachedSurface", [(DWORD, "dwFlags"), (LPDIRECTDRAWSURFACE4, "lpDDSurface")]),
    StdMethod(DDRESULT, "EnumAttachedSurfaces", [(LPVOID, "lpContext"), (LPDDENUMSURFACESCALLBACK2, "lpEnumSurfacesCallback")]),
    StdMethod(DDRESULT, "EnumOverlayZOrders", [(DirectDrawEnumOverlayZOrderFlags, "dwFlags"), (LPVOID, "lpContext"), (LPDDENUMSURFACESCALLBACK2, "lpfnCallback")]),
    StdMethod(DDRESULT, "Flip", [(LPDIRECTDRAWSURFACE4, "lpDDSurfaceTargetOverride"), (DirectDrawFlipFlags, "dwFlags")]),
    StdMethod(DDRESULT, "GetAttachedSurface", [(LPDDSCAPS2, "lpDDSCaps"), Out(Pointer(LPDIRECTDRAWSURFACE4), "lplpDDAttachedSurface")]),
    StdMethod(DDRESULT, "GetBltStatus", [(DirectDrawGetBltStatusFlags, "dwFlags")]),
    StdMethod(DDRESULT, "GetCaps", [Out(LPDDSCAPS2, "lpDDSCaps")]),
    StdMethod(DDRESULT, "GetClipper", [Out(Pointer(LPDIRECTDRAWCLIPPER), "lplpDDClipper")]),
    StdMethod(DDRESULT, "GetColorKey", [(DirectDrawSurfaceSetGetColorKeyFlags, "dwFlags"), Out(LPDDCOLORKEY, "lpDDColorKey")]),
    StdMethod(DDRESULT, "GetDC", [Out(Pointer(HDC), "phDC")]),
    StdMethod(DDRESULT, "GetFlipStatus", [(DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "GetOverlayPosition", [Out(LPLONG, "lplX"), Out(LPLONG, "lplY")]),
    StdMethod(DDRESULT, "GetPalette", [Out(Pointer(LPDIRECTDRAWPALETTE), "lplpDDPalette")]),
    StdMethod(DDRESULT, "GetPixelFormat", [Out(LPDDPIXELFORMAT, "lpDDPixelFormat")]),
    StdMethod(DDRESULT, "GetSurfaceDesc", [Out(LPDDSURFACEDESC2, "lpDDSurfaceDesc")]),
    StdMethod(DDRESULT, "Initialize", [(LPDIRECTDRAW, "lpDD"), (LPDDSURFACEDESC2, "lpDDSurfaceDesc")]),
    StdMethod(DDRESULT, "IsLost", []),
    StdMethod(DDRESULT, "Lock", [(LPRECT, "lpDestRect"), (LPDDSURFACEDESC2, "lpDDSurfaceDesc"), (DirectDrawSurfaceLockFlags, "dwFlags"), (HANDLE, "hEvent")]),
    StdMethod(DDRESULT, "ReleaseDC", [(HDC, "hDC")]),
    StdMethod(DDRESULT, "Restore", []),
    StdMethod(DDRESULT, "SetClipper", [(LPDIRECTDRAWCLIPPER, "lpDDClipper")]),
    StdMethod(DDRESULT, "SetColorKey", [(DirectDrawSurfaceSetGetColorKeyFlags, "dwFlags"), Out(LPDDCOLORKEY, "lpDDColorKey")]),
    StdMethod(DDRESULT, "SetOverlayPosition", [(LONG, "lX"), (LONG, "lY")]),
    StdMethod(DDRESULT, "SetPalette", [(LPDIRECTDRAWPALETTE, "lpDDPalette")]),
    StdMethod(DDRESULT, "Unlock", [(LPRECT, "lpRect")]),
    StdMethod(DDRESULT, "UpdateOverlay", [(LPRECT, "lpSrcRect"), (LPDIRECTDRAWSURFACE4, "lpDDDestSurface"), (LPRECT, "lpDestRect"), (DirectDrawSurfaceOverlayFlags, "dwFlags"), (LPDDOVERLAYFX, "lpDDOverlayFx")]),
    StdMethod(DDRESULT, "UpdateOverlayDisplay", [(DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "UpdateOverlayZOrder", [(DirectDrawUpdateOverlayZOrderFlags, "dwFlags"), (LPDIRECTDRAWSURFACE4, "lpDDSReference")]),
    StdMethod(DDRESULT, "GetDDInterface", [Out(Pointer(LPVOID), "lplpDD")]),
    StdMethod(DDRESULT, "PageLock", [(DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "PageUnlock", [(DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "SetSurfaceDesc", [(LPDDSURFACEDESC2, "lpDDSD"), (DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "SetPrivateData", [(REFGUID, "guidTag"), (LPVOID, "lpData"), (DWORD, "cbSize"), (DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "GetPrivateData", [(REFGUID, "guidTag"), (LPVOID, "lpData"), Out(LPDWORD, "lpcbBufferSize")]),
    StdMethod(DDRESULT, "FreePrivateData", [(REFGUID, "guidTag")]),
    StdMethod(DDRESULT, "GetUniquenessValue", [Out(LPDWORD, "lpValue")]),
    StdMethod(DDRESULT, "ChangeUniquenessValue", []),
]

IDirectDrawSurface7.methods += [
    StdMethod(DDRESULT, "AddAttachedSurface", [(LPDIRECTDRAWSURFACE7, "lpDDSAttachedSurface")]),
    StdMethod(DDRESULT, "AddOverlayDirtyRect", [(LPRECT, "lpRect")]),
    StdMethod(DDRESULT, "Blt", [(LPRECT, "lpDestRect"), (LPDIRECTDRAWSURFACE7, "lpDDSrcSurface"), (LPRECT, "lpSrcRect"), (DirectDrawBltFlags, "dwFlags"), (LPDDBLTFX, "lpDDBltFx")]),
    StdMethod(DDRESULT, "BltBatch", [(LPDDBLTBATCH, "lpDDBltBatch"), (DWORD, "dwCount"), (DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "BltFast", [(DWORD, "dwX"), (DWORD, "dwY"), (LPDIRECTDRAWSURFACE7, "lpDDSrcSurface"), (LPRECT, "lpSrcRect"), (DWORD, "dwTrans")]),
    StdMethod(DDRESULT, "DeleteAttachedSurface", [(DWORD, "dwFlags"), (LPDIRECTDRAWSURFACE7, "lpDDSurface")]),
    StdMethod(DDRESULT, "EnumAttachedSurfaces", [(LPVOID, "lpContext"), (LPDDENUMSURFACESCALLBACK7, "lpEnumSurfacesCallback")]),
    StdMethod(DDRESULT, "EnumOverlayZOrders", [(DirectDrawEnumOverlayZOrderFlags, "dwFlags"), (LPVOID, "lpContext"), (LPDDENUMSURFACESCALLBACK7, "lpfnCallback")]),
    StdMethod(DDRESULT, "Flip", [(LPDIRECTDRAWSURFACE7, "lpDDSurfaceTargetOverride"), (DirectDrawFlipFlags, "dwFlags")]),
    StdMethod(DDRESULT, "GetAttachedSurface", [(LPDDSCAPS2, "lpDDSCaps"), Out(Pointer(LPDIRECTDRAWSURFACE7), "lplpDDAttachedSurface")]),
    StdMethod(DDRESULT, "GetBltStatus", [(DirectDrawGetBltStatusFlags, "dwFlags")]),
    StdMethod(DDRESULT, "GetCaps", [Out(LPDDSCAPS2, "lpDDSCaps")]),
    StdMethod(DDRESULT, "GetClipper", [Out(Pointer(LPDIRECTDRAWCLIPPER), "lplpDDClipper")]),
    StdMethod(DDRESULT, "GetColorKey", [(DirectDrawSurfaceSetGetColorKeyFlags, "dwFlags"), Out(LPDDCOLORKEY, "lpDDColorKey")]),
    StdMethod(DDRESULT, "GetDC", [Out(Pointer(HDC), "phDC")]),
    StdMethod(DDRESULT, "GetFlipStatus", [(DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "GetOverlayPosition", [Out(LPLONG, "lplX"), Out(LPLONG, "lplY")]),
    StdMethod(DDRESULT, "GetPalette", [Out(Pointer(LPDIRECTDRAWPALETTE), "lplpDDPalette")]),
    StdMethod(DDRESULT, "GetPixelFormat", [Out(LPDDPIXELFORMAT, "lpDDPixelFormat")]),
    StdMethod(DDRESULT, "GetSurfaceDesc", [Out(LPDDSURFACEDESC2, "lpDDSurfaceDesc")]),
    StdMethod(DDRESULT, "Initialize", [(LPDIRECTDRAW, "lpDD"), (LPDDSURFACEDESC2, "lpDDSurfaceDesc")]),
    StdMethod(DDRESULT, "IsLost", []),
    StdMethod(DDRESULT, "Lock", [(LPRECT, "lpDestRect"), (LPDDSURFACEDESC2, "lpDDSurfaceDesc"), (DirectDrawSurfaceLockFlags, "dwFlags"), (HANDLE, "hEvent")]),
    StdMethod(DDRESULT, "ReleaseDC", [(HDC, "hDC")]),
    StdMethod(DDRESULT, "Restore", []),
    StdMethod(DDRESULT, "SetClipper", [(LPDIRECTDRAWCLIPPER, "lpDDClipper")]),
    StdMethod(DDRESULT, "SetColorKey", [(DirectDrawSurfaceSetGetColorKeyFlags, "dwFlags"), Out(LPDDCOLORKEY, "lpDDColorKey")]),
    StdMethod(DDRESULT, "SetOverlayPosition", [(LONG, "lX"), (LONG, "lY")]),
    StdMethod(DDRESULT, "SetPalette", [(LPDIRECTDRAWPALETTE, "lpDDPalette")]),
    StdMethod(DDRESULT, "Unlock", [(LPRECT, "lpRect")]),
    StdMethod(DDRESULT, "UpdateOverlay", [(LPRECT, "lpSrcRect"), (LPDIRECTDRAWSURFACE7, "lpDDDestSurface"), (LPRECT, "lpDestRect"), (DirectDrawSurfaceOverlayFlags, "dwFlags"), (LPDDOVERLAYFX, "lpDDOverlayFx")]),
    StdMethod(DDRESULT, "UpdateOverlayDisplay", [(DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "UpdateOverlayZOrder", [(DirectDrawUpdateOverlayZOrderFlags, "dwFlags"), (LPDIRECTDRAWSURFACE7, "lpDDSReference")]),
    StdMethod(DDRESULT, "GetDDInterface", [Out(Pointer(LPVOID), "lplpDD")]),
    StdMethod(DDRESULT, "PageLock", [(DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "PageUnlock", [(DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "SetSurfaceDesc", [(LPDDSURFACEDESC2, "lpDDSD"), (DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "SetPrivateData", [(REFGUID, "guidTag"), (LPVOID, "lpData"), (DWORD, "cbSize"), (DWORD, "dwFlags")]),
    StdMethod(DDRESULT, "GetPrivateData", [(REFGUID, "guidTag"), (LPVOID, "lpData"), Out(LPDWORD, "lpcbBufferSize")]),
    StdMethod(DDRESULT, "FreePrivateData", [(REFGUID, "guidTag")]),
    StdMethod(DDRESULT, "GetUniquenessValue", [Out(LPDWORD, "lpValue")]),
    StdMethod(DDRESULT, "ChangeUniquenessValue", []),
    StdMethod(DDRESULT, "SetPriority", [(DWORD, "dwPriority")]),
    StdMethod(DDRESULT, "GetPriority", [Out(LPDWORD, "lpdwPriority")]),
    StdMethod(DDRESULT, "SetLOD", [(DWORD, "dwMaxLOD")]),
    StdMethod(DDRESULT, "GetLOD", [Out(LPDWORD, "lpdwMaxLOD")]),
]

IDirectDrawColorControl.methods += [
    StdMethod(DDRESULT, "GetColorControls", [(LPDDCOLORCONTROL, "lpColorControl")]),
    StdMethod(DDRESULT, "SetColorControls", [(LPDDCOLORCONTROL, "lpColorControl")]),
]

IDirectDrawGammaControl.methods += [
    StdMethod(DDRESULT, "GetGammaRamp", [(DWORD, "dwFlags"), Out(LPDDGAMMARAMP, "lpRampData")]),
    StdMethod(DDRESULT, "SetGammaRamp", [(DirectDrawSetGammaRampFlags, "dwFlags"), (LPDDGAMMARAMP, "lpRampData")]),
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

ddraw = Module("ddraw")
ddraw.addFunctions([
    StdFunction(HRESULT, "DirectDrawEnumerateW", [(LPDDENUMCALLBACKW, "lpCallback"), (LPVOID, "lpContext")]),
    StdFunction(HRESULT, "DirectDrawEnumerateA", [(LPDDENUMCALLBACKA, "lpCallback"), (LPVOID, "lpContext")]),
    StdFunction(HRESULT, "DirectDrawEnumerateExW", [(LPDDENUMCALLBACKEXW, "lpCallback"), (LPVOID, "lpContext"), (DDENUM, "dwFlags")]),
    StdFunction(HRESULT, "DirectDrawEnumerateExA", [(LPDDENUMCALLBACKEXA, "lpCallback"), (LPVOID, "lpContext"), (DDENUM, "dwFlags")]),
    StdFunction(HRESULT, "DirectDrawCreate", [(Pointer(GUID), "lpGUID"), Out(Pointer(LPDIRECTDRAW), "lplpDD"), (LPUNKNOWN, "pUnkOuter")]),
    StdFunction(HRESULT, "DirectDrawCreateEx", [(Pointer(GUID), "lpGuid"), Out(Pointer(ObjPointer(Void)), "lplpDD"), (REFIID, "iid"), (LPUNKNOWN, "pUnkOuter")]),
    StdFunction(HRESULT, "DirectDrawCreateClipper", [(DWORD, "dwFlags"), Out(Pointer(LPDIRECTDRAWCLIPPER), "lplpDDClipper"), (LPUNKNOWN, "pUnkOuter")]),
    StdFunction(Void, "AcquireDDThreadLock", [], internal=True),
    StdFunction(Void, "ReleaseDDThreadLock", [], internal=True),
    StdFunction(DWORD, "D3DParseUnknownCommand", [(LPVOID, "lpCmd"), Out(Pointer(LPVOID), "lpRetCmd")]),
    StdFunction(HRESULT, "DllCanUnloadNow", []),
    StdFunction(HRESULT, "DllGetClassObject", [(REFCLSID, "rclsid"), (REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppv")]),

    # XXX
    StdFunction(HRESULT, "CompleteCreateSysmemSurface", [Int, Int]),
    StdFunction(HRESULT, "GetSurfaceFromDC", [Int, Int, Int]),
    StdFunction(HRESULT, "DDInternalLock", [Int, Int]),
    StdFunction(HRESULT, "DDInternalUnlock", [Int]),
    StdFunction(HRESULT, "DSoundHelp", [HWND, Int, Int]),
    StdFunction(HRESULT, "GetDDSurfaceLocal", [Int, Int, Int]),
    StdFunction(HANDLE, "GetOLEThunkData", [Int]),
    StdFunction(HRESULT, "RegisterSpecialCase", [Int, Int, Int, Int]),
    StdFunction(HRESULT, "DDGetAttachedSurfaceLcl", [Int, Int, Int]),

    # TODO: SetAppCompatData
])

