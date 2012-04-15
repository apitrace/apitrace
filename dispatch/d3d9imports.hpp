/**************************************************************************
 *
 * Copyright 2011 VMware, Inc.
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
 * Central place for all D3D9 includes, and respective OS dependent headers.
 */

#ifndef _D3D9IMPORTS_HPP_
#define _D3D9IMPORTS_HPP_

#include <windows.h>
#include <tchar.h>

#include "compat.h"

#include <d3d9.h>
#include <d3dx9.h>


#ifndef D3DFMT_A2B10G10R10_XR_BIAS
#define D3DFMT_A2B10G10R10_XR_BIAS 119
#endif

#ifndef D3DFMT_ATI1
#define D3DFMT_ATI1 ((D3DFORMAT)MAKEFOURCC('A','T','I','1'))
#endif

#ifndef D3DFMT_ATI2
#define D3DFMT_ATI2 ((D3DFORMAT)MAKEFOURCC('A','T','I','2'))
#endif

#ifndef D3DFMT_AYUV
#define D3DFMT_AYUV ((D3DFORMAT)MAKEFOURCC('A','Y','U','V'))
#endif

#ifndef D3DFMT_DF16
#define D3DFMT_DF16 ((D3DFORMAT)MAKEFOURCC('D','F','1','6'))
#endif

#ifndef D3DFMT_DF24
#define D3DFMT_DF24 ((D3DFORMAT)MAKEFOURCC('D','F','2','4'))
#endif

#ifndef D3DFMT_INTZ
#define D3DFMT_INTZ ((D3DFORMAT)MAKEFOURCC('I','N','T','Z'))
#endif

#ifndef D3DFMT_NULL
#define D3DFMT_NULL ((D3DFORMAT)MAKEFOURCC('N','U','L','L'))
#endif

#ifndef D3DFMT_NV12
#define D3DFMT_NV12 ((D3DFORMAT)MAKEFOURCC('N','V','1','2'))
#endif

#ifndef D3DFMT_RAWZ
#define D3DFMT_RAWZ ((D3DFORMAT)MAKEFOURCC('R','A','W','Z'))
#endif


#endif /* _D3D9IMPORTS_HPP_ */
