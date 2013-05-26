##########################################################################
#
# Copyright 2012 VMware, Inc.
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


from d3d9 import *

REFERENCE_TIME = Alias("REFERENCE_TIME", LONGLONG)

DXVA2_ProcAmp = Flags(UINT, [
    "DXVA2_ProcAmp_None",
    "DXVA2_ProcAmp_Brightness",
    "DXVA2_ProcAmp_Contrast",
    "DXVA2_ProcAmp_Hue",
    "DXVA2_ProcAmp_Saturation",
])

HRESULT = FakeEnum(HRESULT, [
    "DXVA2_E_NOT_INITIALIZED",
    "DXVA2_E_NEW_VIDEO_DEVICE",
    "DXVA2_E_VIDEO_DEVICE_LOCKED",
    "DXVA2_E_NOT_AVAILABLE",
])

DXVA2_SampleFormat = Enum("DXVA2_SampleFormat", [
    "DXVA2_SampleFormatMask",
    "DXVA2_SampleUnknown",
    "DXVA2_SampleProgressiveFrame",
    "DXVA2_SampleFieldInterleavedEvenFirst",
    "DXVA2_SampleFieldInterleavedOddFirst",
    "DXVA2_SampleFieldSingleEven",
    "DXVA2_SampleFieldSingleOdd",
    "DXVA2_SampleSubStream",
])

DXVA2_VideoChromaSubSampling = Enum("DXVA2_VideoChromaSubSampling", [
    "DXVA2_VideoChromaSubsamplingMask",
    "DXVA2_VideoChromaSubsampling_Unknown",
    "DXVA2_VideoChromaSubsampling_ProgressiveChroma",
    "DXVA2_VideoChromaSubsampling_Horizontally_Cosited",
    "DXVA2_VideoChromaSubsampling_Vertically_Cosited",
    "DXVA2_VideoChromaSubsampling_Vertically_AlignedChromaPlanes",
    "DXVA2_VideoChromaSubsampling_MPEG2",
    "DXVA2_VideoChromaSubsampling_MPEG1",
    "DXVA2_VideoChromaSubsampling_DV_PAL",
    "DXVA2_VideoChromaSubsampling_Cosited",
])

DXVA2_NominalRange = Enum("DXVA2_NominalRange", [
    "DXVA2_NominalRangeMask",
    "DXVA2_NominalRange_Unknown",
    "DXVA2_NominalRange_Normal",
    "DXVA2_NominalRange_Wide",
    "DXVA2_NominalRange_0_255",
    "DXVA2_NominalRange_16_235",
    "DXVA2_NominalRange_48_208",
])

DXVA2_VideoLighting = Enum("DXVA2_VideoLighting", [
    "DXVA2_VideoLightingMask",
    "DXVA2_VideoLighting_Unknown",
    "DXVA2_VideoLighting_bright",
    "DXVA2_VideoLighting_office",
    "DXVA2_VideoLighting_dim",
    "DXVA2_VideoLighting_dark",
])

DXVA2_VideoPrimaries = Enum("DXVA2_VideoPrimaries", [
    "DXVA2_VideoPrimariesMask",
    "DXVA2_VideoPrimaries_Unknown",
    "DXVA2_VideoPrimaries_reserved",
    "DXVA2_VideoPrimaries_BT709",
    "DXVA2_VideoPrimaries_BT470_2_SysM",
    "DXVA2_VideoPrimaries_BT470_2_SysBG",
    "DXVA2_VideoPrimaries_SMPTE170M",
    "DXVA2_VideoPrimaries_SMPTE240M",
    "DXVA2_VideoPrimaries_EBU3213",
    "DXVA2_VideoPrimaries_SMPTE_C",
])

DXVA2_VideoTransferFunction = Enum("DXVA2_VideoTransferFunction", [
    "DXVA2_VideoTransFuncMask",
    "DXVA2_VideoTransFunc_Unknown",
    "DXVA2_VideoTransFunc_10",
    "DXVA2_VideoTransFunc_18",
    "DXVA2_VideoTransFunc_20",
    "DXVA2_VideoTransFunc_22",
    "DXVA2_VideoTransFunc_709",
    "DXVA2_VideoTransFunc_240M",
    "DXVA2_VideoTransFunc_sRGB",
    "DXVA2_VideoTransFunc_28",
])

