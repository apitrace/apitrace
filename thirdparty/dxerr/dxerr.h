//--------------------------------------------------------------------------------------
// File: DXErr.h
//
// DirectX Error Library
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

// This version only supports UNICODE.

#pragma once

#if !defined(NOMINMAX)
#define NOMINMAX
#endif

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------------------------
// DXGetErrorString
//--------------------------------------------------------------------------------------
const char* WINAPI DXGetErrorStringA( HRESULT hr );

#define DXGetErrorString DXGetErrorStringA

//--------------------------------------------------------------------------------------
// DXGetErrorDescription has to be modified to return a copy in a buffer rather than
// the original static string.
//--------------------------------------------------------------------------------------
void WINAPI DXGetErrorDescriptionA( HRESULT hr, char* desc, size_t count );

#define DXGetErrorDescription DXGetErrorDescriptionA

#ifdef __cplusplus
}
#endif //__cplusplus
