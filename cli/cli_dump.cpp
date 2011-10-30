/**************************************************************************
 *
 * Copyright 2010 VMware, Inc.
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

#include "trace_parser.hpp"

enum ColorOption {
    COLOR_OPTION_NEVER = 0,
    COLOR_OPTION_ALWAYS = 1,
    COLOR_OPTION_AUTO = -1
};

static ColorOption color = COLOR_OPTION_AUTO;

static const char *synopsis = "Dump given trace(s) to standard output.";

static void
usage(void)
{
    std::cout
        << "usage: apitrace dump [OPTIONS] <trace-file>...\n"
        << synopsis << "\n"
        "\n"
        "    --color=<WHEN>\n"
        "    --colour=<WHEN>     Colored syntax highlighting\n"
        "                        WHEN is 'auto', 'always', or 'never'\n";
}

static int
command(int argc, char *argv[], int first_arg_command)
{
    int i;

    for (i = first_arg_command; i < argc; ++i) {
        const char *arg = argv[i];

        if (arg[0] != '-') {
            break;
        }

        if (!strcmp(arg, "--")) {
            break;
        } else if (!strcmp(arg, "--help")) {
            usage();
            return 0;
        } else if (!strcmp(arg, "--color=auto") ||
                   !strcmp(arg, "--colour=auto")) {
            color = COLOR_OPTION_AUTO;
        } else if (!strcmp(arg, "--color") ||
                   !strcmp(arg, "--colour") ||
                   !strcmp(arg, "--color=always") ||
                   !strcmp(arg, "--colour=always")) {
            color = COLOR_OPTION_ALWAYS;
        } else if (!strcmp(arg, "--color=never") ||
                   !strcmp(arg, "--colour=never") ||
                   !strcmp(arg, "--no-color") ||
                   !strcmp(arg, "--no-colour")) {
            color = COLOR_OPTION_NEVER;
        } else {
            std::cerr << "error: unknown option " << arg << "\n";
            usage();
            return 1;
        }
    }

    if (color == COLOR_OPTION_AUTO) {
#ifdef _WIN32
        color = COLOR_OPTION_ALWAYS;
#else
        color = isatty(1) ? COLOR_OPTION_ALWAYS : COLOR_OPTION_NEVER;
#endif
    }

    for (; i < argc; ++i) {
        trace::Parser p;

        if (!p.open(argv[i])) {
            std::cerr << "error: failed to open " << argv[i] << "\n";
            return 1;
        }

        trace::Call *call;
        while ((call = p.parse_call())) {
            call->dump(std::cout, color);
            delete call;
        }
    }

    return 0;
}

const Command dump = {
    "dump",
    synopsis,
    usage,
    command
};
