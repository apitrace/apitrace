//-------------------------------------------------------------------------------------
// DirectXTexp.h
//  
// DirectX Texture Library - Private header
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// http://go.microsoft.com/fwlink/?LinkId=248926
//-------------------------------------------------------------------------------------

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#define NOMINMAX
#include <windows.h>

#ifdef USE_XNAMATH
#include <xnamath.h>
#else
#include <directxmath.h>
#include <directxpackedvector.h>
#endif

#include <assert.h>

#include <malloc.h>
#include <memory>

#include <vector>

#include <stdlib.h>
#include <search.h>

#include <ole2.h>

#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/) || defined(_WIN7_PLATFORM_UPDATE)
#include <d2d1.h>
#endif

#pragma warning(push)
#pragma warning(disable : 4005)
#include <wincodec.h>
#pragma warning(pop)

#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/) && !defined(DXGI_1_2_FORMATS)
#define DXGI_1_2_FORMATS
#endif

#include "directxtex.h"

#include "scoped.h"

struct IWICImagingFactory;

#define TEX_FILTER_MASK 0xF00000

namespace DirectX
{
    //---------------------------------------------------------------------------------
    // WIC helper functions
    DXGI_FORMAT _WICToDXGI( _In_ const GUID& guid );
    bool _DXGIToWIC( _In_ DXGI_FORMAT format, _Out_ GUID& guid );

    IWICImagingFactory* _GetWIC();

    bool _IsWIC2();

    inline WICBitmapDitherType _GetWICDither( _In_ DWORD flags )
    {
        static_assert( TEX_FILTER_DITHER == 0x10000, "TEX_FILTER_DITHER* flag values don't match mask" );

        static_assert( TEX_FILTER_DITHER == WIC_FLAGS_DITHER, "TEX_FILTER_DITHER* should match WIC_FLAGS_DITHER*" );
        static_assert( TEX_FILTER_DITHER_DIFFUSION == WIC_FLAGS_DITHER_DIFFUSION, "TEX_FILTER_DITHER* should match WIC_FLAGS_DITHER*" );

        switch( flags & 0xF0000 )
        {
        case TEX_FILTER_DITHER:
            return WICBitmapDitherTypeOrdered4x4;

        case TEX_FILTER_DITHER_DIFFUSION:
            return WICBitmapDitherTypeErrorDiffusion;

        default:
            return WICBitmapDitherTypeNone;
        }
    }

    inline WICBitmapInterpolationMode _GetWICInterp( _In_ DWORD flags )
    {
        static_assert( TEX_FILTER_POINT == 0x100000, "TEX_FILTER_ flag values don't match TEX_FILTER_MASK" );

        static_assert( TEX_FILTER_POINT == WIC_FLAGS_FILTER_POINT, "TEX_FILTER_* flags should match WIC_FLAGS_FILTER_*" );
        static_assert( TEX_FILTER_LINEAR == WIC_FLAGS_FILTER_LINEAR, "TEX_FILTER_* flags should match WIC_FLAGS_FILTER_*"  );
        static_assert( TEX_FILTER_CUBIC == WIC_FLAGS_FILTER_CUBIC, "TEX_FILTER_* flags should match WIC_FLAGS_FILTER_*"  );
        static_assert( TEX_FILTER_FANT == WIC_FLAGS_FILTER_FANT, "TEX_FILTER_* flags should match WIC_FLAGS_FILTER_*"  );

        switch( flags & TEX_FILTER_MASK )
        {
        case TEX_FILTER_POINT:
            return WICBitmapInterpolationModeNearestNeighbor;

        case TEX_FILTER_LINEAR:
            return WICBitmapInterpolationModeLinear;

        case TEX_FILTER_CUBIC:
            return WICBitmapInterpolationModeCubic;

        case TEX_FILTER_FANT:
        default:
            return WICBitmapInterpolationModeFant;
        }
    }

    //---------------------------------------------------------------------------------
    // Image helper functions
    void _DetermineImageArray( _In_ const TexMetadata& metadata, _In_ DWORD cpFlags,
                               _Out_ size_t& nImages, _Out_ size_t& pixelSize );

