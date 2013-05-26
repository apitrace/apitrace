/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
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

#ifndef _TRACE_API_HPP_
#define _TRACE_API_HPP_


#include <stdlib.h>

#include "os_string.hpp"


namespace trace {


/**
 * Enum to distuinguish the API for tools.
 *
 * It should never be embedded in the trace file.
 */
enum API {
    API_UNKNOWN = 0,
    API_GL, // GL + GLX/WGL/CGL
    API_EGL, // GL/GLES1/GLES2/VG + EGL
    API_DX, // All DirectX
    API_D3D7,
    API_D3D8,
    API_D3D9,
    API_DXGI, // D3D10.x, D3D11.x
};


} /* namespace trace */

#endif /* _TRACE_API_HPP_ */
