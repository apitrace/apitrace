/*********************************************************************
 *
 * Copyright 2011 Intel Corporation
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *********************************************************************/


#include <stdlib.h>

#include <iostream>

#include "os_string.hpp"
#include "os_process.hpp"
#include "trace_tools.hpp"



namespace trace {


#if defined(__APPLE__)
#define TRACE_VARIABLE "DYLD_LIBRARY_PATH"
#define GL_TRACE_WRAPPER  "OpenGL"
#elif defined(_WIN32)
#define GL_TRACE_WRAPPER  "opengl32.dll"
#else
#define TRACE_VARIABLE "LD_PRELOAD"
#define GL_TRACE_WRAPPER  "glxtrace.so"
#define EGL_TRACE_WRAPPER  "egltrace.so"
#endif


static os::String
findWrapper(const char *wrapperFilename)
{
    os::String wrapperPath;

    os::String processDir = os::getProcessName();
    processDir.trimFilename();

    // Try relative build directory
    // XXX: Just make build and install directory layout match
    wrapperPath = processDir;
    wrapperPath.join("wrappers");
    wrapperPath.join(wrapperFilename);
    if (wrapperPath.exists()) {
        return wrapperPath;
    }

    // Try relative install directory
    wrapperPath = processDir;
#if defined(_WIN32)
    wrapperPath.join("..\\lib\\wrappers");
#elif defined(__APPLE__)
    wrapperPath.join("../lib/wrappers");
#else
    wrapperPath.join("../lib/apitrace/wrappers");
#endif
    wrapperPath.join(wrapperFilename);
    if (wrapperPath.exists()) {
        return wrapperPath;
    }

#ifndef _WIN32
    // Try absolute install directory
    wrapperPath = APITRACE_WRAPPER_INSTALL_DIR;
    wrapperPath.join(wrapperFilename);
    if (wrapperPath.exists()) {
        return wrapperPath;
    }
#endif

    return "";
}


int
traceProgram(API api,
             char * const *argv,
             const char *output,
             bool verbose)
{
    const char *wrapperFilename;

    /*
     * TODO: simplify code
     */

    switch (api) {
    case API_GL:
        wrapperFilename = GL_TRACE_WRAPPER;
        break;
#ifdef EGL_TRACE_WRAPPER
    case API_EGL:
        wrapperFilename = EGL_TRACE_WRAPPER;
        break;
#endif
#ifdef _WIN32
    case API_D3D7:
        wrapperFilename = "ddraw.dll";
        break;
    case API_D3D8:
        wrapperFilename = "d3d8.dll";
        break;
    case API_D3D9:
        wrapperFilename = "d3d9.dll";
        break;
    case API_D3D10:
        wrapperFilename = "d3d10.dll";
        break;
#endif
    default:
        std::cerr << "error: unsupported API\n";
        return 1;
    }

    os::String wrapperPath = findWrapper(wrapperFilename);

    if (!wrapperPath.length()) {
        std::cerr << "error: failed to find " << wrapperFilename << "\n";
        return 1;
    }

#if defined(_WIN32)
    /* On Windows copy the wrapper to the program directory.
     */
    os::String tmpWrapper(argv[0]);
    tmpWrapper.trimFilename();
    tmpWrapper.join(wrapperFilename);

    if (verbose) {
        std::cerr << wrapperPath << " -> " << tmpWrapper << "\n";
    }

    if (tmpWrapper.exists()) {
        std::cerr << "error: not overwriting " << tmpWrapper << "\n";
        return 1;
    }

    if (!os::copyFile(wrapperPath, tmpWrapper, false)) {
        std::cerr << "error: failed to copy " << wrapperPath << " into " << tmpWrapper << "\n";
        return 1;
    }
#endif /* _WIN32 */

#if defined(__APPLE__)
    /* On Mac OS X, using DYLD_LIBRARY_PATH, we actually set the
     * directory, not the file. */
    wrapperPath.trimFilename();
#endif

#if defined(TRACE_VARIABLE)
    if (verbose) {
        std::cerr << TRACE_VARIABLE << "=" << wrapperPath.str() << "\n";
    }
    /* FIXME: Don't modify the current environment */
    os::setEnvironment(TRACE_VARIABLE, wrapperPath.str());
#endif /* TRACE_VARIABLE */

    if (output) {
        os::setEnvironment("TRACE_FILE", output);
    }

    if (verbose) {
        const char *sep = "";
        for (char * const * arg = argv; *arg; ++arg) {
            std::cerr << *arg << sep;
            sep = " ";
        }
        std::cerr << "\n";
    }

    int status = os::execute(argv);

#if defined(TRACE_VARIABLE)
    os::unsetEnvironment(TRACE_VARIABLE);
#endif
#if defined(_WIN32)
    os::removeFile(tmpWrapper);
#endif

    if (output) {
        os::unsetEnvironment("TRACE_FILE");
    }
    
    return status;

}


} /* namespace trace */
