// proxydll.cpp
#include "stdafx.h"
#include "dllmain.hpp"

// global variables
#pragma data_seg (".d3d9_shared")
TraceDirect3DSwapChain9* gl_pmyIDirect3DSwapChain9;
TraceDirect3DDevice9* gl_pmyIDirect3DDevice9;
TraceDirect3D9* gl_pmyIDirect3D9;
HINSTANCE gl_hOriginalDll;
HINSTANCE gl_hThisInstance;
#pragma data_seg ()

BOOL APIENTRY DllMain( HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    // to avoid compiler lvl4 warnings 
    LPVOID lpDummy = lpReserved;
    lpDummy = NULL;

    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH: InitInstance(hModule); break;
        case DLL_PROCESS_DETACH: ExitInstance(); break;

        case DLL_THREAD_ATTACH: break;
        case DLL_THREAD_DETACH: break;
    }
    return TRUE;
}

// Exported function (faking d3d9.dll's one-and-only export)
IDirect3D9* WINAPI Direct3DCreate9(UINT SDKVersion)
{
    if (!gl_hOriginalDll) LoadOriginalDll(); // looking for the "right d3d9.dll"

    // Hooking IDirect3D Object from Original Library
    typedef IDirect3D9 *(WINAPI* D3D9_Type)(UINT SDKVersion);
    D3D9_Type D3DCreate9_fn = (D3D9_Type) GetProcAddress( gl_hOriginalDll, "Direct3DCreate9");

    // Debug
    if (!D3DCreate9_fn)
    {
        OutputDebugString(_T("PROXYDLL: Pointer to original D3DCreate9 function not received ERROR ****\r\n"));
        ::ExitProcess(0); // exit the hard way
    }

    // Request pointer from Original Dll. 
    IDirect3D9 *pIDirect3D9_orig = D3DCreate9_fn(SDKVersion);

    // Create my IDirect3D8 object and store pointer to original object there.
    // note: the object will delete itself once Ref count is zero (similar to COM objects)
    gl_pmyIDirect3D9 = new TraceDirect3D9(pIDirect3D9_orig);

    // Return pointer to hooking Object instead of "real one"
    return (gl_pmyIDirect3D9);
}

void InitInstance(HANDLE hModule) {
    OutputDebugString(_T("PROXYDLL: InitInstance called.\r\n"));

    // Initialisation
    gl_hOriginalDll = NULL;
    gl_hThisInstance = NULL;
    gl_pmyIDirect3D9 = NULL;
    gl_pmyIDirect3DDevice9 = NULL;
    gl_pmyIDirect3DSwapChain9 = NULL;

    // Storing Instance handle into global var
    gl_hThisInstance = (HINSTANCE) hModule;
}

void LoadOriginalDll(void) {
    TCHAR buffer[MAX_PATH];

    // Getting path to system dir and to d3d8.dll
    ::GetSystemDirectory(buffer, MAX_PATH);

    // Append dll name
    _tcscat(buffer, _T("\\d3d9.dll"));

    // try to load the system's d3d9.dll, if pointer empty
    if (!gl_hOriginalDll)
        gl_hOriginalDll = ::LoadLibrary(buffer);

    // Debug
    if (!gl_hOriginalDll) {
        OutputDebugString(_T("PROXYDLL: Original d3d9.dll not loaded ERROR ****\r\n"));
        ::ExitProcess(0); // exit the hard way
    }
}

void ExitInstance() {
    OutputDebugString(_T("PROXYDLL: ExitInstance called.\r\n"));

    // Release the system's d3d9.dll
    if (gl_hOriginalDll) {
        ::FreeLibrary(gl_hOriginalDll);
        gl_hOriginalDll = NULL;
    }
}