    bool _SetupImageArray( _In_bytecount_(pixelSize) uint8_t *pMemory, _In_ size_t pixelSize,
                           _In_ const TexMetadata& metadata, _In_ DWORD cpFlags,
                           _Out_cap_(nImages) Image* images, _In_ size_t nImages );

    //---------------------------------------------------------------------------------
    // Conversion helper functions

    enum TEXP_SCANLINE_FLAGS
    {
        TEXP_SCANLINE_NONE          = 0,
        TEXP_SCANLINE_SETALPHA      = 0x1,  // Set alpha channel to known opaque value
        TEXP_SCANLINE_LEGACY        = 0x2,  // Enables specific legacy format conversion cases
    };

    enum CONVERT_FLAGS
    {
        CONVF_FLOAT     = 0x1,
        CONVF_UNORM     = 0x2,
        CONVF_UINT      = 0x4,
        CONVF_SNORM     = 0x8, 
        CONVF_SINT      = 0x10,
        CONVF_DEPTH     = 0x20,
        CONVF_STENCIL   = 0x40,
        CONVF_SHAREDEXP = 0x80,
        CONVF_BGR       = 0x100,
        CONVF_X2        = 0x200,
        CONVF_PACKED    = 0x400,
        CONVF_BC        = 0x800,
        CONVF_R         = 0x10000,
        CONVF_G         = 0x20000,
        CONVF_B         = 0x40000,
        CONVF_A         = 0x80000,
        CONVF_RGB_MASK  = 0x70000,
        CONVF_RGBA_MASK = 0xF0000,
    };

    DWORD _GetConvertFlags( _In_ DXGI_FORMAT format );

    void _CopyScanline( _Out_bytecap_(outSize) LPVOID pDestination, _In_ size_t outSize, 
                        _In_bytecount_(inSize) LPCVOID pSource, _In_ size_t inSize,
                        _In_ DXGI_FORMAT format, _In_ DWORD flags );

    void _SwizzleScanline( _Out_bytecap_(outSize) LPVOID pDestination, _In_ size_t outSize, 
                           _In_bytecount_(inSize) LPCVOID pSource, _In_ size_t inSize,
                           _In_ DXGI_FORMAT format, _In_ DWORD flags );

    bool _ExpandScanline( _Out_bytecap_(outSize) LPVOID pDestination, _In_ size_t outSize,
                          _In_ DXGI_FORMAT outFormat, 
                          _In_bytecount_(inSize) LPCVOID pSource, _In_ size_t inSize,
                          _In_ DXGI_FORMAT inFormat, _In_ DWORD flags );

    bool _LoadScanline( _Out_cap_(count) XMVECTOR* pDestination, _In_ size_t count,
                        _In_bytecount_(size) LPCVOID pSource, _In_ size_t size, _In_ DXGI_FORMAT format );

    bool _StoreScanline( _Out_bytecap_(size) LPVOID pDestination, _In_ size_t size, _In_ DXGI_FORMAT format,
                         _In_count_(count) const XMVECTOR* pSource, _In_ size_t count );

    HRESULT _ConvertToR32G32B32A32( _In_ const Image& srcImage, _Inout_ ScratchImage& image );

    HRESULT _ConvertFromR32G32B32A32( _In_ const Image& srcImage, _In_ const Image& destImage );
    HRESULT _ConvertFromR32G32B32A32( _In_ const Image& srcImage, _In_ DXGI_FORMAT format, _Inout_ ScratchImage& image );
    HRESULT _ConvertFromR32G32B32A32( _In_count_(nimages) const Image* srcImages, _In_ size_t nimages, _In_ const TexMetadata& metadata,
                                      _In_ DXGI_FORMAT format, _Out_ ScratchImage& result );

    void _ConvertScanline( _Inout_count_(count) XMVECTOR* pBuffer, _In_ size_t count,
                           _In_ DXGI_FORMAT outFormat, _In_ DXGI_FORMAT inFormat, _In_ DWORD flags );

    //---------------------------------------------------------------------------------
    // DDS helper functions
    HRESULT _EncodeDDSHeader( _In_ const TexMetadata& metadata, DWORD flags,
                              _Out_opt_cap_x_(maxsize) LPVOID pDestination, _In_ size_t maxsize, _Out_ size_t& required );

}; // namespace
