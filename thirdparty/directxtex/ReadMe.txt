DIRECTX TEXTURE LIBRARY (DirectXTex)
------------------------------------

Copyright (c) Microsoft Corporation. All rights reserved.

July 29, 2015

This package contains DirectXTex, a shared source library for reading and writing DDS
files, and performing various texture content processing operations including
resizing, format conversion, mip-map generation, block compression for Direct3D runtime
texture resources, and height-map to normal-map conversion. This library makes
use of the Windows Image Component (WIC) APIs. It also includes a simple .TGA reader and
writer since this image file format is commonly used for texture content processing pipelines,
but is not currently supported by a built-in WIC codec.

The source is written for Visual Studio 2012, 2013, or 2015. It is recommended that
you make use of the Windows 8.1 SDK and Windows 7 Service Pack 1 or later.

NOTE: DirectXTex is not supported on Windows phone 8.0 because WIC is not available on
that platform. It is available on Windows phone starting in version 8.1.

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
    FANT or BOX, TRIANGLE, *_DITHER, *_DITHER_DIFFUSION); and support for the .TGA file format.
    This also includes support for JPEG XR/HD Photo bitmap formats (see
    <http://blogs.msdn.com/b/chuckw/archive/2011/01/19/known-issue-texconvex.aspx>)

Texassemble\
    This DirectXTex sample is a command-line utility for creating cubemaps, volume maps, or
    texture arrays from a set of individual input image files.
    
DDSView\
    This DirectXTex sample is a simple Direct3D 11-based viewer for DDS files. For array textures
    or volume maps, the "<" and ">" keyboard keys will show different images contained in the DDS.
    The "1" through "0" keys can also be used to jump to a specific image index.

All content and source code for this package are subject to the terms of the MIT License.
<http://opensource.org/licenses/MIT>.

Documentation is available at <https://github.com/Microsoft/DirectXTex/wiki>.

For the latest version of DirectXTex, bug reports, etc. please visit the project site.

http://go.microsoft.com/fwlink/?LinkId=248926


------------------------------------
RELEASE NOTES

* The alpha mode specification for DDS files was updated between the March 2013 and April 2013 releases. Any
  DDS files created using the DDS_FLAGS_FORCE_DX10_EXT_MISC2 flag or the texconv -dx10 switch using the
  March 2013 release should be refreshed.

* Due to the underlying Windows BMP WIC codec, alpha channels are not supported for 16bpp or 32bpp BMP pixel format files. The Windows 8.x
  version of the Windows BMP WIC codec does support 32bpp pixel formats with alpha when using the BITMAPV5HEADER file header. Note the updated
  WIC is available on Windows 7 SP1 with KB 2670838 installed.

* While DXGI 1.0 and DXGI 1.1 include 5:6:5 (DXGI_FORMAT_B5G6R5_UNORM) and 5:5:5:1 (DXGI_FORMAT_B5G5R5A1_UNORM)
  pixel format enumerations, the DirectX 10.x and 11.0 Runtimes do not support these formats for use with Direct3D. The DirectX 11.1 runtime,
  DXGI 1.2, and the WDDM 1.2 driver model fully support 16bpp formats (5:6:5, 5:5:5:1, and 4:4:4:4).

* WICTextureLoader cannot load .TGA files unless the system has a 3rd party WIC codec installed. You must use the DirectXTex
  library for TGA file format support without relying on an add-on WIC codec.

* Loading of 96bpp floating-point TIFF files results in a corrupted image prior to Windows 8. This fix is available on Windows 7 SP1 with
  KB 2670838 installed.


------------------------------------
RELEASE HISTORY

July 29, 2015
    Fixed rounding problem with 32-bit RGBA/BGRA format conversions
    texconv: use CPU parallel compression for BC1-BC5 (-singleproc disables)
    Updated for VS 2015 and Windows 10 SDK RTM
    Retired VS 2010 and Windows 8.0 Store projects

June 18, 2015
    New BC_FLAGS_USE_3SUBSETS option for BC7 compressors; now defaults to skipping 3 subset blocks
    Fixed bug with MakeTypeless and A8_UNORM
    Fixed file length validation problem in LoadDDSFromFile

March 27, 2015
    Added projects for Windows apps Technical Preview
    Fixed bug with WIC-based mipmap generation for non-WIC supported formats
    Fixed bug with WIC multiframe loader when resizing required
    texconv: Added -nmap/-nmapamp for generating normal maps from height maps
    texconv/texassemble: Updated to load multiframe WIC files (tiff, gif)
    Minor code cleanup

November 24, 2014
    Updates for Visual Studio 2015 Technical Preview
    Minor code cleanup

September 22, 2014
    Format conversion improvements and bug fixes (depth/stencil, alpha-only, float16, RGB -> 1 channel)
    Fixed issue when BC decompressing non-standard compressed rowPitch images
    Explicit calling-convention annotation for all 'public' functions
    Code cleanup
    Xbox One platform updates

July 15, 2014
    texconv command-line tool fixes
    Fixed problem with 'wide' images with CPU Compress
    Updates to Xbox One platform support

April 3, 2014
    Windows phone 8.1 platform support

February 24, 2014
    Direct3D 11 video and Xbox One extended format support
    New APIs: IsPlanar, IsPalettized, IsDepthStencil, ConvertToSinglePlane
    Added 'alphaWeight' parameter to GPU Compress [breaking change]
    texconv '-aw' switch to control the alpha weighting for the BC7 GPU compressor
    Fixed bug with ordered dithering in non-WIC conversion codepaths
    Fixed SaveToDDS* functions when using arbitrary row pitch values

January 24, 2014
    Added sRGB flags for Compress (TEX_COMPRESS_SRGB*)
    Added 'compress' flag parameter to GPU versions of Compress [breaking change]
    Minor fix for potential rounding problem in GPU Compress
    Code cleanup (removed DXGI_1_2_FORMATS control define; ScopedObject typedef removed)
    Dropped VS 2010 support without the Windows 8.1 SDK (removed USE_XNAMATH control define)

December 24, 2013
    texconv updated with -fl and -pow2 command-line switches
    Fixed bug in Resize when doing custom filtering which occurred when exactly doubling the image size
    Added move operators to ScratchImage and Blob classes
    Xbox One platform support

October 21, 2013
    Updated for Visual Studio 2013 and Windows 8.1 SDK RTM
    PremultiplyAlpha updated with new 'flags' parameter and to use sRGB correct blending
    Fixed colorspace conversion issue with DirectCompute compressor when compressing for BC7 SRGB

August 13, 2013
    DirectCompute 4.0 BC6H/BC7 compressor integration
    texconv utility uses DirectCompute compression by default for BC6H/BC7, -nogpu disables use of DirectCompute

August 1, 2013
    Support for BC compression/decompression of non-power-of-2 mipmapped textures
    Fixes for BC6H / BC7 codecs to better match published standard
    Fix for BC4 / BC5 codecs when compressing RGB images
    Minor fix for the BC1-3 codec
    New optional flags for ComputeMSE to compare UNORM vs. SNORM images
    New WIC loading flag added to control use of WIC metadata to return sRGB vs. non-sRGB formats
    Code cleanup and /analyze fixes
    Project file cleanup
    Texconv utility uses parallel BC compression by default for BC6H/BC7, -singleproc disables multithreaded behavior

July 1, 2013
    VS 2013 Preview projects added
    SaveToWIC functions updated with new optional setCustomProps parameter

June 15, 2013
    Custom filtering implementation for Resize & GenerateMipMaps(3D) - Point, Box, Linear, Cubic, and Triangle
        TEX_FILTER_TRIANGLE finite low-pass triangle filter
        TEX_FILTER_WRAP, TEX_FILTER_MIRROR texture semantics for custom filtering
        TEX_FILTER_BOX alias for TEX_FILTER_FANT WIC
    Ordered and error diffusion dithering for non-WIC conversion
    sRGB gamma correct custom filtering and conversion
    DDS_FLAGS_EXPAND_LUMINANCE - Reader conversion option for L8, L16, and A8L8 legacy DDS files
    Added use of WIC metadata for sRGB pixel formats
    Added BitsPerColor utility function
    Fixed Convert threshold parameter usage
    Non-power-of-2 volume map support, fixed bug with non-square volume maps
    Texconv utility update with -xlum, -wrap, and -mirror options; reworked -if options for improved dithering
    Texassemble utility for creating cubemaps, volume maps, and texture arrays
    DDSTextureLoader and WICTextureLoader sync'd with DirectXTK versions

April 16, 2013
    Updated alpha-mode metadata details in .DDS files
    Added new control flags for Convert
    Added new optional flags for ComputeMSE
    Fixed conversion handling for sRGB formats
    Fixed internal routines for handling R10G10B10_XR_BIAS_A2_UNORM, R9G9B9E5_SHAREDEXP, and FORMAT_R1_UNORM
    Fixed WIC I/O for GUID_WICPixelFormat32bppRGBE pixel format files (HD Photo)
    Fixed non-square image handling in GenerateMipMaps3D
    Fixed some error handling in the DDS load code

March 22, 2013
    Supports reading and writing alpha-mode (straight, premultiplied, etc.) metadata in .DDS files
    Added build option to use WICCreateImagingFactory_Proxy instead of CoCreateInstance to obtain WIC factory

January 29, 2013
    Added PremultiplyAlpha to DirectXTex; -pmalpha switch for texconv command-line tool
    Fixed problem with forceSRGB implementation for Ex versions of CreateTexture, CreateShaderResourceView, DDSTextureLoader and WICTextureLoader
 
December 11, 2012 
    Ex versions of CreateTexture, CreateShaderResourceView, DDSTextureLoader and WICTextureLoader
    Fixed BC2 and BC3 decompression issue for unusual color encoding case
    Converted annotation to SAL2 for improved VS 2012 /analyze experience
    Updated DirectXTex, DDSView, and Texconv with VS 2010 + Windows 8.0 SDK project using official 'property sheets'

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