/**************************************************************************
 *
 * Copyright 2020 Joshua Ashton for Valve Software
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
 * AUTHORS,
 * AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * This file contains code from
 * https://github.com/stevemk14ebr/PolyHook_2_0/blob/master/sources/IatHook.cpp
 * which is licensed under the MIT License.
 * See PolyHook_2_0-LICENSE for more information.
 *
 **************************************************************************/

#pragma once

#include <windows.h>
#include <map>
#include <mutex>

namespace IATHook
{
    template <typename T, typename T1, typename T2>
    constexpr T RVA2VA(T1 base, T2 rva)
    {
        return reinterpret_cast<T>(reinterpret_cast<ULONG_PTR>(base) + rva);
    }

    template <typename T>
    constexpr T DataDirectoryFromModuleBase(void* moduleBase, size_t entryID)
    {
        auto dosHdr = reinterpret_cast<PIMAGE_DOS_HEADER>(moduleBase);
        auto ntHdr = RVA2VA<PIMAGE_NT_HEADERS>(moduleBase, dosHdr->e_lfanew);
        auto dataDir = ntHdr->OptionalHeader.DataDirectory;
        return RVA2VA<T>(moduleBase, dataDir[entryID].VirtualAddress);
    }

    static inline PIMAGE_THUNK_DATA FindAddressByName(void* moduleBase, PIMAGE_THUNK_DATA impName, PIMAGE_THUNK_DATA impAddr, const char* funcName)
    {
        for (; impName->u1.Ordinal; ++impName, ++impAddr)
        {
            if (IMAGE_SNAP_BY_ORDINAL(impName->u1.Ordinal))
                continue;

            auto import = RVA2VA<PIMAGE_IMPORT_BY_NAME>(moduleBase, impName->u1.AddressOfData);
            if (strcmp(import->Name, funcName) != 0)
                continue;
            return impAddr;
        }
        return nullptr;
    }

    static inline PIMAGE_THUNK_DATA FindAddressByOrdinal(void* moduleBase, PIMAGE_THUNK_DATA impName, PIMAGE_THUNK_DATA impAddr, uint16_t ordinal)
    {
        for (; impName->u1.Ordinal; ++impName, ++impAddr)
        {
            if (IMAGE_SNAP_BY_ORDINAL(impName->u1.Ordinal) && IMAGE_ORDINAL(impName->u1.Ordinal) == ordinal)
                return impAddr;
        }
        return nullptr;
    }

    static inline PIMAGE_THUNK_DATA FindIatThunkInModule(void* moduleBase, const char* dllName, const char* funcName)
    {
        auto imports = DataDirectoryFromModuleBase<PIMAGE_IMPORT_DESCRIPTOR>(moduleBase, IMAGE_DIRECTORY_ENTRY_IMPORT);
        for (; imports->Name; ++imports)
        {
            if (_stricmp(RVA2VA<LPCSTR>(moduleBase, imports->Name), dllName) != 0)
                continue;

            auto origThunk = RVA2VA<PIMAGE_THUNK_DATA>(moduleBase, imports->OriginalFirstThunk);
            auto thunk = RVA2VA<PIMAGE_THUNK_DATA>(moduleBase, imports->FirstThunk);
            return FindAddressByName(moduleBase, origThunk, thunk, funcName);
        }
        return nullptr;
    }

    static inline PIMAGE_THUNK_DATA FindDelayLoadThunkInModule(void* moduleBase, const char* dllName, const char* funcName)
    {
        auto imports = DataDirectoryFromModuleBase<PIMAGE_DELAYLOAD_DESCRIPTOR>(moduleBase, IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);
        for (; imports->DllNameRVA; ++imports)
        {
            if (_stricmp(RVA2VA<LPCSTR>(moduleBase, imports->DllNameRVA), dllName) != 0)
                continue;

            auto impName = RVA2VA<PIMAGE_THUNK_DATA>(moduleBase, imports->ImportNameTableRVA);
            auto impAddr = RVA2VA<PIMAGE_THUNK_DATA>(moduleBase, imports->ImportAddressTableRVA);
            return FindAddressByName(moduleBase, impName, impAddr, funcName);
        }
        return nullptr;
    }

    static inline PIMAGE_THUNK_DATA FindDelayLoadThunkInModule(void* moduleBase, const char* dllName, uint16_t ordinal)
    {
        auto imports = DataDirectoryFromModuleBase<PIMAGE_DELAYLOAD_DESCRIPTOR>(moduleBase, IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);
        for (; imports->DllNameRVA; ++imports)
        {
            if (_stricmp(RVA2VA<LPCSTR>(moduleBase, imports->DllNameRVA), dllName) != 0)
                continue;

            auto impName = RVA2VA<PIMAGE_THUNK_DATA>(moduleBase, imports->ImportNameTableRVA);
            auto impAddr = RVA2VA<PIMAGE_THUNK_DATA>(moduleBase, imports->ImportAddressTableRVA);
            return FindAddressByOrdinal(moduleBase, impName, impAddr, ordinal);
        }
        return nullptr;
    }

