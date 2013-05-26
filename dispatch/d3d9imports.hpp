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

#include "compat.h"

#include <d3d9.h>


#ifndef D3DFMT_A2B10G10R10_XR_BIAS
#define D3DFMT_A2B10G10R10_XR_BIAS 119
#endif

#ifndef D3DFMT_ATI1N
#define D3DFMT_ATI1N ((D3DFORMAT)MAKEFOURCC('A','T','I','1'))
#endif

#ifndef D3DFMT_ATI2N
#define D3DFMT_ATI2N ((D3DFORMAT)MAKEFOURCC('A','T','I','2'))
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

#ifndef D3DFMT_YV12
#define D3DFMT_YV12 ((D3DFORMAT)MAKEFOURCC('Y','V','1','2'))
#endif

#ifndef D3DFMT_RAWZ
#define D3DFMT_RAWZ ((D3DFORMAT)MAKEFOURCC('R','A','W','Z'))
#endif


#ifndef D3DUSAGE_TEXTAPI
#define D3DUSAGE_TEXTAPI 0x10000000L
#endif

#ifndef D3DUSAGE_RESTRICTED_CONTENT
#define D3DUSAGE_RESTRICTED_CONTENT 0x00000800L
#endif

#ifndef D3DUSAGE_RESTRICT_SHARED_RESOURCE
#define D3DUSAGE_RESTRICT_SHARED_RESOURCE 0x00002000L
#endif

#ifndef D3DUSAGE_RESTRICT_SHARED_RESOURCE_DRIVER
#define D3DUSAGE_RESTRICT_SHARED_RESOURCE_DRIVER 0x00001000L
#endif


#endif /* _D3D9IMPORTS_HPP_ */
