#include "StdAfx.h"

TraceDirect3DDevice9::TraceDirect3DDevice9(IDirect3DDevice9* pOriginal) {
    m_pIDirect3DDevice9 = pOriginal; // store the pointer to original object
}

TraceDirect3DDevice9::~TraceDirect3DDevice9(void) {
}

HRESULT TraceDirect3DDevice9::QueryInterface(REFIID riid, void** ppvObj) {
    // check if original dll can provide interface. then send *our* address
    *ppvObj = NULL;

    HRESULT hRes = m_pIDirect3DDevice9->QueryInterface(riid, ppvObj);

    if (hRes == NOERROR) {
        *ppvObj = this;
    }

    return hRes;
}

ULONG TraceDirect3DDevice9::AddRef(void) {
    return (m_pIDirect3DDevice9->AddRef());
}

ULONG TraceDirect3DDevice9::Release(void) {
    // ATTENTION: This is a booby-trap ! Watch out !
    // If we create our own sprites, surfaces, etc. (thus increasing the ref counter
    // by external action), we need to delete that objects before calling the original
    // Release function	

    // global var
    extern TraceDirect3DDevice9* gl_pmyIDirect3DDevice9;

    // release/delete own objects
    // .....

    // Calling original function now
    ULONG count = m_pIDirect3DDevice9->Release();

    if (count == 0) {
        // now, the Original Object has deleted itself, so do we here
        gl_pmyIDirect3DDevice9 = NULL;
        delete (this); // destructor will be called automatically
    }

    return (count);
}

HRESULT TraceDirect3DDevice9::TestCooperativeLevel(void) {
    return (m_pIDirect3DDevice9->TestCooperativeLevel());
}

UINT TraceDirect3DDevice9::GetAvailableTextureMem(void) {
    return (m_pIDirect3DDevice9->GetAvailableTextureMem());
}

HRESULT TraceDirect3DDevice9::EvictManagedResources(void) {
    return (m_pIDirect3DDevice9->EvictManagedResources());
}

HRESULT TraceDirect3DDevice9::GetDirect3D(IDirect3D9** ppD3D9) {
    return (m_pIDirect3DDevice9->GetDirect3D(ppD3D9));
}

HRESULT TraceDirect3DDevice9::GetDeviceCaps(D3DCAPS9* pCaps) {
    return (m_pIDirect3DDevice9->GetDeviceCaps(pCaps));
}

HRESULT TraceDirect3DDevice9::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode) {
    return (m_pIDirect3DDevice9->GetDisplayMode(iSwapChain, pMode));
}

HRESULT TraceDirect3DDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters) {
    return (m_pIDirect3DDevice9->GetCreationParameters(pParameters));
}

HRESULT TraceDirect3DDevice9::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap) {
    return (m_pIDirect3DDevice9->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap));
}

void TraceDirect3DDevice9::SetCursorPosition(int X, int Y, DWORD Flags) {
    return (m_pIDirect3DDevice9->SetCursorPosition(X, Y, Flags));
}

BOOL TraceDirect3DDevice9::ShowCursor(BOOL bShow) {
    return (m_pIDirect3DDevice9->ShowCursor(bShow));
}

HRESULT TraceDirect3DDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain) {
    return (m_pIDirect3DDevice9->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain));
}

HRESULT TraceDirect3DDevice9::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain) {
    // global var
    extern TraceDirect3DSwapChain9* gl_pmyIDirect3DSwapChain9;

    // We only cover swapchain 0
    if (iSwapChain != 0)
        return (m_pIDirect3DDevice9->GetSwapChain(iSwapChain, pSwapChain));

    if (gl_pmyIDirect3DSwapChain9) {
        *pSwapChain = gl_pmyIDirect3DSwapChain9;
        return (D3D_OK);
    }

    // we intercept this call and provide our own "fake" SwapChain Object
    IDirect3DSwapChain9* pOriginal = NULL;
    HRESULT hres = m_pIDirect3DDevice9->GetSwapChain(iSwapChain, &pOriginal);

    // Create our own SwapChain object and store it in global pointer
    // note: the object will delete itself once Ref count is zero (similar to COM objects)
    gl_pmyIDirect3DSwapChain9
            = new TraceDirect3DSwapChain9(pOriginal, m_pIDirect3DDevice9);

    // store our pointer (the fake one) for returning it to the calling progam
    *pSwapChain = gl_pmyIDirect3DSwapChain9;

    return (hres);
}