    static inline bool DetourCopyMemory(uintptr_t Target, uintptr_t Memory, size_t Length)
    {
        auto pvTarget = reinterpret_cast<void*>(Target);
        auto pvMemory = reinterpret_cast<void*>(Memory);

        DWORD dwOld = 0;
        if (!VirtualProtect(pvTarget, Length, PAGE_EXECUTE_READWRITE, &dwOld))
            return false;

        memcpy(pvTarget, pvMemory, Length);

        // Ignore if this fails, the memory was copied either way
        VirtualProtect(pvTarget, Length, dwOld, &dwOld);
        return true;
    }

    static inline uintptr_t IATThunkHook(uintptr_t Module, PIMAGE_THUNK_DATA NameTable, PIMAGE_THUNK_DATA ImportTable, const char* API, uintptr_t Detour)
    {
        for (; NameTable->u1.Ordinal != 0; ++NameTable, ++ImportTable)
        {
            if (!IMAGE_SNAP_BY_ORDINAL(NameTable->u1.Ordinal))
            {
                auto importName = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(Module + NameTable->u1.ForwarderString);
                auto funcName = &importName->Name[0];

                // If this is the function name we want, hook it
                if (!strcmp(funcName, API))
                {
                    // Copy the pointer variable itself, not the function bytes
                    uintptr_t originalFunc = ImportTable->u1.AddressOfData;
                    uintptr_t newPointer = Detour;

                    if (!DetourCopyMemory(reinterpret_cast<uintptr_t>(&ImportTable->u1.AddressOfData), reinterpret_cast<uintptr_t>(&newPointer), sizeof(ImportTable->u1.AddressOfData)))
                        return 0;

                    // Done
                    return originalFunc;
                }
            }
        }

        return 0;
    }

    static inline uintptr_t IATHook(uintptr_t Module, const char* ImportModule, const char* API, uintptr_t Detour)
    {
        auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(Module);
        auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(Module + dosHeader->e_lfanew);

        // Validate PE Header and (64-bit|32-bit) module type
        if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
            return 0;

        if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
            return 0;

        if (ntHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC)
            return 0;

        // Get the load configuration section which holds the imports
        auto dataDirectory = ntHeaders->OptionalHeader.DataDirectory;
        auto sectionRVA    = dataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
        auto sectionSize   = dataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;

        if (sectionRVA == 0 || sectionSize == 0)
            return 0;

        // https://jpassing.com/2008/01/06/using-import-address-table-hooking-for-testing/
        // https://llvm.org/svn/llvm-project/compiler-rt/trunk/lib/interception/interception_win.cc
        //
        // Iterate over each import descriptor
        auto importDescriptor = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(Module + sectionRVA);

        for (size_t i = 0; importDescriptor[i].Name != 0; i++)
        {
            auto dllName = reinterpret_cast<PSTR>(Module + importDescriptor[i].Name);

            // Is this the specific module the user wants?
            if (!_stricmp(dllName, ImportModule))
            {
                if (!importDescriptor[i].FirstThunk)
                    return 0;

                auto nameTable   = reinterpret_cast<PIMAGE_THUNK_DATA>(Module + importDescriptor[i].OriginalFirstThunk);
                auto importTable = reinterpret_cast<PIMAGE_THUNK_DATA>(Module + importDescriptor[i].FirstThunk);

                auto originalFunc = IATThunkHook(Module, nameTable, importTable, API, Detour);

                if (!originalFunc)
                    continue;

                return originalFunc;
            }
        }

        // API or module name wasn't found
        return 0;
    }

    static inline uintptr_t IATDelayedHook(uintptr_t Module, const char* ImportModule, const char* API, uintptr_t Detour)
    {
        auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(Module);
        auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(Module + dosHeader->e_lfanew);

        // Validate PE Header and (64-bit|32-bit) module type
        if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
            return 0;

        if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
            return 0;

        if (ntHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC)
            return 0;

        // Get the load configuration section which holds the imports
        auto dataDirectory = ntHeaders->OptionalHeader.DataDirectory;
        auto sectionRVA    = dataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].VirtualAddress;
        auto sectionSize   = dataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].Size;

        if (sectionRVA == 0 || sectionSize == 0)
            return 0;

        // Iterate over each delayed import descriptor
        auto importDescriptor = reinterpret_cast<PIMAGE_DELAYLOAD_DESCRIPTOR>(Module + sectionRVA);

        for (size_t i = 0; importDescriptor[i].DllNameRVA != 0; i++)
        {
            auto dllName = reinterpret_cast<PSTR>(Module + importDescriptor[i].DllNameRVA);

            // Is this the specific module the user wants?
            if (!_stricmp(dllName, ImportModule))
            {
                auto nameTable   = reinterpret_cast<PIMAGE_THUNK_DATA>(Module + importDescriptor[i].ImportNameTableRVA);
                auto importTable = reinterpret_cast<PIMAGE_THUNK_DATA>(Module + importDescriptor[i].ImportAddressTableRVA);

                auto originalFunc = IATThunkHook(Module, nameTable, importTable, API, Detour);

                if (!originalFunc)
                    continue;

                return originalFunc;
            }
        }

        // API or module name wasn't found
        return 0;
    }
}

