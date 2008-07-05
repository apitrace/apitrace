// TraceDirect3DDevice8.cpp
#include "stdafx.h"

TraceDirect3DDevice8::TraceDirect3DDevice8(IDirect3DDevice8* pOriginal) {
    m_pIDirect3DDevice8 = pOriginal; // store the pointer to original object
}

TraceDirect3DDevice8::~TraceDirect3DDevice8() {

}

HRESULT __stdcall TraceDirect3DDevice8::QueryInterface(REFIID riid, void** ppvObj) {
    // check if original dll can provide an interface. then send *our* address
    *ppvObj = NULL;

    HRESULT hRes = m_pIDirect3DDevice8->QueryInterface(riid, ppvObj);

    if (hRes == NOERROR) {
        *ppvObj = this;
    }

    return hRes;
}

ULONG __stdcall TraceDirect3DDevice8::AddRef(void) {
    return (m_pIDirect3DDevice8->AddRef());
}

ULONG __stdcall TraceDirect3DDevice8::Release(void) {
    // ATTENTION: This is a booby-trap ! Watch out !
    // If we create our own sprites, surfaces, etc. (thus increasing the ref counter
    // by external action), we need to delete that objects before calling the original
    // Release function	

    // release/delete own objects
    // ... here if any ...

    // Calling original function now
    ULONG count = m_pIDirect3DDevice8->Release();

    // now, the Original Object has deleted itself, so do we here
    delete (this); // destructor will be called automatically

    return (count);
}

HRESULT __stdcall TraceDirect3DDevice8::TestCooperativeLevel(void) {
    return (m_pIDirect3DDevice8->TestCooperativeLevel());
}

UINT __stdcall TraceDirect3DDevice8::GetAvailableTextureMem(void) {
    return (m_pIDirect3DDevice8->GetAvailableTextureMem());
}

HRESULT __stdcall TraceDirect3DDevice8::ResourceManagerDiscardBytes(DWORD Bytes) {
    return (m_pIDirect3DDevice8->ResourceManagerDiscardBytes(Bytes));
}

HRESULT __stdcall TraceDirect3DDevice8::GetDirect3D(IDirect3D8** ppD3D8) {
    return (m_pIDirect3DDevice8->GetDirect3D(ppD3D8));
}

HRESULT __stdcall TraceDirect3DDevice8::GetDeviceCaps(D3DCAPS8* pCaps) {
    return (m_pIDirect3DDevice8->GetDeviceCaps(pCaps));
}

HRESULT __stdcall TraceDirect3DDevice8::GetDisplayMode(D3DDISPLAYMODE* pMode) {
    return (m_pIDirect3DDevice8->GetDisplayMode(pMode));
}

HRESULT __stdcall TraceDirect3DDevice8::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters) {
    return (m_pIDirect3DDevice8->GetCreationParameters(pParameters));
}

HRESULT __stdcall TraceDirect3DDevice8::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface8* pCursorBitmap) {
    return (m_pIDirect3DDevice8->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap));
}

void __stdcall TraceDirect3DDevice8::SetCursorPosition(int XScreenSpace, int YScreenSpace, DWORD Flags) {
    m_pIDirect3DDevice8->SetCursorPosition(XScreenSpace, YScreenSpace, Flags);
}

BOOL __stdcall TraceDirect3DDevice8::ShowCursor(BOOL bShow) {
    return (m_pIDirect3DDevice8->ShowCursor(bShow));
}

HRESULT __stdcall TraceDirect3DDevice8::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain8** pSwapChain) {
    return (m_pIDirect3DDevice8->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain));
}

HRESULT __stdcall TraceDirect3DDevice8::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) {
    return (m_pIDirect3DDevice8->Reset(pPresentationParameters));
}

HRESULT __stdcall TraceDirect3DDevice8::Present(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion)
{
    // we may want to draw own things here before flipping surfaces
    // ... draw own stuff ...
    this->ShowWeAreHere();

    // call original routine
    HRESULT hres = m_pIDirect3DDevice8->Present( pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);

    return (hres);
}

HRESULT __stdcall TraceDirect3DDevice8::GetBackBuffer(UINT BackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface8** ppBackBuffer) {
    return (m_pIDirect3DDevice8->GetBackBuffer(BackBuffer, Type, ppBackBuffer));
}

