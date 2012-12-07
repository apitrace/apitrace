//-------------------------------------------------------------------------------------
// DirectXTex.h
//  
// DirectX Texture Library
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

#pragma warning(push)
#pragma warning(disable : 4005)
#include <stdint.h>
#pragma warning(pop)

#include <algorithm>

#include <dxgiformat.h>
#include <d3d11.h>

#define DIRECTX_TEX_VERSION 100

namespace DirectX
{
    //---------------------------------------------------------------------------------
    // DXGI Format Utilities
    bool IsValid( _In_ DXGI_FORMAT fmt );
    bool IsCompressed( _In_ DXGI_FORMAT fmt );
    bool IsPacked( _In_ DXGI_FORMAT fmt );
    bool IsVideo( _In_ DXGI_FORMAT fmt );
    bool IsSRGB( _In_ DXGI_FORMAT fmt );
    bool IsTypeless( _In_ DXGI_FORMAT fmt );

    size_t BitsPerPixel( _In_ DXGI_FORMAT fmt );

    enum CP_FLAGS
    {
        CP_FLAGS_NONE               = 0x0,      // Normal operation
        CP_FLAGS_LEGACY_DWORD       = 0x1,      // Assume pitch is DWORD aligned instead of BYTE aligned
        CP_FLAGS_24BPP              = 0x10000,  // Override with a legacy 24 bits-per-pixel format size
        CP_FLAGS_16BPP              = 0x20000,  // Override with a legacy 16 bits-per-pixel format size
        CP_FLAGS_8BPP               = 0x40000,  // Override with a legacy 8 bits-per-pixel format size
    };

    void ComputePitch( _In_ DXGI_FORMAT fmt, _In_ size_t width, _In_ size_t height,
                       _Out_ size_t& rowPitch, _Out_ size_t& slicePitch, _In_ DWORD flags = CP_FLAGS_NONE );

    size_t ComputeScanlines( _In_ DXGI_FORMAT fmt, _In_ size_t height );

    DXGI_FORMAT MakeSRGB( _In_ DXGI_FORMAT fmt );
    DXGI_FORMAT MakeTypeless( _In_ DXGI_FORMAT fmt );
    DXGI_FORMAT MakeTypelessUNORM( _In_ DXGI_FORMAT fmt );
    DXGI_FORMAT MakeTypelessFLOAT( _In_ DXGI_FORMAT fmt );

    //---------------------------------------------------------------------------------
    // Texture metadata
    enum TEX_DIMENSION
        // Subset here matches D3D10_RESOURCE_DIMENSION and D3D11_RESOURCE_DIMENSION
    {
        TEX_DIMENSION_TEXTURE1D	= 2,
        TEX_DIMENSION_TEXTURE2D	= 3,
        TEX_DIMENSION_TEXTURE3D	= 4,
    };

    enum TEX_MISC_FLAG
        // Subset here matches D3D10_RESOURCE_MISC_FLAG and D3D11_RESOURCE_MISC_FLAG
    {
        TEX_MISC_TEXTURECUBE = 0x4L,
    };

    struct TexMetadata
    {
        size_t          width;
        size_t          height;     // Should be 1 for 1D textures
        size_t          depth;      // Should be 1 for 1D or 2D textures
        size_t          arraySize;  // For cubemap, this is a multiple of 6
        size_t          mipLevels;
        uint32_t        miscFlags;
        DXGI_FORMAT     format;
        TEX_DIMENSION   dimension;

        size_t ComputeIndex( _In_ size_t mip, _In_ size_t item, _In_ size_t slice ) const;
            // Returns size_t(-1) to indicate an out-of-range error
    };

    enum DDS_FLAGS
    {
        DDS_FLAGS_NONE                  = 0x0,

        DDS_FLAGS_LEGACY_DWORD          = 0x1,
            // Assume pitch is DWORD aligned instead of BYTE aligned (used by some legacy DDS files)

        DDS_FLAGS_NO_LEGACY_EXPANSION   = 0x2,
            // Do not implicitly convert legacy formats that result in larger pixel sizes (24 bpp, 3:3:2, A8L8, A4L4, P8, A8P8) 

