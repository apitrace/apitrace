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


#include <iostream>

#include "os_string.hpp"

#include "cli_resources.hpp"

#ifdef __GLIBC__

#include <dlfcn.h>

static bool
tryLib(const os::String &path, bool verbose)
{
    void *handle = dlopen(path.str(), RTLD_LAZY);
    bool exists = (handle != NULL);
    if (verbose) {
        if (exists) {
            std::cerr << "info: found " << path.str() << "\n";
        } else {
            std::cerr << "info: did not find " << dlerror() << "\n";
        }
    }
    if (exists)
        dlclose(handle);
    return exists;
}
#endif

static bool
tryPath(const os::String &path, bool verbose)
{
    bool exists = path.exists();
    if (verbose) {
        std::cerr << "info: " << (exists ? "found" : "did not find") << " " << path.str() << "\n";
    }
    return exists;
}

os::String
findProgram(const char *programFilename, bool verbose)
{
    os::String programPath;

    os::String processDir = os::getProcessName();
    processDir.trimFilename();

    programPath = processDir;
    programPath.join(programFilename);
    if (tryPath(programPath, verbose)) {
        return programPath;
    }

#ifndef _WIN32
    // Try absolute install directory
    programPath = APITRACE_PROGRAMS_INSTALL_DIR;
    programPath.join(programFilename);
    if (tryPath(programPath, verbose)) {
        return programPath;
    }
#endif

    return "";
}

os::String
findWrapper(const char *wrapperFilename, bool verbose)
{
    os::String wrapperPath;

    os::String processDir = os::getProcessName();
    processDir.trimFilename();

    // Try relative build directory
    // XXX: Just make build and install directory layout match
    wrapperPath = processDir;
#if defined(CMAKE_INTDIR)
    // Go from `Debug\apitrace.exe` to `wrappers\Debug\foo.dll` on MSVC builds.
    wrapperPath.join("..");
    wrapperPath.join("wrappers");
    wrapperPath.join(CMAKE_INTDIR);
#else
    wrapperPath.join("wrappers");
#endif
    wrapperPath.join(wrapperFilename);
    if (tryPath(wrapperPath, verbose)) {
        return wrapperPath;
    }

#ifdef __GLIBC__
    // We want to take advantage of $LIB dynamic string token expansion in
    // glibc dynamic linker to handle multilib layout for us
    wrapperPath = processDir;
    wrapperPath.join("../$LIB/apitrace/wrappers");
    wrapperPath.join(wrapperFilename);
    if (tryLib(wrapperPath, verbose)) {
        return wrapperPath;
    }
#endif

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
    if (tryPath(wrapperPath, verbose)) {
        return wrapperPath;
    }

#ifndef _WIN32
    // Try absolute install directory
    wrapperPath = APITRACE_WRAPPERS_INSTALL_DIR;
    wrapperPath.join(wrapperFilename);
    if (tryPath(wrapperPath, verbose)) {
        return wrapperPath;
    }
#endif

    return "";
}

os::String
findScript(const char *scriptFilename, bool verbose)
{
    os::String scriptPath;

    os::String processDir = os::getProcessName();
    processDir.trimFilename();

    // Try relative build directory
    // XXX: Just make build and install directory layout match
#if defined(APITRACE_SOURCE_DIR)
    scriptPath = APITRACE_SOURCE_DIR;
    scriptPath.join("scripts");
    scriptPath.join(scriptFilename);
    if (tryPath(scriptPath, verbose)) {
        return scriptPath;
    }
#endif

    // Try relative install directory
    scriptPath = processDir;
#if defined(_WIN32)
    scriptPath.join("..\\lib\\scripts");
#elif defined(__APPLE__)
    scriptPath.join("../lib/scripts");
#else
    scriptPath.join("../lib/apitrace/scripts");
#endif
    scriptPath.join(scriptFilename);
    if (tryPath(scriptPath, verbose)) {
        return scriptPath;
    }

#ifndef _WIN32
    // Try absolute install directory
    scriptPath = APITRACE_SCRIPTS_INSTALL_DIR;
    scriptPath.join(scriptFilename);
    if (tryPath(scriptPath, verbose)) {
        return scriptPath;
    }
#endif

    std::cerr << "error: cannot find " << scriptFilename << " script\n";
    exit(1);
}
