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


#include <assert.h>
#include <string.h>

#include <iostream>

#include "cli.hpp"

#include "trace_tools.hpp"


static const char *synopsis = "Generate a new trace by executing the given program.";

static void
usage(void)
{
    std::cout << "usage: apitrace trace PROGRAM [ARGS ...]\n"
        << synopsis << "\n"
        "\n"
        "    The given program will be executed with the given arguments.\n"
        "    During execution, all OpenGL calls will be captured to a trace\n"
        "    file. That trace file can then be used\n"
        "    with other apitrace utilities for replay or analysis.\n"
        "\n"
        "    -v, --verbose       verbose output\n"
        "    -a, --api API       specify API to trace (gl or egl);\n"
        "                        default is `gl`\n"
        "    -o, --output TRACE  specify output trace file;\n"
        "                        default is `PROGRAM.trace`\n";
}

static int
command(int argc, char *argv[])
{
    bool verbose = false;
    trace::API api = trace::API_GL;
    const char *output = NULL;
    int i;

    for (i = 1; i < argc; ) {
        const char *arg = argv[i];

        if (arg[0] != '-') {
            break;
        }

        ++i;

        if (strcmp(arg, "--") == 0) {
            break;
        } else if (strcmp(arg, "--help") == 0) {
            usage();
            return 0;
        } else if (strcmp(arg, "-v") == 0 ||
                   strcmp(arg, "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(arg, "-a") == 0 ||
                   strcmp(arg, "--api") == 0) {
            arg = argv[i++];
            if (strcmp(arg, "gl") == 0) {
                api = trace::API_GL;
            } else if (strcmp(arg, "egl") == 0) {
                api = trace::API_EGL;
            } else {
                std::cerr << "error: unknown API `" << arg << "`\n";
                usage();
                return 1;
            }
        } else if (strcmp(arg, "-o") == 0 ||
                   strcmp(arg, "--output") == 0) {
            output = argv[i++];
        } else {
            std::cerr << "error: unknown option " << arg << "\n";
            usage();
            return 1;
        }
    }

    if (i == argc) {
        std::cerr << "error: no command specified\n";
        usage();
        return 1;
    }

    assert(argv[argc] == 0);
    return trace::traceProgram(api, argv + i, output, verbose);
}

const Command trace_command = {
    "trace",
    synopsis,
    usage,
    command
};