HRESULT __stdcall TraceDirect3DDevice8::GetRasterStatus(D3DRASTER_STATUS* pRasterStatus) {
    return (m_pIDirect3DDevice8->GetRasterStatus(pRasterStatus));
}

void __stdcall TraceDirect3DDevice8::SetGammaRamp(DWORD Flags,CONST D3DGAMMARAMP* pRamp)
{   m_pIDirect3DDevice8->SetGammaRamp( Flags, pRamp);}

void __stdcall TraceDirect3DDevice8::GetGammaRamp(D3DGAMMARAMP* pRamp) {
    m_pIDirect3DDevice8->GetGammaRamp(pRamp);
}

HRESULT __stdcall TraceDirect3DDevice8::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture8** ppTexture) {
    return (m_pIDirect3DDevice8->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture));
}

HRESULT __stdcall TraceDirect3DDevice8::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture8** ppVolumeTexture) {
    return (m_pIDirect3DDevice8->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture));
}

HRESULT __stdcall TraceDirect3DDevice8::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture8** ppCubeTexture) {
    return (m_pIDirect3DDevice8->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture));
}

HRESULT __stdcall TraceDirect3DDevice8::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer8** ppVertexBuffer) {
    return (m_pIDirect3DDevice8->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer));
}

HRESULT __stdcall TraceDirect3DDevice8::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer8** ppIndexBuffer) {
    return (m_pIDirect3DDevice8->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer));
}

HRESULT __stdcall TraceDirect3DDevice8::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, BOOL Lockable, IDirect3DSurface8** ppSurface) {
    return (m_pIDirect3DDevice8->CreateRenderTarget(Width, Height, Format, MultiSample, Lockable, ppSurface));
}

HRESULT __stdcall TraceDirect3DDevice8::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, IDirect3DSurface8** ppSurface) {
    return (m_pIDirect3DDevice8->CreateDepthStencilSurface(Width, Height, Format, MultiSample, ppSurface));
}

HRESULT __stdcall TraceDirect3DDevice8::CreateImageSurface(UINT Width, UINT Height, D3DFORMAT Format, IDirect3DSurface8** ppSurface) {
    return (m_pIDirect3DDevice8->CreateImageSurface(Width, Height, Format, ppSurface));
}

HRESULT __stdcall TraceDirect3DDevice8::CopyRects( IDirect3DSurface8* pSourceSurface,CONST RECT* pSourceRectsArray,UINT cRects,IDirect3DSurface8* pDestinationSurface,CONST POINT* pDestPointsArray)
{   return (m_pIDirect3DDevice8->CopyRects(pSourceSurface, pSourceRectsArray, cRects, pDestinationSurface, pDestPointsArray) );}

HRESULT __stdcall TraceDirect3DDevice8::UpdateTexture(IDirect3DBaseTexture8* pSourceTexture, IDirect3DBaseTexture8* pDestinationTexture) {
    return (m_pIDirect3DDevice8->UpdateTexture(pSourceTexture, pDestinationTexture));
}

HRESULT __stdcall TraceDirect3DDevice8::GetFrontBuffer(IDirect3DSurface8* pDestSurface) {
    return (m_pIDirect3DDevice8->GetFrontBuffer(pDestSurface));
}

HRESULT __stdcall TraceDirect3DDevice8::SetRenderTarget(IDirect3DSurface8* pRenderTarget, IDirect3DSurface8* pNewZStencil) {
    return (m_pIDirect3DDevice8->SetRenderTarget(pRenderTarget, pNewZStencil));
}

HRESULT __stdcall TraceDirect3DDevice8::GetRenderTarget(IDirect3DSurface8** ppRenderTarget) {
    return (m_pIDirect3DDevice8->GetRenderTarget(ppRenderTarget));
}

HRESULT __stdcall TraceDirect3DDevice8::GetDepthStencilSurface(IDirect3DSurface8** ppZStencilSurface) {
    return (m_pIDirect3DDevice8->GetDepthStencilSurface(ppZStencilSurface));
}

HRESULT __stdcall TraceDirect3DDevice8::BeginScene(void) {
    return (m_pIDirect3DDevice8->BeginScene());
}

HRESULT __stdcall TraceDirect3DDevice8::EndScene(void) {
    return (m_pIDirect3DDevice8->EndScene());
}

HRESULT __stdcall TraceDirect3DDevice8::Clear( DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil)
{   return (m_pIDirect3DDevice8->Clear(Count, pRects, Flags, Color, Z, Stencil) );}

