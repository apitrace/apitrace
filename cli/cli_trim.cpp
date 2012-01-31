/**************************************************************************
 *
 * Copyright 2010 VMware, Inc.
 * Copyright 2011 Intel corporation
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/

#include <string.h>

#include "cli.hpp"

#include "os_string.hpp"

#include "trace_callset.hpp"
#include "trace_parser.hpp"
#include "trace_writer.hpp"

static const char *synopsis = "Create a new trace by trimming an existing trace.";

static void
usage(void)
{
    std::cout
        << "usage: apitrace trim [OPTIONS] <trace-file>...\n"
        << synopsis << "\n"
        "\n"
        "       --calls <CALLSET>     Only trim specified calls\n"
        "    -o --output <TRACEFILE>  Output trace file\n"
        "\n"
    ;
}

static int
command(int argc, char *argv[])
{
    std::string output;
    trace::CallSet calls(trace::FREQUENCY_ALL);
    int i;

    for (i = 1; i < argc;) {
        const char *arg = argv[i];

        if (arg[0] != '-') {
            break;
        }

        ++i;

        if (!strcmp(arg, "--")) {
            break;
        } else if (!strcmp(arg, "--help")) {
            usage();
            return 0;
        } else if (!strcmp(arg, "--calls")) {
            calls = trace::CallSet(argv[i++]);
        } else if (!strcmp(arg, "-o") ||
                   !strcmp(arg, "--output")) {
            output = argv[i++];
        } else {
            std::cerr << "error: unknown option " << arg << "\n";
            usage();
            return 1;
        }
    }

    if (i >= argc) {
        std::cerr << "Error: apitrace trim requires a trace file as an argument.\n";
        usage();
        return 1;
    }

    for ( ; i < argc; ++i) {
        trace::Parser p;
        if (!p.open(argv[i])) {
            std::cerr << "error: failed to open " << argv[i] << "\n";
            return 1;
        }

        if (output.empty()) {
            os::String base(argv[i]);
            base.trimExtension();

            output = std::string(base.str()) + std::string("-trim.trace");
        }

        trace::Writer writer;
        if (!writer.open(output.c_str())) {
            std::cerr << "error: failed to create " << argv[i] << "\n";
            return 1;
        }

        trace::Call *call;
        while ((call = p.parse_call())) {
            if (calls.contains(*call)) {
                writer.writeCall(call);
            }
            delete call;
        }

        std::cout << "Trimmed trace is available as " << output << "\n";
    }

    return 0;
}

const Command trim_command = {
    "trim",
    synopsis,
    usage,
    command
};
