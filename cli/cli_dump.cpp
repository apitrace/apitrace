/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
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
#include <limits.h> // for CHAR_MAX
#include <getopt.h>
#ifndef _WIN32
#include <unistd.h> // for isatty()
#endif

#include "cli.hpp"
#include "cli_pager.hpp"

#include "trace_parser.hpp"
#include "trace_dump.hpp"
#include "trace_callset.hpp"
#include "trace_option.hpp"


enum ColorOption {
    COLOR_OPTION_NEVER = 0,
    COLOR_OPTION_ALWAYS = 1,
    COLOR_OPTION_AUTO = -1
};

static ColorOption color = COLOR_OPTION_AUTO;

static bool verbose = false;

static trace::CallSet calls(trace::FREQUENCY_ALL);

static const char *synopsis = "Dump given trace(s) to standard output.";

static void
usage(void)
{
    std::cout
        << "usage: apitrace dump [OPTIONS] TRACE_FILE...\n"
        << synopsis << "\n"
        "\n"
        "    -h, --help           show this help message and exit\n"
        "    -v, --verbose        verbose output\n"
        "    --calls=CALLSET      only dump specified calls\n"
        "    --color[=WHEN]\n"
        "    --colour[=WHEN]      colored syntax highlighting\n"
        "                         WHEN is 'auto', 'always', or 'never'\n"
        "    --thread-ids=[=BOOL] dump thread ids [default: no]\n"
        "    --call-nos[=BOOL]    dump call numbers[default: yes]\n"
        "    --arg-names[=BOOL]   dump argument names [default: yes]\n"
        "\n"
    ;
}

enum {
	CALLS_OPT = CHAR_MAX + 1,
	COLOR_OPT,
    THREAD_IDS_OPT,
    CALL_NOS_OPT,
    ARG_NAMES_OPT,
};

const static char *
shortOptions = "hv";

const static struct option
longOptions[] = {
    {"help", no_argument, 0, 'h'},
    {"verbose", no_argument, 0, 'v'},
    {"calls", required_argument, 0, CALLS_OPT},
    {"colour", optional_argument, 0, COLOR_OPT},
    {"color", optional_argument, 0, COLOR_OPT},
    {"thread-ids", optional_argument, 0, THREAD_IDS_OPT},
    {"call-nos", optional_argument, 0, CALL_NOS_OPT},
    {"arg-names", optional_argument, 0, ARG_NAMES_OPT},
    {0, 0, 0, 0}
};

static int
command(int argc, char *argv[])
{
    trace::DumpFlags dumpFlags = 0;
    bool dumpThreadIds = false;
    
    int opt;
    while ((opt = getopt_long(argc, argv, shortOptions, longOptions, NULL)) != -1) {
        switch (opt) {
        case 'h':
            usage();
            return 0;
        case 'v':
            verbose = true;
            break;
        case CALLS_OPT:
            calls = trace::CallSet(optarg);
            break;
        case COLOR_OPT:
            if (!optarg ||
                !strcmp(optarg, "always")) {
                color = COLOR_OPTION_ALWAYS;
            } else if (!strcmp(optarg, "auto")) {
                color = COLOR_OPTION_AUTO;
            } else if (!strcmp(optarg, "never")) {
                color = COLOR_OPTION_NEVER;
            } else {
                std::cerr << "error: unknown color argument " << optarg << "\n";
                return 1;
            }
            break;
        case THREAD_IDS_OPT:
            dumpThreadIds = trace::boolOption(optarg);
            break;
        case CALL_NOS_OPT:
            if (trace::boolOption(optarg)) {
                dumpFlags &= ~trace::DUMP_FLAG_NO_CALL_NO;
            } else {
                dumpFlags |= trace::DUMP_FLAG_NO_CALL_NO;
            }
            break;
        case ARG_NAMES_OPT:
            if (trace::boolOption(optarg)) {
                dumpFlags &= ~trace::DUMP_FLAG_NO_ARG_NAMES;
            } else {
                dumpFlags |= trace::DUMP_FLAG_NO_ARG_NAMES;
            }
            break;
        default:
            std::cerr << "error: unexpected option `" << opt << "`\n";
            usage();
            return 1;
        }
    }

    if (color == COLOR_OPTION_AUTO) {
#ifdef _WIN32
        color = COLOR_OPTION_ALWAYS;
#else
        color = isatty(STDOUT_FILENO) ? COLOR_OPTION_ALWAYS : COLOR_OPTION_NEVER;
        pipepager();
#endif
    }

    if (color == COLOR_OPTION_NEVER) {
        dumpFlags |= trace::DUMP_FLAG_NO_COLOR;
    }

    for (int i = optind; i < argc; ++i) {
        trace::Parser p;

        if (!p.open(argv[i])) {
            return 1;
        }

        trace::Call *call;
        while ((call = p.parse_call())) {
            if (calls.contains(*call)) {
                if (verbose ||
                    !(call->flags & trace::CALL_FLAG_VERBOSE)) {
                    if (dumpThreadIds) {
                        std::cout << std::hex << call->thread_id << std::dec << " ";
                    }
                    trace::dump(*call, std::cout, dumpFlags);
                }
            }
            delete call;
        }
    }

    return 0;
}

const Command dump_command = {
    "dump",
    synopsis,
    usage,
    command
};
