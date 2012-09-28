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

static const char *synopsis = "Dump frame images obtained from a trace.";

static void
usage(void)
{
    std::cout << "usage apitrace dump-images [OPTIONS] TRACE_FILE\n"
              << synopsis << "\n"
        "\n"
        "    -h, --help           show this help message and exit\n"
        "        --calls=CALLSET  dump images only for specified calls\n"
        "                         (default value is \"*/frame\" which\n"
        "                          which dumps an image for each frame)\n"
        "    -o, --output=PREFIX  prefix to use in naming output files\n"
        "                         (default is trace filename without extension)\n"
        "\n";
}

enum {
    CALLS_OPT = CHAR_MAX + 1,
};

const static char *
shortOptions = "ho:";

const static struct option
longOptions[] = {
    {"help", no_argument, 0, 'h'},
    {"calls", required_argument, 0, CALLS_OPT},
    {"output", required_argument, 0, 'o'},
    {0, 0, 0, 0}
};

static int
command(int argc, char *argv[])
{
    os::String prefix;
    const char *calls, *filename, *output = NULL;

    int opt;
    while ((opt = getopt_long(argc, argv, shortOptions, longOptions, NULL)) != -1) {
        switch (opt) {
        case 'h':
            usage();
            return 0;
        case CALLS_OPT:
            calls = optarg;
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
        std::cerr << "error: apitrace dump-images requires a trace file as an argument.\n";
        usage();
        return 1;
    }

    if (optind < argc - 1) { 
        std::cerr << "error: apitrace dump-images can accept only a single trace file argument.\n";
        usage();
        return 1;
    }

    filename = argv[optind];

    if (output == NULL) {
        prefix = filename;
        prefix.trimDirectory();
        prefix.trimExtension();
        output = prefix.str();
    }

    /* FIXME: It would be cleaner to pull the replaying of the trace
     * in-process here and generate the images directly. But that
     * pulls in a non-trivial amount of the existing 'retrace' code,
     * along with dependencies on GL, etc.
     *
     * It will definitely make sense to do that once all that code has
     * already been pulled in for the "apitrace retrace" (or "apitrace
     * replay") command. */
    std::vector<const char *> command;
    command.push_back("glretrace");
    command.push_back("-s");
    command.push_back(output);
    command.push_back("-S");
    if (calls)
        command.push_back(calls);
    else
        command.push_back("*/frame");
    command.push_back(filename);
    command.push_back(NULL);

    return os::execute((char * const *)&command[0]);
}

const Command dump_images_command = {
    "dump-images",
    synopsis,
    usage,
    command
};
