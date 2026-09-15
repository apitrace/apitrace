/**************************************************************************
 *
 * Copyright (c) 2026 Broadcom. All Rights Reserved.
 * The term “Broadcom” refers to Broadcom Inc.
 * and/or its subsidiaries.
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


#include <assert.h>

#include <algorithm>

#include "os.hpp"
#include "image.hpp"
#include "state_writer.hpp"
#include "com_ptr.hpp"
#include "d3d12imports.hpp"
#include "dxgistate.hpp"
#include "d3dstate.hpp"


namespace d3dstate {


/*
 * Unlike D3D10/D3D11 there is no immediate context to copy through, and a
 * swapchain does not hand its command queue back.  Remember the last command
 * queue used, so that the copy below can be submitted on the same queue that
 * produced the frame, and is therefore ordered after it.
 */
static ID3D12CommandQueue *g_pLastCommandQueue = nullptr;

/*
 * Which back buffer Present was called on.  Snapshots are taken before Present
 * is replayed, but state dumps happen after it, by which point the swapchain
 * has already rotated and the current back buffer is the stale one.
 */
static UINT g_PresentBackBufferIndex = 0;
static bool g_HavePresentBackBufferIndex = false;


void
bindCommandQueue(ID3D12CommandQueue *pCommandQueue)
{
    g_pLastCommandQueue = pCommandQueue;
}


void
unbindCommandQueue(ID3D12CommandQueue *pCommandQueue)
{
    if (g_pLastCommandQueue == pCommandQueue) {
        g_pLastCommandQueue = nullptr;
    }
}


void
notifyPresent(IDXGISwapChain *pSwapChain)
{
    // Only D3D12 needs this, and only D3D12 ever binds a command queue.
    if (!g_pLastCommandQueue) {
        return;
    }

    com_ptr<IDXGISwapChain3> pSwapChain3;
    if (SUCCEEDED(pSwapChain->QueryInterface(IID_IDXGISwapChain3, (void **)&pSwapChain3))) {
        g_PresentBackBufferIndex = pSwapChain3->GetCurrentBackBufferIndex();
        g_HavePresentBackBufferIndex = true;
    }
}


/*
 * Submit pCommandList on pCommandQueue and block until the GPU is done with
 * it.
 */
static bool
executeAndWait(ID3D12CommandQueue *pCommandQueue, ID3D12GraphicsCommandList *pCommandList)
{
    HRESULT hr;

    com_ptr<ID3D12Device> pDevice;
    hr = pCommandQueue->GetDevice(IID_ID3D12Device, (void **)&pDevice);
    if (FAILED(hr)) {
        return false;
    }

    com_ptr<ID3D12Fence> pFence;
    hr = pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_ID3D12Fence, (void **)&pFence);
    if (FAILED(hr)) {
        return false;
    }

    ID3D12CommandList *pCommandLists[] = { pCommandList };
    pCommandQueue->ExecuteCommandLists(1, pCommandLists);

    hr = pCommandQueue->Signal(pFence, 1);
    if (FAILED(hr)) {
        return false;
    }

    if (pFence->GetCompletedValue() < 1) {
        HANDLE hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!hEvent) {
            return false;
        }
        hr = pFence->SetEventOnCompletion(1, hEvent);
        if (SUCCEEDED(hr)) {
            WaitForSingleObject(hEvent, INFINITE);
        }
        CloseHandle(hEvent);
        if (FAILED(hr)) {
            return false;
        }
    }

    return true;
}


/*
 * Copy a subresource into a readback buffer and convert it into an image.
 *
 * StateBefore is the state the caller guarantees the resource is currently in,
 * and which it is restored to afterwards.
 */