DXVA2_SurfaceType = FakeEnum(DWORD, [
    "DXVA2_SurfaceType_DecoderRenderTarget",
    "DXVA2_SurfaceType_ProcessorRenderTarget",
    "DXVA2_SurfaceType_D3DRenderTargetTexture",
])

DXVA2_VideoTransferMatrix = Enum("DXVA2_VideoTransferMatrix", [
    "DXVA2_VideoTransferMatrixMask",
    "DXVA2_VideoTransferMatrix_Unknown",
    "DXVA2_VideoTransferMatrix_BT709",
    "DXVA2_VideoTransferMatrix_BT601",
    "DXVA2_VideoTransferMatrix_SMPTE240M",
])

DXVA2_AYUVSample16 = Struct("DXVA2_AYUVSample16", [
    (USHORT, "Cr"),
    (USHORT, "Cb"),
    (USHORT, "Y"),
    (USHORT, "Alpha"),
])

DXVA2_AYUVSample8 = Struct("DXVA2_AYUVSample8", [
    (UCHAR, "Cr"),
    (UCHAR, "Cb"),
    (UCHAR, "Y"),
    (UCHAR, "Alpha"),
])

DXVA2_ConfigPictureDecode = Struct("DXVA2_ConfigPictureDecode", [
    (GUID, "guidConfigBitstreamEncryption"),
    (GUID, "guidConfigMBcontrolEncryption"),
    (GUID, "guidConfigResidDiffEncryption"),
    (UINT, "ConfigBitstreamRaw"),
    (UINT, "ConfigMBcontrolRasterOrder"),
    (UINT, "ConfigResidDiffHost"),
    (UINT, "ConfigSpatialResid8"),
    (UINT, "ConfigResid8Subtraction"),
    (UINT, "ConfigSpatialHost8or9Clipping"),
    (UINT, "ConfigSpatialResidInterleaved"),
    (UINT, "ConfigIntraResidUnsigned"),
    (UINT, "ConfigResidDiffAccelerator"),
    (UINT, "ConfigHostInverseScan"),
    (UINT, "ConfigSpecificIDCT"),
    (UINT, "Config4GroupedCoefs"),
    (USHORT, "ConfigMinRenderTargetBuffCount"),
    (USHORT, "ConfigDecoderSpecific"),
])

DXVA2_DecodeBufferDesc = Struct("DXVA2_DecodeBufferDesc", [
    (DWORD, "CompressedBufferType"),
    (UINT, "BufferIndex"),
    (UINT, "DataOffset"),
    (UINT, "DataSize"),
    (UINT, "FirstMBaddress"),
    (UINT, "NumMBsInBuffer"),
    (UINT, "Width"),
    (UINT, "Height"),
    (UINT, "Stride"),
    (UINT, "ReservedBits"),
    (PVOID, "pvPVPState"),
])

DXVA2_DecodeExtensionData = Struct("DXVA2_DecodeExtensionData", [
    (UINT, "Function"),
    (PVOID, "pPrivateInputData"),
    (UINT, "PrivateInputDataSize"),
    (PVOID, "pPrivateOutputData"),
    (UINT, "PrivateOutputDataSize"),
])

DXVA2_DecodeExecuteParams = Struct("DXVA2_DecodeExecuteParams", [
    (UINT, "NumCompBuffers"),
    (Array(DXVA2_DecodeBufferDesc, "{self}.NumCompBuffers"), "pCompressedBuffers"),
    (Pointer(DXVA2_DecodeExtensionData), "pExtensionData"),
])

DXVA2_ExtendedFormat = Struct("DXVA2_ExtendedFormat", [
    (UINT, "value"),
])

DXVA2_Fixed32 = Struct("DXVA2_Fixed32", [
    (USHORT, "Fraction"),
    (SHORT, "Value"),
])

DXVA2_FilterValues = Struct("DXVA2_FilterValues", [
    (DXVA2_Fixed32, "Level"),
    (DXVA2_Fixed32, "Threshold"),
    (DXVA2_Fixed32, "Radius"),
])

DXVA2_Frequency = Struct("DXVA2_Frequency", [
    (UINT, "Numerator"),
    (UINT, "Denominator"),
])

DXVA2_ProcAmpValues = Struct("DXVA2_ProcAmpValues", [
    (DXVA2_Fixed32, "Brightness"),
    (DXVA2_Fixed32, "Contrast"),
    (DXVA2_Fixed32, "Hue"),
    (DXVA2_Fixed32, "Saturation"),
])

