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
#include <getopt.h>

#include <iostream>

#include "cli.hpp"

#include "trace_tools.hpp"


static const char *synopsis = "Generate a new trace by executing the given program.";

static void
usage(void)
{
    std::cout << "usage: apitrace trace [OPTIONS] PROGRAM [ARGS ...]\n"
        << synopsis << "\n"
        "\n"
        "    The given program will be executed with the given arguments.\n"
        "    During execution, all OpenGL calls will be captured to a trace\n"
        "    file. That trace file can then be used\n"
        "    with other apitrace utilities for replay or analysis.\n"
        "\n"
        "    -v, --verbose       verbose output\n"
        "    -a, --api=API       specify API to trace ("
#ifdef _WIN32
                                                      "gl, d3d7, d3d8, d3d9, or d3d10"
#else
                                                      "gl or egl"
#endif
                                                      ");\n"
        "                        default is `gl`\n"
        "    -o, --output=TRACE  specify output trace file;\n"
        "                        default is `PROGRAM.trace`\n";
}

const static char *
shortOptions = "+hva:o:";

const static struct option
longOptions[] = {
    {"help", no_argument, 0, 'h'},
    {"verbose", no_argument, 0, 'v'},
    {"api", required_argument, 0, 'a'},
    {"output", required_argument, 0, 'o'},
    {0, 0, 0, 0}
};

static int
command(int argc, char *argv[])
{
    bool verbose = false;
    trace::API api = trace::API_GL;
    const char *output = NULL;

    int opt;
    while ((opt = getopt_long(argc, argv, shortOptions, longOptions, NULL)) != -1) {
        switch (opt) {
        case 'h':
            usage();
            return 0;
        case 'v':
            verbose = true;
            break;
        case 'a':
            if (strcmp(optarg, "gl") == 0) {
                api = trace::API_GL;
            } else if (strcmp(optarg, "egl") == 0) {
                api = trace::API_EGL;
            } else if (strcmp(optarg, "d3d7") == 0) {
                api = trace::API_D3D7;
            } else if (strcmp(optarg, "d3d8") == 0) {
                api = trace::API_D3D8;
            } else if (strcmp(optarg, "d3d9") == 0) {
                api = trace::API_D3D9;
            } else if (strcmp(optarg, "d3d10") == 0) {
                api = trace::API_D3D10;
            } else {
                std::cerr << "error: unknown API `" << optarg << "`\n";
                usage();
                return 1;
            }
            break;
        case 'o':
            output = optarg;
            break;
        default:
            std::cerr << "error: unexpected option `" << opt << "`\n";
            usage();
            return 1;
        }
    }

    if (optind == argc) {
        std::cerr << "error: no command specified\n";
        usage();
        return 1;
    }

    assert(argv[argc] == 0);
    return trace::traceProgram(api, argv + optind, output, verbose);
}

const Command trace_command = {
    "trace",
    synopsis,
    usage,
    command
};