static image::Image *
getSubResourceImage(ID3D12Device *pDevice,
                    ID3D12CommandQueue *pCommandQueue,
                    ID3D12Resource *pResource,
                    UINT Subresource,
                    D3D12_RESOURCE_STATES StateBefore)
{
    HRESULT hr;

    D3D12_RESOURCE_DESC ResourceDesc = pResource->GetDesc();
    if (ResourceDesc.SampleDesc.Count > 1) {
        // TODO: resolve multisampled resources first.
        os::log("apitrace: warning: multisampled D3D12 resources are not supported\n");
        return nullptr;
    }

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT Footprint;
    UINT64 TotalBytes = 0;
    pDevice->GetCopyableFootprints(&ResourceDesc, Subresource, 1, 0,
                                   &Footprint, nullptr, nullptr, &TotalBytes);
    if (!TotalBytes) {
        return nullptr;
    }

    D3D12_HEAP_PROPERTIES HeapProperties = {};
    HeapProperties.Type = D3D12_HEAP_TYPE_READBACK;

    D3D12_RESOURCE_DESC BufferDesc = {};
    BufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    BufferDesc.Width = TotalBytes;
    BufferDesc.Height = 1;
    BufferDesc.DepthOrArraySize = 1;
    BufferDesc.MipLevels = 1;
    BufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    BufferDesc.SampleDesc.Count = 1;
    BufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    com_ptr<ID3D12Resource> pStagingResource;
    hr = pDevice->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE,
                                          &BufferDesc, D3D12_RESOURCE_STATE_COPY_DEST,
                                          nullptr, IID_ID3D12Resource,
                                          (void **)&pStagingResource);
    if (FAILED(hr)) {
        return nullptr;
    }

    com_ptr<ID3D12CommandAllocator> pCommandAllocator;
    hr = pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                         IID_ID3D12CommandAllocator,
                                         (void **)&pCommandAllocator);
    if (FAILED(hr)) {
        return nullptr;
    }

    com_ptr<ID3D12GraphicsCommandList> pCommandList;
    hr = pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                    pCommandAllocator, nullptr,
                                    IID_ID3D12GraphicsCommandList,
                                    (void **)&pCommandList);
    if (FAILED(hr)) {
        return nullptr;
    }

    D3D12_RESOURCE_BARRIER Barrier = {};
    Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    Barrier.Transition.pResource = pResource;
    Barrier.Transition.Subresource = Subresource;
    Barrier.Transition.StateBefore = StateBefore;
    Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;

    if (StateBefore != D3D12_RESOURCE_STATE_COPY_SOURCE) {
        pCommandList->ResourceBarrier(1, &Barrier);
    }

    D3D12_TEXTURE_COPY_LOCATION DstLocation = {};
    DstLocation.pResource = pStagingResource;
    DstLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    DstLocation.PlacedFootprint = Footprint;

    D3D12_TEXTURE_COPY_LOCATION SrcLocation = {};
    SrcLocation.pResource = pResource;
    SrcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    SrcLocation.SubresourceIndex = Subresource;

    pCommandList->CopyTextureRegion(&DstLocation, 0, 0, 0, &SrcLocation, nullptr);

    if (StateBefore != D3D12_RESOURCE_STATE_COPY_SOURCE) {
        std::swap(Barrier.Transition.StateBefore, Barrier.Transition.StateAfter);
        pCommandList->ResourceBarrier(1, &Barrier);
    }

    hr = pCommandList->Close();
    if (FAILED(hr)) {
        return nullptr;
    }

    if (!executeAndWait(pCommandQueue, pCommandList)) {
        return nullptr;
    }

    void *pData = nullptr;
    D3D12_RANGE ReadRange = { 0, static_cast<SIZE_T>(TotalBytes) };
    hr = pStagingResource->Map(0, &ReadRange, &pData);
    if (FAILED(hr)) {
        return nullptr;
    }

    image::Image *image;
    image = ConvertImage(Footprint.Footprint.Format,
                         pData,
                         Footprint.Footprint.RowPitch,
                         Footprint.Footprint.Width,
                         Footprint.Footprint.Height,
                         false);

    D3D12_RANGE WrittenRange = { 0, 0 };
    pStagingResource->Unmap(0, &WrittenRange);

    return image;
}


image::Image *
getRenderTargetImage(IDXGISwapChain *pSwapChain, ID3D12Device *pDevice)
{
    HRESULT hr;

    if (!g_pLastCommandQueue) {
        // Without the queue that rendered the frame the copy below could not
        // be ordered against it, and would race with it.
        os::log("apitrace: warning: no D3D12 command queue to read the back buffer with\n");
        return nullptr;
    }

    // Prefer the back buffer Present was called on, as recorded above.
    UINT BufferIndex = 0;
    if (g_HavePresentBackBufferIndex) {
        BufferIndex = g_PresentBackBufferIndex;
    } else {
        com_ptr<IDXGISwapChain3> pSwapChain3;
        if (SUCCEEDED(pSwapChain->QueryInterface(IID_IDXGISwapChain3, (void **)&pSwapChain3))) {
            BufferIndex = pSwapChain3->GetCurrentBackBufferIndex();
        }
    }

    com_ptr<ID3D12Resource> pResource;
    hr = pSwapChain->GetBuffer(BufferIndex, IID_ID3D12Resource, (void **)&pResource);
    if (FAILED(hr)) {
        return nullptr;
    }

    /*
     * A back buffer must be in the PRESENT (ie. COMMON) state for Present to
     * be legal, which is where we are.
     */
    return getSubResourceImage(pDevice, g_pLastCommandQueue, pResource, 0,
                               D3D12_RESOURCE_STATE_PRESENT);
}


void
dumpDevice(StateWriter &writer, IDXGISwapChain *pSwapChain, ID3D12Device *pDevice)
{
    /*
     * D3D12 command lists cannot be queried for the state bound to them, so
     * the presented back buffer is all there is to dump.  Label it
     * RENDER_TARGET_0, as the other D3D versions label their render targets.
     */
    writer.beginMember("framebuffer");
    writer.beginObject();
    image::Image *image = getRenderTargetImage(pSwapChain, pDevice);
    if (image) {
        writer.beginMember("RENDER_TARGET_0");
        writer.writeImage(image);
        writer.endMember();
        delete image;
    }
    writer.endObject();
    writer.endMember(); // framebuffer
}


} /* namespace d3dstate */