DXVA2_ValueRange = Struct("DXVA2_ValueRange", [
    (DXVA2_Fixed32, "MinValue"),
    (DXVA2_Fixed32, "MaxValue"),
    (DXVA2_Fixed32, "DefaultValue"),
    (DXVA2_Fixed32, "StepSize"),
])

DXVA2_VideoDesc = Struct("DXVA2_VideoDesc", [
    (UINT, "SampleWidth"),
    (UINT, "SampleHeight"),
    (DXVA2_ExtendedFormat, "SampleFormat"),
    (D3DFORMAT, "Format"),
    (DXVA2_Frequency, "InputSampleFreq"),
    (DXVA2_Frequency, "OutputFrameFreq"),
    (UINT, "UABProtectionLevel"),
    (UINT, "Reserved"),
])

DXVA2_VideoProcessBltParams = Struct("DXVA2_VideoProcessBltParams", [
    (REFERENCE_TIME, "TargetFrame"),
    (RECT, "TargetRect"),
    (SIZE, "ConstrictionSize"),
    (UINT, "StreamingFlags"),
    (DXVA2_AYUVSample16, "BackgroundColor"),
    (DXVA2_ExtendedFormat, "DestFormat"),
    (DXVA2_ProcAmpValues, "ProcAmpValues"),
    (DXVA2_Fixed32, "Alpha"),
    (DXVA2_FilterValues, "NoiseFilterLuma"),
    (DXVA2_FilterValues, "NoiseFilterChroma"),
    (DXVA2_FilterValues, "DetailFilterLuma"),
    (DXVA2_FilterValues, "DetailFilterChroma"),
    (DWORD, "DestData"),
])

DXVA2_VideoProcessorCaps = Struct("DXVA2_VideoProcessorCaps", [
    (UINT, "DeviceCaps"),
    (D3DPOOL, "InputPool"),
    (UINT, "NumForwardRefSamples"),
    (UINT, "NumBackwardRefSamples"),
    (UINT, "Reserved"),
    (UINT, "DeinterlaceTechnology"),
    (UINT, "ProcAmpControlCaps"),
    (UINT, "VideoProcessorOperations"),
    (UINT, "NoiseFilterTechnology"),
    (UINT, "DetailFilterTechnology"),
])


# See also DXVADDI_PVP_KEY128
DXVA2_PVP_KEY128 = Struct('DXVA2_PVP_KEY128', [
    (Array(BYTE, 16), 'Data'),
])

# See also DXVADDI_PVP_SETKEY
DXVA2_PVP_SETKEY = Struct('DXVA2_PVP_SETKEY', [
    (DXVA2_PVP_KEY128, 'ContentKey'),
])

DXVA2_DECODEBUFFERDESC = Struct("DXVA2_DECODEBUFFERDESC", [
    (ObjPointer(IDirect3DSurface9), "pRenderTarget"),
    (DWORD, "CompressedBufferType"),
    (DWORD, "BufferIndex"),
    (DWORD, "DataOffset"),
    (DWORD, "DataSize"),
    (DWORD, "FirstMBaddress"),
    (DWORD, "NumMBsInBuffer"),
    (DWORD, "Width"),
    (DWORD, "Height"),
    (DWORD, "Stride"),
    (DWORD, "ReservedBits"),
    (PVOID, "pCipherCounter"),
])

DXVA2_DECODEEXECUTE = Struct("DXVA2_DECODEEXECUTE", [
    (UINT, "NumCompBuffers"),
    (Array(DXVA2_DECODEBUFFERDESC, "{self}.NumCompBuffers"), "pCompressedBuffers"),
])

DXVA2_VIDEOSAMPLE = Struct("DXVA2_VIDEOSAMPLE", [
    (REFERENCE_TIME, "Start"),
    (REFERENCE_TIME, "End"),
    (DXVA2_ExtendedFormat, "SampleFormat"),
    (DWORD, "SampleFlags"),
    (ObjPointer(IDirect3DSurface9), "SrcSurface"),
    (RECT, "SrcRect"),
    (RECT, "DstRect"),
    #(Array(DXVA2_AYUVSample8, 16), "Pal"),
    (DXVA2_Fixed32, "PlanarAlpha"),
])