UINT TraceDirect3DDevice9::GetNumberOfSwapChains(void) {
    return (m_pIDirect3DDevice9->GetNumberOfSwapChains());
}

HRESULT TraceDirect3DDevice9::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) {
    return (m_pIDirect3DDevice9->Reset(pPresentationParameters));
}

HRESULT TraceDirect3DDevice9::Present(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion)
{
    // we may want to draw own things here before flipping surfaces
    // ... draw own stuff ...
    this->ShowWeAreHere();

    // call original routine
    HRESULT hres = m_pIDirect3DDevice9->Present( pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);

    return (hres);
}

HRESULT TraceDirect3DDevice9::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer) {
    return (m_pIDirect3DDevice9->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer));
}

HRESULT TraceDirect3DDevice9::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus) {
    return (m_pIDirect3DDevice9->GetRasterStatus(iSwapChain, pRasterStatus));
}

HRESULT TraceDirect3DDevice9::SetDialogBoxMode(BOOL bEnableDialogs) {
    return (m_pIDirect3DDevice9->SetDialogBoxMode(bEnableDialogs));
}

void TraceDirect3DDevice9::SetGammaRamp(UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp)
{
    return(m_pIDirect3DDevice9->SetGammaRamp(iSwapChain,Flags,pRamp));
}

void TraceDirect3DDevice9::GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp) {
    return (m_pIDirect3DDevice9->GetGammaRamp(iSwapChain, pRamp));
}

HRESULT TraceDirect3DDevice9::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) {
    return (m_pIDirect3DDevice9->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle));
}

HRESULT TraceDirect3DDevice9::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle) {
    return (m_pIDirect3DDevice9->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle));
}

HRESULT TraceDirect3DDevice9::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle) {
    return (m_pIDirect3DDevice9->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle));
}

HRESULT TraceDirect3DDevice9::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle) {
    return (m_pIDirect3DDevice9->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle));
}

HRESULT TraceDirect3DDevice9::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle) {
    return (m_pIDirect3DDevice9->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle));
}

HRESULT TraceDirect3DDevice9::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
    return (m_pIDirect3DDevice9->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle));
}

HRESULT TraceDirect3DDevice9::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
    return (m_pIDirect3DDevice9->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle));
}

HRESULT TraceDirect3DDevice9::UpdateSurface(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint)
{
    return(m_pIDirect3DDevice9->UpdateSurface(pSourceSurface,pSourceRect,pDestinationSurface,pDestPoint));
}

HRESULT TraceDirect3DDevice9::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture) {
    return (m_pIDirect3DDevice9->UpdateTexture(pSourceTexture, pDestinationTexture));
}

HRESULT TraceDirect3DDevice9::GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface) {
    return (m_pIDirect3DDevice9->GetRenderTargetData(pRenderTarget, pDestSurface));
}

HRESULT TraceDirect3DDevice9::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface) {
    return (m_pIDirect3DDevice9->GetFrontBufferData(iSwapChain, pDestSurface));
}

HRESULT TraceDirect3DDevice9::StretchRect(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter)
{
    return(m_pIDirect3DDevice9->StretchRect(pSourceSurface,pSourceRect,pDestSurface,pDestRect,Filter));
}

HRESULT TraceDirect3DDevice9::ColorFill(IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color)
{
    return(m_pIDirect3DDevice9->ColorFill(pSurface,pRect,color));
}

HRESULT TraceDirect3DDevice9::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
    return (m_pIDirect3DDevice9->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle));
}

HRESULT TraceDirect3DDevice9::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget) {
    return (m_pIDirect3DDevice9->SetRenderTarget(RenderTargetIndex, pRenderTarget));
}

HRESULT TraceDirect3DDevice9::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget) {
    return (m_pIDirect3DDevice9->GetRenderTarget(RenderTargetIndex, ppRenderTarget));
}

HRESULT TraceDirect3DDevice9::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) {
    return (m_pIDirect3DDevice9->SetDepthStencilSurface(pNewZStencil));
}

HRESULT TraceDirect3DDevice9::GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface) {
    return (m_pIDirect3DDevice9->GetDepthStencilSurface(ppZStencilSurface));
}

HRESULT TraceDirect3DDevice9::BeginScene(void) {
    return (m_pIDirect3DDevice9->BeginScene());
}

