DIRECTX TEXTURE LIBRARY (DirectXTex)
------------------------------------

Copyright (c) Microsoft Corporation. All rights reserved.

November 15, 2012

This package contains DirectXTex, a shared source library for reading and writing DDS
files, and performing various texture content processing operations including
resizing, format conversion, mip-map generation, block compression for Direct3D runtime
texture resources, and height-map to normal-map conversion. This library makes
use of the Windows Image Component (WIC) APIs. It also includes a simple .TGA reader and
writer since this image file format is commonly used for texture content processing pipelines,
but is not currently supported by a built-in WIC codec.

The source is written for Visual C++ 2010 using the Direct3D headers from either
a current DirectX SDK or Windows SDK. It can also be compiled using Visual Studio 2012 and the
Windows SDK 8.0 headers.

It is recommended that you make use of Visual C++ 2010 Service Pack 1 or VS 2012, and
Windows 7 Service Pack 1 or Windows 8.

DDSTextureLoader\
    This contains a streamlined version of the DirectX SDK sample DDSWithoutD3DX11 texture
    loading code for a simple light-weight runtime DDS loader. This version only supports
    Direct3D 11 and performs no runtime pixel data conversions (i.e. 24bpp legacy DDS files
    always fail). This is ideal for runtime usage, and supports the full complement of
    Direct3D 11 texture resources (1D, 2D, volume maps, cubemaps, mipmap levels,
    texture arrays, BC formats, etc.).

WICTextureLoader\
    This contains a Direct3D 11 2D texture loader that uses WIC to load a bitmap
    (BMP, JPEG, PNG, HD Photo, or other WIC supported file container), resize if needed
    based on the current feature level (or by explicit parameter), format convert to a
    DXGI_FORMAT if required, and then create a 2D texture. Furthermore, if a Direct3D 11
    device context is provided and the current device supports it for the given pixel format,
    it will auto-generate mipmaps. Note this does not support 1D textures, volume textures,
    cubemaps, or texture arrays. DDSTextureLoader is recommended for fully "precooked" textures
    for maximum performance and image quality, but this loader can be useful for creating
    simple 2D texture from standard image files at runtime.

    Note: This function is not thread-safe if given a non-NULL device context for the auto-gen
    mip-map support.

DirectXTex\
    This contains the DirectXTex library. This includes a full-featured DDS reader and writer
    including legacy format conversions, a TGA reader and writer, a WIC-based bitmap reader and
    writer (BMP, JPEG, PNG, TIFF, and HD Photo), and various texture processing functions. This
    is intended primarily for tool usage.

    Note that the majority of the header files here are intended for internal implementation
    of the library only (BC.h, DDS.h, DirectXTexP.h, and scoped.h). Only DirectXTex.h is
    meant as a 'public' header for the library.

