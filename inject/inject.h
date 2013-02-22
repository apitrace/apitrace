/**************************************************************************
 *
 * Copyright 2011-2012 Jose Fonseca
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


/*
 * Code for the DLL that will be injected in the target process.
 *
 * The injected DLL will manipulate the import tables to hook the
 * modules/functions of interest.
 *
 * See also:
 * - http://www.codeproject.com/KB/system/api_spying_hack.aspx
 * - http://www.codeproject.com/KB/threads/APIHooking.aspx
 * - http://msdn.microsoft.com/en-us/magazine/cc301808.aspx
 */


#include <windows.h>


static inline const char *
getSeparator(const char *szFilename) {
    const char *p, *q;
    p = NULL;
    q = szFilename;
    char c;
    do  {
        c = *q++;
        if (c == '\\' || c == '/' || c == ':') {
            p = q;
        }
    } while (c);
    return p;
}


static inline const char *
getBaseName(const char *szFilename) {
    const char *pSeparator = getSeparator(szFilename);
    if (!pSeparator) {
        return szFilename;
    }
    return pSeparator;
}


static inline void
getDirName(char *szFilename) {
    char *pSeparator = const_cast<char *>(getSeparator(szFilename));
    if (pSeparator) {
        *pSeparator = '\0';
    }
}


static inline void
getModuleName(char *szModuleName, size_t n, const char *szFilename) {
    char *p = szModuleName;
    const char *q = getBaseName(szFilename);
    char c;
    while (--n) {
        c = *q++;
        if (c == '.' || c == '\0') {
            break;
        }
        *p++ = c;
    };
    *p++ = '\0';
}


#define USE_SHARED_MEM 1


#if USE_SHARED_MEM

#define SHARED_MEM_SIZE 4096

static LPVOID pSharedMem = NULL;
static HANDLE hFileMapping = NULL;


static LPSTR
OpenSharedMemory(void) {
    if (pSharedMem) {
        return (LPSTR)pSharedMem;
    }

    hFileMapping = CreateFileMapping(
        INVALID_HANDLE_VALUE,   // system paging file
        NULL,                   // lpAttributes
        PAGE_READWRITE,         // read/write access
        0,                      // dwMaximumSizeHigh
        SHARED_MEM_SIZE,              // dwMaximumSizeLow
        TEXT("injectfilemap")); // name of map object
    if (hFileMapping == NULL) {
        fprintf(stderr, "Failed to create file mapping\n");
        return NULL;
    }

    BOOL bAlreadyExists = (GetLastError() == ERROR_ALREADY_EXISTS);

    pSharedMem = MapViewOfFile(
        hFileMapping,
        FILE_MAP_WRITE, // read/write access
        0,              // dwFileOffsetHigh
        0,              // dwFileOffsetLow
        0);             // dwNumberOfBytesToMap (entire file)
    if (pSharedMem == NULL) {
        fprintf(stderr, "Failed to map view \n");
        return NULL;
    }

    if (!bAlreadyExists) {
        memset(pSharedMem, 0, SHARED_MEM_SIZE);
    }

    return (LPSTR)pSharedMem;
}


static inline VOID
CloseSharedMem(void) {
    if (!pSharedMem) {
        return;
    }

    UnmapViewOfFile(pSharedMem);
    pSharedMem = NULL;

    CloseHandle(hFileMapping);
    hFileMapping = NULL;
}


static inline VOID
SetSharedMem(LPCSTR lpszSrc) {
    LPSTR lpszDst = OpenSharedMemory();
    if (!lpszDst) {
        return;
    }

    size_t n = 1;
    while (*lpszSrc && n < SHARED_MEM_SIZE) {
        *lpszDst++ = *lpszSrc++;
        n++;
    }
    *lpszDst = '\0';
}


static inline VOID
GetSharedMem(LPSTR lpszDst, size_t n) {
    LPCSTR lpszSrc = OpenSharedMemory();
    if (!lpszSrc) {
        return;
    }

    while (*lpszSrc && --n) {
        *lpszDst++ = *lpszSrc++;
    }
    *lpszDst = '\0';
}


#endif /* USE_SHARED_MEM */
