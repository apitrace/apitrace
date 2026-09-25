/**************************************************************************
 *
 * Copyright (c) 2012-2024 Broadcom. All Rights Reserved.
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


#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include <initguid.h>
#include <windows.h>

#include "winsdk_compat.h"

#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <dxgi1_4.h>

#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

#include "tri_vs_5_0.h"
#include "tri_ps_5_0.h"


int
main(int argc, char *argv[])
{
    HRESULT hr;

    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX),
        CS_CLASSDC,
        DefWindowProc,
        0,
        0,
        hInstance,
        NULL,
        NULL,
        NULL,
        NULL,
        "SimpleDX12",
        NULL
    };
    RegisterClassEx(&wc);

    const int WindowWidth = 250;
    const int WindowHeight = 250;

    DWORD dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW;

    RECT rect = {0, 0, WindowWidth, WindowHeight};
    AdjustWindowRect(&rect, dwStyle, FALSE);

    HWND hWnd = CreateWindow(wc.lpszClassName,
                             "Simple example using DirectX12",
                             dwStyle,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             rect.right - rect.left,
                             rect.bottom - rect.top,
                             NULL,
                             NULL,
                             hInstance,
                             NULL);
    if (!hWnd) {
        return 1;
    }

    ComPtr<ID3D12Debug> pDebug;
    hr = D3D12GetDebugInterface(IID_PPV_ARGS(&pDebug));
    if (SUCCEEDED(hr)) {
        pDebug->EnableDebugLayer();
    }

    ComPtr<IDXGIFactory4> pFactory;
    hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
    if (FAILED(hr)) {
        return 1;
    }

    ComPtr<IDXGIAdapter> pAdapter;
    hr = pFactory->EnumAdapters(0, &pAdapter);
    if (FAILED(hr)) {
        return 1;
    }

    ComPtr<ID3D12Device> pDevice;
    hr = D3D12CreateDevice(pAdapter.Get(),
                           D3D_FEATURE_LEVEL_11_0,
                           IID_PPV_ARGS(&pDevice));
    if (FAILED(hr)) {
        return 1;
    }

    pDevice->SetName(L"Device");

    D3D12_COMMAND_QUEUE_DESC CommandQueueDesc;
    ZeroMemory(&CommandQueueDesc, sizeof CommandQueueDesc);
    CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    CommandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    ComPtr<ID3D12CommandQueue> pCommandQueue;
    hr = pDevice->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&pCommandQueue));
    if (FAILED(hr)) {
        return 1;
    }

    pCommandQueue->SetName(L"CommandQueue");

    const UINT NumBuffers = 2;

    DXGI_SWAP_CHAIN_DESC1 SwapChainDesc;
    ZeroMemory(&SwapChainDesc, sizeof SwapChainDesc);
    SwapChainDesc.Width = WindowWidth;
    SwapChainDesc.Height = WindowHeight;
    SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SwapChainDesc.SampleDesc.Count = 1;
    SwapChainDesc.SampleDesc.Quality = 0;
    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChainDesc.BufferCount = NumBuffers;
    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    ComPtr<IDXGISwapChain1> pSwapChain1;
    hr = pFactory->CreateSwapChainForHwnd(pCommandQueue.Get(), // the queue is flushed on Present
                                          hWnd,
                                          &SwapChainDesc,
                                          NULL, /* pFullscreenDesc */
                                          NULL, /* pRestrictToOutput */
                                          &pSwapChain1);
    if (FAILED(hr)) {
        return 1;
    }

    ComPtr<IDXGISwapChain3> pSwapChain;
    hr = pSwapChain1->QueryInterface(IID_PPV_ARGS(&pSwapChain));
    if (FAILED(hr)) {
        return 1;
    }

    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
    ZeroMemory(&DescriptorHeapDesc, sizeof DescriptorHeapDesc);
    DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    DescriptorHeapDesc.NumDescriptors = NumBuffers;
    DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    ComPtr<ID3D12DescriptorHeap> pRenderTargetViewHeap;
    hr = pDevice->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&pRenderTargetViewHeap));
    if (FAILED(hr)) {
        return 1;
    }

    pRenderTargetViewHeap->SetName(L"RenderTargetViewHeap");

    UINT RenderTargetViewSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    D3D12_RENDER_TARGET_VIEW_DESC RenderTargetViewDesc;
    ZeroMemory(&RenderTargetViewDesc, sizeof RenderTargetViewDesc);
    RenderTargetViewDesc.Format = SwapChainDesc.Format;
    RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    RenderTargetViewDesc.Texture2D.MipSlice = 0;

    ComPtr<ID3D12Resource> pBackBuffers[NumBuffers];
    D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetViewHandle = pRenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();
    for (UINT i = 0; i < NumBuffers; ++i) {
        hr = pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffers[i]));
        if (FAILED(hr)) {
            return 1;
        }

        pBackBuffers[i]->SetName(L"BackBuffer");

        pDevice->CreateRenderTargetView(pBackBuffers[i].Get(), &RenderTargetViewDesc, RenderTargetViewHandle);
        RenderTargetViewHandle.ptr += RenderTargetViewSize;
    }

    // Empty root signature, as the shaders take no resources
    D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;
    ZeroMemory(&RootSignatureDesc, sizeof RootSignatureDesc);
    RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> pRootSignatureBlob;
    hr = D3D12SerializeRootSignature(&RootSignatureDesc,
                                     D3D_ROOT_SIGNATURE_VERSION_1,
                                     &pRootSignatureBlob,
                                     NULL); /* ppErrorBlob */
    if (FAILED(hr)) {
        return 1;
    }

    ComPtr<ID3D12RootSignature> pRootSignature;
    hr = pDevice->CreateRootSignature(0,
                                      pRootSignatureBlob->GetBufferPointer(),
                                      pRootSignatureBlob->GetBufferSize(),
                                      IID_PPV_ARGS(&pRootSignature));
    if (FAILED(hr)) {
        return 1;
    }

    pRootSignature->SetName(L"RootSignature");

    struct Vertex {
        float position[4];
        float color[4];
    };

    static const D3D12_INPUT_ELEMENT_DESC InputElementDescs[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, position), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, color),    D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc;
    ZeroMemory(&PipelineStateDesc, sizeof PipelineStateDesc);
    PipelineStateDesc.pRootSignature = pRootSignature.Get();
    PipelineStateDesc.VS.pShaderBytecode = g_VS;
    PipelineStateDesc.VS.BytecodeLength = sizeof g_VS;
    PipelineStateDesc.PS.pShaderBytecode = g_PS;
    PipelineStateDesc.PS.BytecodeLength = sizeof g_PS;
    PipelineStateDesc.SampleMask = UINT_MAX;
    PipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    PipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    PipelineStateDesc.RasterizerState.FrontCounterClockwise = true;
    PipelineStateDesc.RasterizerState.DepthClipEnable = true;
    PipelineStateDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
        PipelineStateDesc.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    }
    PipelineStateDesc.InputLayout.pInputElementDescs = InputElementDescs;
    PipelineStateDesc.InputLayout.NumElements = 2;
    PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    PipelineStateDesc.NumRenderTargets = 1;
    PipelineStateDesc.RTVFormats[0] = SwapChainDesc.Format;
    PipelineStateDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
    PipelineStateDesc.SampleDesc.Count = 1;

    ComPtr<ID3D12PipelineState> pPipelineState;
    hr = pDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState));
    if (FAILED(hr)) {
        return 1;
    }

    pPipelineState->SetName(L"PipelineState");

    static const Vertex vertices[] = {
        { { -0.9f, -0.9f, 0.5f, 1.0f}, { 0.8f, 0.0f, 0.0f, 0.1f } },
        { {  0.9f, -0.9f, 0.5f, 1.0f}, { 0.0f, 0.9f, 0.0f, 0.1f } },
        { {  0.0f,  0.9f, 0.5f, 1.0f}, { 0.0f, 0.0f, 0.7f, 0.1f } },
    };

    D3D12_HEAP_PROPERTIES HeapProperties;
    ZeroMemory(&HeapProperties, sizeof HeapProperties);
    HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
    HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC ResourceDesc;
    ZeroMemory(&ResourceDesc, sizeof ResourceDesc);
    ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    ResourceDesc.Width = sizeof vertices;
    ResourceDesc.Height = 1;
    ResourceDesc.DepthOrArraySize = 1;
    ResourceDesc.MipLevels = 1;
    ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    ResourceDesc.SampleDesc.Count = 1;
    ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    ComPtr<ID3D12Resource> pVertexBuffer;
    hr = pDevice->CreateCommittedResource(&HeapProperties,
                                          D3D12_HEAP_FLAG_NONE,
                                          &ResourceDesc,
                                          D3D12_RESOURCE_STATE_GENERIC_READ,
                                          NULL, /* pOptimizedClearValue */
                                          IID_PPV_ARGS(&pVertexBuffer));
    if (FAILED(hr)) {
        return 1;
    }

    pVertexBuffer->SetName(L"VertexBuffer");

    void *pMappedData = NULL;
    D3D12_RANGE ReadRange = { 0, 0 };
    hr = pVertexBuffer->Map(0, &ReadRange, &pMappedData);
    if (FAILED(hr)) {
        return 1;
    }
    memcpy(pMappedData, vertices, sizeof vertices);
    pVertexBuffer->Unmap(0, NULL);

    D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
    VertexBufferView.BufferLocation = pVertexBuffer->GetGPUVirtualAddress();
    VertexBufferView.SizeInBytes = sizeof vertices;
    VertexBufferView.StrideInBytes = sizeof(Vertex);

    ComPtr<ID3D12CommandAllocator> pCommandAllocator;
    hr = pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCommandAllocator));
    if (FAILED(hr)) {
        return 1;
    }

    ComPtr<ID3D12GraphicsCommandList> pCommandList;
    hr = pDevice->CreateCommandList(0,
                                    D3D12_COMMAND_LIST_TYPE_DIRECT,
                                    pCommandAllocator.Get(),
                                    NULL, /* pInitialState */
                                    IID_PPV_ARGS(&pCommandList));
    if (FAILED(hr)) {
        return 1;
    }

    pCommandList->SetName(L"CommandList");

    UINT BackBufferIndex = pSwapChain->GetCurrentBackBufferIndex();

    RenderTargetViewHandle = pRenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();
    RenderTargetViewHandle.ptr += BackBufferIndex * RenderTargetViewSize;

    D3D12_RESOURCE_BARRIER ResourceBarrier;
    ZeroMemory(&ResourceBarrier, sizeof ResourceBarrier);
    ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    ResourceBarrier.Transition.pResource = pBackBuffers[BackBufferIndex].Get();
    ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    pCommandList->ResourceBarrier(1, &ResourceBarrier);

    pCommandList->OMSetRenderTargets(1, &RenderTargetViewHandle, FALSE, NULL);

    const float clearColor[4] = { 0.3f, 0.1f, 0.3f, 1.0f };
    pCommandList->ClearRenderTargetView(RenderTargetViewHandle, clearColor, 0, NULL);

    pCommandList->SetGraphicsRootSignature(pRootSignature.Get());
    pCommandList->SetPipelineState(pPipelineState.Get());

    pCommandList->IASetVertexBuffers(0, 1, &VertexBufferView);

    D3D12_VIEWPORT ViewPort;
    ViewPort.TopLeftX = 0;
    ViewPort.TopLeftY = 0;
    ViewPort.Width = WindowWidth;
    ViewPort.Height = WindowHeight;
    ViewPort.MinDepth = 0.0f;
    ViewPort.MaxDepth = 1.0f;
    pCommandList->RSSetViewports(1, &ViewPort);

    D3D12_RECT ScissorRect = { 0, 0, WindowWidth, WindowHeight };
    pCommandList->RSSetScissorRects(1, &ScissorRect);

    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    pCommandList->DrawInstanced(3, 1, 0, 0);

    ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    pCommandList->ResourceBarrier(1, &ResourceBarrier);

    hr = pCommandList->Close();
    if (FAILED(hr)) {
        return 1;
    }

    pCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList **)pCommandList.GetAddressOf());

    hr = pSwapChain->Present(0, 0);
    if (FAILED(hr)) {
        return 1;
    }

    // Wait for the GPU to be done with the command list before tearing
    // everything down.
    ComPtr<ID3D12Fence> pFence;
    hr = pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence));
    if (FAILED(hr)) {
        return 1;
    }

    HANDLE hFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!hFenceEvent) {
        return 1;
    }

    hr = pFence->SetEventOnCompletion(1, hFenceEvent);
    if (FAILED(hr)) {
        return 1;
    }

    hr = pCommandQueue->Signal(pFence.Get(), 1);
    if (FAILED(hr)) {
        return 1;
    }

    WaitForSingleObject(hFenceEvent, INFINITE);
    CloseHandle(hFenceEvent);

    DestroyWindow(hWnd);

    return 0;
}