Texconv\
    This DirectXTex sample is an implementation of the "texconv" command-line texture utility
    from the DirectX SDK utilizing DirectXTex rather than D3DX.

    It supports the same arguments as the Texture Conversion Tool Extended (texconvex.exe) DirectX
    SDK utility. See <http://msdn.microsoft.com/en-us/library/ee422506.aspx>. The primary differences
    are the -10 and -11 arguments are not applicable; the filter names (POINT, LINEAR, CUBIC,
    FANT, POINT_DITHER, LINEAR_DITHER, CUBIC_DITHER, FANT_DITHER); and support for the .TGA file format.
    This also includes support for JPEG XR/HD Photo bitmap formats (see
    <http://blogs.msdn.com/b/chuckw/archive/2011/01/19/known-issue-texconvex.aspx>)
    
DDSView\
    This DirectXTex sample is a simple Direct3D 11-based viewer for DDS files. For array textures
    or volume maps, the "<" and ">" keyboard keys will show different images contained in the DDS.
    The "1" through "0" keys can also be used to jump to a specific image index.

XNAMath\
    This contains a copy of XNA Math version 2.05, which is an updated version of the library. This is
    required if building content with USE_XNAMATH (the default for the VS 2010 projects). The VS 2012
    projects use DirectXMath in the Windows SDK 8.0 instead.
    For details see
    <http://blogs.msdn.com/b/chuckw/archive/2012/06/22/xna-math-version-2-05-smoothing-the-transition-to-directxmath.aspx>

All content and source code for this package except XNA Math are bound to the Microsoft Public License (Ms-PL)
<http://www.microsoft.com/en-us/openness/licenses.aspx#MPL>. The XNA Math library is subject
to the DirectX SDK (June 2010) End-User License Agreement.

http://go.microsoft.com/fwlink/?LinkId=248926


------------------------------------
RELEASE NOTES

* The DirectXTex library does not support block compression or decompression of mipmapped non-power-of-2 textures,
  although DDSTextureLoader will load these files correctly if the underlying device supports it.

* The DirectXTex library only supports CLAMP filtering, and does not yet support MIRROR or WRAP filtering
  (WIC operations only support CLAMP filtering).

* The DirectXTex library only supports box and POINT filtering, and does not support LINEAR or CUBIC filtering,
  for 3D volume mipmap-generation.

* Due to the underlying Windows BMP WIC codec, alpha channels are not supported for 16bpp or 32bpp BMP pixel format files. The Windows 8
  version of the Windows BMP WIC codec does support 32bpp pixel formats with alpha when using the BITMAPV5HEADER file header. Note the updated
  WIC is available on Windows 7 SP1 with KB 2670838 installed.

* The WIC conversion cases currently ignore TEX_FILTER_SRGB_IN and TEX_FILTER_SRGB_OUT out.

* For the DXGI 1.1 version of DirectXTex, 4:4:4:4 pixel format DDS files are always expanded to 8:8:8:8 upon load since DXGI 1.0
  and DXGI 1.1 versions of Direct3D do not support these resource formats. The DXGI 1.2 versions of DirectXTex and DDSTextureLoader
  make use of the DXGI_FORMAT_B4G4R4A4_UNORM format instead.

* While DXGI 1.0 and DXGI 1.1 include 5:6:5 (DXGI_FORMAT_B5G6R5_UNORM) and 5:5:5:1 (DXGI_FORMAT_B5G5R5A1_UNORM)
  pixel format enumerations, the DirectX 10.x and 11.0 Runtimes do not support these formats for use with Direct3D. The DirectX 11.1 runtime,
  DXGI 1.2, and the WDDM 1.2 driver model fully support 16bpp formats (5:6:5, 5:5:5:1, and 4:4:4:4). The DXGI 1.2 version of WICTextureLoader
  will load 16bpp pixel images as 5:6:5 or 5:5:5:1 rather than expand them to 32bpp RGBA.

* WICTextureLoader cannot load .TGA files unless the system has a 3rd party WIC codec installed. You must use the DirectXTex
  library for TGA file format support without relying on an add-on WIC codec.

* Loading of 96bpp floating-point TIFF files results in a corrupted image prior to Windows 8. This fix is available on Windows 7 SP1 with
  KB 2670838 installed.


------------------------------------
RELEASE HISTORY

November 15, 2012
    Added support for WIC2 when available on Windows 8 and Windows 7 with KB 2670838
    Added optional targetGUID parameter to SaveWIC* APIs to influence final container pixel format choice
    Fixed bug in SaveDDS* which was generating invalid DDS files for 1D dimension textures
    Improved robustness of CaptureTexture when resolving MSAA source textures
    Sync'd DDSTextureLoader, ScreenGrab, and WICTextureLoader standalone versions with latest DirectXTK release

September 28, 2012
    Added ScreenGrab module for creating runtime screenshots
    Renamed project files for better naming consistency
    New Typeless utilities for DirectXTex
    Some minor code cleanup for DirectXTex's WIC writer function
    Bug fixes and new -tu/-tf options for texconv

June 22, 2012
    Moved to using XNA Math 2.05 instead of XNA Math 2.04 for USE_XNAMATH builds
    Fixed BGR vs. RGB color channel swizzle problem with 24bpp legacy .DDS files in DirectXTex
    Update to DirectXTex WIC and WICTextureLoader for additional 96bpp float format handling on Windows 8

May 31, 2012
    Minor fix for DDSTextureLoader's retry fallback that can happen with 10level9 feature levels
    Switched to use "_DEBUG" instead of "DEBUG" and cleaned up debug warnings
    added Metro style application project files for DirectXTex

April 20, 2012
    DirectTex's WIC-based writer opts-in for the Windows 8 BMP encoder option for writing 32 bpp RGBA files with the BITMAPV5HEADER

March 30, 2012
    WICTextureLoader updated with Windows 8 WIC pixel formats
    DirectXTex updated with limited non-power-of-2 texture support and TEX_FILTER_SEPARATE_ALPHA option
    Texconv updated with '-sepalpha' command-line option
    Added USE_XNAMATH control define to build DirectXTex using either XNAMath or DirectXMath
    Added VS 2012 project files (which use DirectXMath instead of XNAMath and define DXGI_1_2_FORMATS)

March 15, 2012
    Fix for resource leak in CreateShaderResourceView() Direct3D 11 helper function in DirectXTex

March 5, 2012
    Fix for too much temp memory allocated by WICTextureLoader; cleaned up legacy 'min/max' macro usage in DirectXTex

February 21, 2012
    WICTextureLoader updated to handle systems and device drivers without BGRA or 16bpp format support

February 20, 2012
    Some code cleanup for DirectXTex and DDSTextureLoader
    Fixed bug in 10:10:10:2 format fixup in the LoadDDSFromMemory function
    Fixed bugs in "non-zero alpha" special-case handling in LoadTGAFromFile
    Fixed bug in _SwizzleScanline when copying alpha channel for BGRA<->RGBA swizzling

February 11, 2012
    Update of DDSTextureLoader to also build in Metro style apps; added WICTextureLoader
    Added CMYK WIC pixel formats to the DirectXTex conversion table

January 30, 2012
    Minor code-cleanup for DirectXTex to enable use of PCH through 'directxtexp.h' header

January 24, 2011
    Some code-cleanup for DirectXTex
    Added DXGI 1.2 implementation for DDSTextureLoader and DirectXTex guarded with DXGI_1_2_FORMATS compiliation define 

December 16, 2011
    Fixed x64 compilation warnings in DDSTextureLoader

November 30, 2011
    Fixed some of the constants used in IsSupportedTexture(),
    added ability to strip off top levels of mips in DDSTextureLoader,
    changed DirectXTex to use CoCreateInstance rather than LoadLibrary to obtain the WIC factory,
    a few minor /analyze related annotations for DirectXTex

October 27, 2011
    Original release