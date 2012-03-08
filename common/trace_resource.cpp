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
#include "trace_tools.hpp"



namespace trace {


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
    scriptPath = APITRACE_WRAPPER_INSTALL_DIR;
    scriptPath.join(scriptFilename);
    if (scriptPath.exists()) {
        return scriptPath;
    }
#endif

    std::cerr << "error: cannot find " << scriptFilename << " script\n";

    return "";
}


} /* namespace trace */
