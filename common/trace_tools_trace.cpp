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
#include "trace_resource.hpp"
#include "trace_tools.hpp"



namespace trace {


#if defined(__APPLE__)
#define TRACE_VARIABLE "DYLD_LIBRARY_PATH"
#define GL_TRACE_WRAPPER  "OpenGL"
#elif defined(_WIN32)
#define TRACE_VARIABLE ""
#define GL_TRACE_WRAPPER  "opengl32.dll"
#else
#define TRACE_VARIABLE "LD_PRELOAD"
#define GL_TRACE_WRAPPER  "glxtrace.so"
#define EGL_TRACE_WRAPPER  "egltrace.so"
#endif


int
traceProgram(API api,
             char * const *argv,
             const char *output,
             bool verbose)
{
    const char *relPath;
    const char *absPath;

    /*
     * TODO: simplify code
     */

    switch (api) {
    case API_GL:
        relPath = "wrappers/" GL_TRACE_WRAPPER;
        absPath = APITRACE_WRAPPER_INSTALL_DIR "/" GL_TRACE_WRAPPER;
        break;
    case API_EGL:
#ifndef EGL_TRACE_WRAPPER
        std::cerr << "error: unsupported API\n";
        return 1;
#else
        relPath = "wrappers/" EGL_TRACE_WRAPPER;
        absPath = APITRACE_WRAPPER_INSTALL_DIR "/" EGL_TRACE_WRAPPER;
        break;
#endif
#ifdef _WIN32
    case API_D3D7:
        relPath = "wrappers\\ddraw.dll";
        absPath = APITRACE_WRAPPER_INSTALL_DIR "\\ddraw.dll";
        break;
    case API_D3D8:
        relPath = "wrappers\\d3d8.dll";
        absPath = APITRACE_WRAPPER_INSTALL_DIR "\\d3d8.dll";
        break;
    case API_D3D9:
        relPath = "wrappers\\d3d9.dll";
        absPath = APITRACE_WRAPPER_INSTALL_DIR "\\d3d9.dll";
        break;
    case API_D3D10:
        relPath = "wrappers\\d3d10.dll";
        absPath = APITRACE_WRAPPER_INSTALL_DIR "\\d3d10.dll";
        break;
#endif
    default:
        std::cerr << "error: unsupported API\n";
        return 1;
    }

    os::String wrapper;
    wrapper = findFile(relPath, absPath, verbose);

    if (!wrapper.length()) {
        return 1;
    }

#if defined(_WIN32)
    /* On Windows copt the wrapper to the program directory.
     */
    os::String wrapperName (wrapper);
    wrapperName.trimDirectory();

    os::String tmpWrapper(argv[0]);
    tmpWrapper.trimFilename();
    tmpWrapper.join(wrapperName);

    if (tmpWrapper.exists()) {
        std::cerr << "error: not overwriting " << tmpWrapper << "\n";
        return 1;
    }

    if (!os::copyFile(wrapper, tmpWrapper, false)) {
        std::cerr << "error: failed to copy " << wrapper << " into " << tmpWrapper << "\n";
        return 1;
    }
#endif /* _WIN32 */

#if defined(__APPLE__)
    /* On Mac OS X, using DYLD_LIBRARY_PATH, we actually set the
     * directory, not the file. */
    wrapper.trimFilename();
#endif

#if defined(TRACE_VARIABLE)

    if (verbose) {
        std::cerr << TRACE_VARIABLE << "=" << wrapper.str() << "\n";
    }

    /* FIXME: Don't modify the current environment */
    os::setEnvironment(TRACE_VARIABLE, wrapper.str());

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
