/**************************************************************************
 *
 * Copyright 2015 VMware, Inc.
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


#include "d3d10imports.hpp"
#include "d3d11imports.hpp"
#include "json.hpp"


// TODO: This would be easier with C++11 `auto` keyword
template< typename Interface, typename DescStruct >
static inline void
dumpStateObjectDesc(JSONWriter &json, Interface *pObj)
{
    if (pObj) {
        DescStruct Desc;
        pObj->GetDesc(&Desc);
        dumpStateObject(json, Desc);
    } else {
        json.writeNull();
    }
}


void dumpStateObject(JSONWriter &, const D3D10_BLEND_DESC &);
void dumpStateObject(JSONWriter &, const D3D11_BLEND_DESC &);