        DDS_FLAGS_NO_R10B10G10A2_FIXUP  = 0x4,
            // Do not use work-around for long-standing D3DX DDS file format issue which reversed the 10:10:10:2 color order masks

        DDS_FLAGS_FORCE_RGB             = 0x8,
            // Convert DXGI 1.1 BGR formats to DXGI_FORMAT_R8G8B8A8_UNORM to avoid use of optional WDDM 1.1 formats

        DDS_FLAGS_NO_16BPP              = 0x10,
            // Conversions avoid use of 565, 5551, and 4444 formats and instead expand to 8888 to avoid use of optional WDDM 1.2 formats

        DDS_FLAGS_FORCE_DX10_EXT        = 0x10000,
            // Always use the 'DX10' header extension for DDS writer (i.e. don't try to write DX9 compatible DDS files)
    };

    enum WIC_FLAGS
    {
        WIC_FLAGS_NONE                  = 0x0,

        WIC_FLAGS_FORCE_RGB             = 0x1,
            // Loads DXGI 1.1 BGR formats as DXGI_FORMAT_R8G8B8A8_UNORM to avoid use of optional WDDM 1.1 formats

        WIC_FLAGS_NO_X2_BIAS            = 0x2,
            // Loads DXGI 1.1 X2 10:10:10:2 format as DXGI_FORMAT_R10G10B10A2_UNORM

        WIC_FLAGS_NO_16BPP              = 0x4,
            // Loads 565, 5551, and 4444 formats as 8888 to avoid use of optional WDDM 1.2 formats

        WIC_FLAGS_ALLOW_MONO            = 0x8,
            // Loads 1-bit monochrome (black & white) as R1_UNORM rather than 8-bit greyscale

        WIC_FLAGS_ALL_FRAMES            = 0x10,
            // Loads all images in a multi-frame file, converting/resizing to match the first frame as needed, defaults to 0th frame otherwise

        WIC_FLAGS_DITHER                = 0x10000,
            // Use ordered 4x4 dithering for any required conversions

        WIC_FLAGS_DITHER_DIFFUSION      = 0x20000,
            // Use error-diffusion dithering for any required conversions

        WIC_FLAGS_FILTER_POINT          = 0x100000,
        WIC_FLAGS_FILTER_LINEAR         = 0x200000,
        WIC_FLAGS_FILTER_CUBIC          = 0x300000,
        WIC_FLAGS_FILTER_FANT           = 0x400000, // Combination of Linear and Box filter
            // Filtering mode to use for any required image resizing (only needed when loading arrays of differently sized images; defaults to Fant)
    };

    HRESULT GetMetadataFromDDSMemory( _In_bytecount_(size) LPCVOID pSource, _In_ size_t size, _In_ DWORD flags,
                                      _Out_ TexMetadata& metadata );
    HRESULT GetMetadataFromDDSFile( _In_z_ LPCWSTR szFile, DWORD flags,
                                    _Out_ TexMetadata& metadata );

    HRESULT GetMetadataFromTGAMemory( _In_bytecount_(size) LPCVOID pSource, _In_ size_t size,
                                      _Out_ TexMetadata& metadata );
    HRESULT GetMetadataFromTGAFile( _In_z_ LPCWSTR szFile,
                                    _Out_ TexMetadata& metadata );

    HRESULT GetMetadataFromWICMemory( _In_bytecount_(size) LPCVOID pSource, _In_ size_t size, _In_ DWORD flags,
                                      _Out_ TexMetadata& metadata );
    HRESULT GetMetadataFromWICFile( _In_z_ LPCWSTR szFile, _In_ DWORD flags,
                                    _Out_ TexMetadata& metadata );

    //---------------------------------------------------------------------------------
    // Bitmap image container
    struct Image
    {
        size_t      width;
        size_t      height;
        DXGI_FORMAT format;
        size_t      rowPitch;
        size_t      slicePitch;
        uint8_t*    pixels;
    };