HRESULT __stdcall TraceDirect3DDevice8::SetTransform( D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix)
{   return (m_pIDirect3DDevice8->SetTransform(State, pMatrix) );}

HRESULT __stdcall TraceDirect3DDevice8::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix) {
    return (m_pIDirect3DDevice8->GetTransform(State, pMatrix));
}

HRESULT __stdcall TraceDirect3DDevice8::MultiplyTransform( D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix)
{   return (m_pIDirect3DDevice8->MultiplyTransform( State,pMatrix) );}

HRESULT __stdcall TraceDirect3DDevice8::SetViewport( CONST D3DVIEWPORT8* pViewport)
{   return (m_pIDirect3DDevice8->SetViewport( pViewport) );}

HRESULT __stdcall TraceDirect3DDevice8::GetViewport(D3DVIEWPORT8* pViewport) {
    return (m_pIDirect3DDevice8->GetViewport(pViewport));
}

HRESULT __stdcall TraceDirect3DDevice8::SetMaterial( CONST D3DMATERIAL8* pMaterial)
{   return (m_pIDirect3DDevice8->SetMaterial( pMaterial) );}

HRESULT __stdcall TraceDirect3DDevice8::GetMaterial(D3DMATERIAL8* pMaterial) {
    return (m_pIDirect3DDevice8->GetMaterial(pMaterial));
}

HRESULT __stdcall TraceDirect3DDevice8::SetLight( DWORD Index,CONST D3DLIGHT8* pLight)
{   return (m_pIDirect3DDevice8->SetLight(Index, pLight) );}

HRESULT __stdcall TraceDirect3DDevice8::GetLight(DWORD Index, D3DLIGHT8* pLight) {
    return (m_pIDirect3DDevice8->GetLight(Index, pLight));
}

HRESULT __stdcall TraceDirect3DDevice8::LightEnable(DWORD Index, BOOL Enable) {
    return (m_pIDirect3DDevice8->LightEnable(Index, Enable));
}

HRESULT __stdcall TraceDirect3DDevice8::GetLightEnable(DWORD Index, BOOL* pEnable) {
    return (m_pIDirect3DDevice8->GetLightEnable(Index, pEnable));
}

HRESULT __stdcall TraceDirect3DDevice8::SetClipPlane(DWORD Index, CONST float* pPlane) {
    return (m_pIDirect3DDevice8->SetClipPlane(Index, pPlane));
}

HRESULT __stdcall TraceDirect3DDevice8::GetClipPlane(DWORD Index, float* pPlane) {
    return (m_pIDirect3DDevice8->GetClipPlane(Index, pPlane));
}

HRESULT __stdcall TraceDirect3DDevice8::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) {
    return (m_pIDirect3DDevice8->SetRenderState(State, Value));
}

HRESULT __stdcall TraceDirect3DDevice8::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) {
    return (m_pIDirect3DDevice8->GetRenderState(State, pValue));
}

HRESULT __stdcall TraceDirect3DDevice8::BeginStateBlock(void) {
    return (m_pIDirect3DDevice8->BeginStateBlock());
}

HRESULT __stdcall TraceDirect3DDevice8::EndStateBlock(DWORD* pToken) {
    return (m_pIDirect3DDevice8->EndStateBlock(pToken));
}

HRESULT __stdcall TraceDirect3DDevice8::ApplyStateBlock(DWORD Token) {
    return (m_pIDirect3DDevice8->ApplyStateBlock(Token));
}

HRESULT __stdcall TraceDirect3DDevice8::CaptureStateBlock(DWORD Token) {
    return (m_pIDirect3DDevice8->CaptureStateBlock(Token));
}

HRESULT __stdcall TraceDirect3DDevice8::DeleteStateBlock(DWORD Token) {
    return (m_pIDirect3DDevice8->DeleteStateBlock(Token));
}

HRESULT __stdcall TraceDirect3DDevice8::CreateStateBlock(D3DSTATEBLOCKTYPE Type, DWORD* pToken) {
    return (m_pIDirect3DDevice8->CreateStateBlock(Type, pToken));
}

HRESULT __stdcall TraceDirect3DDevice8::SetClipStatus( CONST D3DCLIPSTATUS8* pClipStatus)
{   return (m_pIDirect3DDevice8->SetClipStatus( pClipStatus) );}

