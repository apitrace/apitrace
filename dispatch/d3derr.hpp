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
 * D3DERR return codes.
 *
 * These were defined in d3dx10.h/d3dx11.h on DirectX SDK, and moved to
 * winerror.h on Windows 8 SDK.
 *
 * See also:
 * - http://msdn.microsoft.com/en-us/library/windows/desktop/ff476174.aspx
 * - http://msdn.microsoft.com/en-us/library/windows/desktop/bb205278.aspx
 * - http://msdn.microsoft.com/en-us/library/windows/desktop/bb172554.aspx
 */

#ifndef _D3DERR_HPP_
#define _D3DERR_HPP_


#ifndef _FACD3D
#define _FACD3D  0x876
#endif

#ifndef MAKE_D3DHRESULT
#define MAKE_D3DHRESULT(code) MAKE_HRESULT(1, _FACD3D, code)
#endif

#ifndef MAKE_D3DSTATUS
#define MAKE_D3DSTATUS(code) MAKE_HRESULT(0, _FACD3D, code)
#endif

#ifndef D3DERR_INVALIDCALL
#define D3DERR_INVALIDCALL MAKE_D3DHRESULT(2156)
#endif

#ifndef D3DERR_WASSTILLDRAWING
#define D3DERR_WASSTILLDRAWING MAKE_D3DHRESULT(540)
#endif


#endif /* _D3DERR_HPP_ */