    class ScratchImage
    {
    public:
        ScratchImage() : _nimages(0), _size(0), _image(0), _memory(0) {}
        ~ScratchImage() { Release(); }

        HRESULT Initialize( _In_ const TexMetadata& mdata );

        HRESULT Initialize1D( _In_ DXGI_FORMAT fmt, _In_ size_t length, _In_ size_t arraySize, _In_ size_t mipLevels );
        HRESULT Initialize2D( _In_ DXGI_FORMAT fmt, _In_ size_t width, _In_ size_t height, _In_ size_t arraySize, _In_ size_t mipLevels );
        HRESULT Initialize3D( _In_ DXGI_FORMAT fmt, _In_ size_t width, _In_ size_t height, _In_ size_t depth, _In_ size_t mipLevels );
        HRESULT InitializeCube( _In_ DXGI_FORMAT fmt, _In_ size_t width, _In_ size_t height, _In_ size_t nCubes, _In_ size_t mipLevels );

        HRESULT InitializeFromImage( _In_ const Image& srcImage, _In_ bool allow1D = false );
        HRESULT InitializeArrayFromImages( _In_count_(nImages) const Image* images, _In_ size_t nImages, _In_ bool allow1D = false ); 
        HRESULT InitializeCubeFromImages( _In_count_(nImages) const Image* images, _In_ size_t nImages );
        HRESULT Initialize3DFromImages( _In_count_(depth) const Image* images, _In_ size_t depth );

        void Release();

        bool OverrideFormat( _In_ DXGI_FORMAT f );

        const TexMetadata& GetMetadata() const { return _metadata; }
        const Image* GetImage(_In_ size_t mip, _In_ size_t item, _In_ size_t slice) const;

        const Image* GetImages() const { return _image; }
        size_t GetImageCount() const { return _nimages; }

        uint8_t* GetPixels() const { return _memory; }
        size_t GetPixelsSize() const { return _size; }

    private:
        size_t      _nimages;
        size_t      _size;
        TexMetadata _metadata;
        Image*      _image;
        uint8_t*    _memory;

        // Hide copy constructor and assignment operator
        ScratchImage( const ScratchImage& );
        ScratchImage& operator=( const ScratchImage& );
    };

    //---------------------------------------------------------------------------------
    // Memory blob (allocated buffer pointer is always 16-byte aligned)
    class Blob
    {
    public:
        Blob() : _buffer(0), _size(0) {}
        ~Blob() { Release(); }

        HRESULT Initialize( _In_ size_t size );

        void Release();

        void *GetBufferPointer() const { return _buffer; }
        size_t GetBufferSize() const { return _size; }

    private:
        void*   _buffer;
        size_t  _size;

        // Hide copy constructor and assignment operator
        Blob( const Blob& );
        Blob& operator=( const Blob& );
    };

    //---------------------------------------------------------------------------------
    // Image I/O

    // DDS operations
    HRESULT LoadFromDDSMemory( _In_bytecount_(size) LPCVOID pSource, _In_ size_t size, _In_ DWORD flags,
                               _Out_opt_ TexMetadata* metadata, _Out_ ScratchImage& image );
    HRESULT LoadFromDDSFile( _In_z_ LPCWSTR szFile, _In_ DWORD flags,
                             _Out_opt_ TexMetadata* metadata, _Out_ ScratchImage& image );

    HRESULT SaveToDDSMemory( _In_ const Image& image, _In_ DWORD flags,
                             _Out_ Blob& blob );
    HRESULT SaveToDDSMemory( _In_count_(nimages) const Image* images, _In_ size_t nimages, _In_ const TexMetadata& metadata, _In_ DWORD flags,
                             _Out_ Blob& blob );

    HRESULT SaveToDDSFile( _In_ const Image& image, _In_ DWORD flags, _In_z_ LPCWSTR szFile );
    HRESULT SaveToDDSFile( _In_count_(nimages) const Image* images, _In_ size_t nimages, _In_ const TexMetadata& metadata, _In_ DWORD flags, _In_z_ LPCWSTR szFile );

