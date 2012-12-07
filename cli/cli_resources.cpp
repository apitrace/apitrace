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


os::String
findProgram(const char*programFilename)
{
    os::String programPath;

    os::String processDir = os::getProcessName();
    processDir.trimFilename();

    programPath = processDir;
    programPath.join(programFilename);
    if (programPath.exists()) {
        return programPath;
    }

#ifndef _WIN32
    // Try absolute install directory
    programPath = APITRACE_PROGRAMS_INSTALL_DIR;
    programPath.join(programFilename);
    if (programPath.exists()) {
        return programPath;
    }
#endif

    return "";
}

os::String
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
    wrapperPath = APITRACE_WRAPPERS_INSTALL_DIR;
    wrapperPath.join(wrapperFilename);
    if (wrapperPath.exists()) {
        return wrapperPath;
    }
#endif

    return "";
}

os::String
findScript(const char *scriptFilename)
{
    os::String scriptPath;

    os::String processDir = os::getProcessName();
    processDir.trimFilename();

    // Try relative build directory
    // XXX: Just make build and install directory layout match
    scriptPath = processDir;
    scriptPath.join("scripts");
    scriptPath.join(scriptFilename);
    if (scriptPath.exists()) {
        return scriptPath;
    }

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
    if (scriptPath.exists()) {
        return scriptPath;
    }

#ifndef _WIN32
    // Try absolute install directory
    scriptPath = APITRACE_SCRIPTS_INSTALL_DIR;
    scriptPath.join(scriptFilename);
    if (scriptPath.exists()) {
        return scriptPath;
    }
#endif

    std::cerr << "error: cannot find " << scriptFilename << " script\n";

    return "";
}
