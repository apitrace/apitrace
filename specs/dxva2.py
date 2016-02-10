##########################################################################
#
# Copyright 2012-2015 VMware, Inc.
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


HRESULT = MAKE_HRESULT(errors = [
    "DXVA2_E_NOT_INITIALIZED",
    "DXVA2_E_NEW_VIDEO_DEVICE",
    "DXVA2_E_VIDEO_DEVICE_LOCKED",
    "DXVA2_E_NOT_AVAILABLE",
])

DXVA2_SampleFormat = FakeEnum(UINT, [
    "DXVA2_SampleUnknown",
    "DXVA2_SampleProgressiveFrame",
    "DXVA2_SampleFieldInterleavedEvenFirst",
    "DXVA2_SampleFieldInterleavedOddFirst",
    "DXVA2_SampleFieldSingleEven",
    "DXVA2_SampleFieldSingleOdd",
    "DXVA2_SampleSubStream",
])

DXVA2_VideoChromaSubSampling = FakeEnum(UINT, [
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

DXVA2_NominalRange = FakeEnum(UINT, [
    "DXVA2_NominalRange_Unknown",
    "DXVA2_NominalRange_Normal",
    "DXVA2_NominalRange_Wide",
    "DXVA2_NominalRange_0_255",
    "DXVA2_NominalRange_16_235",
    "DXVA2_NominalRange_48_208",
])

DXVA2_VideoTransferMatrix = FakeEnum(UINT, [
    "DXVA2_VideoTransferMatrix_Unknown",
    "DXVA2_VideoTransferMatrix_BT709",
    "DXVA2_VideoTransferMatrix_BT601",
    "DXVA2_VideoTransferMatrix_SMPTE240M",
])

DXVA2_VideoLighting = FakeEnum(UINT, [
    "DXVA2_VideoLighting_Unknown",
    "DXVA2_VideoLighting_bright",
    "DXVA2_VideoLighting_office",
    "DXVA2_VideoLighting_dim",
    "DXVA2_VideoLighting_dark",
])

DXVA2_VideoPrimaries = FakeEnum(UINT, [
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

DXVA2_VideoTransferFunction = FakeEnum(UINT, [
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

DXVA2_ExtendedFormat = Struct("DXVA2_ExtendedFormat", [
    (DXVA2_SampleFormat, "SampleFormat"),
    (DXVA2_VideoChromaSubSampling, "VideoChromaSubsampling"),
    (DXVA2_NominalRange, "NominalRange"),
    (DXVA2_VideoTransferMatrix, "VideoTransferMatrix"),
    (DXVA2_VideoLighting, "VideoLighting"),
    (DXVA2_VideoPrimaries, "VideoPrimaries"),
    (DXVA2_VideoTransferFunction, "VideoTransferFunction"),
])

DXVA2_Frequency = Struct("DXVA2_Frequency", [
    (UINT, "Numerator"),
    (UINT, "Denominator"),
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

DXVA2_DeinterlaceTech = Flags(UINT, [
    "DXVA2_DeinterlaceTech_Unknown",
    "DXVA2_DeinterlaceTech_BOBLineReplicate",
    "DXVA2_DeinterlaceTech_BOBVerticalStretch",
    "DXVA2_DeinterlaceTech_BOBVerticalStretch4Tap",
    "DXVA2_DeinterlaceTech_MedianFiltering",
    "DXVA2_DeinterlaceTech_EdgeFiltering",
    "DXVA2_DeinterlaceTech_FieldAdaptive",
    "DXVA2_DeinterlaceTech_PixelAdaptive",
    "DXVA2_DeinterlaceTech_MotionVectorSteered",
    "DXVA2_DeinterlaceTech_InverseTelecine",
])

DXVA2_Filter = Enum("DXVA2_Filter", [
    "DXVA2_NoiseFilterLumaLevel",
    "DXVA2_NoiseFilterLumaThreshold",
    "DXVA2_NoiseFilterLumaRadius",
    "DXVA2_NoiseFilterChromaLevel",
    "DXVA2_NoiseFilterChromaThreshold",
    "DXVA2_NoiseFilterChromaRadius",
    "DXVA2_DetailFilterLumaLevel",
    "DXVA2_DetailFilterLumaThreshold",
    "DXVA2_DetailFilterLumaRadius",
    "DXVA2_DetailFilterChromaLevel",
    "DXVA2_DetailFilterChromaThreshold",
    "DXVA2_DetailFilterChromaRadius",
])

DXVA2_NoiseFilterTech = Flags(UINT, [
    "DXVA2_NoiseFilterTech_Unsupported",
    "DXVA2_NoiseFilterTech_Unknown",
    "DXVA2_NoiseFilterTech_Median",
    "DXVA2_NoiseFilterTech_Temporal",
    "DXVA2_NoiseFilterTech_BlockNoise",
    "DXVA2_NoiseFilterTech_MosquitoNoise",
])

DXVA2_DetailFilterTech = Flags(UINT, [
    "DXVA2_DetailFilterTech_Unsupported",
    "DXVA2_DetailFilterTech_Unknown",
    "DXVA2_DetailFilterTech_Edge",
    "DXVA2_DetailFilterTech_Sharpening",
])

DXVA2_ProcAmp = Flags(UINT, [
    "DXVA2_ProcAmp_None",
    "DXVA2_ProcAmp_Brightness",
    "DXVA2_ProcAmp_Contrast",
    "DXVA2_ProcAmp_Hue",
    "DXVA2_ProcAmp_Saturation",
])

DXVA2_VideoProcess = Flags(UINT, [
    "DXVA2_VideoProcess_None",
    "DXVA2_VideoProcess_YUV2RGB",
    "DXVA2_VideoProcess_StretchX",
    "DXVA2_VideoProcess_StretchY",
    "DXVA2_VideoProcess_AlphaBlend",
    "DXVA2_VideoProcess_SubRects",
    "DXVA2_VideoProcess_SubStreams",
    "DXVA2_VideoProcess_SubStreamsExtended",
    "DXVA2_VideoProcess_YUV2RGBExtended",
    "DXVA2_VideoProcess_AlphaBlendExtended",
    "DXVA2_VideoProcess_Constriction",
    "DXVA2_VideoProcess_NoiseFilter",
    "DXVA2_VideoProcess_DetailFilter",
    "DXVA2_VideoProcess_PlanarAlpha",
    "DXVA2_VideoProcess_LinearScaling",
    "DXVA2_VideoProcess_GammaCompensated",
    "DXVA2_VideoProcess_MaintainsOriginalFieldData",
])

DXVA2_VPDev = Flags(UINT, [
    "DXVA2_VPDev_HardwareDevice",
    "DXVA2_VPDev_EmulatedDXVA1",
    "DXVA2_VPDev_SoftwareDevice",
])

DXVA2_SampleData = Flags(UINT, [
    "DXVA2_SampleData_RFF",
    "DXVA2_SampleData_TFF",
    "DXVA2_SampleData_RFF_TFF_Present",
])

DXVA2_DestData = Flags(UINT, [
    "DXVA2_DestData_RFF",
    "DXVA2_DestData_TFF",
    "DXVA2_DestData_RFF_TFF_Present",
])

DXVA2_VideoProcessorCaps = Struct("DXVA2_VideoProcessorCaps", [
    (DXVA2_VPDev, "DeviceCaps"),
    (D3DPOOL, "InputPool"),
    (UINT, "NumForwardRefSamples"),
    (UINT, "NumBackwardRefSamples"),
    (UINT, "Reserved"),
    (DXVA2_DeinterlaceTech, "DeinterlaceTechnology"),
    (DXVA2_ProcAmp, "ProcAmpControlCaps"),
    (DXVA2_VideoProcess, "VideoProcessorOperations"),
    (DXVA2_NoiseFilterTech, "NoiseFilterTechnology"),
    (DXVA2_DetailFilterTech, "DetailFilterTechnology"),
])

DXVA2_Fixed32 = Struct("DXVA2_Fixed32", [
    (USHORT, "Fraction"),
    (SHORT, "Value"),
])

DXVA2_AYUVSample8 = Struct("DXVA2_AYUVSample8", [
    (UCHAR, "Cr"),
    (UCHAR, "Cb"),
    (UCHAR, "Y"),
    (UCHAR, "Alpha"),
])

DXVA2_AYUVSample16 = Struct("DXVA2_AYUVSample16", [
    (USHORT, "Cr"),
    (USHORT, "Cb"),
    (USHORT, "Y"),
    (USHORT, "Alpha"),
])

REFERENCE_TIME = Alias("REFERENCE_TIME", LONGLONG)
DXVA2_VideoSample = Struct("DXVA2_VideoSample", [
    (REFERENCE_TIME, "Start"),
    (REFERENCE_TIME, "End"),
    (DXVA2_ExtendedFormat, "SampleFormat"),
    (ObjPointer(IDirect3DSurface9), "SrcSurface"),
    (RECT, "SrcRect"),
    (RECT, "DstRect"),
    (Array(DXVA2_AYUVSample8, 16), "Pal"),
    (DXVA2_Fixed32, "PlanarAlpha"),
    (DWORD, "SampleData"),
])

DXVA2_ValueRange = Struct("DXVA2_ValueRange", [
    (DXVA2_Fixed32, "MinValue"),
    (DXVA2_Fixed32, "MaxValue"),
    (DXVA2_Fixed32, "DefaultValue"),
    (DXVA2_Fixed32, "StepSize"),
])

DXVA2_ProcAmpValues = Struct("DXVA2_ProcAmpValues", [
    (DXVA2_Fixed32, "Brightness"),
    (DXVA2_Fixed32, "Contrast"),
    (DXVA2_Fixed32, "Hue"),
    (DXVA2_Fixed32, "Saturation"),
])

DXVA2_FilterValues = Struct("DXVA2_FilterValues", [
    (DXVA2_Fixed32, "Level"),
    (DXVA2_Fixed32, "Threshold"),
    (DXVA2_Fixed32, "Radius"),
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

DXVA2_BufferType = FakeEnum(UINT, [
    "DXVA2_PictureParametersBufferType",
    "DXVA2_MacroBlockControlBufferType",
    "DXVA2_ResidualDifferenceBufferType",
    "DXVA2_DeblockingControlBufferType",
    "DXVA2_InverseQuantizationMatrixBufferType",
    "DXVA2_SliceControlBufferType",
    "DXVA2_BitStreamDateBufferType",
    "DXVA2_MotionVectorBuffer",
    "DXVA2_FilmGrainBuffer",
])

DXVA2_Type = FakeEnum(DWORD, [
    "DXVA2_VideoDecoderRenderTarget",
    "DXVA2_VideoProcessorRenderTarget",
    "DXVA2_VideoSoftwareRenderTarget",
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

DXVA2_AES_CTR_IV = Struct("DXVA2_AES_CTR_IV", [
    (UINT64, "IV"),
    (UINT64, "Count"),
])

DXVA2_DecodeBufferDesc = Struct("DXVA2_DecodeBufferDesc", [
    (DXVA2_BufferType, "CompressedBufferType"),
    (UINT, "BufferIndex"),
    (UINT, "DataOffset"),
    (UINT, "DataSize"),
    (UINT, "FirstMBaddress"),
    (UINT, "NumMBsInBuffer"),
    (UINT, "Width"),
    (UINT, "Height"),
    (UINT, "Stride"),
    (UINT, "ReservedBits"),
    (Blob(VOID, "sizeof(DXVA2_AES_CTR_IV)"), "pvPVPState"),
])

DXVA2_DecodeExtensionData = Struct("DXVA2_DecodeExtensionData", [
    (UINT, "Function"),
    (Blob(Void, "{self}.PrivateInputDataSize"), "pPrivateInputData"),
    (UINT, "PrivateInputDataSize"),
    (Blob(Void, "{self}.PrivateOutputDataSize"), "pPrivateOutputData"),
    (UINT, "PrivateOutputDataSize"),
])

DXVA2_DecodeExecuteParams = Struct("DXVA2_DecodeExecuteParams", [
    (UINT, "NumCompBuffers"),
    (Array(DXVA2_DecodeBufferDesc, "{self}.NumCompBuffers"), "pCompressedBuffers"),
    (Pointer(DXVA2_DecodeExtensionData), "pExtensionData"),
])

RESET_TOKEN = Handle("resetToken", UINT)

IDirect3DDeviceManager9 = Interface("IDirect3DDeviceManager9", IUnknown)
IDirectXVideoAccelerationService = Interface("IDirectXVideoAccelerationService", IUnknown)
IDirectXVideoDecoderService = Interface("IDirectXVideoDecoderService", IDirectXVideoAccelerationService)
IDirectXVideoProcessorService = Interface("IDirectXVideoProcessorService", IDirectXVideoAccelerationService)
IDirectXVideoDecoder = Interface("IDirectXVideoDecoder", IUnknown)
IDirectXVideoProcessor = Interface("IDirectXVideoProcessor", IUnknown)

IDirect3DDeviceManager9.methods += [
    StdMethod(HRESULT, "ResetDevice", [(ObjPointer(IDirect3DDevice9), "pDevice"), (RESET_TOKEN, "resetToken")]),
    StdMethod(HRESULT, "OpenDeviceHandle", [Out(Pointer(HANDLE), "phDevice")]),
    StdMethod(HRESULT, "CloseDeviceHandle", [(HANDLE, "hDevice")]),
    StdMethod(HRESULT, "TestDevice", [(HANDLE, "hDevice")]),
    StdMethod(HRESULT, "LockDevice", [(HANDLE, "hDevice"), Out(Pointer(ObjPointer(IDirect3DDevice9)), "ppDevice"), (BOOL, "fBlock")]),
    StdMethod(HRESULT, "UnlockDevice", [(HANDLE, "hDevice"), (BOOL, "fSaveState")]),
    StdMethod(HRESULT, "GetVideoService", [(HANDLE, "hDevice"), (REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppService")]),
]

IDirectXVideoAccelerationService.methods += [
    StdMethod(HRESULT, "CreateSurface", [(UINT, "Width"), (UINT, "Height"), (UINT, "BackBuffers"), (D3DFORMAT, "Format"), (D3DPOOL, "Pool"), (D3DUSAGE, "Usage"), (DXVA2_Type, "DxvaType"), Out(Array(ObjPointer(IDirect3DSurface9), "1 + BackBuffers"), "ppSurface"), Out(Pointer(HANDLE), "pSharedHandle")]),
]

IDirectXVideoDecoderService.methods += [
    StdMethod(HRESULT, "GetDecoderDeviceGuids", [Out(Pointer(UINT), "pCount"), Out(Pointer(Array(GUID, "pCount ? *pCount : 0")), "pGuids")], sideeffects=False),
    StdMethod(HRESULT, "GetDecoderRenderTargets", [(REFGUID, "Guid"), Out(Pointer(UINT), "pCount"), Out(Pointer(Array(D3DFORMAT, "pCount ? *pCount : 0")), "pFormats")], sideeffects=False),
    StdMethod(HRESULT, "GetDecoderConfigurations", [(REFGUID, "Guid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), (OpaquePointer(Void), "pReserved"), Out(Pointer(UINT), "pCount"), Out(Pointer(Array(DXVA2_ConfigPictureDecode, "pCount ? *pCount : 0")), "ppConfigs")], sideeffects=False),
    StdMethod(HRESULT, "CreateVideoDecoder", [(REFGUID, "Guid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), (Pointer(Const(DXVA2_ConfigPictureDecode)), "pConfig"), (Array(ObjPointer(IDirect3DSurface9), "NumRenderTargets"), "ppDecoderRenderTargets"), (UINT, "NumRenderTargets"), Out(Pointer(ObjPointer(IDirectXVideoDecoder)), "ppDecode")]),
]

IDirectXVideoProcessorService.methods += [
    StdMethod(HRESULT, "RegisterVideoProcessorSoftwareDevice", [(OpaquePointer(Void), "pCallbacks")], sideeffects=False),
    StdMethod(HRESULT, "GetVideoProcessorDeviceGuids", [(Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), Out(Pointer(UINT), "pCount"), Out(Pointer(Array(GUID, "pCount ? *pCount : 0")), "pGuids")], sideeffects=False),
    StdMethod(HRESULT, "GetVideoProcessorRenderTargets", [(REFGUID, "VideoProcDeviceGuid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), Out(Pointer(UINT), "pCount"), Out(Pointer(Array(D3DFORMAT, "pCount ? *pCount : 0")), "pFormats")], sideeffects=False),
    StdMethod(HRESULT, "GetVideoProcessorSubStreamFormats", [(REFGUID, "VideoProcDeviceGuid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), (D3DFORMAT, "RenderTargetFormat"), Out(Pointer(UINT), "pCount"), Out(Pointer(Array(D3DFORMAT, "pCount ? *pCount : 0")), "pFormats")], sideeffects=False),
    StdMethod(HRESULT, "GetVideoProcessorCaps", [(REFGUID, "VideoProcDeviceGuid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), (D3DFORMAT, "RenderTargetFormat"), Out(Pointer(DXVA2_VideoProcessorCaps), "pCaps")], sideeffects=False),
    StdMethod(HRESULT, "GetProcAmpRange", [(REFGUID, "VideoProcDeviceGuid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), (D3DFORMAT, "RenderTargetFormat"), (UINT, "ProcAmpCap"), Out(Pointer(DXVA2_ValueRange), "pRange")], sideeffects=False),
    StdMethod(HRESULT, "GetFilterPropertyRange", [(REFGUID, "VideoProcDeviceGuid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), (D3DFORMAT, "RenderTargetFormat"), (UINT, "FilterSetting"), Out(Pointer(DXVA2_ValueRange), "pRange")], sideeffects=False),
    StdMethod(HRESULT, "CreateVideoProcessor", [(REFGUID, "VideoProcDeviceGuid"), (Pointer(Const(DXVA2_VideoDesc)), "pVideoDesc"), (D3DFORMAT, "RenderTargetFormat"), (UINT, "MaxNumSubStreams"), Out(Pointer(ObjPointer(IDirectXVideoProcessor)), "ppVidProcess")]),
]

IDirectXVideoDecoder.methods += [
    StdMethod(HRESULT, "GetVideoDecoderService", [Out(Pointer(ObjPointer(IDirectXVideoDecoderService)), "ppService")]),
    StdMethod(HRESULT, "GetCreationParameters", [Out(Pointer(GUID), "pDeviceGuid"), Out(Pointer(DXVA2_VideoDesc), "pVideoDesc"), Out(Pointer(DXVA2_ConfigPictureDecode), "pConfig"), Out(Pointer(Pointer(ObjPointer(IDirect3DSurface9))), "pDecoderRenderTargets"), Out(Pointer(UINT), "pNumSurfaces")]),
    StdMethod(HRESULT, "GetBuffer", [(DXVA2_BufferType, "BufferType"), Out(Pointer(LinearPointer(Void, "*pBufferSize")), "ppBuffer"), Out(Pointer(UINT), "pBufferSize")]),
    StdMethod(HRESULT, "ReleaseBuffer", [(DXVA2_BufferType, "BufferType")]),
    StdMethod(HRESULT, "BeginFrame", [(ObjPointer(IDirect3DSurface9), "pRenderTarget"), (Blob(Void, 16), "pvPVPData")]),
    StdMethod(HRESULT, "EndFrame", [Out(Pointer(HANDLE), "pHandleComplete")]),
    StdMethod(HRESULT, "Execute", [(Pointer(Const(DXVA2_DecodeExecuteParams)), "pExecuteParams")]),
]

IDirectXVideoProcessor.methods += [
    StdMethod(HRESULT, "GetVideoProcessorService", [Out(Pointer(ObjPointer(IDirectXVideoProcessorService)), "ppService")]),
    StdMethod(HRESULT, "GetCreationParameters", [Out(Pointer(GUID), "pDeviceGuid"), Out(Pointer(DXVA2_VideoDesc), "pVideoDesc"), Out(Pointer(D3DFORMAT), "pRenderTargetFormat"), Out(Pointer(UINT), "pMaxNumSubStreams")], sideeffects=False),
    StdMethod(HRESULT, "GetVideoProcessorCaps", [Out(Pointer(DXVA2_VideoProcessorCaps), "pCaps")], sideeffects=False),
    StdMethod(HRESULT, "GetProcAmpRange", [(UINT, "ProcAmpCap"), Out(Pointer(DXVA2_ValueRange), "pRange")], sideeffects=False),
    StdMethod(HRESULT, "GetFilterPropertyRange", [(UINT, "FilterSetting"), Out(Pointer(DXVA2_ValueRange), "pRange")], sideeffects=False),
    StdMethod(HRESULT, "VideoProcessBlt", [(ObjPointer(IDirect3DSurface9), "pRenderTarget"), (Pointer(Const(DXVA2_VideoProcessBltParams)), "pBltParams"), (Array(Const(DXVA2_VideoSample), "NumSamples"), "pSamples"), (UINT, "NumSamples"), Out(Pointer(HANDLE), "pHandleComplete")]),
]

DXVA2_SurfaceType = Enum("DXVA2_SurfaceType", [
    "DXVA2_SurfaceType_DecoderRenderTarget",
    "DXVA2_SurfaceType_ProcessorRenderTarget",
    "DXVA2_SurfaceType_D3DRenderTargetTexture",
])

IDirectXVideoMemoryConfiguration = Interface("IDirectXVideoMemoryConfiguration", IUnknown)
IDirectXVideoMemoryConfiguration.methods += [
    StdMethod(HRESULT, "GetAvailableSurfaceTypeByIndex", [(DWORD, "dwTypeIndex"), Out(Pointer(DXVA2_SurfaceType), "pdwType")], sideeffects=False),
    StdMethod(HRESULT, "SetSurfaceType", [(DXVA2_SurfaceType, "dwType")]),
]

dxva2 = Module("dxva2")
dxva2.addInterfaces([
    IDirectXVideoAccelerationService,
    IDirectXVideoDecoderService,
    IDirectXVideoProcessorService,
    IDirectXVideoMemoryConfiguration,
])

dxva2.addFunctions([
    StdFunction(HRESULT, "DXVA2CreateDirect3DDeviceManager9", [Out(Pointer(RESET_TOKEN), "pResetToken"), Out(Pointer(ObjPointer(IDirect3DDeviceManager9)), "ppDeviceManager")]),
    StdFunction(HRESULT, "DXVA2CreateVideoService", [(ObjPointer(IDirect3DDevice9), "pDD"), (REFIID, "riid"), Out(Pointer(ObjPointer(Void)), "ppService")]),
])