    // TGA operations
    HRESULT LoadFromTGAMemory( _In_bytecount_(size) LPCVOID pSource, _In_ size_t size,
                               _Out_opt_ TexMetadata* metadata, _Out_ ScratchImage& image );
    HRESULT LoadFromTGAFile( _In_z_ LPCWSTR szFile,
                             _Out_opt_ TexMetadata* metadata, _Out_ ScratchImage& image );

    HRESULT SaveToTGAMemory( _In_ const Image& image, _Out_ Blob& blob );
    HRESULT SaveToTGAFile( _In_ const Image& image, _In_z_ LPCWSTR szFile );

    // WIC operations
    HRESULT LoadFromWICMemory( _In_bytecount_(size) LPCVOID pSource, _In_ size_t size, _In_ DWORD flags,
                               _Out_opt_ TexMetadata* metadata, _Out_ ScratchImage& image );
    HRESULT LoadFromWICFile( _In_z_ LPCWSTR szFile, _In_ DWORD flags,
                             _Out_opt_ TexMetadata* metadata, _Out_ ScratchImage& image );

    HRESULT SaveToWICMemory( _In_ const Image& image, _In_ DWORD flags, _In_ REFGUID guidContainerFormat,
                             _Out_ Blob& blob, _In_opt_ const GUID* targetFormat = nullptr );
    HRESULT SaveToWICMemory( _In_count_(nimages) const Image* images, _In_ size_t nimages, _In_ DWORD flags, _In_ REFGUID guidContainerFormat,
                             _Out_ Blob& blob, _In_opt_ const GUID* targetFormat = nullptr );

    HRESULT SaveToWICFile( _In_ const Image& image, _In_ DWORD flags, _In_ REFGUID guidContainerFormat,
                           _In_z_ LPCWSTR szFile, _In_opt_ const GUID* targetFormat = nullptr );
    HRESULT SaveToWICFile( _In_count_(nimages) const Image* images, _In_ size_t nimages, _In_ DWORD flags, _In_ REFGUID guidContainerFormat,
                           _In_z_ LPCWSTR szFile, _In_opt_ const GUID* targetFormat = nullptr );

    enum WICCodecs
    {
        WIC_CODEC_BMP       =1,     // Windows Bitmap (.bmp)
        WIC_CODEC_JPEG,             // Joint Photographic Experts Group (.jpg, .jpeg)
        WIC_CODEC_PNG,              // Portable Network Graphics (.png)
        WIC_CODEC_TIFF,             // Tagged Image File Format  (.tif, .tiff)
        WIC_CODEC_GIF,              // Graphics Interchange Format  (.gif)
        WIC_CODEC_WMP,              // Windows Media Photo / HD Photo / JPEG XR (.hdp, .jxr, .wdp)
        WIC_CODEC_ICO,              // Windows Icon (.ico)
    };

    REFGUID GetWICCodec( _In_ WICCodecs codec );

    //---------------------------------------------------------------------------------
    // Texture conversion, resizing, mipmap generation, and block compression

    enum TEX_FR_FLAGS
    {
        TEX_FR_ROTATE0          = 0x0,
        TEX_FR_ROTATE90         = 0x1,
        TEX_FR_ROTATE180        = 0x2,
        TEX_FR_ROTATE270        = 0x3,
        TEX_FR_FLIP_HORIZONTAL  = 0x08,
        TEX_FR_FLIP_VERTICAL    = 0x10,
    };

    HRESULT FlipRotate( _In_ const Image& srcImage, _In_ DWORD flags, _Out_ ScratchImage& image );
    HRESULT FlipRotate( _In_count_(nimages) const Image* srcImages, _In_ size_t nimages, _In_ const TexMetadata& metadata,
                        _In_ DWORD flags, _Out_ ScratchImage& result );
        // Flip and/or rotate image

    enum TEX_FILTER_FLAGS
    {
        TEX_FILTER_DEFAULT          = 0,

        // Clamp filtering only

        TEX_FILTER_SEPARATE_ALPHA   = 0x100,
            // Resize color and alpha channel independently

