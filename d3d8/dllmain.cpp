// proxydll.cpp
#include "stdafx.h"

// global variables
#pragma data_seg (".d3d8_shared")
static HINSTANCE gl_hOriginalDll;
static HINSTANCE gl_hThisInstance;
#pragma data_seg ()

static void InitInstance(HANDLE hModule) {
    OutputDebugString(_T("PROXYDLL: InitInstance called.\r\n"));

    // Initialisation
    gl_hOriginalDll = NULL;
    gl_hThisInstance = NULL;

    // Storing Instance handle into global var
    gl_hThisInstance = (HINSTANCE) hModule;
}

static void LoadOriginalDll(void) {
    TCHAR buffer[MAX_PATH];

    // Getting path to system dir and to d3d8.dll
    ::GetSystemDirectory(buffer, MAX_PATH);

    // Append dll name
    _tcscat(buffer, _T("\\d3d8.dll"));

    // try to load the system's d3d8.dll, if pointer empty
    if (!gl_hOriginalDll)
        gl_hOriginalDll = ::LoadLibrary(buffer);

    // Debug
    if (!gl_hOriginalDll) {
        OutputDebugString(_T("PROXYDLL: Original d3d8.dll not loaded ERROR ****\r\n"));
        ::ExitProcess(0); // exit the hard way
    }
}

static void ExitInstance() {
    OutputDebugString(_T("PROXYDLL: ExitInstance called.\r\n"));

    // Release the system's d3d8.dll
    if (gl_hOriginalDll) {
        ::FreeLibrary(gl_hOriginalDll);
        gl_hOriginalDll = NULL;
    }
}

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

// Exported function (faking d3d8.dll's one-and-only export)
IDirect3D8* WINAPI Direct3DCreate8(UINT SDKVersion)
{
    if (!gl_hOriginalDll) LoadOriginalDll(); // looking for the "right d3d8.dll"

    // Hooking IDirect3D Object from Original Library
    typedef IDirect3D8 *(WINAPI* D3D8_Type)(UINT SDKVersion);
    D3D8_Type D3DCreate8_fn = (D3D8_Type) GetProcAddress( gl_hOriginalDll, "Direct3DCreate8");

    // Debug
    if (!D3DCreate8_fn)
    {
        OutputDebugString(_T("PROXYDLL: Pointer to original D3DCreate8 function not received ERROR ****\r\n"));
        ::ExitProcess(0); // exit the hard way
    }

    // Request pointer from Original Dll. 
    IDirect3D8 *pIDirect3D8_orig = D3DCreate8_fn(SDKVersion);

    // Create my IDirect3D8 object and store pointer to original object there.
    // note: the object will delete itself once Ref count is zero (similar to COM objects)
    return new TraceDirect3D8(pIDirect3D8_orig);
}