HRESULT TraceDirect3DDevice9::EndScene(void) {
    return (m_pIDirect3DDevice9->EndScene());
}

HRESULT TraceDirect3DDevice9::Clear(DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil)
{
    return(m_pIDirect3DDevice9->Clear(Count,pRects,Flags,Color,Z,Stencil));
}

HRESULT TraceDirect3DDevice9::SetTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix)
{
    return(m_pIDirect3DDevice9->SetTransform(State,pMatrix));
}

HRESULT TraceDirect3DDevice9::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix) {
    return (m_pIDirect3DDevice9->GetTransform(State, pMatrix));
}

HRESULT TraceDirect3DDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix)
{
    return(m_pIDirect3DDevice9->MultiplyTransform(State,pMatrix));
}

HRESULT TraceDirect3DDevice9::SetViewport(CONST D3DVIEWPORT9* pViewport)
{
    return(m_pIDirect3DDevice9->SetViewport(pViewport));
}

HRESULT TraceDirect3DDevice9::GetViewport(D3DVIEWPORT9* pViewport) {
    return (m_pIDirect3DDevice9->GetViewport(pViewport));
}

HRESULT TraceDirect3DDevice9::SetMaterial(CONST D3DMATERIAL9* pMaterial)
{
    return(m_pIDirect3DDevice9->SetMaterial(pMaterial));
}

HRESULT TraceDirect3DDevice9::GetMaterial(D3DMATERIAL9* pMaterial) {
    return (m_pIDirect3DDevice9->GetMaterial(pMaterial));
}

HRESULT TraceDirect3DDevice9::SetLight(DWORD Index,CONST D3DLIGHT9* pLight)
{
    return(m_pIDirect3DDevice9->SetLight(Index,pLight));
}

HRESULT TraceDirect3DDevice9::GetLight(DWORD Index, D3DLIGHT9* pLight) {
    return (m_pIDirect3DDevice9->GetLight(Index, pLight));
}

HRESULT TraceDirect3DDevice9::LightEnable(DWORD Index, BOOL Enable) {
    return (m_pIDirect3DDevice9->LightEnable(Index, Enable));
}

HRESULT TraceDirect3DDevice9::GetLightEnable(DWORD Index, BOOL* pEnable) {
    return (m_pIDirect3DDevice9->GetLightEnable(Index, pEnable));
}

HRESULT TraceDirect3DDevice9::SetClipPlane(DWORD Index, CONST float* pPlane) {
    return (m_pIDirect3DDevice9->SetClipPlane(Index, pPlane));
}

HRESULT TraceDirect3DDevice9::GetClipPlane(DWORD Index, float* pPlane) {
    return (m_pIDirect3DDevice9->GetClipPlane(Index, pPlane));
}

HRESULT TraceDirect3DDevice9::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) {
    return (m_pIDirect3DDevice9->SetRenderState(State, Value));
}

HRESULT TraceDirect3DDevice9::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) {
    return (m_pIDirect3DDevice9->GetRenderState(State, pValue));
}

HRESULT TraceDirect3DDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB) {
    return (m_pIDirect3DDevice9->CreateStateBlock(Type, ppSB));
}

HRESULT TraceDirect3DDevice9::BeginStateBlock(void) {
    return (m_pIDirect3DDevice9->BeginStateBlock());
}

HRESULT TraceDirect3DDevice9::EndStateBlock(IDirect3DStateBlock9** ppSB) {
    return (m_pIDirect3DDevice9->EndStateBlock(ppSB));
}

HRESULT TraceDirect3DDevice9::SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus)
{
    return(m_pIDirect3DDevice9->SetClipStatus(pClipStatus));
}

HRESULT TraceDirect3DDevice9::GetClipStatus(D3DCLIPSTATUS9* pClipStatus) {
    return (m_pIDirect3DDevice9->GetClipStatus(pClipStatus));
}

HRESULT TraceDirect3DDevice9::GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture) {
    return (m_pIDirect3DDevice9->GetTexture(Stage, ppTexture));
}

HRESULT TraceDirect3DDevice9::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture) {
    return (m_pIDirect3DDevice9->SetTexture(Stage, pTexture));
}

HRESULT TraceDirect3DDevice9::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue) {
    return (m_pIDirect3DDevice9->GetTextureStageState(Stage, Type, pValue));
}

HRESULT TraceDirect3DDevice9::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) {
    return (m_pIDirect3DDevice9->SetTextureStageState(Stage, Type, Value));
}