        TEX_FILTER_DITHER           = 0x10000,
            // Use ordered 4x4 dithering for any required conversions
        TEX_FILTER_DITHER_DIFFUSION = 0x20000,
            // Use error-diffusion dithering for any required conversions

        TEX_FILTER_POINT            = 0x100000,
        TEX_FILTER_LINEAR           = 0x200000,
        TEX_FILTER_CUBIC            = 0x300000,
        TEX_FILTER_FANT             = 0x400000, // Equiv to Box filtering for mipmap generation
            // Filtering mode to use for any required image resizing

        TEX_FILTER_SRGB_IN          = 0x1000000,
        TEX_FILTER_SRGB_OUT         = 0x2000000,
        TEX_FILTER_SRGB             = 0x3000000,
            // sRGB <-> RGB for use in conversion operations
            // if the input format type is IsSRGB(), then SRGB_IN is on by default
            // if the output format type is IsSRGB(), then SRGB_OUT is on by default
    };

    HRESULT Resize( _In_ const Image& srcImage, _In_ size_t width, _In_ size_t height, _In_ DWORD filter,
                    _Out_ ScratchImage& image );
    HRESULT Resize( _In_count_(nimages) const Image* srcImages, _In_ size_t nimages, _In_ const TexMetadata& metadata,
                    _In_ size_t width, _In_ size_t height, _In_ DWORD filter, _Out_ ScratchImage& result );
        // Resize the image to width x height. Defaults to Fant filtering.
        // Note for a complex resize, the result will always have mipLevels == 1

    HRESULT Convert( _In_ const Image& srcImage, _In_ DXGI_FORMAT format, _In_ DWORD filter, _In_ float threshold,
                     _Out_ ScratchImage& image );
    HRESULT Convert( _In_count_(nimages) const Image* srcImages, _In_ size_t nimages, _In_ const TexMetadata& metadata,
                     _In_ DXGI_FORMAT format, _In_ DWORD filter, _In_ float threshold, _Out_ ScratchImage& result );
        // Convert the image to a new format
    
    HRESULT GenerateMipMaps( _In_ const Image& baseImage, _In_ DWORD filter, _In_ size_t levels,
                             _Out_ ScratchImage& mipChain, bool allow1D = false );
    HRESULT GenerateMipMaps( _In_count_(nimages) const Image* srcImages, _In_ size_t nimages, _In_ const TexMetadata& metadata,
                             _In_ DWORD filter, _In_ size_t levels, _Out_ ScratchImage& mipChain );
        // levels of '0' indicates a full mipchain, otherwise is generates that number of total levels (including the source base image)
        // Defaults to Fant filtering which is equivalent to a box filter

    HRESULT GenerateMipMaps3D( _In_count_(depth) const Image* baseImages, _In_ size_t depth, _In_ DWORD filter, _In_ size_t levels,
                               _Out_ ScratchImage& mipChain );
    HRESULT GenerateMipMaps3D( _In_count_(nimages) const Image* srcImages, _In_ size_t nimages, _In_ const TexMetadata& metadata,
                               _In_ DWORD filter, _In_ size_t levels, _Out_ ScratchImage& mipChain );
        // levels of '0' indicates a full mipchain, otherwise is generates that number of total levels (including the source base image)
        // Defaults to Fant filtering which is equivalent to a box filter

    enum TEX_COMPRESS_FLAGS
    {
        TEX_COMPRESS_DEFAULT        = 0,

        TEX_COMPRESS_RGB_DITHER     = 0x10000,
            // Enables dithering RGB colors for BC1-3 compression

        TEX_COMPRESS_A_DITHER       = 0x20000,
            // Enables dithering alpha for BC1-3 compression

        TEX_COMPRESS_DITHER         = 0x30000,
            // Enables both RGB and alpha dithering for BC1-3 compression

        TEX_COMPRESS_UNIFORM        = 0x40000,
            // Uniform color weighting for BC1-3 compression; by default uses perceptual weighting

        TEX_COMPRESS_PARALLEL       = 0x10000000,
            // Compress is free to use multithreading to improve performance (by default it does not use multithreading)
    };