HRESULT __stdcall TraceDirect3DDevice8::GetClipStatus(D3DCLIPSTATUS8* pClipStatus) {
    return (m_pIDirect3DDevice8->GetClipStatus(pClipStatus));
}

HRESULT __stdcall TraceDirect3DDevice8::GetTexture(DWORD Stage, IDirect3DBaseTexture8** ppTexture) {
    return (m_pIDirect3DDevice8->GetTexture(Stage, ppTexture));
}

HRESULT __stdcall TraceDirect3DDevice8::SetTexture(DWORD Stage, IDirect3DBaseTexture8* pTexture) {
    return (m_pIDirect3DDevice8->SetTexture(Stage, pTexture));
}

HRESULT __stdcall TraceDirect3DDevice8::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue) {
    return (m_pIDirect3DDevice8->GetTextureStageState(Stage, Type, pValue));
}

HRESULT __stdcall TraceDirect3DDevice8::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) {
    return (m_pIDirect3DDevice8->SetTextureStageState(Stage, Type, Value));
}

HRESULT __stdcall TraceDirect3DDevice8::ValidateDevice(DWORD* pNumPasses) {
    return (m_pIDirect3DDevice8->ValidateDevice(pNumPasses));
}

HRESULT __stdcall TraceDirect3DDevice8::GetInfo(DWORD DevInfoID, void* pDevInfoStruct, DWORD DevInfoStructSize) {
    return (m_pIDirect3DDevice8->GetInfo(DevInfoID, pDevInfoStruct, DevInfoStructSize));
}

HRESULT __stdcall TraceDirect3DDevice8::SetPaletteEntries( UINT PaletteNumber,CONST PALETTEENTRY* pEntries)
{   return (m_pIDirect3DDevice8->SetPaletteEntries(PaletteNumber, pEntries) );}

HRESULT __stdcall TraceDirect3DDevice8::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) {
    return (m_pIDirect3DDevice8->GetPaletteEntries(PaletteNumber, pEntries));
}

HRESULT __stdcall TraceDirect3DDevice8::SetCurrentTexturePalette(UINT PaletteNumber) {
    return (m_pIDirect3DDevice8->SetCurrentTexturePalette(PaletteNumber));
}

HRESULT __stdcall TraceDirect3DDevice8::GetCurrentTexturePalette(UINT *PaletteNumber) {
    return (m_pIDirect3DDevice8->GetCurrentTexturePalette(PaletteNumber));
}

HRESULT __stdcall TraceDirect3DDevice8::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) {
    return (m_pIDirect3DDevice8->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount));
}

HRESULT __stdcall TraceDirect3DDevice8::DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, UINT minIndex, UINT NumVertices, UINT startIndex, UINT primCount) {
    return (m_pIDirect3DDevice8->DrawIndexedPrimitive(Type, minIndex, NumVertices, startIndex, primCount));
}

HRESULT __stdcall TraceDirect3DDevice8::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
    return (m_pIDirect3DDevice8->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride));
}

HRESULT __stdcall TraceDirect3DDevice8::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertexIndices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
    return (m_pIDirect3DDevice8->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertexIndices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride));
}

HRESULT __stdcall TraceDirect3DDevice8::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer8* pDestBuffer, DWORD Flags) {
    return (m_pIDirect3DDevice8->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, Flags));
}

HRESULT __stdcall TraceDirect3DDevice8::CreateVertexShader( CONST DWORD* pDeclaration,CONST DWORD* pFunction,DWORD* pHandle,DWORD Usage)
{   return (m_pIDirect3DDevice8->CreateVertexShader( pDeclaration, pFunction, pHandle, Usage));}

HRESULT __stdcall TraceDirect3DDevice8::SetVertexShader(DWORD Handle) {
    return (m_pIDirect3DDevice8->SetVertexShader(Handle));
}

HRESULT __stdcall TraceDirect3DDevice8::GetVertexShader(DWORD* pHandle) {
    return (m_pIDirect3DDevice8->GetVertexShader(pHandle));
}

HRESULT __stdcall TraceDirect3DDevice8::DeleteVertexShader(DWORD Handle) {
    return (m_pIDirect3DDevice8->DeleteVertexShader(Handle));
}

