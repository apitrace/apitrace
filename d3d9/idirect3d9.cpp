#include "StdAfx.h"

TraceDirect3D9::TraceDirect3D9(IDirect3D9 *pOriginal) {
    m_pIDirect3D9 = pOriginal;
}

TraceDirect3D9::~TraceDirect3D9(void) {
}

HRESULT __stdcall TraceDirect3D9::QueryInterface(REFIID riid, void** ppvObj) {
    *ppvObj = NULL;

    // call this to increase AddRef at original object
    // and to check if such an interface is there

    HRESULT hRes = m_pIDirect3D9->QueryInterface(riid, ppvObj);

    if (hRes == NOERROR) // if OK, send our "fake" address
    {
        *ppvObj = this;
    }

    return hRes;
}

ULONG __stdcall TraceDirect3D9::AddRef(void) {
    return (m_pIDirect3D9->AddRef());
}

ULONG __stdcall TraceDirect3D9::Release(void) {
    extern TraceDirect3D9* gl_pmyIDirect3D9;

    // call original routine
    ULONG count = m_pIDirect3D9->Release();

    // in case no further Ref is there, the Original Object has deleted itself
    // so do we here
    if (count == 0) {
        gl_pmyIDirect3D9 = NULL;
        delete (this);
    }

    return (count);
}

HRESULT __stdcall TraceDirect3D9::RegisterSoftwareDevice(void* pInitializeFunction) {
    return (m_pIDirect3D9->RegisterSoftwareDevice(pInitializeFunction));
}

UINT __stdcall TraceDirect3D9::GetAdapterCount(void) {
    return (m_pIDirect3D9->GetAdapterCount());
}

HRESULT __stdcall TraceDirect3D9::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier) {
    return (m_pIDirect3D9->GetAdapterIdentifier(Adapter, Flags, pIdentifier));
}

UINT __stdcall TraceDirect3D9::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format) {
    return (m_pIDirect3D9->GetAdapterModeCount(Adapter, Format));
}

HRESULT __stdcall TraceDirect3D9::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode) {
    return (m_pIDirect3D9->EnumAdapterModes(Adapter, Format, Mode, pMode));
}

HRESULT __stdcall TraceDirect3D9::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode) {
    return (m_pIDirect3D9->GetAdapterDisplayMode(Adapter, pMode));
}

HRESULT __stdcall TraceDirect3D9::CheckDeviceType(UINT iAdapter, D3DDEVTYPE DevType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed) {
    return (m_pIDirect3D9->CheckDeviceType(iAdapter, DevType, DisplayFormat, BackBufferFormat, bWindowed));
}

HRESULT __stdcall TraceDirect3D9::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) {
    return (m_pIDirect3D9->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat));
}

HRESULT __stdcall TraceDirect3D9::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) {
    return (m_pIDirect3D9->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels));
}

HRESULT __stdcall TraceDirect3D9::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) {
    return (m_pIDirect3D9->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat));
}

HRESULT __stdcall TraceDirect3D9::CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) {
    return (m_pIDirect3D9->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat));
}

HRESULT __stdcall TraceDirect3D9::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps) {
    return (m_pIDirect3D9->GetDeviceCaps(Adapter, DeviceType, pCaps));
}

HMONITOR __stdcall TraceDirect3D9::GetAdapterMonitor(UINT Adapter) {
    return (m_pIDirect3D9->GetAdapterMonitor(Adapter));
}

HRESULT __stdcall TraceDirect3D9::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface) {
    // global var
    extern TraceDirect3DDevice9* gl_pmyIDirect3DDevice9;

    // we intercept this call and provide our own "fake" Device Object
    HRESULT hres = m_pIDirect3D9->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

    // Create our own Device object and store it in global pointer
    // note: the object will delete itself once Ref count is zero (similar to COM objects)
    gl_pmyIDirect3DDevice9 = new TraceDirect3DDevice9(*ppReturnedDeviceInterface);

    // store our pointer (the fake one) for returning it to the calling progam
    *ppReturnedDeviceInterface = gl_pmyIDirect3DDevice9;

    return (hres);
}

