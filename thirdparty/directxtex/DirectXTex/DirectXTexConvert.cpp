//-------------------------------------------------------------------------------------
// DirectXTexConvert.cpp
//  
// DirectX Texture Library - Image conversion 
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

#ifdef USE_XNAMATH
#if XNAMATH_VERSION < 205
#error This file requires XNAMATH v2.05 or later
#endif
#else
using namespace DirectX::PackedVector;
#endif

namespace DirectX
{

//-------------------------------------------------------------------------------------
// Copies an image row with optional clearing of alpha value to 1.0
// (can be used in place as well) otherwise copies the image row unmodified.
//-------------------------------------------------------------------------------------
void _CopyScanline( LPVOID pDestination, size_t outSize, LPCVOID pSource, size_t inSize, DXGI_FORMAT format, DWORD flags )
{
    assert( pDestination && outSize > 0 );
    assert( pSource && inSize > 0 );
    assert( IsValid(format) && !IsVideo(format) );

    if ( flags & TEXP_SCANLINE_SETALPHA )
    {
        switch( format )
        {
        //-----------------------------------------------------------------------------
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
            {
                uint32_t alpha;
                if ( format == DXGI_FORMAT_R32G32B32A32_FLOAT )
                    alpha = 0x3f800000;
                else if ( format == DXGI_FORMAT_R32G32B32A32_SINT )
                    alpha = 0x7fffffff;
                else
                    alpha = 0xffffffff;

                if ( pDestination == pSource )
                {
                    uint32_t *dPtr = reinterpret_cast<uint32_t*>(pDestination);
                    for( size_t count = 0; count < outSize; count += 16 )
                    {
                        dPtr += 3;
                        *(dPtr++) = alpha;
                    }
                }
                else
                {
                    const uint32_t * __restrict sPtr = reinterpret_cast<const uint32_t*>(pSource);
                    uint32_t * __restrict dPtr = reinterpret_cast<uint32_t*>(pDestination);
                    size_t size = std::min<size_t>( outSize, inSize );
                    for( size_t count = 0; count < size; count += 16 )
                    {
                        *(dPtr++) = *(sPtr++);
                        *(dPtr++) = *(sPtr++);
                        *(dPtr++) = *(sPtr++);
                        *(dPtr++) = alpha;
                        sPtr++;
                    }
                }
            }
            return;

        //-----------------------------------------------------------------------------
        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R16G16B16A16_SINT:
            {
                uint16_t alpha;
                if ( format == DXGI_FORMAT_R16G16B16A16_FLOAT )
                    alpha = 0x3c00;
                else if ( format == DXGI_FORMAT_R16G16B16A16_SNORM || format == DXGI_FORMAT_R16G16B16A16_SINT )
                    alpha = 0x7fff;
                else
                    alpha = 0xffff;

                if ( pDestination == pSource )
                {
                    uint16_t *dPtr = reinterpret_cast<uint16_t*>(pDestination);
                    for( size_t count = 0; count < outSize; count += 8 )
                    {
                        dPtr += 3;
                        *(dPtr++) = alpha;
                    }
                }
                else
                {
                    const uint16_t * __restrict sPtr = reinterpret_cast<const uint16_t*>(pSource);
                    uint16_t * __restrict dPtr = reinterpret_cast<uint16_t*>(pDestination);
                    size_t size = std::min<size_t>( outSize, inSize );
                    for( size_t count = 0; count < size; count += 8 )
                    {
                        *(dPtr++) = *(sPtr++);
                        *(dPtr++) = *(sPtr++);
                        *(dPtr++) = *(sPtr++);
                        *(dPtr++) = alpha;
                        sPtr++;
                    }
                }
            }
            return;

        //-----------------------------------------------------------------------------
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
            if ( pDestination == pSource )
            {
                uint32_t *dPtr = reinterpret_cast<uint32_t*>(pDestination);
                for( size_t count = 0; count < outSize; count += 4 )
                {
#pragma warning(suppress: 6001 6101) // PREFast doesn't properly understand the aliasing here.
                    *dPtr |= 0xC0000000;
                    ++dPtr;
                }
            }
            else
            {
                const uint32_t * __restrict sPtr = reinterpret_cast<const uint32_t*>(pSource);
                uint32_t * __restrict dPtr = reinterpret_cast<uint32_t*>(pDestination);
                size_t size = std::min<size_t>( outSize, inSize );
                for( size_t count = 0; count < size; count += 4 )
                {
                    *(dPtr++) = *(sPtr++) | 0xC0000000;
                }
            }
            return;

        //-----------------------------------------------------------------------------
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            {
                const uint32_t alpha = ( format == DXGI_FORMAT_R8G8B8A8_SNORM || format == DXGI_FORMAT_R8G8B8A8_SINT ) ? 0x7f000000 : 0xff000000;

                if ( pDestination == pSource )
                {
                    uint32_t *dPtr = reinterpret_cast<uint32_t*>(pDestination);
                    for( size_t count = 0; count < outSize; count += 4 )
                    {
                        uint32_t t = *dPtr & 0xFFFFFF;
                        t |= alpha;
                        *(dPtr++) = t;
                    }
                }
                else
                {
                    const uint32_t * __restrict sPtr = reinterpret_cast<const uint32_t*>(pSource);
                    uint32_t * __restrict dPtr = reinterpret_cast<uint32_t*>(pDestination);
                    size_t size = std::min<size_t>( outSize, inSize );
                    for( size_t count = 0; count < size; count += 4 )
                    {
                        uint32_t t = *(sPtr++) & 0xFFFFFF;
                        t |= alpha;
                        *(dPtr++) = t;
                    }
                }
            }
            return;

        //-----------------------------------------------------------------------------
        case DXGI_FORMAT_B5G5R5A1_UNORM:
            if ( pDestination == pSource )
            {
                uint16_t *dPtr = reinterpret_cast<uint16_t*>(pDestination);
                for( size_t count = 0; count < outSize; count += 2 )
                {
                    *(dPtr++) |= 0x8000;
                }
            }
            else
            {
                const uint16_t * __restrict sPtr = reinterpret_cast<const uint16_t*>(pSource);
                uint16_t * __restrict dPtr = reinterpret_cast<uint16_t*>(pDestination);
                size_t size = std::min<size_t>( outSize, inSize );
                for( size_t count = 0; count < size; count += 2 )
                {
                    *(dPtr++) = *(sPtr++) | 0x8000;
                }
            }
            return;

        //-----------------------------------------------------------------------------
        case DXGI_FORMAT_A8_UNORM:
            memset( pDestination, 0xff, outSize );
            return;

#ifdef DXGI_1_2_FORMATS
        //-----------------------------------------------------------------------------
        case DXGI_FORMAT_B4G4R4A4_UNORM:
            if ( pDestination == pSource )
            {
                uint16_t *dPtr = reinterpret_cast<uint16_t*>(pDestination);
                for( size_t count = 0; count < outSize; count += 2 )
                {
                    *(dPtr++) |= 0xF000;
                }
            }
            else
            {
                const uint16_t * __restrict sPtr = reinterpret_cast<const uint16_t*>(pSource);
                uint16_t * __restrict dPtr = reinterpret_cast<uint16_t*>(pDestination);
                size_t size = std::min<size_t>( outSize, inSize );
                for( size_t count = 0; count < size; count += 2 )
                {
                    *(dPtr++) = *(sPtr++) | 0xF000;
                }
            }
            return;
#endif // DXGI_1_2_FORMATS
        }
    }

    // Fall-through case is to just use memcpy (assuming this is not an in-place operation)
    if ( pDestination == pSource )
        return;

    size_t size = std::min<size_t>( outSize, inSize );
    memcpy_s( pDestination, outSize, pSource, size );
}


//-------------------------------------------------------------------------------------
// Swizzles (RGB <-> BGR) an image row with optional clearing of alpha value to 1.0
// (can be used in place as well) otherwise copies the image row unmodified.
//-------------------------------------------------------------------------------------
void _SwizzleScanline( LPVOID pDestination, size_t outSize, LPCVOID pSource, size_t inSize, DXGI_FORMAT format, DWORD flags )
{
    assert( pDestination && outSize > 0 );
    assert( pSource && inSize > 0 );
    assert( IsValid(format) && !IsVideo(format) );

    switch( format )
    {
    //---------------------------------------------------------------------------------
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        if ( flags & TEXP_SCANLINE_LEGACY )
        {
            // Swap Red (R) and Blue (B) channel (used for D3DFMT_A2R10G10B10 legacy sources)
            if ( pDestination == pSource )
            {
                uint32_t *dPtr = reinterpret_cast<uint32_t*>(pDestination);
                for( size_t count = 0; count < outSize; count += 4 )
                {
#pragma warning(suppress: 6001 6101) // PREFast doesn't properly understand the aliasing here.
                    uint32_t t = *dPtr;

                    uint32_t t1 = (t & 0x3ff00000) >> 20;
                    uint32_t t2 = (t & 0x000003ff) << 20;
                    uint32_t t3 = (t & 0x000ffc00);
                    uint32_t ta = ( flags & TEXP_SCANLINE_SETALPHA ) ? 0xC0000000 : (t & 0xC0000000);

                    *(dPtr++) = t1 | t2 | t3 | ta;
                }
            }
            else
            {
                const uint32_t * __restrict sPtr = reinterpret_cast<const uint32_t*>(pSource);
                uint32_t * __restrict dPtr = reinterpret_cast<uint32_t*>(pDestination);
                size_t size = std::min<size_t>( outSize, inSize );
                for( size_t count = 0; count < size; count += 4 )
                {
                    uint32_t t = *(sPtr++);

                    uint32_t t1 = (t & 0x3ff00000) >> 20;
                    uint32_t t2 = (t & 0x000003ff) << 20;
                    uint32_t t3 = (t & 0x000ffc00);
                    uint32_t ta = ( flags & TEXP_SCANLINE_SETALPHA ) ? 0xC0000000 : (t & 0xC0000000);

                    *(dPtr++) = t1 | t2 | t3 | ta;
                }
            }
            return;
        }
        break;

    //---------------------------------------------------------------------------------
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        // Swap Red (R) and Blue (B) channels (used to convert from DXGI 1.1 BGR formats to DXGI 1.0 RGB)
        if ( pDestination == pSource )
        {
            uint32_t *dPtr = reinterpret_cast<uint32_t*>(pDestination);
            for( size_t count = 0; count < outSize; count += 4 )
            {
                uint32_t t = *dPtr;

                uint32_t t1 = (t & 0x00ff0000) >> 16;
                uint32_t t2 = (t & 0x000000ff) << 16;
                uint32_t t3 = (t & 0x0000ff00);
                uint32_t ta = ( flags & TEXP_SCANLINE_SETALPHA ) ? 0xff000000 : (t & 0xFF000000);

                *(dPtr++) = t1 | t2 | t3 | ta;
            }
        }
        else
        {
            const uint32_t * __restrict sPtr = reinterpret_cast<const uint32_t*>(pSource);
            uint32_t * __restrict dPtr = reinterpret_cast<uint32_t*>(pDestination);
            size_t size = std::min<size_t>( outSize, inSize );
            for( size_t count = 0; count < size; count += 4 )
            {
                uint32_t t = *(sPtr++);

                uint32_t t1 = (t & 0x00ff0000) >> 16;
                uint32_t t2 = (t & 0x000000ff) << 16;
                uint32_t t3 = (t & 0x0000ff00);
                uint32_t ta = ( flags & TEXP_SCANLINE_SETALPHA ) ? 0xff000000 : (t & 0xFF000000);

                *(dPtr++) = t1 | t2 | t3 | ta;
            }
        }
        return;
    }

    // Fall-through case is to just use memcpy (assuming this is not an in-place operation)
    if ( pDestination == pSource )
        return;

    size_t size = std::min<size_t>( outSize, inSize );
    memcpy_s( pDestination, outSize, pSource, size );
}


//-------------------------------------------------------------------------------------
// Converts an image row with optional clearing of alpha value to 1.0
// Returns true if supported, false if expansion case not supported
//-------------------------------------------------------------------------------------
bool _ExpandScanline( LPVOID pDestination, size_t outSize, DXGI_FORMAT outFormat,  
                      LPCVOID pSource, size_t inSize, DXGI_FORMAT inFormat, DWORD flags )
{
    assert( pDestination && outSize > 0 );
    assert( pSource && inSize > 0 );
    assert( IsValid(outFormat) && !IsVideo(outFormat) );
    assert( IsValid(inFormat) && !IsVideo(inFormat) );

    switch( inFormat )
    {
    case DXGI_FORMAT_B5G6R5_UNORM:
        if ( outFormat != DXGI_FORMAT_R8G8B8A8_UNORM )
            return false;

        // DXGI_FORMAT_B5G6R5_UNORM -> DXGI_FORMAT_R8G8B8A8_UNORM
        {
            const uint16_t * __restrict sPtr = reinterpret_cast<const uint16_t*>(pSource);
            uint32_t * __restrict dPtr = reinterpret_cast<uint32_t*>(pDestination);

            for( size_t ocount = 0, icount = 0; ((icount < inSize) && (ocount < outSize)); icount += 2, ocount += 4 )
            {
                uint16_t t = *(sPtr++);

                uint32_t t1 = ((t & 0xf800) >> 8) | ((t & 0xe000) >> 13);
                uint32_t t2 = ((t & 0x07e0) << 5) | ((t & 0x0600) >> 5);
                uint32_t t3 = ((t & 0x001f) << 19) | ((t & 0x001c) << 14);

                *(dPtr++) = t1 | t2 | t3 | 0xff000000;
            }
        }
        return true;
        
    case DXGI_FORMAT_B5G5R5A1_UNORM:
        if ( outFormat != DXGI_FORMAT_R8G8B8A8_UNORM )
            return false;

        // DXGI_FORMAT_B5G5R5A1_UNORM -> DXGI_FORMAT_R8G8B8A8_UNORM
        {
            const uint16_t * __restrict sPtr = reinterpret_cast<const uint16_t*>(pSource);
            uint32_t * __restrict dPtr = reinterpret_cast<uint32_t*>(pDestination);

            for( size_t ocount = 0, icount = 0; ((icount < inSize) && (ocount < outSize)); icount += 2, ocount += 4 )
            {
                uint16_t t = *(sPtr++);

                uint32_t t1 = ((t & 0x7c00) >> 7) | ((t & 0x7000) >> 12);
                uint32_t t2 = ((t & 0x03e0) << 6) | ((t & 0x0380) << 1);
                uint32_t t3 = ((t & 0x001f) << 19) | ((t & 0x001c) << 14);
                uint32_t ta = ( flags & TEXP_SCANLINE_SETALPHA ) ? 0xff000000 : ((t & 0x8000) ? 0xff000000 : 0);

                *(dPtr++) = t1 | t2 | t3 | ta;
            }
        }
        return true;

#ifdef DXGI_1_2_FORMATS
    case DXGI_FORMAT_B4G4R4A4_UNORM:
        if ( outFormat != DXGI_FORMAT_R8G8B8A8_UNORM )
            return false;

        // DXGI_FORMAT_B4G4R4A4_UNORM -> DXGI_FORMAT_R8G8B8A8_UNORM
        {
            const uint16_t * __restrict sPtr = reinterpret_cast<const uint16_t*>(pSource);
            uint32_t * __restrict dPtr = reinterpret_cast<uint32_t*>(pDestination);

            for( size_t ocount = 0, icount = 0; ((icount < inSize) && (ocount < outSize)); icount += 2, ocount += 4 )
            {
                uint16_t t = *(sPtr++);

                uint32_t t1 = ((t & 0x0f00) >> 4) | ((t & 0x0f00) >> 8);
                uint32_t t2 = ((t & 0x00f0) << 8) | ((t & 0x00f0) << 4);
                uint32_t t3 = ((t & 0x000f) << 20) | ((t & 0x000f) << 16);
                uint32_t ta = ( flags & TEXP_SCANLINE_SETALPHA ) ? 0xff000000 : (((t & 0xf000) << 16) | ((t & 0xf000) << 12));

                *(dPtr++) = t1 | t2 | t3 | ta;
            }
        }
        return true;
#endif // DXGI_1_2_FORMATS
    }

    return false;
}


//-------------------------------------------------------------------------------------
// Loads an image row into standard RGBA XMVECTOR (aligned) array
//-------------------------------------------------------------------------------------
#define LOAD_SCANLINE( type, func )\
        if ( size >= sizeof(type) )\
        {\
            const type * __restrict sPtr = reinterpret_cast<const type*>(pSource);\
            for( size_t icount = 0; icount < size; icount += sizeof(type) )\
            {\
                if ( dPtr >= ePtr ) break;\
                *(dPtr++) = func( sPtr++ );\
            }\
            return true;\
        }\
        return false;

#define LOAD_SCANLINE3( type, func, defvec )\
        if ( size >= sizeof(type) )\
        {\
            const type * __restrict sPtr = reinterpret_cast<const type*>(pSource);\
            for( size_t icount = 0; icount < size; icount += sizeof(type) )\
            {\
                XMVECTOR v = func( sPtr++ );\
                if ( dPtr >= ePtr ) break;\
                *(dPtr++) = XMVectorSelect( defvec, v, g_XMSelect1110 );\
            }\
            return true;\
        }\
        return false;

#define LOAD_SCANLINE2( type, func, defvec )\
        if ( size >= sizeof(type) )\
        {\
            const type * __restrict sPtr = reinterpret_cast<const type*>(pSource);\
            for( size_t icount = 0; icount < size; icount += sizeof(type) )\
            {\
                XMVECTOR v = func( sPtr++ );\
                if ( dPtr >= ePtr ) break;\
                *(dPtr++) = XMVectorSelect( defvec, v, g_XMSelect1100 );\
            }\
            return true;\
        }\
        return false;

bool _LoadScanline( XMVECTOR* pDestination, size_t count,
                    LPCVOID pSource, size_t size, DXGI_FORMAT format )
{
    assert( pDestination && count > 0 && (((uintptr_t)pDestination & 0xF) == 0) );
    assert( pSource && size > 0 );
    assert( IsValid(format) && !IsVideo(format) && !IsTypeless(format) && !IsCompressed(format) );

    XMVECTOR* __restrict dPtr = pDestination;
    if ( !dPtr )
        return false;

    const XMVECTOR* ePtr = pDestination + count;

    switch( format )
    {
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
        {
            size_t msize = (size > (sizeof(XMVECTOR)*count)) ? (sizeof(XMVECTOR)*count) : size;
            memcpy_s( dPtr, sizeof(XMVECTOR)*count, pSource, msize );
        }
        return true;

    case DXGI_FORMAT_R32G32B32A32_UINT:
        LOAD_SCANLINE( XMUINT4, XMLoadUInt4 )

    case DXGI_FORMAT_R32G32B32A32_SINT:
        LOAD_SCANLINE( XMINT4, XMLoadSInt4 )

    case DXGI_FORMAT_R32G32B32_FLOAT:
        LOAD_SCANLINE3( XMFLOAT3, XMLoadFloat3, g_XMIdentityR3 )

    case DXGI_FORMAT_R32G32B32_UINT:
        LOAD_SCANLINE3( XMUINT3, XMLoadUInt3, g_XMIdentityR3 )

    case DXGI_FORMAT_R32G32B32_SINT:
        LOAD_SCANLINE3( XMINT3, XMLoadSInt3, g_XMIdentityR3 )

    case DXGI_FORMAT_R16G16B16A16_FLOAT:
        LOAD_SCANLINE( XMHALF4, XMLoadHalf4 )

    case DXGI_FORMAT_R16G16B16A16_UNORM:
        LOAD_SCANLINE( XMUSHORTN4, XMLoadUShortN4 ) 

    case DXGI_FORMAT_R16G16B16A16_UINT:
        LOAD_SCANLINE( XMUSHORT4, XMLoadUShort4 )

    case DXGI_FORMAT_R16G16B16A16_SNORM:
        LOAD_SCANLINE( XMSHORTN4, XMLoadShortN4 )

    case DXGI_FORMAT_R16G16B16A16_SINT:
        LOAD_SCANLINE( XMSHORT4, XMLoadShort4 )

    case DXGI_FORMAT_R32G32_FLOAT:
        LOAD_SCANLINE2( XMFLOAT2, XMLoadFloat2, g_XMIdentityR3 )

    case DXGI_FORMAT_R32G32_UINT:
        LOAD_SCANLINE2( XMUINT2, XMLoadUInt2, g_XMIdentityR3 )

    case DXGI_FORMAT_R32G32_SINT:
        LOAD_SCANLINE2( XMINT2, XMLoadSInt2, g_XMIdentityR3 )

    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        if ( size >= (sizeof(float)+sizeof(uint32_t)) )
        {
            const float * sPtr = reinterpret_cast<const float*>(pSource);
            for( size_t icount = 0; icount < size; icount += (sizeof(float)+sizeof(uint32_t)) )
            {
                const uint8_t* ps8 = reinterpret_cast<const uint8_t*>( &sPtr[1] );
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSet( sPtr[0], static_cast<float>( *ps8 ), 0.f, 1.f );
                sPtr += 2;
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        LOAD_SCANLINE( XMUDECN4, XMLoadUDecN4 );

    case DXGI_FORMAT_R10G10B10A2_UINT:
        LOAD_SCANLINE( XMUDEC4, XMLoadUDec4 );

    case DXGI_FORMAT_R11G11B10_FLOAT:
        LOAD_SCANLINE3( XMFLOAT3PK, XMLoadFloat3PK, g_XMIdentityR3 );

    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        LOAD_SCANLINE( XMUBYTEN4, XMLoadUByteN4 )

    case DXGI_FORMAT_R8G8B8A8_UINT:
        LOAD_SCANLINE( XMUBYTE4, XMLoadUByte4 )

    case DXGI_FORMAT_R8G8B8A8_SNORM:
        LOAD_SCANLINE( XMBYTEN4, XMLoadByteN4 )

    case DXGI_FORMAT_R8G8B8A8_SINT:
        LOAD_SCANLINE( XMBYTE4, XMLoadByte4 )

    case DXGI_FORMAT_R16G16_FLOAT:
        LOAD_SCANLINE2( XMHALF2, XMLoadHalf2, g_XMIdentityR3 )

    case DXGI_FORMAT_R16G16_UNORM:
        LOAD_SCANLINE2( XMUSHORTN2, XMLoadUShortN2, g_XMIdentityR3 )

    case DXGI_FORMAT_R16G16_UINT:
        LOAD_SCANLINE2( XMUSHORT2, XMLoadUShort2, g_XMIdentityR3 )

    case DXGI_FORMAT_R16G16_SNORM:
        LOAD_SCANLINE2( XMSHORTN2, XMLoadShortN2, g_XMIdentityR3 )

    case DXGI_FORMAT_R16G16_SINT:
        LOAD_SCANLINE2( XMSHORT2, XMLoadShort2, g_XMIdentityR3 )

    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
        if ( size >= sizeof(float) )
        {
            const float* __restrict sPtr = reinterpret_cast<const float*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(float) )
            {
                XMVECTOR v = XMLoadFloat( sPtr++ );
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSelect( g_XMIdentityR3, v, g_XMSelect1000 );
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R32_UINT:
        if ( size >= sizeof(uint32_t) )
        {
            const uint32_t* __restrict sPtr = reinterpret_cast<const uint32_t*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(uint32_t) )
            {
                XMVECTOR v = XMLoadInt( sPtr++ );
                v = XMConvertVectorUIntToFloat( v, 0 );
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSelect( g_XMIdentityR3, v, g_XMSelect1000 );
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R32_SINT:
        if ( size >= sizeof(int32_t) )
        {
            const int32_t * __restrict sPtr = reinterpret_cast<const int32_t*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(int32_t) )
            {
                XMVECTOR v = XMLoadInt( reinterpret_cast<const uint32_t*> (sPtr++) );
                v = XMConvertVectorIntToFloat( v, 0 );
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSelect( g_XMIdentityR3, v, g_XMSelect1000 );
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        if ( size >= sizeof(uint32_t) )
        {
            const uint32_t * sPtr = reinterpret_cast<const uint32_t*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(uint32_t) )
            {
                float d = static_cast<float>( *sPtr & 0xFFFFFF ) / 16777215.f;
                float s = static_cast<float>( ( *sPtr & 0xFF000000 ) >> 24 );
                ++sPtr;
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSet( d, s, 0.f, 1.f );
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R8G8_UNORM:
        LOAD_SCANLINE2( XMUBYTEN2, XMLoadUByteN2, g_XMIdentityR3 )

    case DXGI_FORMAT_R8G8_UINT:
        LOAD_SCANLINE2( XMUBYTE2, XMLoadUByte2, g_XMIdentityR3 )

    case DXGI_FORMAT_R8G8_SNORM:
        LOAD_SCANLINE2( XMBYTEN2, XMLoadByteN2, g_XMIdentityR3 )

    case DXGI_FORMAT_R8G8_SINT:
        LOAD_SCANLINE2( XMBYTE2, XMLoadByte2, g_XMIdentityR3 )

    case DXGI_FORMAT_R16_FLOAT:
        if ( size >= sizeof(HALF) )
        {
            const HALF * __restrict sPtr = reinterpret_cast<const HALF*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(HALF) )
            {
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSet( XMConvertHalfToFloat(*sPtr++), 0.f, 0.f, 1.f );
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
        if ( size >= sizeof(uint16_t) )
        {
            const uint16_t* __restrict sPtr = reinterpret_cast<const uint16_t*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(uint16_t) )
            {
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSet( static_cast<float>(*sPtr++) / 65535.f, 0.f, 0.f, 1.f );
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R16_UINT:
        if ( size >= sizeof(uint16_t) )
        {
            const uint16_t * __restrict sPtr = reinterpret_cast<const uint16_t*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(uint16_t) )
            {
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSet( static_cast<float>(*sPtr++), 0.f, 0.f, 1.f );
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R16_SNORM:
        if ( size >= sizeof(int16_t) )
        {
            const int16_t * __restrict sPtr = reinterpret_cast<const int16_t*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(int16_t) )
            {
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSet( static_cast<float>(*sPtr++) / 32767.f, 0.f, 0.f, 1.f );
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R16_SINT:
        if ( size >= sizeof(int16_t) )
        {
            const int16_t * __restrict sPtr = reinterpret_cast<const int16_t*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(int16_t) )
            {
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSet( static_cast<float>(*sPtr++), 0.f, 0.f, 1.f );
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R8_UNORM:
        if ( size >= sizeof(uint8_t) )
        {
            const uint8_t * __restrict sPtr = reinterpret_cast<const uint8_t*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(uint8_t) )
            {
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSet( static_cast<float>(*sPtr++) / 255.f, 0.f, 0.f, 1.f );
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R8_UINT:
        if ( size >= sizeof(uint8_t) )
        {
            const uint8_t * __restrict sPtr = reinterpret_cast<const uint8_t*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(uint8_t) )
            {
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSet( static_cast<float>(*sPtr++), 0.f, 0.f, 1.f );
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R8_SNORM:
        if ( size >= sizeof(char) )
        {
            const char * __restrict sPtr = reinterpret_cast<const char*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(char) )
            {
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSet( static_cast<float>(*sPtr++) / 127.f, 0.f, 0.f, 1.f );
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R8_SINT:
        if ( size >= sizeof(char) )
        {
            const char * __restrict sPtr = reinterpret_cast<const char*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(char) )
            {
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSet( static_cast<float>(*sPtr++), 0.f, 0.f, 1.f );
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_A8_UNORM:
        if ( size >= sizeof(uint8_t) )
        {
            const uint8_t * __restrict sPtr = reinterpret_cast<const uint8_t*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(uint8_t) )
            {
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSet( 0.f, 0.f, 0.f, static_cast<float>(*sPtr++) / 255.f );
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R1_UNORM:
        if ( size >= sizeof(uint8_t) )
        {
            const uint8_t * __restrict sPtr = reinterpret_cast<const uint8_t*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(uint8_t) )
            {
                for( size_t bcount = 0; bcount < 8; ++bcount )
                {
                    if ( dPtr >= ePtr ) break;
                    *(dPtr++) = XMVectorSet( (((*sPtr >> bcount) & 0x1) ? 1.f : 0.f), 0.f, 0.f, 1.f );
                }
                
                ++sPtr;
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        LOAD_SCANLINE3( XMFLOAT3SE, XMLoadFloat3SE, g_XMIdentityR3 )

    case DXGI_FORMAT_R8G8_B8G8_UNORM:
        if ( size >= sizeof(XMUBYTEN4) )
        {
            const XMUBYTEN4 * __restrict sPtr = reinterpret_cast<const XMUBYTEN4*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(XMUBYTEN4) )
            {
                XMVECTOR v = XMLoadUByteN4( sPtr++ );
                XMVECTOR v1 = XMVectorSwizzle<0, 3, 2, 1>( v );
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSelect( g_XMIdentityR3, v, g_XMSelect1110 );
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSelect( g_XMIdentityR3, v1, g_XMSelect1110 );
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_G8R8_G8B8_UNORM:
        if ( size >= sizeof(XMUBYTEN4) )
        {
            const XMUBYTEN4 * __restrict sPtr = reinterpret_cast<const XMUBYTEN4*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(XMUBYTEN4) )
            {
                XMVECTOR v = XMLoadUByteN4( sPtr++ );
                XMVECTOR v0 = XMVectorSwizzle<1, 0, 3, 2>( v );
                XMVECTOR v1 = XMVectorSwizzle<1, 2, 3, 0>( v );
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSelect( g_XMIdentityR3, v0, g_XMSelect1110 );
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSelect( g_XMIdentityR3, v1, g_XMSelect1110 );
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_B5G6R5_UNORM:
        if ( size >= sizeof(XMU565) )
        {
            static XMVECTORF32 s_Scale = { 1.f/31.f, 1.f/63.f, 1.f/31.f, 1.f };
            const XMU565 * __restrict sPtr = reinterpret_cast<const XMU565*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(XMU565) )
            {
                XMVECTOR v = XMLoadU565( sPtr++ );
                v = XMVectorMultiply( v, s_Scale );
                v = XMVectorSwizzle<2, 1, 0, 3>( v );
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSelect( g_XMIdentityR3, v, g_XMSelect1110 );
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_B5G5R5A1_UNORM:
        if ( size >= sizeof(XMU555) )
        {
            static XMVECTORF32 s_Scale = { 1.f/31.f, 1.f/31.f, 1.f/31.f, 1.f };
            const XMU555 * __restrict sPtr = reinterpret_cast<const XMU555*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(XMU555) )
            {
                XMVECTOR v = XMLoadU555( sPtr++ );
                v = XMVectorMultiply( v, s_Scale );
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSwizzle<2, 1, 0, 3>( v );
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        if ( size >= sizeof(XMUBYTEN4) )
        {
            const XMUBYTEN4 * __restrict sPtr = reinterpret_cast<const XMUBYTEN4*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(XMUBYTEN4) )
            {
                XMVECTOR v = XMLoadUByteN4( sPtr++ );
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSwizzle<2, 1, 0, 3>( v );
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        if ( size >= sizeof(XMUBYTEN4) )
        {
            const XMUBYTEN4 * __restrict sPtr = reinterpret_cast<const XMUBYTEN4*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(XMUBYTEN4) )
            {
                XMVECTOR v = XMLoadUByteN4( sPtr++ );
                v = XMVectorSwizzle<2, 1, 0, 3>( v );
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSelect( g_XMIdentityR3, v, g_XMSelect1110 );
            }
            return true;
        }
        return false;

#ifdef DXGI_1_2_FORMATS
    case DXGI_FORMAT_B4G4R4A4_UNORM:
        if ( size >= sizeof(XMUNIBBLE4) )
        {
            static XMVECTORF32 s_Scale = { 1.f/15.f, 1.f/15.f, 1.f/15.f, 1.f/15.f };
            const XMUNIBBLE4 * __restrict sPtr = reinterpret_cast<const XMUNIBBLE4*>(pSource);
            for( size_t icount = 0; icount < size; icount += sizeof(XMUNIBBLE4) )
            {
                XMVECTOR v = XMLoadUNibble4( sPtr++ );
                v = XMVectorMultiply( v, s_Scale );
                if ( dPtr >= ePtr ) break;
                *(dPtr++) = XMVectorSwizzle<2, 1, 0, 3>( v );
            }
            return true;
        }
        return false;

    // we don't support the video formats ( see IsVideo function )
#endif // DXGI_1_2_FORMATS

    default:
        return false;
    }
}


//-------------------------------------------------------------------------------------
// Stores an image row from standard RGBA XMVECTOR (aligned) array
//-------------------------------------------------------------------------------------
#define STORE_SCANLINE( type, func )\
        if ( size >= sizeof(type) )\
        {\
            type * __restrict dPtr = reinterpret_cast<type*>(pDestination);\
            for( size_t icount = 0; icount < size; icount += sizeof(type) )\
            {\
                if ( sPtr >= ePtr ) break;\
                func( dPtr++, *sPtr++ );\
            }\
        }\
        return true;

bool _StoreScanline( LPVOID pDestination, size_t size, DXGI_FORMAT format,
                     const XMVECTOR* pSource, size_t count )
{
    assert( pDestination && size > 0 );
    assert( pSource && count > 0 && (((uintptr_t)pSource & 0xF) == 0) );
    assert( IsValid(format) && !IsVideo(format) && !IsTypeless(format) && !IsCompressed(format) );

    const XMVECTOR* __restrict sPtr = pSource;
    if ( !sPtr )
        return false;

    const XMVECTOR* ePtr = pSource + count;

    switch( format )
    {
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
        STORE_SCANLINE( XMFLOAT4, XMStoreFloat4 )

    case DXGI_FORMAT_R32G32B32A32_UINT:
        STORE_SCANLINE( XMUINT4, XMStoreUInt4 )

    case DXGI_FORMAT_R32G32B32A32_SINT:
        STORE_SCANLINE( XMINT4, XMStoreSInt4 )

    case DXGI_FORMAT_R32G32B32_FLOAT:
        STORE_SCANLINE( XMFLOAT3, XMStoreFloat3 )

    case DXGI_FORMAT_R32G32B32_UINT:
        STORE_SCANLINE( XMUINT3, XMStoreUInt3 )

    case DXGI_FORMAT_R32G32B32_SINT:
        STORE_SCANLINE( XMINT3, XMStoreSInt3 )

    case DXGI_FORMAT_R16G16B16A16_FLOAT:
        STORE_SCANLINE( XMHALF4, XMStoreHalf4 )

    case DXGI_FORMAT_R16G16B16A16_UNORM:
        STORE_SCANLINE( XMUSHORTN4, XMStoreUShortN4 ) 

    case DXGI_FORMAT_R16G16B16A16_UINT:
        STORE_SCANLINE( XMUSHORT4, XMStoreUShort4 )

    case DXGI_FORMAT_R16G16B16A16_SNORM:
        STORE_SCANLINE( XMSHORTN4, XMStoreShortN4 )

    case DXGI_FORMAT_R16G16B16A16_SINT:
        STORE_SCANLINE( XMSHORT4, XMStoreShort4 )

    case DXGI_FORMAT_R32G32_FLOAT:
        STORE_SCANLINE( XMFLOAT2, XMStoreFloat2 )

    case DXGI_FORMAT_R32G32_UINT:
        STORE_SCANLINE( XMUINT2, XMStoreUInt2 )

    case DXGI_FORMAT_R32G32_SINT:
        STORE_SCANLINE( XMINT2, XMStoreSInt2 )

    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        if ( size >= (sizeof(float)+sizeof(uint32_t)) )
        {
            float *dPtr = reinterpret_cast<float*>(pDestination);
            for( size_t icount = 0; icount < size; icount += (sizeof(float)+sizeof(uint32_t)) )
            {
                if ( sPtr >= ePtr ) break;
                XMFLOAT4 f;
                XMStoreFloat4( &f, *sPtr++ );
                dPtr[0] = f.x;
                uint8_t* ps8 = reinterpret_cast<uint8_t*>( &dPtr[1] );
                ps8[0] = static_cast<uint8_t>( std::min<float>( 255.f, std::max<float>( 0.f, f.y ) ) );
                ps8[1] = ps8[2] = ps8[3] = 0;
                dPtr += 2;
            }
        }
        return true;

    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        STORE_SCANLINE( XMUDECN4, XMStoreUDecN4 );

    case DXGI_FORMAT_R10G10B10A2_UINT:
        STORE_SCANLINE( XMUDEC4, XMStoreUDec4 );

    case DXGI_FORMAT_R11G11B10_FLOAT:
        STORE_SCANLINE( XMFLOAT3PK, XMStoreFloat3PK );

    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        STORE_SCANLINE( XMUBYTEN4, XMStoreUByteN4 )

    case DXGI_FORMAT_R8G8B8A8_UINT:
        STORE_SCANLINE( XMUBYTE4, XMStoreUByte4 )

    case DXGI_FORMAT_R8G8B8A8_SNORM:
        STORE_SCANLINE( XMBYTEN4, XMStoreByteN4 )

    case DXGI_FORMAT_R8G8B8A8_SINT:
        STORE_SCANLINE( XMBYTE4, XMStoreByte4 )

    case DXGI_FORMAT_R16G16_FLOAT:
        STORE_SCANLINE( XMHALF2, XMStoreHalf2 )

    case DXGI_FORMAT_R16G16_UNORM:
        STORE_SCANLINE( XMUSHORTN2, XMStoreUShortN2 )

    case DXGI_FORMAT_R16G16_UINT:
        STORE_SCANLINE( XMUSHORT2, XMStoreUShort2 )

    case DXGI_FORMAT_R16G16_SNORM:
        STORE_SCANLINE( XMSHORTN2, XMStoreShortN2 )

    case DXGI_FORMAT_R16G16_SINT:
        STORE_SCANLINE( XMSHORT2, XMStoreShort2 )

    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
        if ( size >= sizeof(float) )
        {
            float * __restrict dPtr = reinterpret_cast<float*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(float) )
            {
                if ( sPtr >= ePtr ) break;
                XMStoreFloat( dPtr++, *(sPtr++) );
            }
        }
        return true;

    case DXGI_FORMAT_R32_UINT:
        if ( size >= sizeof(uint32_t) )
        {
            uint32_t * __restrict dPtr = reinterpret_cast<uint32_t*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(uint32_t) )
            {
                if ( sPtr >= ePtr ) break;
                XMVECTOR v = XMConvertVectorFloatToUInt( *(sPtr++), 0 );
                XMStoreInt( dPtr++, v );
            }
        }
        return true;

    case DXGI_FORMAT_R32_SINT:
        if ( size >= sizeof(uint32_t) )
        {
            uint32_t * __restrict dPtr = reinterpret_cast<uint32_t*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(uint32_t) )
            {
                if ( sPtr >= ePtr ) break;
                XMVECTOR v = XMConvertVectorFloatToInt( *(sPtr++), 0 );
                XMStoreInt( dPtr++, v );
            }
        }
        return true;

    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        if ( size >= sizeof(uint32_t) )
        {
            static const XMVECTORF32 clamp = { 1.f, 255.f, 0.f, 0.f };
            XMVECTOR zero = XMVectorZero();
            uint32_t *dPtr = reinterpret_cast<uint32_t*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(uint32_t) )
            {
                if ( sPtr >= ePtr ) break;
                XMFLOAT4 f;
                XMStoreFloat4( &f, XMVectorClamp( *sPtr++, zero, clamp ) );
                *dPtr++ = (static_cast<uint32_t>( f.x * 16777215.f ) & 0xFFFFFF)
                          | ((static_cast<uint32_t>( f.y ) & 0xFF) << 24);
            }
        }
        return true;

    case DXGI_FORMAT_R8G8_UNORM:
        STORE_SCANLINE( XMUBYTEN2, XMStoreUByteN2 )

    case DXGI_FORMAT_R8G8_UINT:
        STORE_SCANLINE( XMUBYTE2, XMStoreUByte2 )

    case DXGI_FORMAT_R8G8_SNORM:
        STORE_SCANLINE( XMBYTEN2, XMStoreByteN2 )

    case DXGI_FORMAT_R8G8_SINT:
        STORE_SCANLINE( XMBYTE2, XMStoreByte2 )

    case DXGI_FORMAT_R16_FLOAT:
        if ( size >= sizeof(HALF) )
        {
            HALF * __restrict dPtr = reinterpret_cast<HALF*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(HALF) )
            {
                if ( sPtr >= ePtr ) break;
                float v = XMVectorGetX( *sPtr++ );
                *(dPtr++) = XMConvertFloatToHalf(v);
            }
        }
        return true;

    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
        if ( size >= sizeof(int16_t) )
        {
            int16_t * __restrict dPtr = reinterpret_cast<int16_t*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(int16_t) )
            {
                if ( sPtr >= ePtr ) break;
                float v = XMVectorGetX( *sPtr++ );
                v = std::max<float>( std::min<float>( v, 1.f ), 0.f );
                *(dPtr++) = static_cast<uint16_t>( v*65535.f + 0.5f );
            }
        }
        return true;

    case DXGI_FORMAT_R16_UINT:
        if ( size >= sizeof(uint16_t) )
        {
            uint16_t * __restrict dPtr = reinterpret_cast<uint16_t*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(uint16_t) )
            {
                if ( sPtr >= ePtr ) break;
                float v = XMVectorGetX( *sPtr++ );
                v = std::max<float>( std::min<float>( v, 65535.f ), 0.f );
                *(dPtr++) = static_cast<uint16_t>(v);
            }
        }
        return true;

    case DXGI_FORMAT_R16_SNORM:
        if ( size >= sizeof(int16_t) )
        {
            int16_t * __restrict dPtr = reinterpret_cast<int16_t*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(int16_t) )
            {
                if ( sPtr >= ePtr ) break;
                float v = XMVectorGetX( *sPtr++ );
                v = std::max<float>( std::min<float>( v, 1.f ), -1.f );
                *(dPtr++) = static_cast<uint16_t>( v * 32767.f );
            }
        }
        return true;

    case DXGI_FORMAT_R16_SINT:
        if ( size >= sizeof(int16_t) )
        {
            int16_t * __restrict dPtr = reinterpret_cast<int16_t*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(int16_t) )
            {
                if ( sPtr >= ePtr ) break;
                float v = XMVectorGetX( *sPtr++ );
                v = std::max<float>( std::min<float>( v, 32767.f ), -32767.f );
                *(dPtr++) = static_cast<int16_t>(v);
            }
        }
        return true;

    case DXGI_FORMAT_R8_UNORM:
        if ( size >= sizeof(uint8_t) )
        {
            uint8_t * __restrict dPtr = reinterpret_cast<uint8_t*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(uint8_t) )
            {
                if ( sPtr >= ePtr ) break;
                float v = XMVectorGetX( *sPtr++ );
                v = std::max<float>( std::min<float>( v, 1.f ), 0.f );
                *(dPtr++) = static_cast<uint8_t>( v * 255.f);
            }
        }
        return true;

    case DXGI_FORMAT_R8_UINT:
        if ( size >= sizeof(uint8_t) )
        {
            uint8_t * __restrict dPtr = reinterpret_cast<uint8_t*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(uint8_t) )
            {
                if ( sPtr >= ePtr ) break;
                float v = XMVectorGetX( *sPtr++ );
                v = std::max<float>( std::min<float>( v, 255.f ), 0.f );
                *(dPtr++) = static_cast<uint8_t>(v);
            }
        }
        return true;

    case DXGI_FORMAT_R8_SNORM:
        if ( size >= sizeof(char) )
        {
            char * __restrict dPtr = reinterpret_cast<char*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(char) )
            {
                if ( sPtr >= ePtr ) break;
                float v = XMVectorGetX( *sPtr++ );
                v = std::max<float>( std::min<float>( v, 1.f ), -1.f );
                *(dPtr++) = static_cast<char>( v * 127.f );
            }
        }
        return true;

    case DXGI_FORMAT_R8_SINT:
        if ( size >= sizeof(char) )
        {
            char * __restrict dPtr = reinterpret_cast<char*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(char) )
            {
                if ( sPtr >= ePtr ) break;
                float v = XMVectorGetX( *sPtr++ );
                v = std::max<float>( std::min<float>( v, 127.f ), -127.f );
                *(dPtr++) = static_cast<char>( v );
            }
        }
        return true;

    case DXGI_FORMAT_A8_UNORM:
        if ( size >= sizeof(uint8_t) )
        {
            uint8_t * __restrict dPtr = reinterpret_cast<uint8_t*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(uint8_t) )
            {
                if ( sPtr >= ePtr ) break;
                float v = XMVectorGetW( *sPtr++ );
                v = std::max<float>( std::min<float>( v, 1.f ), 0.f );
                *(dPtr++) = static_cast<uint8_t>( v * 255.f);
            }
        }
        return true;

    case DXGI_FORMAT_R1_UNORM:
        if ( size >= sizeof(uint8_t) )
        {
            uint8_t * __restrict dPtr = reinterpret_cast<uint8_t*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(uint8_t) )
            {
                uint8_t pixels = 0;
                for( size_t bcount = 0; bcount < 8; ++bcount )
                {
                    if ( sPtr >= ePtr ) break;
                    float v = XMVectorGetX( *sPtr++ );
                    if ( v > 0.5f )
                        pixels |= 1 << bcount;
                }
                *(dPtr++) = pixels;
            }
        }
        return true;

    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        STORE_SCANLINE( XMFLOAT3SE, XMStoreFloat3SE )

    case DXGI_FORMAT_R8G8_B8G8_UNORM:
        if ( size >= sizeof(XMUBYTEN4) )
        {
            XMUBYTEN4 * __restrict dPtr = reinterpret_cast<XMUBYTEN4*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(XMUBYTEN4) )
            {
                if ( sPtr >= ePtr ) break;
                XMVECTOR v0 = *sPtr++;
                XMVECTOR v1 = (sPtr < ePtr) ? XMVectorSplatY( *sPtr++ ) : XMVectorZero();
                XMVECTOR v = XMVectorSelect( v1, v0, g_XMSelect1110 );
                XMStoreUByteN4( dPtr++, v );
            }
        }
        return true;

    case DXGI_FORMAT_G8R8_G8B8_UNORM:
        if ( size >= sizeof(XMUBYTEN4) )
        {
            static XMVECTORI32 select1101 = {XM_SELECT_1, XM_SELECT_1, XM_SELECT_0, XM_SELECT_1};

            XMUBYTEN4 * __restrict dPtr = reinterpret_cast<XMUBYTEN4*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(XMUBYTEN4) )
            {
                if ( sPtr >= ePtr ) break;
                XMVECTOR v0 = XMVectorSwizzle<1, 0, 3, 2>( *sPtr++ );
                XMVECTOR v1 = (sPtr < ePtr) ? XMVectorSplatY( *sPtr++ ) : XMVectorZero();
                XMVECTOR v = XMVectorSelect( v1, v0, select1101 );
                XMStoreUByteN4( dPtr++, v );
            }
        }
        return true;

    case DXGI_FORMAT_B5G6R5_UNORM:
        if ( size >= sizeof(XMU565) )
        {
            static XMVECTORF32 s_Scale = { 31.f, 63.f, 31.f, 1.f };
            XMU565 * __restrict dPtr = reinterpret_cast<XMU565*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(XMU565) )
            {
                if ( sPtr >= ePtr ) break;
                XMVECTOR v = XMVectorSwizzle<2, 1, 0, 3>( *sPtr++ );
                v = XMVectorMultiply( v, s_Scale );
                XMStoreU565( dPtr++, v );
            }
        }
        return true;

    case DXGI_FORMAT_B5G5R5A1_UNORM:
        if ( size >= sizeof(XMU555) )
        {
            static XMVECTORF32 s_Scale = { 31.f, 31.f, 31.f, 1.f };
            XMU555 * __restrict dPtr = reinterpret_cast<XMU555*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(XMU555) )
            {
                if ( sPtr >= ePtr ) break;
                XMVECTOR v = XMVectorSwizzle<2, 1, 0, 3>( *sPtr++ );
                v = XMVectorMultiply( v, s_Scale );
                XMStoreU555( dPtr++, v );
            }
        }
        return true;

    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        if ( size >= sizeof(XMUBYTEN4) )
        {
            XMUBYTEN4 * __restrict dPtr = reinterpret_cast<XMUBYTEN4*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(XMUBYTEN4) )
            {
                if ( sPtr >= ePtr ) break;
                XMVECTOR v = XMVectorSwizzle<2, 1, 0, 3>( *sPtr++ );
                XMStoreUByteN4( dPtr++, v );
            }
        }
        return true;

    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        if ( size >= sizeof(XMUBYTEN4) )
        {
            XMUBYTEN4 * __restrict dPtr = reinterpret_cast<XMUBYTEN4*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(XMUBYTEN4) )
            {
                if ( sPtr >= ePtr ) break;
                XMVECTOR v = XMVectorPermute<2, 1, 0, 7>( *sPtr++, g_XMIdentityR3 );
                XMStoreUByteN4( dPtr++, v );
            }
        }
        return true;

#ifdef DXGI_1_2_FORMATS
    case DXGI_FORMAT_B4G4R4A4_UNORM:
        if ( size >= sizeof(XMUNIBBLE4) )
        {
            static XMVECTORF32 s_Scale = { 15.f, 15.f, 15.f, 15.f };
            XMUNIBBLE4 * __restrict dPtr = reinterpret_cast<XMUNIBBLE4*>(pDestination);
            for( size_t icount = 0; icount < size; icount += sizeof(XMUNIBBLE4) )
            {
                if ( sPtr >= ePtr ) break;
                XMVECTOR v = XMVectorSwizzle<2, 1, 0, 3>( *sPtr++ );
                v = XMVectorMultiply( v, s_Scale );
                XMStoreUNibble4( dPtr++, v );
            }
        }
        return true;

    // We don't support the video formats ( see IsVideo function )
#endif // DXGI_1_2_FORMATS

    default:
        return false;
    }
}


//-------------------------------------------------------------------------------------
// Convert DXGI image to/from GUID_WICPixelFormat128bppRGBAFloat (no range conversions)
//-------------------------------------------------------------------------------------
HRESULT _ConvertToR32G32B32A32( const Image& srcImage, ScratchImage& image )
{
    if ( !srcImage.pixels )
        return E_POINTER;

    HRESULT hr = image.Initialize2D( DXGI_FORMAT_R32G32B32A32_FLOAT, srcImage.width, srcImage.height, 1, 1 );
    if ( FAILED(hr) )
        return hr;

    const Image *img = image.GetImage( 0, 0, 0 );
    if ( !img )
    {
        image.Release();
        return E_POINTER;
    }

    uint8_t* pDest = img->pixels;
    if ( !pDest )
    {
        image.Release();
        return E_POINTER;
    }

    const uint8_t *pSrc = srcImage.pixels;
    for( size_t h = 0; h < srcImage.height; ++h )
    {
        if ( !_LoadScanline( reinterpret_cast<XMVECTOR*>(pDest), srcImage.width, pSrc, srcImage.rowPitch, srcImage.format ) )
        {
            image.Release();
            return E_FAIL;
        }

        pSrc += srcImage.rowPitch;
        pDest += img->rowPitch;
    }

    return S_OK;
}

HRESULT _ConvertFromR32G32B32A32( _In_ const Image& srcImage, _In_ const Image& destImage )
{
    assert( srcImage.format == DXGI_FORMAT_R32G32B32A32_FLOAT );

    if ( !srcImage.pixels || !destImage.pixels )
        return E_POINTER;

    if ( srcImage.width != destImage.width || srcImage.height != destImage.height )
        return E_FAIL;

    const uint8_t *pSrc = srcImage.pixels;
    uint8_t* pDest = destImage.pixels;

    for( size_t h = 0; h < srcImage.height; ++h )
    {
        if ( !_StoreScanline( pDest, destImage.rowPitch, destImage.format, reinterpret_cast<const XMVECTOR*>(pSrc), srcImage.width ) )
            return E_FAIL;

        pSrc += srcImage.rowPitch;
        pDest += destImage.rowPitch;
    }

    return S_OK;
}

HRESULT _ConvertFromR32G32B32A32( const Image& srcImage, DXGI_FORMAT format, ScratchImage& image )
{
    if ( !srcImage.pixels )
        return E_POINTER;

    HRESULT hr = image.Initialize2D( format, srcImage.width, srcImage.height, 1, 1 );
    if ( FAILED(hr) )
        return hr;

    const Image *img = image.GetImage( 0, 0, 0 );
    if ( !img )
    {
        image.Release();
        return E_POINTER;
    }

    hr = _ConvertFromR32G32B32A32( srcImage, *img );
    if ( FAILED(hr) )
    {
        image.Release();
        return hr;
    }

    return S_OK;
}

HRESULT _ConvertFromR32G32B32A32( const Image* srcImages, size_t nimages, const TexMetadata& metadata, DXGI_FORMAT format, ScratchImage& result )
{
    if ( !srcImages )
        return E_POINTER;

    result.Release();

    assert( metadata.format == DXGI_FORMAT_R32G32B32A32_FLOAT );

    TexMetadata mdata2 = metadata;
    mdata2.format = format;
    HRESULT hr = result.Initialize( mdata2 );
    if ( FAILED(hr) )
        return hr;

    if ( nimages != result.GetImageCount() )
    {
        result.Release();
        return E_FAIL;
    }

    const Image* dest = result.GetImages();
    if ( !dest )
    {
        result.Release();
        return E_POINTER;
    }

    for( size_t index=0; index < nimages; ++index )
    {
        const Image& src = srcImages[ index ];
        const Image& dst = dest[ index ];

        assert( src.format == DXGI_FORMAT_R32G32B32A32_FLOAT );
        assert( dst.format == format );

        if ( src.width != dst.width || src.height != dst.height )
        {
            result.Release();
            return E_FAIL;
        }

        const uint8_t* pSrc = src.pixels;
        uint8_t* pDest = dst.pixels;
        if ( !pSrc || !pDest )
        {
            result.Release();
            return E_POINTER;
        }

        for( size_t h=0; h < src.height; ++h )
        {
            if ( !_StoreScanline( pDest, dst.rowPitch, format, reinterpret_cast<const XMVECTOR*>(pSrc), src.width ) )
            {
                result.Release();
                return E_FAIL;
            }

            pSrc += src.rowPitch;
            pDest += dst.rowPitch;
        }
    }

    return S_OK;
}


//-------------------------------------------------------------------------------------
// RGB -> sRGB
//-------------------------------------------------------------------------------------
static const uint32_t g_fEncodeGamma22[] =
{
    0x00000000, 0x3bd56bd3, 0x3c486344, 0x3c90da15, 0x3cbc2677, 0x3ce67704, 0x3d080183, 0x3d1c7728,
    0x3d30a8fb, 0x3d44a03c, 0x3d586400, 0x3d6bf9e7, 0x3d7f6679, 0x3d8956bd, 0x3d92e906, 0x3d9c6b70,
    0x3da5df22, 0x3daf451b, 0x3db89e3e, 0x3dc1eb50, 0x3dcb2d04, 0x3dd463f7, 0x3ddd90b9, 0x3de6b3ca,
    0x3defcda0, 0x3df8dea6, 0x3e00f3a0, 0x3e0573e3, 0x3e09f046, 0x3e0e68f0, 0x3e12de06, 0x3e174fa6,
    0x3e1bbdf2, 0x3e202906, 0x3e2490fd, 0x3e28f5f1, 0x3e2d57fb, 0x3e31b72f, 0x3e3613a4, 0x3e3a6d6e,
    0x3e3ec4a0, 0x3e43194d, 0x3e476b84, 0x3e4bbb57, 0x3e5008d7, 0x3e54540f, 0x3e589d0f, 0x3e5ce3e5,
    0x3e61289d, 0x3e656b44, 0x3e69abe5, 0x3e6dea8d, 0x3e722745, 0x3e766217, 0x3e7a9b0e, 0x3e7ed235,
    0x3e8183c9, 0x3e839d98, 0x3e85b68c, 0x3e87cea8, 0x3e89e5f2, 0x3e8bfc6b, 0x3e8e1219, 0x3e9026ff,
    0x3e923b20, 0x3e944e7f, 0x3e966120, 0x3e987307, 0x3e9a8436, 0x3e9c94af, 0x3e9ea476, 0x3ea0b38e,
    0x3ea2c1fb, 0x3ea4cfbb, 0x3ea6dcd5, 0x3ea8e94a, 0x3eaaf51c, 0x3ead004e, 0x3eaf0ae2, 0x3eb114d9,
    0x3eb31e37, 0x3eb526fe, 0x3eb72f2f, 0x3eb936cd, 0x3ebb3dd8, 0x3ebd4454, 0x3ebf4a43, 0x3ec14fa5,
    0x3ec3547e, 0x3ec558cd, 0x3ec75c95, 0x3ec95fd8, 0x3ecb6297, 0x3ecd64d4, 0x3ecf6690, 0x3ed167ce,
    0x3ed3688e, 0x3ed568d1, 0x3ed76899, 0x3ed967e9, 0x3edb66bf, 0x3edd651f, 0x3edf630a, 0x3ee16080,
    0x3ee35d84, 0x3ee55a16, 0x3ee75636, 0x3ee951e8, 0x3eeb4d2a, 0x3eed4800, 0x3eef4269, 0x3ef13c68,
    0x3ef335fc, 0x3ef52f26, 0x3ef727ea, 0x3ef92046, 0x3efb183c, 0x3efd0fcd, 0x3eff06fa, 0x3f007ee2,
    0x3f017a16, 0x3f027519, 0x3f036fec, 0x3f046a8f, 0x3f056502, 0x3f065f47, 0x3f07595d, 0x3f085344,
    0x3f094cfe, 0x3f0a468b, 0x3f0b3feb, 0x3f0c391e, 0x3f0d3224, 0x3f0e2aff, 0x3f0f23af, 0x3f101c32,
    0x3f11148c, 0x3f120cba, 0x3f1304bf, 0x3f13fc9a, 0x3f14f44b, 0x3f15ebd3, 0x3f16e333, 0x3f17da6b,
    0x3f18d17a, 0x3f19c860, 0x3f1abf1f, 0x3f1bb5b7, 0x3f1cac28, 0x3f1da272, 0x3f1e9895, 0x3f1f8e92,
    0x3f20846a, 0x3f217a1c, 0x3f226fa8, 0x3f23650f, 0x3f245a52, 0x3f254f70, 0x3f264469, 0x3f27393f,
    0x3f282df1, 0x3f29227f, 0x3f2a16ea, 0x3f2b0b31, 0x3f2bff56, 0x3f2cf358, 0x3f2de738, 0x3f2edaf6,
    0x3f2fce91, 0x3f30c20b, 0x3f31b564, 0x3f32a89b, 0x3f339bb1, 0x3f348ea6, 0x3f35817a, 0x3f36742f,
    0x3f3766c3, 0x3f385936, 0x3f394b8a, 0x3f3a3dbe, 0x3f3b2fd3, 0x3f3c21c8, 0x3f3d139e, 0x3f3e0556,
    0x3f3ef6ee, 0x3f3fe868, 0x3f40d9c4, 0x3f41cb01, 0x3f42bc20, 0x3f43ad22, 0x3f449e06, 0x3f458ecc,
    0x3f467f75, 0x3f477001, 0x3f486071, 0x3f4950c2, 0x3f4a40f8, 0x3f4b3111, 0x3f4c210d, 0x3f4d10ed,
    0x3f4e00b2, 0x3f4ef05a, 0x3f4fdfe7, 0x3f50cf58, 0x3f51beae, 0x3f52ade8, 0x3f539d07, 0x3f548c0c,
    0x3f557af5, 0x3f5669c4, 0x3f575878, 0x3f584711, 0x3f593590, 0x3f5a23f6, 0x3f5b1241, 0x3f5c0072,
    0x3f5cee89, 0x3f5ddc87, 0x3f5eca6b, 0x3f5fb835, 0x3f60a5e7, 0x3f619380, 0x3f6280ff, 0x3f636e65,
    0x3f645bb3, 0x3f6548e8, 0x3f663604, 0x3f672309, 0x3f680ff4, 0x3f68fcc8, 0x3f69e983, 0x3f6ad627,
    0x3f6bc2b3, 0x3f6caf27, 0x3f6d9b83, 0x3f6e87c8, 0x3f6f73f5, 0x3f70600c, 0x3f714c0b, 0x3f7237f4,
    0x3f7323c4, 0x3f740f7f, 0x3f74fb22, 0x3f75e6af, 0x3f76d225, 0x3f77bd85, 0x3f78a8ce, 0x3f799401,
    0x3f7a7f1e, 0x3f7b6a25, 0x3f7c5516, 0x3f7d3ff1, 0x3f7e2ab6, 0x3f7f1566, 0x3f800000, 0x3f800000
};

#pragma prefast(suppress : 25000, "FXMVECTOR is 16 bytes")
static inline XMVECTOR _TableEncodeGamma22( FXMVECTOR v )
{
    float f[4];
    XMStoreFloat4( (XMFLOAT4*)f, v );

    for( size_t i=0; i < 4; ++i )
    {
        float f2 = sqrtf(f[i]) * 254.0f;

        uint32_t  i2 = static_cast<uint32_t>(f2);
        i2 = std::min<uint32_t>( i2, _countof( g_fEncodeGamma22 )-2 );

        float fS = f2 - (float) i2;
        float fA = ((float *) g_fEncodeGamma22)[i2];
        float fB = ((float *) g_fEncodeGamma22)[i2 + 1];

        f[i] = fA + fS * (fB - fA);
    }

    return XMLoadFloat4( (XMFLOAT4*)f );
}


//-------------------------------------------------------------------------------------
// sRGB -> RGB
//-------------------------------------------------------------------------------------
static const uint32_t g_fDecodeGamma22[] =
{
    0x00000000, 0x3b144eb0, 0x3b9ef3b0, 0x3bf84b42, 0x3c2a5c46, 0x3c59c180, 0x3c850eb5, 0x3c9da52a,
    0x3cb6967a, 0x3ccfd852, 0x3ce9628b, 0x3d01974b, 0x3d0e9b82, 0x3d1bbba3, 0x3d28f5bc, 0x3d364822,
    0x3d43b159, 0x3d51301d, 0x3d5ec344, 0x3d6c69c9, 0x3d7a22c4, 0x3d83f6ad, 0x3d8ae465, 0x3d91da35,
    0x3d98d7c7, 0x3d9fdcd2, 0x3da6e914, 0x3dadfc47, 0x3db51635, 0x3dbc36a3, 0x3dc35d62, 0x3dca8a3a,
    0x3dd1bd02, 0x3dd8f591, 0x3de033bb, 0x3de7775d, 0x3deec050, 0x3df60e74, 0x3dfd61a6, 0x3e025ce5,
    0x3e060b61, 0x3e09bc38, 0x3e0d6f5f, 0x3e1124c8, 0x3e14dc68, 0x3e189630, 0x3e1c521a, 0x3e201016,
    0x3e23d01d, 0x3e279225, 0x3e2b5624, 0x3e2f1c10, 0x3e32e3e4, 0x3e36ad94, 0x3e3a7918, 0x3e3e4668,
    0x3e42157f, 0x3e45e654, 0x3e49b8e0, 0x3e4d8d1d, 0x3e516304, 0x3e553a8d, 0x3e5913b4, 0x3e5cee70,
    0x3e60cabf, 0x3e64a89b, 0x3e6887fb, 0x3e6c68db, 0x3e704b3a, 0x3e742f0e, 0x3e781454, 0x3e7bfb04,
    0x3e7fe321, 0x3e81e650, 0x3e83dbc0, 0x3e85d1dc, 0x3e87c8a3, 0x3e89c015, 0x3e8bb830, 0x3e8db0ee,
    0x3e8faa51, 0x3e91a454, 0x3e939ef9, 0x3e959a3b, 0x3e97961b, 0x3e999295, 0x3e9b8fa7, 0x3e9d8d52,
    0x3e9f8b93, 0x3ea18a6a, 0x3ea389d2, 0x3ea589cb, 0x3ea78a56, 0x3ea98b6e, 0x3eab8d15, 0x3ead8f47,
    0x3eaf9204, 0x3eb1954a, 0x3eb39917, 0x3eb59d6c, 0x3eb7a246, 0x3eb9a7a5, 0x3ebbad88, 0x3ebdb3ec,
    0x3ebfbad3, 0x3ec1c237, 0x3ec3ca1a, 0x3ec5d27c, 0x3ec7db58, 0x3ec9e4b4, 0x3ecbee85, 0x3ecdf8d3,
    0x3ed0039a, 0x3ed20ed8, 0x3ed41a8a, 0x3ed626b5, 0x3ed83351, 0x3eda4065, 0x3edc4de9, 0x3ede5be0,
    0x3ee06a4a, 0x3ee27923, 0x3ee4886a, 0x3ee69821, 0x3ee8a845, 0x3eeab8d8, 0x3eecc9d6, 0x3eeedb3f,
    0x3ef0ed13, 0x3ef2ff53, 0x3ef511fb, 0x3ef7250a, 0x3ef93883, 0x3efb4c61, 0x3efd60a7, 0x3eff7553,
    0x3f00c531, 0x3f01cfeb, 0x3f02dad9, 0x3f03e5f5, 0x3f04f145, 0x3f05fcc4, 0x3f070875, 0x3f081456,
    0x3f092067, 0x3f0a2ca8, 0x3f0b3917, 0x3f0c45b7, 0x3f0d5284, 0x3f0e5f7f, 0x3f0f6caa, 0x3f107a03,
    0x3f118789, 0x3f12953b, 0x3f13a31d, 0x3f14b12b, 0x3f15bf64, 0x3f16cdca, 0x3f17dc5e, 0x3f18eb1b,
    0x3f19fa05, 0x3f1b091b, 0x3f1c185c, 0x3f1d27c7, 0x3f1e375c, 0x3f1f471d, 0x3f205707, 0x3f21671b,
    0x3f227759, 0x3f2387c2, 0x3f249852, 0x3f25a90c, 0x3f26b9ef, 0x3f27cafb, 0x3f28dc30, 0x3f29ed8b,
    0x3f2aff11, 0x3f2c10bd, 0x3f2d2290, 0x3f2e348b, 0x3f2f46ad, 0x3f3058f7, 0x3f316b66, 0x3f327dfd,
    0x3f3390ba, 0x3f34a39d, 0x3f35b6a7, 0x3f36c9d6, 0x3f37dd2b, 0x3f38f0a5, 0x3f3a0443, 0x3f3b1808,
    0x3f3c2bf2, 0x3f3d4000, 0x3f3e5434, 0x3f3f688c, 0x3f407d07, 0x3f4191a8, 0x3f42a66c, 0x3f43bb54,
    0x3f44d05f, 0x3f45e58e, 0x3f46fadf, 0x3f481054, 0x3f4925ed, 0x3f4a3ba8, 0x3f4b5186, 0x3f4c6789,
    0x3f4d7daa, 0x3f4e93f0, 0x3f4faa57, 0x3f50c0e0, 0x3f51d78b, 0x3f52ee58, 0x3f540545, 0x3f551c55,
    0x3f563386, 0x3f574ad7, 0x3f58624b, 0x3f5979de, 0x3f5a9191, 0x3f5ba965, 0x3f5cc15b, 0x3f5dd971,
    0x3f5ef1a6, 0x3f6009fc, 0x3f612272, 0x3f623b08, 0x3f6353bc, 0x3f646c90, 0x3f658586, 0x3f669e98,
    0x3f67b7cb, 0x3f68d11b, 0x3f69ea8d, 0x3f6b041b, 0x3f6c1dc9, 0x3f6d3795, 0x3f6e5180, 0x3f6f6b8b,
    0x3f7085b2, 0x3f719ff7, 0x3f72ba5b, 0x3f73d4dc, 0x3f74ef7c, 0x3f760a38, 0x3f772512, 0x3f78400b,
    0x3f795b20, 0x3f7a7651, 0x3f7b91a2, 0x3f7cad0e, 0x3f7dc896, 0x3f7ee43c, 0x3f800000, 0x3f800000
};


#pragma prefast(suppress : 25000, "FXMVECTOR is 16 bytes")
static inline XMVECTOR _TableDecodeGamma22( FXMVECTOR v )
{
    float f[4];
    XMStoreFloat4( (XMFLOAT4*)f, v );

    for( size_t i=0; i < 4; ++i )
    {
        float f2 = f[i] * f[i] * 254.0f;
        uint32_t  i2 = static_cast<uint32_t>(f2);
        i2 = std::min<uint32_t>( i2, _countof(g_fDecodeGamma22)-2 );

        float fS = f2 - (float) i2;
        float fA = ((float *) g_fDecodeGamma22)[i2];
        float fB = ((float *) g_fDecodeGamma22)[i2 + 1];

        f[i] = fA + fS * (fB - fA);
    }

    return XMLoadFloat4( (XMFLOAT4*)f );
}


//-------------------------------------------------------------------------------------
// Convert scanline based on source/target formats
//-------------------------------------------------------------------------------------
struct ConvertData
{
    DXGI_FORMAT format;
    size_t datasize;
    DWORD flags;
};

static const ConvertData g_ConvertTable[] = {
    { DXGI_FORMAT_R32G32B32A32_FLOAT,           32, CONVF_FLOAT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R32G32B32A32_UINT,            32, CONVF_UINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R32G32B32A32_SINT,            32, CONVF_SINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R32G32B32_FLOAT,              32, CONVF_FLOAT | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_R32G32B32_UINT,               32, CONVF_UINT | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_R32G32B32_SINT,               32, CONVF_SINT | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_R16G16B16A16_FLOAT,           16, CONVF_FLOAT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R16G16B16A16_UNORM,           16, CONVF_UNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A }, 
    { DXGI_FORMAT_R16G16B16A16_UINT,            16, CONVF_UINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A }, 
    { DXGI_FORMAT_R16G16B16A16_SNORM,           16, CONVF_SNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A }, 
    { DXGI_FORMAT_R16G16B16A16_SINT,            16, CONVF_SINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A }, 
    { DXGI_FORMAT_R32G32_FLOAT,                 32, CONVF_FLOAT | CONVF_R | CONVF_G },
    { DXGI_FORMAT_R32G32_UINT,                  32, CONVF_UINT | CONVF_R | CONVF_G  },
    { DXGI_FORMAT_R32G32_SINT,                  32, CONVF_SINT | CONVF_R | CONVF_G  },
    { DXGI_FORMAT_D32_FLOAT_S8X24_UINT,         32, CONVF_FLOAT | CONVF_DEPTH | CONVF_STENCIL },
    { DXGI_FORMAT_R10G10B10A2_UNORM,            10, CONVF_UNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R10G10B10A2_UINT,             10, CONVF_UINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R11G11B10_FLOAT,              10, CONVF_FLOAT | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_R8G8B8A8_UNORM,               8, CONVF_UNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,          8, CONVF_UNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R8G8B8A8_UINT,                8, CONVF_UINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R8G8B8A8_SNORM,               8, CONVF_SNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R8G8B8A8_SINT,                8, CONVF_SINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_R16G16_FLOAT,                 16, CONVF_FLOAT | CONVF_R | CONVF_G },
    { DXGI_FORMAT_R16G16_UNORM,                 16, CONVF_UNORM | CONVF_R | CONVF_G },
    { DXGI_FORMAT_R16G16_UINT,                  16, CONVF_UINT | CONVF_R | CONVF_G },
    { DXGI_FORMAT_R16G16_SNORM,                 16, CONVF_SNORM | CONVF_R | CONVF_G },
    { DXGI_FORMAT_R16G16_SINT,                  16, CONVF_SINT | CONVF_R | CONVF_G },
    { DXGI_FORMAT_D32_FLOAT,                    32, CONVF_FLOAT | CONVF_DEPTH },
    { DXGI_FORMAT_R32_FLOAT,                    32, CONVF_FLOAT | CONVF_R },
    { DXGI_FORMAT_R32_UINT,                     32, CONVF_UINT | CONVF_R },
    { DXGI_FORMAT_R32_SINT,                     32, CONVF_SINT | CONVF_R },
    { DXGI_FORMAT_D24_UNORM_S8_UINT,            32, CONVF_UNORM | CONVF_DEPTH | CONVF_STENCIL },
    { DXGI_FORMAT_R8G8_UNORM,                   8, CONVF_UNORM | CONVF_R | CONVF_G },
    { DXGI_FORMAT_R8G8_UINT,                    8, CONVF_UINT | CONVF_R | CONVF_G },
    { DXGI_FORMAT_R8G8_SNORM,                   8, CONVF_SNORM | CONVF_R | CONVF_G },
    { DXGI_FORMAT_R8G8_SINT,                    8, CONVF_SINT | CONVF_R | CONVF_G },
    { DXGI_FORMAT_R16_FLOAT,                    16, CONVF_FLOAT | CONVF_R },
    { DXGI_FORMAT_D16_UNORM,                    16, CONVF_UNORM | CONVF_DEPTH },
    { DXGI_FORMAT_R16_UNORM,                    16, CONVF_UNORM | CONVF_R },
    { DXGI_FORMAT_R16_UINT,                     16, CONVF_UINT | CONVF_R },
    { DXGI_FORMAT_R16_SNORM,                    16, CONVF_SNORM | CONVF_R },
    { DXGI_FORMAT_R16_SINT,                     16, CONVF_SINT | CONVF_R },
    { DXGI_FORMAT_R8_UNORM,                     8, CONVF_UNORM | CONVF_R },
    { DXGI_FORMAT_R8_UINT,                      8, CONVF_UINT | CONVF_R },
    { DXGI_FORMAT_R8_SNORM,                     8, CONVF_SNORM | CONVF_R },
    { DXGI_FORMAT_R8_SINT,                      8, CONVF_SINT | CONVF_R },
    { DXGI_FORMAT_A8_UNORM,                     8, CONVF_UNORM | CONVF_A },
    { DXGI_FORMAT_R1_UNORM,                     1, CONVF_UNORM | CONVF_R },
    { DXGI_FORMAT_R9G9B9E5_SHAREDEXP,           9, CONVF_SHAREDEXP | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_R8G8_B8G8_UNORM,              8, CONVF_UNORM | CONVF_PACKED | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_G8R8_G8B8_UNORM,              8, CONVF_UNORM | CONVF_PACKED | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_BC1_UNORM,                    8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_BC1_UNORM_SRGB,               8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_BC2_UNORM,                    8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_BC2_UNORM_SRGB,               8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_BC3_UNORM,                    8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_BC3_UNORM_SRGB,               8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_BC4_UNORM,                    8, CONVF_UNORM | CONVF_BC | CONVF_R },
    { DXGI_FORMAT_BC4_SNORM,                    8, CONVF_SNORM | CONVF_BC | CONVF_R },
    { DXGI_FORMAT_BC5_UNORM,                    8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G },
    { DXGI_FORMAT_BC5_SNORM,                    8, CONVF_SNORM | CONVF_BC | CONVF_R | CONVF_G },
    { DXGI_FORMAT_B5G6R5_UNORM,                 5, CONVF_UNORM | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_B5G5R5A1_UNORM,               5, CONVF_UNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_B8G8R8A8_UNORM,               8, CONVF_UNORM | CONVF_BGR | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_B8G8R8X8_UNORM,               8, CONVF_UNORM | CONVF_BGR | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,   10, CONVF_UNORM | CONVF_X2 | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,          8, CONVF_UNORM | CONVF_BGR | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,          8, CONVF_UNORM | CONVF_BGR | CONVF_R | CONVF_G | CONVF_B },
    { DXGI_FORMAT_BC6H_UF16,                    16, CONVF_FLOAT | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_BC6H_SF16,                    16, CONVF_FLOAT | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_BC7_UNORM,                    8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    { DXGI_FORMAT_BC7_UNORM_SRGB,               8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
#ifdef DXGI_1_2_FORMATS
    { DXGI_FORMAT_B4G4R4A4_UNORM,               4, CONVF_UNORM | CONVF_BGR | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
#endif
};

#pragma prefast( suppress : 25004, "Signature must match bsearch_s" );
static int __cdecl _ConvertCompare( void *context, const void* ptr1, const void *ptr2 )
{
    UNREFERENCED_PARAMETER(context);
    const ConvertData *p1 = reinterpret_cast<const ConvertData*>(ptr1);
    const ConvertData *p2 = reinterpret_cast<const ConvertData*>(ptr2);
    if ( p1->format == p2->format ) return 0;
    else return (p1->format < p2->format ) ? -1 : 1;
}

DWORD _GetConvertFlags( DXGI_FORMAT format )
{
#ifdef _DEBUG
    // Ensure conversion table is in ascending order
    assert( _countof(g_ConvertTable) > 0 );
    DXGI_FORMAT lastvalue = g_ConvertTable[0].format;
    for( size_t index=1; index < _countof(g_ConvertTable); ++index )
    {
        assert( g_ConvertTable[index].format > lastvalue );
        lastvalue = g_ConvertTable[index].format;
    }
#endif

    ConvertData key = { format, 0 };
    const ConvertData* in = (const ConvertData*) bsearch_s( &key, g_ConvertTable, _countof(g_ConvertTable), sizeof(ConvertData),
                                                            _ConvertCompare, 0 );
    return (in) ? in->flags : 0;
}

void _ConvertScanline( XMVECTOR* pBuffer, size_t count, DXGI_FORMAT outFormat, DXGI_FORMAT inFormat, DWORD flags )
{
    assert( pBuffer && count > 0 && (((uintptr_t)pBuffer & 0xF) == 0) );
    assert( IsValid(outFormat) && !IsVideo(outFormat) && !IsTypeless(outFormat) );
    assert( IsValid(inFormat) && !IsVideo(inFormat) && !IsTypeless(inFormat) );

    if ( !pBuffer )
        return;

#ifdef _DEBUG
    // Ensure conversion table is in ascending order
    assert( _countof(g_ConvertTable) > 0 );
    DXGI_FORMAT lastvalue = g_ConvertTable[0].format;
    for( size_t index=1; index < _countof(g_ConvertTable); ++index )
    {
        assert( g_ConvertTable[index].format > lastvalue );
        lastvalue = g_ConvertTable[index].format;
    }
#endif

    // Determine conversion details about source and dest formats
    ConvertData key = { inFormat, 0 };
    const ConvertData* in = (const ConvertData*) bsearch_s( &key, g_ConvertTable, _countof(g_ConvertTable), sizeof(ConvertData),
                                                            _ConvertCompare, 0 );
    key.format = outFormat;
    const ConvertData* out = (const ConvertData*) bsearch_s( &key, g_ConvertTable, _countof(g_ConvertTable), sizeof(ConvertData),
                                                            _ConvertCompare, 0 );
    if ( !in || !out )
    {
        assert(false);
        return;
    }

    assert( _GetConvertFlags( inFormat ) == in->flags );
    assert( _GetConvertFlags( outFormat ) == out->flags );

    // Handle SRGB filtering modes
    if ( IsSRGB( inFormat ) )
        flags |= TEX_FILTER_SRGB_IN;

    if ( IsSRGB( outFormat ) )
        flags |= TEX_FILTER_SRGB_OUT;

    if ( in->flags & CONVF_SNORM )
        flags &= ~TEX_FILTER_SRGB_IN;

    if ( out->flags & CONVF_SNORM )
        flags &= ~TEX_FILTER_SRGB_OUT;

    if ( (flags & (TEX_FILTER_SRGB_IN|TEX_FILTER_SRGB_OUT)) == (TEX_FILTER_SRGB_IN|TEX_FILTER_SRGB_OUT) )
    {
        flags &= ~(TEX_FILTER_SRGB_IN|TEX_FILTER_SRGB_OUT);
    }

    // sRGB input processing (sRGB -> RGB)
    if ( flags & TEX_FILTER_SRGB_IN )
    {
        if ( (in->flags & CONVF_FLOAT) || (in->flags & CONVF_UNORM) )
        {
            XMVECTOR* ptr = pBuffer;
            for( size_t i=0; i < count; ++i )
            {
                // rgb = rgb^(2.2); a=a
                XMVECTOR v = *ptr;
                XMVECTOR v1 = _TableDecodeGamma22( v );
                *ptr++ = XMVectorSelect( v, v1, g_XMSelect1110 );
            }
        }
    }

    // Handle conversion special cases
    DWORD diffFlags = in->flags ^ out->flags;
    if ( diffFlags != 0)
    {
        if ( out->flags & CONVF_UNORM )
        {
            if ( in->flags & CONVF_SNORM )
            {
                // SNORM -> UNORM
                XMVECTOR* ptr = pBuffer;
                for( size_t i=0; i < count; ++i )
                {
                    XMVECTOR v = *ptr;
                    *ptr++ = XMVectorMultiplyAdd( v, g_XMOneHalf, g_XMOneHalf );
                }
            }
            else if ( in->flags & CONVF_FLOAT )
            {
                // FLOAT -> UNORM
                XMVECTOR* ptr = pBuffer;
                for( size_t i=0; i < count; ++i )
                {
                    XMVECTOR v = *ptr;
                    *ptr++ = XMVectorSaturate( v );
                }
            }
        }
        else if ( out->flags & CONVF_SNORM )
        {
            if ( in->flags & CONVF_UNORM )
            {
                // UNORM -> SNORM
                static XMVECTORF32 two = { 2.0f, 2.0f, 2.0f, 2.0f };
                XMVECTOR* ptr = pBuffer;
                for( size_t i=0; i < count; ++i )
                {
                    XMVECTOR v = *ptr;
                    *ptr++ = XMVectorMultiplyAdd( v, two, g_XMNegativeOne );
                }
            }
            else if ( in->flags & CONVF_FLOAT )
            {
                // FLOAT -> SNORM
                XMVECTOR* ptr = pBuffer;
                for( size_t i=0; i < count; ++i )
                {
                    XMVECTOR v = *ptr;
                    *ptr++ = XMVectorClamp( v, g_XMNegativeOne, g_XMOne );
                }
            }
        }

        // !CONVF_A -> CONVF_A is handled because LoadScanline ensures alpha defaults to 1.0 for no-alpha formats

        // CONVF_PACKED cases are handled because LoadScanline/StoreScanline handles packing/unpacking

        if ( ((out->flags & CONVF_RGBA_MASK) == CONVF_A) && !(in->flags & CONVF_A) )
        {
            // !CONVF_A -> A format
            XMVECTOR* ptr = pBuffer;
            for( size_t i=0; i < count; ++i )
            {
                XMVECTOR v = *ptr;
                *ptr++ = XMVectorSplatX( v );
            }
        }
        else if ( ((in->flags & CONVF_RGBA_MASK) == CONVF_A) && !(out->flags & CONVF_A) )
        {
            // A format -> !CONVF_A
            XMVECTOR* ptr = pBuffer;
            for( size_t i=0; i < count; ++i )
            {
                XMVECTOR v = *ptr;
                *ptr++ = XMVectorSplatW( v );
            }
        }
        else if ( ((in->flags & CONVF_RGB_MASK) == CONVF_R) && ((out->flags & CONVF_RGB_MASK) == (CONVF_R|CONVF_G|CONVF_B)) )
        {
            // R format -> RGB format
            XMVECTOR* ptr = pBuffer;
            for( size_t i=0; i < count; ++i )
            {
                XMVECTOR v = *ptr;
                XMVECTOR v1 = XMVectorSplatX( v );
                *ptr++ = XMVectorSelect( v, v1, g_XMSelect1110 );
            }
        }
    }

    // sRGB output processing (RGB -> sRGB)
    if ( flags & TEX_FILTER_SRGB_OUT )
    {
        if ( (out->flags & CONVF_FLOAT) || (out->flags & CONVF_UNORM) )
        {
            XMVECTOR* ptr = pBuffer;
            for( size_t i=0; i < count; ++i )
            {
                // rgb = rgb^(1/2.2); a=a
                XMVECTOR v = *ptr;
                XMVECTOR v1 = _TableEncodeGamma22( v );
                *ptr++ = XMVectorSelect( v, v1, g_XMSelect1110 );
            }
        }
    }
}


//-------------------------------------------------------------------------------------
// Convert the source image using WIC
//-------------------------------------------------------------------------------------
static HRESULT _ConvertUsingWIC( _In_ const Image& srcImage, _In_ const WICPixelFormatGUID& pfGUID,
                                 _In_ const WICPixelFormatGUID& targetGUID,
                                 _In_ DWORD filter, _In_ float threshold,  _In_ const Image& destImage )
{
    assert( srcImage.width == destImage.width );
    assert( srcImage.height == destImage.height );

    IWICImagingFactory* pWIC = _GetWIC();
    if ( !pWIC )
        return E_NOINTERFACE;

    ScopedObject<IWICFormatConverter> FC;
    HRESULT hr = pWIC->CreateFormatConverter( &FC );
    if ( FAILED(hr) )
        return hr;

    // Need to implement usage of TEX_FILTER_SRGB_IN/TEX_FILTER_SRGB_OUT

    BOOL canConvert = FALSE;
    hr = FC->CanConvert( pfGUID, targetGUID, &canConvert );
    if ( FAILED(hr) || !canConvert )
    {
        // This case is not an issue for the subset of WIC formats that map directly to DXGI
        return E_UNEXPECTED;
    }

    ScopedObject<IWICBitmap> source;
    hr = pWIC->CreateBitmapFromMemory( static_cast<UINT>( srcImage.width ), static_cast<UINT>( srcImage.height ), pfGUID,
                                       static_cast<UINT>( srcImage.rowPitch ), static_cast<UINT>( srcImage.slicePitch ),
                                       srcImage.pixels, &source );
    if ( FAILED(hr) )
        return hr;

    hr = FC->Initialize( source.Get(), targetGUID, _GetWICDither( filter ), 0, threshold, WICBitmapPaletteTypeCustom );
    if ( FAILED(hr) )
        return hr;

    hr = FC->CopyPixels( 0, static_cast<UINT>( destImage.rowPitch ), static_cast<UINT>( destImage.slicePitch ), destImage.pixels );  
    if ( FAILED(hr) )
        return hr;

    return S_OK;
}


//-------------------------------------------------------------------------------------
// Convert the source using WIC and then convert to DXGI format from there
//-------------------------------------------------------------------------------------
static HRESULT _ConvertFromWIC( _In_ const Image& srcImage, _In_ const WICPixelFormatGUID& pfGUID,
                                _In_ DWORD filter, _In_ float threshold,  _In_ const Image& destImage )
{
    assert( srcImage.width == destImage.width );
    assert( srcImage.height == destImage.height );

    IWICImagingFactory* pWIC = _GetWIC();
    if ( !pWIC )
        return E_NOINTERFACE;

    ScopedObject<IWICFormatConverter> FC;
    HRESULT hr = pWIC->CreateFormatConverter( &FC );
    if ( FAILED(hr) )
        return hr;

    BOOL canConvert = FALSE;
    hr = FC->CanConvert( pfGUID, GUID_WICPixelFormat128bppRGBAFloat, &canConvert );
    if ( FAILED(hr) || !canConvert )
    {
        // This case is not an issue for the subset of WIC formats that map directly to DXGI
        return E_UNEXPECTED;
    }

    ScratchImage temp;
    hr = temp.Initialize2D( DXGI_FORMAT_R32G32B32A32_FLOAT, srcImage.width, srcImage.height, 1, 1 );
    if ( FAILED(hr) )
        return hr;

    const Image *timg = temp.GetImage( 0, 0, 0 );
    if ( !timg )
        return E_POINTER;

    ScopedObject<IWICBitmap> source;
    hr = pWIC->CreateBitmapFromMemory( static_cast<UINT>( srcImage.width ), static_cast<UINT>( srcImage.height ), pfGUID,
                                       static_cast<UINT>( srcImage.rowPitch ), static_cast<UINT>( srcImage.slicePitch ),
                                       srcImage.pixels, &source );
    if ( FAILED(hr) )
        return hr;

    hr = FC->Initialize( source.Get(), GUID_WICPixelFormat128bppRGBAFloat, _GetWICDither( filter ), 0, threshold, WICBitmapPaletteTypeCustom );
    if ( FAILED(hr) )
        return hr;

    hr = FC->CopyPixels( 0, static_cast<UINT>( timg->rowPitch ), static_cast<UINT>( timg->slicePitch ), timg->pixels );  
    if ( FAILED(hr) )
        return hr;

    // Perform conversion on temp image which is now in R32G32B32A32_FLOAT format to final image
    uint8_t *pSrc = timg->pixels;
    uint8_t *pDest = destImage.pixels;
    if ( !pSrc || !pDest )
        return E_POINTER;

    for( size_t h = 0; h < srcImage.height; ++h )
    {
        _ConvertScanline( reinterpret_cast<XMVECTOR*>(pSrc), srcImage.width, destImage.format, DXGI_FORMAT_R32G32B32A32_FLOAT, filter );

        if ( !_StoreScanline( pDest, destImage.rowPitch, destImage.format, reinterpret_cast<const XMVECTOR*>(pSrc), srcImage.width ) )
            return E_FAIL;

        pSrc += timg->rowPitch;
        pDest += destImage.rowPitch;
    }

    return S_OK;
}


//-------------------------------------------------------------------------------------
// Convert the source from DXGI format then use WIC to convert to final format
//-------------------------------------------------------------------------------------
static HRESULT _ConvertToWIC( _In_ const Image& srcImage, 
                              _In_ const WICPixelFormatGUID& targetGUID, _In_ DWORD filter, _In_ float threshold,  _In_ const Image& destImage )
{
    assert( srcImage.width == destImage.width );
    assert( srcImage.height == destImage.height );

    IWICImagingFactory* pWIC = _GetWIC();
    if ( !pWIC )
        return E_NOINTERFACE;

    ScopedObject<IWICFormatConverter> FC;
    HRESULT hr = pWIC->CreateFormatConverter( &FC );
    if ( FAILED(hr) )
        return hr;

    BOOL canConvert = FALSE;
    hr = FC->CanConvert( GUID_WICPixelFormat128bppRGBAFloat, targetGUID, &canConvert );
    if ( FAILED(hr) || !canConvert )
    {
        // This case is not an issue for the subset of WIC formats that map directly to DXGI
        return E_UNEXPECTED;
    }

    ScratchImage temp;
    hr = temp.Initialize2D( DXGI_FORMAT_R32G32B32A32_FLOAT, srcImage.width, srcImage.height, 1, 1 );
    if ( FAILED(hr) )
        return hr;

    const Image *timg = temp.GetImage( 0, 0, 0 );
    if ( !timg )
        return E_POINTER;

    const uint8_t *pSrc = srcImage.pixels;
    if ( !pSrc )
        return E_POINTER;

    uint8_t *pDest = timg->pixels;
    if ( !pDest )
        return E_POINTER;

    for( size_t h = 0; h < srcImage.height; ++h )
    {
        if ( !_LoadScanline( reinterpret_cast<XMVECTOR*>(pDest), srcImage.width, pSrc, srcImage.rowPitch, srcImage.format ) )
            return E_FAIL;

        _ConvertScanline( reinterpret_cast<XMVECTOR*>(pDest), srcImage.width, DXGI_FORMAT_R32G32B32A32_FLOAT, srcImage.format, filter );

        pSrc += srcImage.rowPitch;
        pDest += timg->rowPitch;
    }

    // Perform conversion on temp image which is now in R32G32B32A32_FLOAT format
    ScopedObject<IWICBitmap> source;
    hr = pWIC->CreateBitmapFromMemory( static_cast<UINT>( timg->width ), static_cast<UINT>( timg->height ), GUID_WICPixelFormat128bppRGBAFloat,
                                       static_cast<UINT>( timg->rowPitch ), static_cast<UINT>( timg->slicePitch ),
                                       timg->pixels, &source );
    if ( FAILED(hr) )
        return hr;

    hr = FC->Initialize( source.Get(), targetGUID, _GetWICDither( filter ), 0, threshold, WICBitmapPaletteTypeCustom );
    if ( FAILED(hr) )
        return hr;

    hr = FC->CopyPixels( 0, static_cast<UINT>( destImage.rowPitch ), static_cast<UINT>( destImage.slicePitch ), destImage.pixels );  
    if ( FAILED(hr) )
        return hr;

    return S_OK;
}


//-------------------------------------------------------------------------------------
// Convert the source image (not using WIC)
//-------------------------------------------------------------------------------------
static HRESULT _Convert( _In_ const Image& srcImage, _In_ DWORD filter, _In_ const Image& destImage )
{
    assert( srcImage.width == destImage.width );
    assert( srcImage.height == destImage.height );

    ScopedAlignedArrayXMVECTOR scanline( reinterpret_cast<XMVECTOR*>( _aligned_malloc( (sizeof(XMVECTOR)*srcImage.width), 16 ) ) );
    if ( !scanline )
        return E_OUTOFMEMORY;

    const uint8_t *pSrc = srcImage.pixels;
    uint8_t *pDest = destImage.pixels;
    if ( !pSrc || !pDest )
        return E_POINTER;

    for( size_t h = 0; h < srcImage.height; ++h )
    {
        if ( !_LoadScanline( scanline.get(), srcImage.width, pSrc, srcImage.rowPitch, srcImage.format ) )
            return E_FAIL;

        _ConvertScanline( scanline.get(), srcImage.width, destImage.format, srcImage.format, filter );

        if ( !_StoreScanline( pDest, destImage.rowPitch, destImage.format, scanline.get(), srcImage.width ) )
            return E_FAIL;

        pSrc += srcImage.rowPitch;
        pDest += destImage.rowPitch;
    }

    return S_OK;
}


//=====================================================================================
// Entry-points
//=====================================================================================

//-------------------------------------------------------------------------------------
// Convert image
//-------------------------------------------------------------------------------------
HRESULT Convert( const Image& srcImage, DXGI_FORMAT format, DWORD filter, float threshold, ScratchImage& image )
{
    if ( (srcImage.format == format) || !IsValid( format ) )
        return E_INVALIDARG;

    if ( !srcImage.pixels )
        return E_POINTER;

    if ( IsCompressed(srcImage.format) || IsCompressed(format)
         || IsVideo(srcImage.format) || IsVideo(format) 
         || IsTypeless(srcImage.format) || IsTypeless(format) )
        return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );

#ifdef _AMD64_
    if ( (srcImage.width > 0xFFFFFFFF) || (srcImage.height > 0xFFFFFFFF) )
        return E_INVALIDARG;
#endif

    HRESULT hr = image.Initialize2D( format, srcImage.width, srcImage.height, 1, 1 );
    if ( FAILED(hr) )
        return hr;
   
    const Image *rimage = image.GetImage( 0, 0, 0 );
    if ( !rimage )
    {
        image.Release();
        return E_POINTER;
    }

    WICPixelFormatGUID pfGUID;
    if ( _DXGIToWIC( srcImage.format, pfGUID ) )
    {
        WICPixelFormatGUID targetGUID;
        if ( _DXGIToWIC( format, targetGUID ) )
        {
            // Case 1: Both source and target formats are WIC supported
            hr = _ConvertUsingWIC( srcImage, pfGUID, targetGUID, filter, threshold, *rimage );
        }
        else
        {
            // Case 2: Source format is supported by WIC, but not the target format
            hr = _ConvertFromWIC( srcImage, pfGUID, filter, threshold, *rimage );
        }
    }
    else
    {
        WICPixelFormatGUID targetGUID;
        if ( _DXGIToWIC( format, targetGUID ) )
        {
            // Case 3: Source format is not supported by WIC, but does support the target format
            hr = _ConvertToWIC( srcImage, targetGUID, filter, threshold, *rimage );
        }
        else
        {
            // Case 4: Both source and target format are not supported by WIC
            hr = _Convert( srcImage, filter, *rimage );
        }
    }

    if ( FAILED(hr) )
    {
        image.Release();
        return hr;
    }

    return S_OK;
}


//-------------------------------------------------------------------------------------
// Convert image (complex)
//-------------------------------------------------------------------------------------
HRESULT Convert( const Image* srcImages, size_t nimages, const TexMetadata& metadata,
                 DXGI_FORMAT format, DWORD filter, float threshold, ScratchImage& result )
{
    if ( !srcImages || !nimages || (metadata.format == format) || !IsValid(format) )
        return E_INVALIDARG;

    if ( IsCompressed(metadata.format) || IsCompressed(format)
         || IsVideo(metadata.format) || IsVideo(format) 
         || IsTypeless(metadata.format) || IsTypeless(format) )
        return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );

#ifdef _AMD64_
    if ( (metadata.width > 0xFFFFFFFF) || (metadata.height > 0xFFFFFFFF) )
        return E_INVALIDARG;
#endif

    TexMetadata mdata2 = metadata;
    mdata2.format = format;
    HRESULT hr = result.Initialize( mdata2 );
    if ( FAILED(hr) )
        return hr;

    if ( nimages != result.GetImageCount() )
    {
        result.Release();
        return E_FAIL;
    }

    const Image* dest = result.GetImages();
    if ( !dest )
    {
        result.Release();
        return E_POINTER;
    }

    WICPixelFormatGUID pfGUID, targetGUID;
    bool wicpf = _DXGIToWIC( metadata.format, pfGUID );
    bool wictargetpf = _DXGIToWIC( format, targetGUID );

    for( size_t index=0; index < nimages; ++index )
    {
        const Image& src = srcImages[ index ];
        if ( src.format != metadata.format )
        {
            result.Release();
            return E_FAIL;
        }

#ifdef _AMD64_
        if ( (src.width > 0xFFFFFFFF) || (src.height > 0xFFFFFFFF) )
            return E_FAIL;
#endif

        const Image& dst = dest[ index ];
        assert( dst.format == format );

        if ( src.width != dst.width || src.height != dst.height )
        {
            result.Release();
            return E_FAIL;
        }

        if ( wicpf )
        {
            if ( wictargetpf )
            {
                // Case 1: Both source and target formats are WIC supported
                hr = _ConvertUsingWIC( src, pfGUID, targetGUID, filter, threshold, dst );
            }
            else
            {
                // Case 2: Source format is supported by WIC, but not the target format
                hr = _ConvertFromWIC( src, pfGUID, filter, threshold, dst );
            }
        }
        else
        {
            if ( wictargetpf )
            {
                // Case 3: Source format is not supported by WIC, but does support the target format
                hr = _ConvertToWIC( src, targetGUID, filter, threshold, dst );
            }
            else
            {
                // Case 4: Both source and target format are not supported by WIC
                hr = _Convert( src, filter, dst );
            }
        }

        if ( FAILED(hr) )
        {
            result.Release();
            return hr;
        }
    }

    return S_OK;
}

}; // namespace
