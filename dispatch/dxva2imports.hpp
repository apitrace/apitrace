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


#pragma once

#include "d3d9imports.hpp"

#include <dxva2api.h>


#ifndef DXVA2_E_NOT_INITIALIZED
#define DXVA2_E_NOT_INITIALIZED ((HRESULT)0x80041000L)
#endif

#ifndef DXVA2_E_NEW_VIDEO_DEVICE
#define DXVA2_E_NEW_VIDEO_DEVICE ((HRESULT)0x80041001L)
#endif

#ifndef DXVA2_E_VIDEO_DEVICE_LOCKED
#define DXVA2_E_VIDEO_DEVICE_LOCKED ((HRESULT)0x80041002L)
#endif

#ifndef DXVA2_E_NOT_AVAILABLE
#define DXVA2_E_NOT_AVAILABLE ((HRESULT)0x80041003L)
#endif

