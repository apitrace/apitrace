/*********************************************************************
 *
 * Copyright 2011 Jose Fonseca
 * Copyright 2012 Intel Corporation
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
#include <limits.h> // for CHAR_MAX
#include <getopt.h>
#include <iostream>

#include "os_string.hpp"
#include "os_process.hpp"

#include "trace_parser.hpp"
#include "cli_resources.hpp"

#include "cli.hpp"
#include "cli_retrace.hpp"


static trace::API
guessApi(const char *filename)
{
    trace::Parser p;
    if (!p.open(filename)) {
        exit(1);
        return trace::API_UNKNOWN;
    }
    trace::Call *call;
    while ((call = p.parse_call())) {
        delete call;

        if (p.api != trace::API_UNKNOWN) {
            return p.api;
        }
    }

    return trace::API_UNKNOWN;
}

int
executeRetrace(const std::vector<const char *> & opts,
               const char *traceName,
               trace::API api) {
    const char *retraceName;
    switch (api) {
    case trace::API_GL:
        retraceName = "glretrace";
        break;
    case trace::API_EGL:
        retraceName = "eglretrace";
        break;
    case trace::API_DX:
    case trace::API_D3D7:
    case trace::API_D3D8:
    case trace::API_D3D9:
    case trace::API_DXGI:
        // Use prefix so that it can be used with WINE
        retraceName = "d3dretrace.exe";
        break;
    default:
        std::cerr << "warning: could not guess trace's API\n";
        retraceName = "glretrace";
        break;
    }

    std::vector<const char *> command;
    os::String retracePath = findProgram(retraceName);
    if (retracePath.length()) {
        command.push_back(retracePath);
    } else {
        command.push_back(retraceName);
    }

    command.insert(command.end(), opts.begin(), opts.end());

    if (traceName) {
        command.push_back(traceName);
    }
    command.push_back(NULL);

    return os::execute((char * const *)&command[0]);
}

int
executeRetrace(const std::vector<const char *> & opts,
               const char *traceName) {
    trace::API api = guessApi(traceName);
    return executeRetrace(opts, traceName, api);
}


static const char *synopsis = "Replay a trace.";

static void
usage(void)
{
    std::vector<const char *>opts;
    opts.push_back("--help");
    trace::API api = trace::API_GL;
    executeRetrace(opts, NULL, api);
}

static int
command(int argc, char *argv[])
{
    std::vector<const char *> opts;
    for (int i = 1; i < argc; ++i) {
        opts.push_back(argv[i]);
    }

    trace::API api = trace::API_GL;
    if (argc >= 1) {
        const char *lastArg = argv[argc -1];
        if (lastArg[0] != '-') {
            api = guessApi(lastArg);
        }
    }

    return executeRetrace(opts, NULL, api);
}

const Command retrace_command = {
    "replay",
    synopsis,
    usage,
    command
};
