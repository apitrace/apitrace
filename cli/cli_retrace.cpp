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

#include "cli.hpp"

#include "os_string.hpp"
#include "os_process.hpp"

#include "trace_resource.hpp"

static const char *synopsis = "Replay a trace.";

static void
usage(void)
{
    std::cout << "usage apitrace retrace [OPTIONS] TRACE_FILE\n"
              << synopsis << "\n"
	    "\n"
	    "    -h, --help             Show this help message and exit\n"
	    "    -w, --wait             Wait for user termination after the last frame\n"
	    "\n";
}

const static char *
shortOptions = "hw";

const static struct option
longOptions[] = {
    {"help", no_argument, 0, 'h'},
    {"wait", required_argument, 0, 'w'},
    {0, 0, 0, 0}
};

static int
command(int argc, char *argv[])
{
    bool wait = false;
    const char *filename;

    int opt;
    while ((opt = getopt_long(argc, argv, shortOptions, longOptions, NULL)) != -1) {
        switch (opt) {
        case 'h':
            usage();
            return 0;
        case 'w':
            wait = true;
            break;
        default:
            std::cerr << "error: unexpected option `" << opt << "`\n";
            usage();
            return 1;
        }
    }

    if (optind >= argc) {
        std::cerr << "error: apitrace retrace requires a trace file as an argument.\n";
        usage();
        return 1;
    }

    if (optind < argc - 1) { 
        std::cerr << "error: apitrace retrace can accept only a single trace file argument.\n";
        usage();
        return 1;
    }

    filename = argv[optind];

    std::vector<const char *> command;

    os::String glretracePath = trace::findProgram("glretrace");
    command.push_back(glretracePath);

    if (wait) {
        command.push_back("--wait");
    }

    command.push_back(filename);
    command.push_back(NULL);

    return os::execute((char * const *)&command[0]);
}

const Command retrace_command = {
    "retrace",
    synopsis,
    usage,
    command
};
