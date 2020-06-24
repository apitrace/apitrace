
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

#include "cxx_compat.hpp" // for std::to_string, std::make_unique

#include "cli.hpp"
#include "cli_pager.hpp"

#include "trace_parser.hpp"
#include "trace_option.hpp"

static const char *synopsis = "Print given trace file(s) information in JSON format";

static void
usage(void)
{
    std::cout
        << "usage: apitrace info [OPTIONS] TRACE_FILE...\n"
        << synopsis << "\n"
        "\n"
        "    -h, --help        show this help message and exit\n"
        "    --dump-frames     dump per frame information\n"
        "\n"
    ;
}

enum {
    DUMP_FRAMES_OPT = CHAR_MAX + 1,
};

const static char *
shortOptions = "h";

const static struct option
longOptions[] = {
    {"help", no_argument, 0, 'h'},
    {"frames", no_argument, 0, DUMP_FRAMES_OPT},
    {0, 0, 0, 0}
};

static const char*
getApiName(int api) {
  if (api < trace::API_UNKNOWN || api >= trace::API_MAX)
    api = trace::API_UNKNOWN;
  return trace::API_NAMES[api];
}

struct FrameEntry {
    size_t firstCallId, lastCallId;
    size_t totalCalls;
    size_t sizeInBytes;
};

static int
command(int argc, char *argv[])
{
    bool flagDumpFrames = false;
    int opt;
    while ((opt = getopt_long(argc, argv, shortOptions, longOptions, NULL)) != -1) {
        switch (opt) {
        case 'h':
            usage();
            return 0;
        case DUMP_FRAMES_OPT:
            flagDumpFrames = true;
            break;
        default:
            std::cerr << "error: unexpected option `" << (char)opt << "`\n";
            usage();
            return 1;
        }
    }

    typedef std::vector<FrameEntry> FrameEntries;
    FrameEntries frames;

    for (int i = optind; i < argc; ++i) {
        unsigned long framesCount = 0;
        trace::API api = trace::API_UNKNOWN;
        trace::Parser p;

        if (!p.open(argv[i])) {
            return 1;
        }

        trace::Call *call;
        size_t callsInFrame = 0;
        size_t firstCallId = 0;
        size_t frameBytesOffset = 0;
        bool endFrame = true;
        while ((call = p.parse_call())) {
            if (flagDumpFrames) {
                ++callsInFrame;
                if (endFrame) {
                    firstCallId = call->no;
                    endFrame = false;
                }
            }
            if (api == trace::API_UNKNOWN && p.api != trace::API_UNKNOWN)
                api = p.api;
            if (call->flags & trace::CALL_FLAG_END_FRAME) {
                ++framesCount;
                if (flagDumpFrames) {
                    size_t curBytesOffset = p.dataBytesRead();
                    frames.push_back(FrameEntry{firstCallId, call->no, callsInFrame, curBytesOffset-frameBytesOffset});
                    frameBytesOffset = curBytesOffset;
                    endFrame = true;
                    callsInFrame = 0;
                }
            }
            delete call;
        }

        std::cout <<
            "{" << std::endl <<
            "  \"FileName\": \"" << argv[i] << "\"," << std::endl <<
            "  \"ContainerVersion\": " << p.getVersion() << "," << std::endl <<
            "  \"ContainerType\": \"" << p.containerType() << "\"," << std::endl <<
            "  \"API\": \"" << getApiName(api) << "\"," << std::endl <<
            "  \"FramesCount\": " << framesCount << "," << std::endl <<
            "  \"ActualDataSize\": " << p.dataBytesRead() << "," << std::endl <<
            "  \"ContainerSize\": " << p.containerSizeInBytes();
        if (flagDumpFrames) {
            std::cout << "," << std::endl;
            std::cout <<
                "  \"Frames\": [{" << std::endl;
            for (auto it = frames.begin(); it != frames.end(); ++it) {
                std::cout <<
                    "    \"FirstCallId\": " << it->firstCallId << "," << std::endl <<
                    "    \"LastCallId\": " << it->lastCallId << "," << std::endl <<
                    "    \"TotalCalls\": " << it->totalCalls << "," << std::endl <<
                    "    \"SizeInBytes\": " << it->sizeInBytes << std::endl;
                if (it != std::prev(frames.end())) {
                    std::cout << "  }, {" << std::endl;
                }
            }
            std::cout << "  }]" << std::endl;
        } else {
            std::cout << std::endl;
        }
        std::cout << "}" << std::endl;
    }

    return 0;
}

const Command info_command = {
    "info",
    synopsis,
    usage,
    command
};
