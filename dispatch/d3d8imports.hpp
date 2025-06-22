/**************************************************************************
 *
 * Copyright 2012 VMware, Inc.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/

/*
 * Central place for all D3D8 includes, and respective OS dependent headers.
 */

#pragma once

#include <windows.h>

#include "winsdk_compat.h"

#include <d3d8.h>


#ifndef D3DFMT_NULL
#define D3DFMT_NULL ((D3DFORMAT)MAKEFOURCC('N','U','L','L'))
#endif

#ifndef D3DFMT_AL16
#define D3DFMT_AL16 ((D3DFORMAT)MAKEFOURCC('A','L','1','6'))
#endif

#ifndef D3DFMT_R16
#define D3DFMT_R16 ((D3DFORMAT)MAKEFOURCC(' ','R','1','6'))
#endif

#ifndef D3DSI_INSTLENGTH_MASK
#define D3DSI_INSTLENGTH_MASK 0x0F000000
#endif

#ifndef D3DSI_INSTLENGTH_SHIFT
#define D3DSI_INSTLENGTH_SHIFT 24
#endif
