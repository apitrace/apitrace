//-------------------------------------------------------------------------------------
// DirectXTexResize.cpp
//  
// DirectX Texture Library - Image resizing operations
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

#include "directxtexp.h"

namespace DirectX
{

extern HRESULT _ResizeSeparateColorAndAlpha( _In_ IWICImagingFactory* pWIC, _In_ IWICBitmap* original,
                                             _In_ size_t newWidth, _In_ size_t newHeight, _In_ DWORD filter, _Inout_ const Image* img );

//-------------------------------------------------------------------------------------
// Do image resize using WIC
//-------------------------------------------------------------------------------------
static HRESULT _PerformResizeUsingWIC( _In_ const Image& srcImage, _In_ DWORD filter,
                                       _In_ const WICPixelFormatGUID& pfGUID, _In_ const Image& destImage )
{
    if ( !srcImage.pixels || !destImage.pixels )
        return E_POINTER;

    assert( srcImage.format == destImage.format );

    IWICImagingFactory* pWIC = _GetWIC();
    if ( !pWIC )
        return E_NOINTERFACE;

    ScopedObject<IWICComponentInfo> componentInfo;
    HRESULT hr = pWIC->CreateComponentInfo( pfGUID, &componentInfo );
    if ( FAILED(hr) )
        return hr;

    ScopedObject<IWICPixelFormatInfo2> pixelFormatInfo;
    hr = componentInfo->QueryInterface( __uuidof(IWICPixelFormatInfo2), (void**)&pixelFormatInfo );
    if ( FAILED(hr) )
        return hr;

    BOOL supportsTransparency = FALSE;
    hr = pixelFormatInfo->SupportsTransparency( &supportsTransparency );
    if ( FAILED(hr) )
        return hr;

    ScopedObject<IWICBitmap> source;
    hr = pWIC->CreateBitmapFromMemory( static_cast<UINT>( srcImage.width ), static_cast<UINT>( srcImage.height ), pfGUID,
                                       static_cast<UINT>( srcImage.rowPitch ), static_cast<UINT>( srcImage.slicePitch ),
                                       srcImage.pixels, &source );
    if ( FAILED(hr) )
        return hr;

    if ( (filter & TEX_FILTER_SEPARATE_ALPHA) && supportsTransparency )
    {
        hr = _ResizeSeparateColorAndAlpha( pWIC, source.Get(), destImage.width, destImage.height, filter, &destImage );
        if ( FAILED(hr) )
            return hr;
    }
    else
    {
        ScopedObject<IWICBitmapScaler> scaler;
        hr = pWIC->CreateBitmapScaler( &scaler );
        if ( FAILED(hr) )
            return hr;

        hr = scaler->Initialize( source.Get(), static_cast<UINT>( destImage.width ), static_cast<UINT>( destImage.height ), _GetWICInterp( filter ) );
        if ( FAILED(hr) )
            return hr;

        WICPixelFormatGUID pfScaler;
        hr = scaler->GetPixelFormat( &pfScaler );
        if ( FAILED(hr) )
            return hr;

        if ( memcmp( &pfScaler, &pfGUID, sizeof(WICPixelFormatGUID) ) == 0 )
        {
            hr = scaler->CopyPixels( 0, static_cast<UINT>( destImage.rowPitch ), static_cast<UINT>( destImage.slicePitch ), destImage.pixels );
            if ( FAILED(hr) )
                return hr;
        }
        else
        {
            // The WIC bitmap scaler is free to return a different pixel format than the source image, so here we
            // convert it back
            ScopedObject<IWICFormatConverter> FC;
            hr = pWIC->CreateFormatConverter( &FC );
            if ( FAILED(hr) )
                return hr;

            hr = FC->Initialize( scaler.Get(), pfGUID, _GetWICDither( filter ), 0, 0, WICBitmapPaletteTypeCustom );
            if ( FAILED(hr) )
                return hr;

            hr = FC->CopyPixels( 0, static_cast<UINT>( destImage.rowPitch ), static_cast<UINT>( destImage.slicePitch ), destImage.pixels );  
            if ( FAILED(hr) )
                return hr;
        }
    }

    return S_OK;
}


//-------------------------------------------------------------------------------------
// Do conversion, resize using WIC, conversion cycle
//-------------------------------------------------------------------------------------
static HRESULT _PerformResizeViaF32( _In_ const Image& srcImage, _In_ DWORD filter, _In_ const Image& destImage )
{
    if ( !srcImage.pixels || !destImage.pixels )
        return E_POINTER;

    assert( srcImage.format != DXGI_FORMAT_R32G32B32A32_FLOAT );
    assert( srcImage.format == destImage.format );

    ScratchImage temp;
    HRESULT hr = _ConvertToR32G32B32A32( srcImage, temp );
    if ( FAILED(hr) )
        return hr;

    const Image *tsrc = temp.GetImage( 0, 0, 0 );
    if ( !tsrc )
        return E_POINTER;

    ScratchImage rtemp;
    hr = rtemp.Initialize2D( DXGI_FORMAT_R32G32B32A32_FLOAT, destImage.width, destImage.height, 1, 1 );
    if ( FAILED(hr) )
        return hr;

    const Image *tdest = rtemp.GetImage( 0, 0, 0 );
    if ( !tdest )
        return E_POINTER;

    hr = _PerformResizeUsingWIC( *tsrc, filter, GUID_WICPixelFormat128bppRGBAFloat, *tdest );
    if ( FAILED(hr) )
        return hr;

    temp.Release();

    hr = _ConvertFromR32G32B32A32( *tdest, destImage );
    if ( FAILED(hr) )
        return hr;

    return S_OK;
}


//=====================================================================================
// Entry-points
//=====================================================================================

//-------------------------------------------------------------------------------------
// Resize image
//-------------------------------------------------------------------------------------
HRESULT Resize( const Image& srcImage, size_t width, size_t height, DWORD filter, ScratchImage& image )
{
    if ( width == 0 || height == 0 )
        return E_INVALIDARG;

#ifdef _AMD64_
    if ( (srcImage.width > 0xFFFFFFFF) || (srcImage.height > 0xFFFFFFFF) )
        return E_INVALIDARG;

    if ( (width > 0xFFFFFFFF) || (height > 0xFFFFFFFF) )
        return E_INVALIDARG;
#endif

    if ( !srcImage.pixels )
        return E_POINTER;

    if ( IsCompressed( srcImage.format ) )
    {
        // We don't support resizing compressed images
        return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
    }

    HRESULT hr = image.Initialize2D( srcImage.format, width, height, 1, 1 );
    if ( FAILED(hr) )
        return hr;
   
    const Image *rimage = image.GetImage( 0, 0, 0 );
    if ( !rimage )
        return E_POINTER;

    // WIC only supports CLAMP

    WICPixelFormatGUID pfGUID;
    if ( _DXGIToWIC( srcImage.format, pfGUID ) )
    {
        // Case 1: Source format is supported by Windows Imaging Component
        hr = _PerformResizeUsingWIC( srcImage, filter, pfGUID, *rimage );
    }
    else
    {
        // Case 2: Source format is not supported by WIC, so we have to convert, resize, and convert back
        hr = _PerformResizeViaF32( srcImage, filter, *rimage );
    }

    if ( FAILED(hr) )
    {
        image.Release();
        return hr;
    }

    return S_OK;
}


//-------------------------------------------------------------------------------------
// Resize image (complex)
//-------------------------------------------------------------------------------------
HRESULT Resize( const Image* srcImages, size_t nimages, const TexMetadata& metadata,
                size_t width, size_t height, DWORD filter, ScratchImage& result )
{
    if ( !srcImages || !nimages || width == 0 || height == 0 )
        return E_INVALIDARG;

#ifdef _AMD64_
    if ( (width > 0xFFFFFFFF) || (height > 0xFFFFFFFF) )
        return E_INVALIDARG;
#endif

    TexMetadata mdata2 = metadata;
    mdata2.width = width;
    mdata2.height = height;
    mdata2.mipLevels = 1;
    HRESULT hr = result.Initialize( mdata2 );
    if ( FAILED(hr) )
        return hr;

    WICPixelFormatGUID pfGUID;
    bool wicpf = _DXGIToWIC( metadata.format, pfGUID );

    switch ( metadata.dimension )
    {
    case TEX_DIMENSION_TEXTURE1D:
    case TEX_DIMENSION_TEXTURE2D:
        assert( metadata.depth == 1 );

        for( size_t item = 0; item < metadata.arraySize; ++item )
        {
            size_t srcIndex = metadata.ComputeIndex( 0, item, 0 );
            if ( srcIndex >= nimages )
            {
                result.Release();
                return E_FAIL;
            }

            const Image* srcimg = &srcImages[ srcIndex ];
            const Image* destimg = result.GetImage( 0, item, 0 );
            if ( !srcimg || !destimg )
            {
                result.Release();
                return E_POINTER;
            }

            if ( srcimg->format != metadata.format )
            {
                result.Release();
                return E_FAIL;
            }

#ifdef _AMD64_
            if ( (srcimg->width > 0xFFFFFFFF) || (srcimg->height > 0xFFFFFFFF) )
            {
                result.Release();
                return E_FAIL;
            }
#endif

            if ( wicpf )
            {
                // Case 1: Source format is supported by Windows Imaging Component
                hr = _PerformResizeUsingWIC( *srcimg, filter, pfGUID, *destimg );
            }
            else
            {
                // Case 2: Source format is not supported by WIC, so we have to convert, resize, and convert back
                hr = _PerformResizeViaF32( *srcimg, filter, *destimg );
            }

            if ( FAILED(hr) )
            {
                result.Release();
                return hr;
            }
        }
        break;

    case TEX_DIMENSION_TEXTURE3D:
        assert( metadata.arraySize == 1 );

        for( size_t slice = 0; slice < metadata.depth; ++slice )
        {
            size_t srcIndex = metadata.ComputeIndex( 0, 0, slice );
            if ( srcIndex >= nimages )
            {
                result.Release();
                return E_FAIL;
            }

            const Image* srcimg = &srcImages[ srcIndex ];
            const Image* destimg = result.GetImage( 0, 0, slice );
            if ( !srcimg || !destimg )
            {
                result.Release();
                return E_POINTER;
            }

            if ( srcimg->format != metadata.format )
            {
                result.Release();
                return E_FAIL;
            }

#ifdef _AMD64_
            if ( (srcimg->width > 0xFFFFFFFF) || (srcimg->height > 0xFFFFFFFF) )
            {
                result.Release();
                return E_FAIL;
            }
#endif

            if ( wicpf )
            {
                // Case 1: Source format is supported by Windows Imaging Component
                hr = _PerformResizeUsingWIC( *srcimg, filter, pfGUID, *destimg );
            }
            else
            {
                // Case 2: Source format is not supported by WIC, so we have to convert, resize, and convert back
                hr = _PerformResizeViaF32( *srcimg, filter, *destimg );
            }

            if ( FAILED(hr) )
            {
                result.Release();
                return hr;
            }
        }
        break;

    default:
        result.Release();
        return E_FAIL;
    }

    return S_OK;
}

}; // namespace
