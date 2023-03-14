/**************************************************************************
 *
 * Copyright 2016 VMware, Inc
 * All Rights Reserved.
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

#pragma once


#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <string>


class Module
{
    const char *m_szName;
    HMODULE m_hModule = nullptr;
public:
    Module(const char *szName) :
        m_szName(szName)
    {
    }

    ~Module()
    {
        if (m_hModule) {
            FreeLibrary(m_hModule);
        }
    }

    PROC
    getProcAddress(LPCSTR lpProcName)
    {
        if (!m_hModule) {
            char szSystemDir[MAX_PATH] = {0};
            const char* pathOverride = getenv("APITRACE_FORCE_MODULE_PATH");
            if (pathOverride && *pathOverride) {
                strncpy(szSystemDir, pathOverride, MAX_PATH);
            } else if (!GetSystemDirectoryA(szSystemDir, MAX_PATH)) {
                return nullptr;
            }
            std::string sDllPath = szSystemDir;
            sDllPath += "\\";
            sDllPath += m_szName;
            sDllPath += ".dll";
            m_hModule = LoadLibraryA(sDllPath.c_str());
            if (!m_hModule) {
                return nullptr;
            }
        }
        return GetProcAddress(m_hModule, lpProcName);
    }
};
