//Copyright (c) 2007-2008, Marton Anka
//
//Permission is hereby granted, free of charge, to any person obtaining a 
//copy of this software and associated documentation files (the "Software"), 
//to deal in the Software without restriction, including without limitation 
//the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//and/or sell copies of the Software, and to permit persons to whom the 
//Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included 
//in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
//OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
//THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
//FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
//IN THE SOFTWARE.

#include "stdafx.h"
#include "mhook-lib/mhook.h"

//=========================================================================
// Define _NtOpenProcess so we can dynamically bind to the function
//
typedef struct _CLIENT_ID {
	DWORD_PTR UniqueProcess;
	DWORD_PTR UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef ULONG (WINAPI* _NtOpenProcess)(OUT PHANDLE ProcessHandle, 
	     IN ACCESS_MASK AccessMask, IN PVOID ObjectAttributes, 
		 IN PCLIENT_ID ClientId ); 

//=========================================================================
// Define _SelectObject so we can dynamically bind to the function
typedef HGDIOBJ (WINAPI* _SelectObject)(HDC hdc, HGDIOBJ hgdiobj); 

//=========================================================================
// Define _getaddrinfo so we can dynamically bind to the function
typedef int (WSAAPI* _getaddrinfo)(const char* nodename, const char* servname, const struct addrinfo* hints, struct addrinfo** res);

//=========================================================================
// Define _HeapAlloc so we can dynamically bind to the function
typedef LPVOID (WINAPI *_HeapAlloc)(HANDLE, DWORD, SIZE_T);

//=========================================================================
// Define _NtClose so we can dynamically bind to the function
typedef ULONG (WINAPI* _NtClose)(IN HANDLE Handle);

//=========================================================================
// Get the current (original) address to the functions to be hooked
//
_NtOpenProcess TrueNtOpenProcess = (_NtOpenProcess)
	GetProcAddress(GetModuleHandle(L"ntdll"), "NtOpenProcess");

_SelectObject TrueSelectObject = (_SelectObject)
	GetProcAddress(GetModuleHandle(L"gdi32"), "SelectObject");

_getaddrinfo Truegetaddrinfo = (_getaddrinfo)GetProcAddress(GetModuleHandle(L"ws2_32"), "getaddrinfo");

_HeapAlloc TrueHeapAlloc = (_HeapAlloc)GetProcAddress(GetModuleHandle(L"kernel32"), "HeapAlloc");

_NtClose TrueNtClose = (_NtClose)GetProcAddress(GetModuleHandle(L"ntdll"), "NtClose");

//=========================================================================
// This is the function that will replace NtOpenProcess once the hook 
// is in place
//
ULONG WINAPI HookNtOpenProcess(OUT PHANDLE ProcessHandle, 
							   IN ACCESS_MASK AccessMask, 
							   IN PVOID ObjectAttributes, 
							   IN PCLIENT_ID ClientId)
{
	printf("***** Call to open process %d\n", ClientId->UniqueProcess);
	return TrueNtOpenProcess(ProcessHandle, AccessMask, 
		ObjectAttributes, ClientId);
}

//=========================================================================
// This is the function that will replace SelectObject once the hook 
// is in place
//
HGDIOBJ WINAPI HookSelectobject(HDC hdc, HGDIOBJ hgdiobj)
{
	printf("***** Call to SelectObject(0x%p, 0x%p)\n", hdc, hgdiobj);
	return TrueSelectObject(hdc, hgdiobj);
}

//=========================================================================
// This is the function that will replace SelectObject once the hook 
// is in place
//
int WSAAPI Hookgetaddrinfo(const char* nodename, const char* servname, const struct addrinfo* hints, struct addrinfo** res)
{
	printf("***** Call to getaddrinfo(0x%p, 0x%p, 0x%p, 0x%p)\n", nodename, servname, hints, res);
	return Truegetaddrinfo(nodename, servname, hints, res);
}

//=========================================================================
// This is the function that will replace HeapAlloc once the hook 
// is in place
//
LPVOID WINAPI HookHeapAlloc(HANDLE a_Handle, DWORD a_Bla, SIZE_T a_Bla2) {
	printf("***** Call to HeapAlloc(0x%p, %u, 0x%p)\n", a_Handle, a_Bla, a_Bla2);
	return TrueHeapAlloc(a_Handle, a_Bla, a_Bla2);
}

//=========================================================================
// This is the function that will replace NtClose once the hook 
// is in place
//
ULONG WINAPI HookNtClose(HANDLE hHandle) {
	printf("***** Call to NtClose(0x%p)\n", hHandle);
	return TrueNtClose(hHandle);
}

//=========================================================================
// This is where the work gets done.
//
int wmain(int argc, WCHAR* argv[])
{
	HANDLE hProc = NULL;

	// Set the hook
	if (Mhook_SetHook((PVOID*)&TrueNtOpenProcess, HookNtOpenProcess)) {
		// Now call OpenProcess and observe NtOpenProcess being redirected
		// under the hood.
		hProc = OpenProcess(PROCESS_ALL_ACCESS, 
			FALSE, GetCurrentProcessId());
		if (hProc) {
			printf("Successfully opened self: %p\n", hProc);
			CloseHandle(hProc);
		} else {
			printf("Could not open self: %d\n", GetLastError());
		}
		// Remove the hook
		Mhook_Unhook((PVOID*)&TrueNtOpenProcess);
	}

	// Call OpenProces again - this time there won't be a redirection as
	// the hook has bee removed.
	hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
	if (hProc) {
		printf("Successfully opened self: %p\n", hProc);
		CloseHandle(hProc);
	} else {
		printf("Could not open self: %d\n", GetLastError());
	}

	// Test another hook, this time in SelectObject
	// (SelectObject is interesting in that on XP x64, the second instruction
	// in the trampoline uses IP-relative addressing and we need to do some
	// extra work under the hood to make things work properly. This really
	// is more of a test case rather than a demo.)
	printf("Testing SelectObject.\n");
	if (Mhook_SetHook((PVOID*)&TrueSelectObject, HookSelectobject)) {
		// error checking omitted for brevity. doesn't matter much 
		// in this context anyway.
		HDC hdc = GetDC(NULL);
		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hbm = CreateCompatibleBitmap(hdc, 32, 32);
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);
		SelectObject(hdcMem, hbmOld);
		DeleteObject(hbm);
		DeleteDC(hdcMem);
		ReleaseDC(NULL, hdc);
		// Remove the hook
		Mhook_Unhook((PVOID*)&TrueSelectObject);
	}

	printf("Testing getaddrinfo.\n");
	if (Mhook_SetHook((PVOID*)&Truegetaddrinfo, Hookgetaddrinfo)) {
		// error checking omitted for brevity. doesn't matter much 
		// in this context anyway.
		WSADATA wd = {0};
		WSAStartup(MAKEWORD(2, 2), &wd);
		char* ip = "localhost";
		struct addrinfo aiHints;
		struct addrinfo *res = NULL;
		memset(&aiHints, 0, sizeof(aiHints));
		aiHints.ai_family = PF_UNSPEC;
		aiHints.ai_socktype = SOCK_STREAM;
		if (getaddrinfo(ip, NULL, &aiHints, &res)) {
			printf("getaddrinfo failed\n");
		} else {
			int n = 0;
			while(res) {
				res = res->ai_next;
				n++;
			}
			printf("got %d addresses\n", n);
		}
		WSACleanup();
		// Remove the hook
		Mhook_Unhook((PVOID*)&Truegetaddrinfo);
	}

	printf("Testing HeapAlloc.\n");
	if (Mhook_SetHook((PVOID*)&TrueHeapAlloc, HookHeapAlloc))
	{
		free(malloc(10));
		// Remove the hook
		Mhook_Unhook((PVOID*)&TrueHeapAlloc);
	}

	printf("Testing NtClose.\n");
	if (Mhook_SetHook((PVOID*)&TrueNtClose, HookNtClose))
	{
		CloseHandle(NULL);
		// Remove the hook
		Mhook_Unhook((PVOID*)&TrueNtClose);
	}

	return 0;
}

