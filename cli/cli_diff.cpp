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

#include <string.h>
#include <iostream>

#include "cli.hpp"
#include "os_string.hpp"
#include "os_process.hpp"
#include "trace_resource.hpp"

static const char *synopsis = "Identify differences between two traces.";

static void
usage(void)
{
    std::cout
        << "usage: apitrace diff <trace-1> <trace-2>\n"
        << synopsis << "\n"
        "\n"
        "    Both input files should be the result of running 'apitrace trace'.\n";
}

static int
command(int argc, char *argv[])
{
    int i;

    for (i = 1; i < argc; ++i) {
        const char *arg = argv[i];

        if (arg[0] != '-') {
            break;
        }

        if (!strcmp(arg, "--")) {
            i++;
            break;
        } else if (!strcmp(arg, "--help")) {
            usage();
            return 0;
        } else {
            std::cerr << "error: unknown option " << arg << "\n";
            usage();
            return 1;
        }
    }

    if (argc - i != 2) {
        std::cerr << "Error: diff requires exactly two trace files as arguments.\n";
        usage();
        return 1;
    }

    char *file1, *file2;

    file1 = argv[i];
    file2 = argv[i+1];

    os::String command = trace::findScript("tracediff.sh");

    char* args[4];

    args[0] = (char *) command.str();
    args[1] = file1;
    args[2] = file2;
    args[3] = NULL;

#ifdef _WIN32
    std::cerr << "The 'apitrace diff' command is not yet supported on this O/S.\n";
    return 1;
#else
    os::String apitrace = os::getProcessName();
    setenv("APITRACE", apitrace.str(), 1);

    return os::execute(args);
#endif
}

const Command diff_command = {
    "diff",
    synopsis,
    usage,
    command
};
