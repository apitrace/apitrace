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
#include "state_writer.hpp"
#include "com_ptr.hpp"


// Template to extract the type of ID3D*::GetDesc methods
template< typename T >
struct ExtractDescType
{
};

template <typename InterfaceType, typename DescType>
struct ExtractDescType< void (__stdcall InterfaceType::*)(DescType *) >
{
    typedef DescType type;
};


template< typename Interface >
static inline void
dumpStateObjectDesc(StateWriter &writer, Interface *pObj)
{
    if (pObj) {
        typename ExtractDescType< decltype( &Interface::GetDesc ) >::type Desc;
        pObj->GetDesc(&Desc);
        dumpStateObject(writer, Desc);
    } else {
        writer.writeNull();
    }
}

template< typename Interface >
static inline void
dumpStateObjectDesc(StateWriter &writer, com_ptr<Interface> & pObj)
{
    dumpStateObjectDesc(writer, static_cast<Interface *>(pObj));
}


void dumpStateObject(StateWriter &, const RECT &); // D3D10_RECT, D3D11_RECT

void dumpStateObject(StateWriter &, const DXGI_FORMAT &);

void dumpStateObject(StateWriter &, const D3D10_BLEND_DESC &);
void dumpStateObject(StateWriter &, const D3D10_BLEND_DESC1 &);
void dumpStateObject(StateWriter &, const D3D10_BUFFER_DESC &);
void dumpStateObject(StateWriter &, const D3D10_COUNTER_DESC &);
void dumpStateObject(StateWriter &, const D3D10_DEPTH_STENCIL_DESC &);
void dumpStateObject(StateWriter &, const D3D10_DEPTH_STENCILOP_DESC &);
void dumpStateObject(StateWriter &, const D3D10_DEPTH_STENCIL_VIEW_DESC &);
void dumpStateObject(StateWriter &, const D3D10_INPUT_ELEMENT_DESC &);
// FIXME: aliases with D3D_PRIMITIVE_TOPOLOGY clashing with D3D11_PRIMITIVE_TOPOLOGY
//void dumpStateObject(StateWriter &, const D3D10_PRIMITIVE_TOPOLOGY &);
void dumpStateObject(StateWriter &, const D3D10_QUERY_DESC &);
void dumpStateObject(StateWriter &, const D3D10_RASTERIZER_DESC &);
void dumpStateObject(StateWriter &, const D3D10_RENDER_TARGET_VIEW_DESC &);
void dumpStateObject(StateWriter &, const D3D10_SAMPLER_DESC &);
void dumpStateObject(StateWriter &, const D3D10_SHADER_RESOURCE_VIEW_DESC &);
void dumpStateObject(StateWriter &, const D3D10_SHADER_RESOURCE_VIEW_DESC1 &);
void dumpStateObject(StateWriter &, const D3D10_TEXTURE1D_DESC &);
void dumpStateObject(StateWriter &, const D3D10_TEXTURE2D_DESC &);
void dumpStateObject(StateWriter &, const D3D10_TEXTURE3D_DESC &);
void dumpStateObject(StateWriter &, const D3D10_VIEWPORT &);

void dumpStateObject(StateWriter &, const D3D11_BLEND_DESC &);
void dumpStateObject(StateWriter &, const D3D11_BLEND_DESC1 &);
void dumpStateObject(StateWriter &, const D3D11_BUFFER_DESC &);
void dumpStateObject(StateWriter &, const D3D11_CLASS_INSTANCE_DESC &);
void dumpStateObject(StateWriter &, const D3D11_COUNTER_DESC &);
void dumpStateObject(StateWriter &, const D3D11_DEPTH_STENCIL_DESC &);
void dumpStateObject(StateWriter &, const D3D11_DEPTH_STENCILOP_DESC &);
void dumpStateObject(StateWriter &, const D3D11_DEPTH_STENCIL_VIEW_DESC &);
void dumpStateObject(StateWriter &, const D3D11_DEVICE_CONTEXT_TYPE &);
void dumpStateObject(StateWriter &, const D3D11_INPUT_ELEMENT_DESC &);
void dumpStateObject(StateWriter &, const D3D11_PACKED_MIP_DESC &);
void dumpStateObject(StateWriter &, const D3D11_PRIMITIVE_TOPOLOGY &);
void dumpStateObject(StateWriter &, const D3D11_QUERY_DESC &);
void dumpStateObject(StateWriter &, const D3D11_RASTERIZER_DESC &);
void dumpStateObject(StateWriter &, const D3D11_RASTERIZER_DESC1 &);
void dumpStateObject(StateWriter &, const D3D11_RENDER_TARGET_BLEND_DESC &);
void dumpStateObject(StateWriter &, const D3D11_RENDER_TARGET_VIEW_DESC &);
void dumpStateObject(StateWriter &, const D3D11_SAMPLER_DESC &);
void dumpStateObject(StateWriter &, const D3D11_SHADER_RESOURCE_VIEW_DESC &);
void dumpStateObject(StateWriter &, const D3D11_TEXTURE1D_DESC &);
void dumpStateObject(StateWriter &, const D3D11_TEXTURE2D_DESC &);
void dumpStateObject(StateWriter &, const D3D11_TEXTURE3D_DESC &);
void dumpStateObject(StateWriter &, const D3D11_UNORDERED_ACCESS_VIEW_DESC &);
void dumpStateObject(StateWriter &, const D3D11_VIEWPORT &);
