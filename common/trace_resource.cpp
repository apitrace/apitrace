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
findFile(const char *relPath,
         const char *absPath,
         bool verbose)
{
    os::String complete;

    /* First look in the same directory from which this process is
     * running, (to support developers running a compiled program that
     * has not been installed. */
    os::String process_dir = os::getProcessName();
    process_dir.trimFilename();

    complete = process_dir;
    complete.join(relPath);

    if (complete.exists())
        return complete;

    /* Second, look in the directory for installed wrappers. */
    complete = absPath;
    if (complete.exists())
        return complete;

    if (verbose) {
        std::cerr << "error: cannot find " << relPath << " or " << absPath << "\n";
    }

    return "";
}


#define SCRIPTS_SUBDIR "lib/apitrace/scripts"

os::String
findScript(const char *name)
{
    os::String path;

#if defined(APITRACE_SOURCE_DIR)
    // Try the absolute source  path -- useful for development or quick
    // experiment.  Relative paths don't quite work here due to out of source
    // trees.
    path = APITRACE_SOURCE_DIR "/scripts";
    path.join(name);
    if (path.exists()) {
        return path;
    }
#endif

#if defined(_WIN32)
    // Windows has no standard installation path, so find it relatively to the
    // process name, which is assumed to be in a bin subdirectory.
    path = os::getProcessName();
    path.trimFilename();
    path.join("..\\lib\\apitrace\\scripts");
    path.join(name);
    if (path.exists()) {
        return path;
    }
#else
    // Assume a predefined installation path on Unices
    path = APITRACE_INSTALL_PREFIX "/lib/apitrace/scripts";
    path.join(name);
    if (path.exists()) {
        return path;
    }
#endif

    std::cerr << "error: cannot find " << name << " script\n";

    return "";
}


} /* namespace trace */
