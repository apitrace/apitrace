
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

#include <memory>
#include <fstream>
#include <string>
#include <regex>

#include "cxx_compat.hpp" // for std::to_string, std::make_unique

#include "cli.hpp"
#include "cli_pager.hpp"

#include "trace_parser.hpp"
#include "trace_dump_internal.hpp"
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
        "    --grep[=REGEX]       dump only calls whose function names match regex\n"
        "    --thread-ids=[=BOOL] dump thread ids [default: no]\n"
        "    --call-nos[=BOOL]    dump call numbers[default: yes]\n"
        "    --arg-names[=BOOL]   dump argument names [default: yes]\n"
        "    --blobs              dump blobs into files\n"
        "    --multiline[=BOOL]   dump newline in strings literally [default: yes]\n"
        "\n"
    ;
}

enum {
    CALLS_OPT = CHAR_MAX + 1,
    COLOR_OPT,
    GREP_OPT,
    THREAD_IDS_OPT,
    CALL_NOS_OPT,
    ARG_NAMES_OPT,
    BLOBS_OPT,
    MULTILINE_OPT,
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
    {"grep", required_argument, 0, GREP_OPT},
    {"thread-ids", optional_argument, 0, THREAD_IDS_OPT},
    {"call-nos", optional_argument, 0, CALL_NOS_OPT},
    {"arg-names", optional_argument, 0, ARG_NAMES_OPT},
    {"blobs", no_argument, 0, BLOBS_OPT},
    {"multiline", optional_argument, 0, MULTILINE_OPT},
    {0, 0, 0, 0}
};


class BlobDumper : public trace::Dumper
{
    unsigned callNo = 0;
    unsigned blobNo = 0;
public:
    BlobDumper(std::ostream &_os, trace::DumpFlags _flags) :
        Dumper(_os, _flags)
    {
    }

    void visit(trace::Blob *blob) override {
        std::string fileName = "blob_call" + std::to_string(callNo);
        if (blobNo) {
            fileName += "_";
            fileName += std::to_string(blobNo);
        }
        fileName += ".bin";

        {
            std::ofstream stream(fileName, std::ofstream::binary);
            stream.write((const char *)blob->buf, blob->size);
            stream.close();
        }

        os << pointer << "blob(\"" << fileName << "\")" << normal;
        ++blobNo;
    }

    void visit(trace::Repr *repr) override {
        _visit(repr->machineValue);
    }

    void visit(trace::Call *call) override {
        callNo = call->no;
        blobNo = 0;
        Dumper::visit(call);
    }
};


static int
command(int argc, char *argv[])
{
    trace::DumpFlags dumpFlags = 0;
    bool blobs = false;
    bool grep = false;
    std::regex grepRegex;

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
            calls.merge(optarg);
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
        case GREP_OPT:
            grepRegex = std::regex(optarg);
            grep = true;
            break;
        case THREAD_IDS_OPT:
            if (trace::boolOption(optarg)) {
                dumpFlags |= trace::DUMP_FLAG_THREAD_IDS;
            } else {
                dumpFlags &= ~trace::DUMP_FLAG_THREAD_IDS;
            }
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
        case MULTILINE_OPT:
            if (trace::boolOption(optarg)) {
                dumpFlags &= ~trace::DUMP_FLAG_NO_MULTILINE;
            } else {
                dumpFlags |= trace::DUMP_FLAG_NO_MULTILINE;
            }
            break;
        case BLOBS_OPT:
            blobs = true;
            break;
        default:
            std::cerr << "error: unexpected option `" << (char)opt << "`\n";
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

    std::unique_ptr<trace::Dumper> dumper;

    if (blobs) {
        dumper = std::make_unique<BlobDumper>(std::cout, dumpFlags);
    } else {
        dumper = std::make_unique<trace::Dumper>(std::cout, dumpFlags);
    }

    for (int i = optind; i < argc; ++i) {
        trace::Parser p;

        if (!p.open(argv[i])) {
            return 1;
        }

        if (!grep) {
            const trace::Properties &properties = p.getProperties();
            for (auto & kv : properties) {
                std::cout << "// " << kv.first << " = ";
                dumper->visitString(kv.second.c_str());
                std::cout << std::endl;
            }
        }

        trace::Call *call;
        while ((call = p.parse_call())) {
            // Give a few calls of tolerance before bailing out to allow pending
            // multi-threaded calls out of order to dump
            const unsigned long long call_no_tol = 100;

            if (call->no > calls.getLast() + call_no_tol) {
                delete call;
                break;
            }
            if (calls.contains(*call) &&
                (!grep ||
                 std::regex_search(call->sig->name, grepRegex))) {
                if (verbose ||
                    !(call->flags & trace::CALL_FLAG_VERBOSE)) {
                    dumper->visit(call);
                    if (dumpFlags & trace::DUMP_FLAG_NO_MULTILINE) {
                        std::cout << '\n';
                    }
                    if (grep) {
                        std::cout << std::flush;
                    }
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