DXVA2_VIDEOPROCESSBLT = Struct("DXVA2_VIDEOPROCESSBLT", [
    (REFERENCE_TIME, "TargetFrame"),
    (RECT, "TargetRect"),
    (SIZE, "ConstrictionSize"),
    (DWORD, "StreamingFlags"),
    (DXVA2_AYUVSample16, "BackgroundColor"),
    (DXVA2_ExtendedFormat, "DestFormat"),
    (DWORD, "DestFlags"),
    (DXVA2_ProcAmpValues, "ProcAmpValues"),
    (DXVA2_Fixed32, "Alpha"),
    (DXVA2_FilterValues, "NoiseFilterLuma"),
    (DXVA2_FilterValues, "NoiseFilterChroma"),
    (DXVA2_FilterValues, "DetailFilterLuma"),
    (DXVA2_FilterValues, "DetailFilterChroma"),
    (Array(DXVA2_VIDEOSAMPLE, "{self}.NumSrcSurfaces"), "pSrcSurfaces"),
    (UINT, "NumSrcSurfaces"),
])

DXVA2_EXTENSIONEXECUTE = Opaque('DXVA2_EXTENSIONEXECUTE')
DXVA2_DECODEBUFFERINFO = Opaque('DXVA2_DECODEBUFFERINFO')


IDirect3DDecodeDevice9 = Interface("IDirect3DDecodeDevice9", IUnknown)
IDirect3DDecodeDevice9.methods += [
    StdMethod(HRESULT, "DecodeBeginFrame", [(Pointer(DXVA2_PVP_SETKEY), "pPVPSetKey")]),
    StdMethod(HRESULT, "DecodeEndFrame", [(Pointer(HANDLE), "pHandleComplete")]),
    StdMethod(HRESULT, "DecodeSetRenderTarget", [(ObjPointer(IDirect3DSurface9), "pRenderTarget")]),
    StdMethod(HRESULT, "DecodeExecute", [(Pointer(DXVA2_DECODEEXECUTE), "pExecuteParams")]),
]

IDirect3DVideoProcessDevice9 = Interface("IDirect3DVideoProcessDevice9", IUnknown)
IDirect3DVideoProcessDevice9.methods += [
    StdMethod(HRESULT, "VideoProcessBeginFrame", []),
    StdMethod(HRESULT, "VideoProcessEndFrame", [(Pointer(HANDLE), "pHandleComplete")]),
    StdMethod(HRESULT, "VideoProcessSetRenderTarget", [(ObjPointer(IDirect3DSurface9), "pRenderTarget")]),
    StdMethod(HRESULT, "VideoProcessBlt", [(Pointer(DXVA2_VIDEOPROCESSBLT), "pData")]),
]

IDirect3DDXVAExtensionDevice9 = Interface("IDirect3DDXVAExtensionDevice9", IUnknown)
IDirect3DDXVAExtensionDevice9.methods += [
    StdMethod(HRESULT, "ExtensionExecute", [(OpaquePointer(DXVA2_EXTENSIONEXECUTE), "pData")]),
]