    HRESULT Compress( _In_ const Image& srcImage, _In_ DXGI_FORMAT format, _In_ DWORD compress, _In_ float alphaRef,
                      _Out_ ScratchImage& cImage );
    HRESULT Compress( _In_count_(nimages) const Image* srcImages, _In_ size_t nimages, _In_ const TexMetadata& metadata,
                      _In_ DXGI_FORMAT format, _In_ DWORD compress, _In_ float alphaRef, _Out_ ScratchImage& cImages );
        // Note that alphaRef is only used by BC1. 0.5f is a typical value to use

    HRESULT Decompress( _In_ const Image& cImage, _In_ DXGI_FORMAT format, _Out_ ScratchImage& image );
    HRESULT Decompress( _In_count_(nimages) const Image* cImages, _In_ size_t nimages, _In_ const TexMetadata& metadata,
                        _In_ DXGI_FORMAT format, _Out_ ScratchImage& images );

    //---------------------------------------------------------------------------------
    // Normal map operations

    enum CNMAP_FLAGS
    {
        CNMAP_DEFAULT           = 0,

        CNMAP_CHANNEL_RED       = 0x1,
        CNMAP_CHANNEL_GREEN     = 0x2,
        CNMAP_CHANNEL_BLUE      = 0x3,
        CNMAP_CHANNEL_ALPHA     = 0x4,
        CNMAP_CHANNEL_LUMINANCE = 0x5,
            // Channel selection when evaluting color value for height
            // Luminance is a combination of red, green, and blue

        CNMAP_MIRROR_U          = 0x1000,
        CNMAP_MIRROR_V          = 0x2000,
        CNMAP_MIRROR            = 0x3000,
            // Use mirror semantics for scanline references (defaults to wrap)

        CNMAP_INVERT_SIGN       = 0x4000,
            // Inverts normal sign

        CNMAP_COMPUTE_OCCLUSION = 0x8000,
            // Computes a crude occlusion term stored in the alpha channel
    };

    HRESULT ComputeNormalMap( _In_ const Image& srcImage, _In_ DWORD flags, _In_ float amplitude,
                              _In_ DXGI_FORMAT format, _Out_ ScratchImage& normalMap );
    HRESULT ComputeNormalMap( _In_count_(nimages) const Image* srcImages, _In_ size_t nimages, _In_ const TexMetadata& metadata,
                              _In_ DWORD flags, _In_ float amplitude, _In_ DXGI_FORMAT format, _Out_ ScratchImage& normalMaps );

    //---------------------------------------------------------------------------------
    // Misc image operations
    struct Rect
    {
        size_t x;
        size_t y;
        size_t w;
        size_t h;

        Rect() {}
        Rect( size_t _x, size_t _y, size_t _w, size_t _h ) : x(_x), y(_y), w(_w), h(_h) {}
    };

    HRESULT CopyRectangle( _In_ const Image& srcImage, _In_ const Rect& srcRect, _In_ const Image& dstImage,
                           _In_ DWORD filter, _In_ size_t xOffset, _In_ size_t yOffset );

    HRESULT ComputeMSE( _In_ const Image& image1, _In_ const Image& image2, _Out_ float& mse, _Out_opt_cap_c_(4) float* mseV );

    //---------------------------------------------------------------------------------
    // Direct3D 11 functions
    bool IsSupportedTexture( _In_ ID3D11Device* pDevice, _In_ const TexMetadata& metadata );

    HRESULT CreateTexture( _In_ ID3D11Device* pDevice, _In_count_(nimages) const Image* srcImages, _In_ size_t nimages, _In_ const TexMetadata& metadata,
                           _Deref_out_ ID3D11Resource** ppResource );

    HRESULT CreateShaderResourceView( _In_ ID3D11Device* pDevice, _In_count_(nimages) const Image* srcImages, _In_ size_t nimages, _In_ const TexMetadata& metadata,
                                      _Deref_out_ ID3D11ShaderResourceView** ppSRV );

    HRESULT CaptureTexture( _In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pContext, _In_ ID3D11Resource* pSource, _Out_ ScratchImage& result );

#include "DirectXTex.inl"

}; // namespace
