// TraceDirect3DDevice8.h
#pragma once

class Log;

class TraceDirect3DDevice8: public IDirect3DDevice8 {
public:

    TraceDirect3DDevice8(IDirect3DDevice8* pOriginal);
    virtual ~TraceDirect3DDevice8();

    // START: The original DX8.1a function definitions
    HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj);
    ULONG __stdcall AddRef(void);
    ULONG __stdcall Release(void);
    HRESULT __stdcall TestCooperativeLevel(void);
    UINT __stdcall GetAvailableTextureMem(void);
    HRESULT __stdcall ResourceManagerDiscardBytes(DWORD Bytes);
    HRESULT __stdcall GetDirect3D(IDirect3D8** ppD3D8);
    HRESULT __stdcall GetDeviceCaps(D3DCAPS8* pCaps);
    HRESULT __stdcall GetDisplayMode(D3DDISPLAYMODE* pMode);
    HRESULT __stdcall GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters);
    HRESULT __stdcall SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface8* pCursorBitmap);
    void __stdcall SetCursorPosition(int XScreenSpace, int YScreenSpace, DWORD Flags);
    BOOL __stdcall ShowCursor(BOOL bShow);
    HRESULT __stdcall CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain8** pSwapChain);
    HRESULT __stdcall Reset(D3DPRESENT_PARAMETERS* pPresentationParameters);
    HRESULT __stdcall Present(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion);
    HRESULT __stdcall GetBackBuffer(UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface8** ppBackBuffer);
    HRESULT __stdcall GetRasterStatus(D3DRASTER_STATUS* pRasterStatus);
    void __stdcall SetGammaRamp(DWORD Flags,CONST D3DGAMMARAMP* pRamp);
    void __stdcall GetGammaRamp(D3DGAMMARAMP* pRamp);
    HRESULT __stdcall CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture8** ppTexture);
    HRESULT __stdcall CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture8** ppVolumeTexture);
    HRESULT __stdcall CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture8** ppCubeTexture);
    HRESULT __stdcall CreateVertexBuffer(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer8** ppVertexBuffer);
    HRESULT __stdcall CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer8** ppIndexBuffer);
    HRESULT __stdcall CreateRenderTarget(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,BOOL Lockable,IDirect3DSurface8** ppSurface);
    HRESULT __stdcall CreateDepthStencilSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,IDirect3DSurface8** ppSurface);
    HRESULT __stdcall CreateImageSurface(UINT Width,UINT Height,D3DFORMAT Format,IDirect3DSurface8** ppSurface);
    HRESULT __stdcall CopyRects(IDirect3DSurface8* pSourceSurface,CONST RECT* pSourceRectsArray,UINT cRects,IDirect3DSurface8* pDestinationSurface,CONST POINT* pDestPointsArray);
    HRESULT __stdcall UpdateTexture(IDirect3DBaseTexture8* pSourceTexture,IDirect3DBaseTexture8* pDestinationTexture);
    HRESULT __stdcall GetFrontBuffer(IDirect3DSurface8* pDestSurface);
    HRESULT __stdcall SetRenderTarget(IDirect3DSurface8* pRenderTarget,IDirect3DSurface8* pNewZStencil);
    HRESULT __stdcall GetRenderTarget(IDirect3DSurface8** ppRenderTarget);
    HRESULT __stdcall GetDepthStencilSurface(IDirect3DSurface8** ppZStencilSurface);
    HRESULT __stdcall BeginScene(THIS);
    HRESULT __stdcall EndScene(THIS);
    HRESULT __stdcall Clear(DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil);
    HRESULT __stdcall SetTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix);
    HRESULT __stdcall GetTransform(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix);
    HRESULT __stdcall MultiplyTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix);
    HRESULT __stdcall SetViewport(CONST D3DVIEWPORT8* pViewport);
    HRESULT __stdcall GetViewport(D3DVIEWPORT8* pViewport);
    HRESULT __stdcall SetMaterial(CONST D3DMATERIAL8* pMaterial);
    HRESULT __stdcall GetMaterial(D3DMATERIAL8* pMaterial);
    HRESULT __stdcall SetLight(DWORD Index,CONST D3DLIGHT8* pLight);
    HRESULT __stdcall GetLight(DWORD Index,D3DLIGHT8* pLight);
    HRESULT __stdcall LightEnable(DWORD Index,BOOL Enable);
    HRESULT __stdcall GetLightEnable(DWORD Index,BOOL* pEnable);
    HRESULT __stdcall SetClipPlane(DWORD Index,CONST float* pPlane);
    HRESULT __stdcall GetClipPlane(DWORD Index,float* pPlane);
    HRESULT __stdcall SetRenderState(D3DRENDERSTATETYPE State,DWORD Value);
    HRESULT __stdcall GetRenderState(D3DRENDERSTATETYPE State,DWORD* pValue);
    HRESULT __stdcall BeginStateBlock(THIS);
    HRESULT __stdcall EndStateBlock(DWORD* pToken);
    HRESULT __stdcall ApplyStateBlock(DWORD Token);
    HRESULT __stdcall CaptureStateBlock(DWORD Token);
    HRESULT __stdcall DeleteStateBlock(DWORD Token);
    HRESULT __stdcall CreateStateBlock(D3DSTATEBLOCKTYPE Type,DWORD* pToken);
    HRESULT __stdcall SetClipStatus(CONST D3DCLIPSTATUS8* pClipStatus);
    HRESULT __stdcall GetClipStatus(D3DCLIPSTATUS8* pClipStatus);
    HRESULT __stdcall GetTexture(DWORD Stage,IDirect3DBaseTexture8** ppTexture);
    HRESULT __stdcall SetTexture(DWORD Stage,IDirect3DBaseTexture8* pTexture);
    HRESULT __stdcall GetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue);
    HRESULT __stdcall SetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value);
    HRESULT __stdcall ValidateDevice(DWORD* pNumPasses);
    HRESULT __stdcall GetInfo(DWORD DevInfoID,void* pDevInfoStruct,DWORD DevInfoStructSize);
    HRESULT __stdcall SetPaletteEntries(UINT PaletteNumber,CONST PALETTEENTRY* pEntries);
    HRESULT __stdcall GetPaletteEntries(UINT PaletteNumber,PALETTEENTRY* pEntries);
    HRESULT __stdcall SetCurrentTexturePalette(UINT PaletteNumber);
    HRESULT __stdcall GetCurrentTexturePalette(UINT *PaletteNumber);
    HRESULT __stdcall DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount);
    HRESULT __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE Type,UINT minIndex,UINT NumVertices,UINT startIndex,UINT primCount);
    HRESULT __stdcall DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
    HRESULT __stdcall DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertexIndices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
    HRESULT __stdcall ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer8* pDestBuffer,DWORD Flags);
    HRESULT __stdcall CreateVertexShader(CONST DWORD* pDeclaration,CONST DWORD* pFunction,DWORD* pHandle,DWORD Usage);
    HRESULT __stdcall SetVertexShader(DWORD Handle);
    HRESULT __stdcall GetVertexShader(DWORD* pHandle);
    HRESULT __stdcall DeleteVertexShader(DWORD Handle);
    HRESULT __stdcall SetVertexShaderConstant(DWORD Register,CONST void* pConstantData,DWORD ConstantCount);
    HRESULT __stdcall GetVertexShaderConstant(DWORD Register,void* pConstantData,DWORD ConstantCount);
    HRESULT __stdcall GetVertexShaderDeclaration(DWORD Handle,void* pData,DWORD* pSizeOfData);
    HRESULT __stdcall GetVertexShaderFunction(DWORD Handle,void* pData,DWORD* pSizeOfData);
    HRESULT __stdcall SetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer8* pStreamData,UINT Stride);
    HRESULT __stdcall GetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer8** ppStreamData,UINT* pStride);
    HRESULT __stdcall SetIndices(IDirect3DIndexBuffer8* pIndexData,UINT BaseVertexIndex);
    HRESULT __stdcall GetIndices(IDirect3DIndexBuffer8** ppIndexData,UINT* pBaseVertexIndex);
    HRESULT __stdcall CreatePixelShader(CONST DWORD* pFunction,DWORD* pHandle);
    HRESULT __stdcall SetPixelShader(DWORD Handle);
    HRESULT __stdcall GetPixelShader(DWORD* pHandle);
    HRESULT __stdcall DeletePixelShader(DWORD Handle);
    HRESULT __stdcall SetPixelShaderConstant(DWORD Register,CONST void* pConstantData,DWORD ConstantCount);
    HRESULT __stdcall GetPixelShaderConstant(DWORD Register,void* pConstantData,DWORD ConstantCount);
    HRESULT __stdcall GetPixelShaderFunction(DWORD Handle,void* pData,DWORD* pSizeOfData);
    HRESULT __stdcall DrawRectPatch(UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo);
    HRESULT __stdcall DrawTriPatch(UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo);
    HRESULT __stdcall DeletePatch(UINT Handle);
    // END: The original DX8.1a function definitions

private:

    IDirect3DDevice8 *m_pIDirect3DDevice8;
    Log *log;

    // This is our test function
    void ShowWeAreHere(void);

};

