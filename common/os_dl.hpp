/**************************************************************************
 *
 * Copyright 2012 Jose Fonseca
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
 * Dynamic library linking abstraction.
 */

#ifndef _OS_DL_HPP_
#define _OS_DL_HPP_


#if defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif


#if defined(_WIN32)
#define OS_LIBRARY_EXTENSION ".dll"
#elif defined(__APPLE__)
#define OS_LIBRARY_EXTENSION ".dylib"
#else
#define OS_LIBRARY_EXTENSION ".so"
#endif


namespace os {

    // TODO: Wrap in a class
#if defined(_WIN32)
    typedef HMODULE Library;
#else
    typedef void * Library;
#endif

    inline Library
    openLibrary(const char *filename) {
#if defined(_WIN32)
        return LoadLibraryA(filename);
#else
        return dlopen(filename, RTLD_LOCAL | RTLD_LAZY);
#endif
    }

    inline void *
    getLibrarySymbol(Library library, const char *symbol) {
#if defined(_WIN32)
        return (void *)GetProcAddress(library, symbol);
#else
        return dlsym(library, symbol);
#endif
    }

    inline void
    closeLibrary(Library library) {
#if defined(_WIN32)
        FreeLibrary(library);
#else
        dlclose(library);
#endif
    }


} /* namespace os */

#endif /* _OS_DL_HPP_ */
