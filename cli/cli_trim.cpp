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
#include <limits.h> // for CHAR_MAX
#include <getopt.h>

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
        << "usage: apitrace trim [OPTIONS] TRACE_FILE...\n"
        << synopsis << "\n"
        "\n"
        "    -h, --help               show this help message and exit\n"
        "        --calls=CALLSET      only trim specified calls\n"
        "    -o, --output=TRACE_FILE  output trace file\n"
        "\n"
    ;
}

enum {
	CALLS_OPT = CHAR_MAX + 1,
};

const static char *
shortOptions = "ho";

const static struct option
longOptions[] = {
    {"help", no_argument, 0, 'h'},
    {"calls", required_argument, 0, CALLS_OPT},
    {"output", optional_argument, 0, 'o'},
    {0, 0, 0, 0}
};

static int
command(int argc, char *argv[])
{
    std::string output;
    trace::CallSet calls(trace::FREQUENCY_ALL);
    int i;

    int opt;
    while ((opt = getopt_long(argc, argv, shortOptions, longOptions, NULL)) != -1) {
        switch (opt) {
        case 'h':
            usage();
            return 0;
        case CALLS_OPT:
            calls = trace::CallSet(optarg);
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

    if (optind >= argc) {
        std::cerr << "error: apitrace trim requires a trace file as an argument.\n";
        usage();
        return 1;
    }

    for (i = optind; i < argc; ++i) {
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