HRESULT TraceDirect3DDevice9::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue) {
    return (m_pIDirect3DDevice9->GetSamplerState(Sampler, Type, pValue));
}

HRESULT TraceDirect3DDevice9::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) {
    return (m_pIDirect3DDevice9->SetSamplerState(Sampler, Type, Value));
}

HRESULT TraceDirect3DDevice9::ValidateDevice(DWORD* pNumPasses) {
    return (m_pIDirect3DDevice9->ValidateDevice(pNumPasses));
}

HRESULT TraceDirect3DDevice9::SetPaletteEntries(UINT PaletteNumber,CONST PALETTEENTRY* pEntries)
{
    return(m_pIDirect3DDevice9->SetPaletteEntries(PaletteNumber, pEntries));
}

HRESULT TraceDirect3DDevice9::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) {
    return (m_pIDirect3DDevice9->GetPaletteEntries(PaletteNumber, pEntries));
}

HRESULT TraceDirect3DDevice9::SetCurrentTexturePalette(UINT PaletteNumber) {
    return (m_pIDirect3DDevice9->SetCurrentTexturePalette(PaletteNumber));
}

HRESULT TraceDirect3DDevice9::GetCurrentTexturePalette(UINT *PaletteNumber) {
    return (m_pIDirect3DDevice9->GetCurrentTexturePalette(PaletteNumber));
}

HRESULT TraceDirect3DDevice9::SetScissorRect(CONST RECT* pRect)
{
    return(m_pIDirect3DDevice9->SetScissorRect( pRect));
}

HRESULT TraceDirect3DDevice9::GetScissorRect(RECT* pRect) {
    return (m_pIDirect3DDevice9->GetScissorRect(pRect));
}

HRESULT TraceDirect3DDevice9::SetSoftwareVertexProcessing(BOOL bSoftware) {
    return (m_pIDirect3DDevice9->SetSoftwareVertexProcessing(bSoftware));
}

BOOL TraceDirect3DDevice9::GetSoftwareVertexProcessing(void) {
    return (m_pIDirect3DDevice9->GetSoftwareVertexProcessing());
}

HRESULT TraceDirect3DDevice9::SetNPatchMode(float nSegments) {
    return (m_pIDirect3DDevice9->SetNPatchMode(nSegments));
}

float TraceDirect3DDevice9::GetNPatchMode(void) {
    return (m_pIDirect3DDevice9->GetNPatchMode());
}

HRESULT TraceDirect3DDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) {
    return (m_pIDirect3DDevice9->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount));
}

HRESULT TraceDirect3DDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) {
    return (m_pIDirect3DDevice9->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount));
}

HRESULT TraceDirect3DDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
    return (m_pIDirect3DDevice9->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride));
}

HRESULT TraceDirect3DDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
    return (m_pIDirect3DDevice9->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride));
}

HRESULT TraceDirect3DDevice9::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags) {
    return (m_pIDirect3DDevice9->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags));
}

HRESULT TraceDirect3DDevice9::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl)
{
    return(m_pIDirect3DDevice9->CreateVertexDeclaration( pVertexElements,ppDecl));
}

HRESULT TraceDirect3DDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl) {
    return (m_pIDirect3DDevice9->SetVertexDeclaration(pDecl));
}

HRESULT TraceDirect3DDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl) {
    return (m_pIDirect3DDevice9->GetVertexDeclaration(ppDecl));
}

HRESULT TraceDirect3DDevice9::SetFVF(DWORD FVF) {
    return (m_pIDirect3DDevice9->SetFVF(FVF));
}

HRESULT TraceDirect3DDevice9::GetFVF(DWORD* pFVF) {
    return (m_pIDirect3DDevice9->GetFVF(pFVF));
}

HRESULT TraceDirect3DDevice9::CreateVertexShader(CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader)
{
    return(m_pIDirect3DDevice9->CreateVertexShader(pFunction,ppShader));
}

HRESULT TraceDirect3DDevice9::SetVertexShader(IDirect3DVertexShader9* pShader) {
    return (m_pIDirect3DDevice9->SetVertexShader(pShader));
}

HRESULT TraceDirect3DDevice9::GetVertexShader(IDirect3DVertexShader9** ppShader) {
    return (m_pIDirect3DDevice9->GetVertexShader(ppShader));
}

HRESULT TraceDirect3DDevice9::SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
    return (m_pIDirect3DDevice9->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount));
}