namespace trace
{
    DWORD fakeWaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);
    DWORD fakeWaitForSingleObjectEx(HANDLE hHandle, DWORD dwMilliseconds, BOOL bAlertable);
    DWORD fakeWaitForMultipleObjects(DWORD nCount, const HANDLE* lpHandles, BOOL bWaitAll, DWORD dwMilliseconds);
    DWORD fakeWaitForMultipleObjectsEx(DWORD nCount, const HANDLE* lpHandles, BOOL bWaitAll, DWORD dwMilliseconds, BOOL bAlertable);
}

extern std::mutex g_D3D12FenceEventMapMutex;
extern std::map<HANDLE, HANDLE> g_D3D12FenceEventMap;

namespace D3D12EventHooks
{
    DWORD D3D12WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds)
    {
        bool shouldFake = false;
        {
            auto lock = std::unique_lock<std::mutex>(g_D3D12FenceEventMapMutex);

            auto elem = g_D3D12FenceEventMap.find(hHandle);
            if (elem != g_D3D12FenceEventMap.end()) {
                hHandle = elem->second;
                shouldFake = true;
                g_D3D12FenceEventMap.erase(elem);
            }
        }

        if (shouldFake)
            return trace::fakeWaitForSingleObject(hHandle, dwMilliseconds);
        else
            return WaitForSingleObject(hHandle, dwMilliseconds);
    }

    DWORD D3D12WaitForSingleObjectEx(HANDLE hHandle, DWORD dwMilliseconds, BOOL bAlertable)
    {
        bool shouldFake = false;
        {
            auto lock = std::unique_lock<std::mutex>(g_D3D12FenceEventMapMutex);

            auto elem = g_D3D12FenceEventMap.find(hHandle);
            if (elem != g_D3D12FenceEventMap.end()) {
                hHandle = elem->second;
                shouldFake = true;
                g_D3D12FenceEventMap.erase(elem);
            }
        }

        if (shouldFake)
            return trace::fakeWaitForSingleObjectEx(hHandle, dwMilliseconds, bAlertable);
        else
            return WaitForSingleObjectEx(hHandle, dwMilliseconds, bAlertable);
    }

    DWORD D3D12WaitForMultipleObjects(DWORD nCount, const HANDLE* lpHandles, BOOL bWaitAll, DWORD dwMilliseconds)
    {
        //return WaitForMultipleObjects(nCount, lpHandles, bWaitAll, dwMilliseconds);
        assert(false);
        return 0;
    }

    DWORD D3D12WaitForMultipleObjectsEx(DWORD nCount, const HANDLE* lpHandles, BOOL bWaitAll, DWORD dwMilliseconds, BOOL bAlertable)
    {
        //return WaitForMultipleObjectsEx(nCount, lpHandles, bWaitAll, dwMilliseconds, bAlertable);
        assert(false);
        return 0;
    }
}

static void _setup_event_hooking()
{
    // TODO(Josh): We need a better solution for this shit!!!
    uintptr_t processModule = reinterpret_cast<uintptr_t>(GetModuleHandleA(NULL));
    
    IATHook::IATHook(processModule, "kernel32.dll", "WaitForSingleObject",      reinterpret_cast<uintptr_t>(D3D12EventHooks::D3D12WaitForSingleObject));
    IATHook::IATHook(processModule, "kernel32.dll", "WaitForSingleObjectEx",    reinterpret_cast<uintptr_t>(D3D12EventHooks::D3D12WaitForSingleObjectEx));
    IATHook::IATHook(processModule, "kernel32.dll", "WaitForMultipleObjects",   reinterpret_cast<uintptr_t>(D3D12EventHooks::D3D12WaitForMultipleObjects));
    IATHook::IATHook(processModule, "kernel32.dll", "WaitForMultipleObjectsEx", reinterpret_cast<uintptr_t>(D3D12EventHooks::D3D12WaitForMultipleObjectsEx));
}
