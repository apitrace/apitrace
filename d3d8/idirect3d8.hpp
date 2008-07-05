// TraceDirect3D8.h
#pragma once

class TraceDirect3D8 : public IDirect3D8    
{
public:

	TraceDirect3D8(IDirect3D8 *pOriginal);
	virtual ~TraceDirect3D8();

	// START: The original DX8.1a function definitions
	HRESULT  __stdcall QueryInterface(REFIID riid, void** ppvObj);
	ULONG    __stdcall AddRef(void);
	ULONG    __stdcall Release(void);
	HRESULT  __stdcall RegisterSoftwareDevice(void* pInitializeFunction);
	UINT     __stdcall GetAdapterCount(void);
	HRESULT  __stdcall GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER8* pIdentifier);
	UINT     __stdcall GetAdapterModeCount(UINT Adapter);
	HRESULT  __stdcall EnumAdapterModes(UINT Adapter,UINT Mode,D3DDISPLAYMODE* pMode);
	HRESULT  __stdcall GetAdapterDisplayMode( UINT Adapter,D3DDISPLAYMODE* pMode);
	HRESULT  __stdcall CheckDeviceType(UINT Adapter,D3DDEVTYPE CheckType,D3DFORMAT DisplayFormat,D3DFORMAT BackBufferFormat,BOOL Windowed);
	HRESULT  __stdcall CheckDeviceFormat(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat);
	HRESULT  __stdcall CheckDeviceMultiSampleType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType);
	HRESULT  __stdcall CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat);
	HRESULT  __stdcall GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS8* pCaps);
	HMONITOR __stdcall GetAdapterMonitor(UINT Adapter);
	HRESULT  __stdcall CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice8** ppReturnedDeviceInterface);
	// END: The original DX8,1a functions definitions

private:
	
	IDirect3D8 * m_pIDirect3D8;
};