HRESULT TraceDirect3DDevice9::GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) {
    return (m_pIDirect3DDevice9->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount));
}

HRESULT TraceDirect3DDevice9::SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) {
    return (m_pIDirect3DDevice9->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount));
}

HRESULT TraceDirect3DDevice9::GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) {
    return (m_pIDirect3DDevice9->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount));
}

HRESULT TraceDirect3DDevice9::SetVertexShaderConstantB(UINT StartRegister,CONST BOOL* pConstantData,UINT BoolCount)
{
    return(m_pIDirect3DDevice9->SetVertexShaderConstantB(StartRegister,pConstantData,BoolCount));
}

HRESULT TraceDirect3DDevice9::GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) {
    return (m_pIDirect3DDevice9->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount));
}

HRESULT TraceDirect3DDevice9::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride) {
    return (m_pIDirect3DDevice9->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride));
}

HRESULT TraceDirect3DDevice9::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* OffsetInBytes, UINT* pStride) {
    return (m_pIDirect3DDevice9->GetStreamSource(StreamNumber, ppStreamData, OffsetInBytes, pStride));
}

HRESULT TraceDirect3DDevice9::SetStreamSourceFreq(UINT StreamNumber, UINT Divider) {
    return (m_pIDirect3DDevice9->SetStreamSourceFreq(StreamNumber, Divider));
}

HRESULT TraceDirect3DDevice9::GetStreamSourceFreq(UINT StreamNumber, UINT* Divider) {
    return (m_pIDirect3DDevice9->GetStreamSourceFreq(StreamNumber, Divider));
}

HRESULT TraceDirect3DDevice9::SetIndices(IDirect3DIndexBuffer9* pIndexData) {
    return (m_pIDirect3DDevice9->SetIndices(pIndexData));
}

HRESULT TraceDirect3DDevice9::GetIndices(IDirect3DIndexBuffer9** ppIndexData) {
    return (m_pIDirect3DDevice9->GetIndices(ppIndexData));
}

HRESULT TraceDirect3DDevice9::CreatePixelShader(CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader)
{
    return(m_pIDirect3DDevice9->CreatePixelShader(pFunction,ppShader));
}

HRESULT TraceDirect3DDevice9::SetPixelShader(IDirect3DPixelShader9* pShader) {
    return (m_pIDirect3DDevice9->SetPixelShader(pShader));
}

HRESULT TraceDirect3DDevice9::GetPixelShader(IDirect3DPixelShader9** ppShader) {
    return (m_pIDirect3DDevice9->GetPixelShader(ppShader));
}

HRESULT TraceDirect3DDevice9::SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
    return (m_pIDirect3DDevice9->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount));
}

HRESULT TraceDirect3DDevice9::GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) {
    return (m_pIDirect3DDevice9->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount));
}

HRESULT TraceDirect3DDevice9::SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) {
    return (m_pIDirect3DDevice9->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount));
}

HRESULT TraceDirect3DDevice9::GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) {
    return (m_pIDirect3DDevice9->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount));
}

HRESULT TraceDirect3DDevice9::SetPixelShaderConstantB(UINT StartRegister,CONST BOOL* pConstantData,UINT BoolCount)
{
    return(m_pIDirect3DDevice9->SetPixelShaderConstantB(StartRegister,pConstantData,BoolCount));
}

HRESULT TraceDirect3DDevice9::GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) {
    return (m_pIDirect3DDevice9->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount));
}

HRESULT TraceDirect3DDevice9::DrawRectPatch(UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{
    return(m_pIDirect3DDevice9->DrawRectPatch(Handle,pNumSegs, pRectPatchInfo));
}

HRESULT TraceDirect3DDevice9::DrawTriPatch(UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{
    return(m_pIDirect3DDevice9->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo));
}

HRESULT TraceDirect3DDevice9::DeletePatch(UINT Handle) {
    return (m_pIDirect3DDevice9->DeletePatch(Handle));
}

HRESULT TraceDirect3DDevice9::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery) {
    return (m_pIDirect3DDevice9->CreateQuery(Type, ppQuery));
}

// This is our test function
void TraceDirect3DDevice9::ShowWeAreHere(void) {
    D3DRECT rec = { 1, 1, 50, 50 };
    m_pIDirect3DDevice9->Clear(1, &rec, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 255, 255, 0), 0, 0);
}