IDirect3DDxva2Container9 = Interface("IDirect3DDxva2Container9", IUnknown)
IDirect3DDxva2Container9.methods += [
    StdMethod(HRESULT, "CreateSurface", [(UINT, "Width"), (UINT, "Height"), (UINT, "BackBuffers"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), (DWORD, "Usage"), (DXVA2_SurfaceType, "DxvaType"), Out(Array(ObjPointer(IDirect3DSurface9), "1 + BackBuffers"), "ppSurface"), (Pointer(HANDLE), "pSharedHandle")]),
    StdMethod(HRESULT, "VidToSysBlt", [(ObjPointer(IDirect3DSurface9), "pSourceSurface"), (Pointer(RECT), "pSourceRect"), (ObjPointer(IDirect3DSurface9), "pDestSurface"), (Pointer(RECT), "pDestRect")]),
    StdMethod(HRESULT, "GetDecodeGuidCount", [Out(Pointer(UINT), "pCount")], sideeffects=False),
    StdMethod(HRESULT, "GetDecodeGuids", [(UINT, "Count"), Out(Array(GUID, "Count"), "pGuids")], sideeffects=False),
    StdMethod(HRESULT, "GetDecodeRenderTargetFormatCount", [(REFGUID, "Guid"), Out(Pointer(UINT), "pCount")], sideeffects=False),
    StdMethod(HRESULT, "GetDecodeRenderTargets", [(REFGUID, "Guid"), (UINT, "Count"), Out(Array(D3DFORMAT, "Count"), "pFormats")], sideeffects=False),
    StdMethod(HRESULT, "GetDecodeCompressedBufferCount", [(REFGUID, "Guid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), Out(Pointer(UINT), "pCount")], sideeffects=False),
    StdMethod(HRESULT, "GetDecodeCompressedBuffers", [(REFGUID, "Guid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), (UINT, "Count"), Out(OpaquePointer(DXVA2_DECODEBUFFERINFO), "pBufferInfo")], sideeffects=False),
    StdMethod(HRESULT, "GetDecodeConfigurationCount", [(REFGUID, "Guid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), Out(Pointer(UINT), "pCount")], sideeffects=False),
    StdMethod(HRESULT, "GetDecodeConfigurations", [(REFGUID, "Guid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), (UINT, "Count"), Out(Array(DXVA2_ConfigPictureDecode, "Count"), "pConfigs")], sideeffects=False),
    StdMethod(HRESULT, "CreateDecodeDevice", [(REFGUID, "Guid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), (Pointer(Const(DXVA2_ConfigPictureDecode)), "pConfig"), (Array(ObjPointer(IDirect3DSurface9), "NumSurfaces"), "ppDecoderRenderTargets"), (UINT, "NumSurfaces"), Out(Pointer(ObjPointer(IDirect3DDecodeDevice9)), "ppDecode")]),
    StdMethod(HRESULT, "GetVideoProcessorDeviceGuidCount", [(Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), Out(Pointer(UINT), "pCount")], sideeffects=False),
    StdMethod(HRESULT, "GetVideoProcessorDeviceGuids", [(Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), (UINT, "Count"), Out(Pointer(GUID), "pGuids")], sideeffects=False),
    StdMethod(HRESULT, "GetVideoProcessorCaps", [(REFGUID, "Guid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), (D3DFORMAT, "Format"), Out(Pointer(DXVA2_VideoProcessorCaps), "pCaps")], sideeffects=False),
    StdMethod(HRESULT, "GetProcAmpRange", [(REFGUID, "Guid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), (D3DFORMAT, "Format"), (UINT, "ProcAmpCap"), Out(Pointer(DXVA2_ValueRange), "pRange")]),
    StdMethod(HRESULT, "GetFilterPropertyRange", [(REFGUID, "Guid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), (D3DFORMAT, "Format"), (UINT, "FilterSetting"), Out(Pointer(DXVA2_ValueRange), "pRange")], sideeffects=False),
    StdMethod(HRESULT, "GetVideoProcessorRenderTargetCount", [(REFGUID, "Guid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), Out(Pointer(UINT), "pCount")], sideeffects=False),
    StdMethod(HRESULT, "GetVideoProcessorRenderTargets", [(REFGUID, "Guid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), (UINT, "Count"), Out(Array(D3DFORMAT, "Count"), "pFormats")], sideeffects=False),
    StdMethod(HRESULT, "GetVideoProcessorSubStreamFormatCount", [(REFGUID, "Guid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), (D3DFORMAT, "Format"), Out(Pointer(UINT), "pCount")], sideeffects=False),
    StdMethod(HRESULT, "GetVideoProcessorSubStreamFormats", [(REFGUID, "Guid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), (D3DFORMAT, "Format"), (UINT, "Count"), Out(Array(D3DFORMAT, "Count"), "pFormats")], sideeffects=False),
    StdMethod(HRESULT, "CreateVideoProcessDevice", [(REFGUID, "Guid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), (D3DFORMAT, "Format"), (UINT, "MaxSubStreams"), Out(Pointer(ObjPointer(IDirect3DVideoProcessDevice9)), "ppVideoProcessDevice")]),
    StdMethod(HRESULT, "GetExtensionGuidCount", [(DWORD, "Extension"), Out(Pointer(UINT), "pCount")], sideeffects=False),
    StdMethod(HRESULT, "GetExtensionGuids", [(DWORD, "Extension"), (UINT, "Count"), Out(Array(GUID, "Count"), "pGuids")], sideeffects=False),
    StdMethod(HRESULT, "GetExtensionCaps", [(REFGUID, "Guid"), (UINT, "arg2"), (OpaquePointer(Void), "arg3"), (UINT, "arg4"), (OpaquePointer(Void), "arg5"), (UINT, "arg6")], sideeffects=False),
    StdMethod(HRESULT, "CreateExtensionDevice", [(REFGUID, "Guid"), (OpaquePointer(Void), "arg2"), (UINT, "arg3"), Out(Pointer(ObjPointer(IDirect3DDXVAExtensionDevice9)), "ppExtension")]),
]

d3d9.addInterfaces([
    IDirect3DDxva2Container9,
])