HRESULT __stdcall TraceDirect3DDevice8::SetVertexShaderConstant(DWORD Register, CONST void* pConstantData, DWORD ConstantCount) {
    return (m_pIDirect3DDevice8->SetVertexShaderConstant(Register, pConstantData, ConstantCount));
}

HRESULT __stdcall TraceDirect3DDevice8::GetVertexShaderConstant(DWORD Register, void* pConstantData, DWORD ConstantCount) {
    return (m_pIDirect3DDevice8->GetVertexShaderConstant(Register, pConstantData, ConstantCount));
}

HRESULT __stdcall TraceDirect3DDevice8::GetVertexShaderDeclaration(DWORD Handle, void* pData, DWORD* pSizeOfData) {
    return (m_pIDirect3DDevice8->GetVertexShaderDeclaration(Handle, pData, pSizeOfData));
}

HRESULT __stdcall TraceDirect3DDevice8::GetVertexShaderFunction(DWORD Handle, void* pData, DWORD* pSizeOfData) {
    return (m_pIDirect3DDevice8->GetVertexShaderFunction(Handle, pData, pSizeOfData));
}

HRESULT __stdcall TraceDirect3DDevice8::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer8* pStreamData, UINT Stride) {
    return (m_pIDirect3DDevice8->SetStreamSource(StreamNumber, pStreamData, Stride));
}

HRESULT __stdcall TraceDirect3DDevice8::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer8** ppStreamData, UINT* pStride) {
    return (m_pIDirect3DDevice8->GetStreamSource(StreamNumber, ppStreamData, pStride));
}

HRESULT __stdcall TraceDirect3DDevice8::SetIndices(IDirect3DIndexBuffer8* pIndexData, UINT BaseVertexIndex) {
    return (m_pIDirect3DDevice8->SetIndices(pIndexData, BaseVertexIndex));
}

HRESULT __stdcall TraceDirect3DDevice8::GetIndices(IDirect3DIndexBuffer8** ppIndexData, UINT* pBaseVertexIndex) {
    return (m_pIDirect3DDevice8->GetIndices(ppIndexData, pBaseVertexIndex));
}

HRESULT __stdcall TraceDirect3DDevice8::CreatePixelShader( CONST DWORD* pFunction,DWORD* pHandle)
{   return (m_pIDirect3DDevice8->CreatePixelShader( pFunction, pHandle));}

HRESULT __stdcall TraceDirect3DDevice8::SetPixelShader(DWORD Handle) {
    return (m_pIDirect3DDevice8->SetPixelShader(Handle));
}

HRESULT __stdcall TraceDirect3DDevice8::GetPixelShader(DWORD* pHandle) {
    return (m_pIDirect3DDevice8->GetPixelShader(pHandle));
}

HRESULT __stdcall TraceDirect3DDevice8::DeletePixelShader(DWORD Handle) {
    return (m_pIDirect3DDevice8->DeletePixelShader(Handle));
}

HRESULT __stdcall TraceDirect3DDevice8::SetPixelShaderConstant(DWORD Register, CONST void* pConstantData, DWORD ConstantCount) {
    return (m_pIDirect3DDevice8->SetPixelShaderConstant(Register, pConstantData, ConstantCount));
}

HRESULT __stdcall TraceDirect3DDevice8::GetPixelShaderConstant(DWORD Register, void* pConstantData, DWORD ConstantCount) {
    return (m_pIDirect3DDevice8->GetPixelShaderConstant(Register, pConstantData, ConstantCount));
}

HRESULT __stdcall TraceDirect3DDevice8::GetPixelShaderFunction(DWORD Handle, void* pData, DWORD* pSizeOfData) {
    return (m_pIDirect3DDevice8->GetPixelShaderFunction(Handle, pData, pSizeOfData));
}

HRESULT __stdcall TraceDirect3DDevice8::DrawRectPatch( UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{   return (m_pIDirect3DDevice8->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo) );}

HRESULT __stdcall TraceDirect3DDevice8::DrawTriPatch( UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{   return (m_pIDirect3DDevice8->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo) );}

HRESULT __stdcall TraceDirect3DDevice8::DeletePatch(UINT Handle) {
    return (m_pIDirect3DDevice8->DeletePatch(Handle));
}

// This is our test function
void TraceDirect3DDevice8::ShowWeAreHere(void) {
    D3DRECT rec = { 1, 1, 50, 50 };
    m_pIDirect3DDevice8->Clear(1, &rec, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255,255,255,0), 0, 0);
}
