/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
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
#include <getopt.h>

#include <iostream>

#include "cli.hpp"

#include "trace_file.hpp"


static const char *synopsis = "Repack a trace file with Snappy compression.";

static void
usage(void)
{
    std::cout
        << "usage: apitrace repack <in-trace-file> <out-trace-file>\n"
        << synopsis << "\n"
        << "\n"
        << "Snappy compression allows for faster replay and smaller memory footprint,\n"
        << "at the expense of a slightly smaller compression ratio than zlib\n"
        << "\n";
}

const static char *
shortOptions = "h";

const static struct option
longOptions[] = {
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
};

static int
repack(const char *inFileName, const char *outFileName)
{
    trace::File *inFile = trace::File::createForRead(inFileName);
    if (!inFile) {
        return 1;
    }

    trace::File *outFile = trace::File::createForWrite(outFileName);
    if (!outFile) {
        delete inFile;
        return 1;
    }

    size_t size = 8192;
    char *buf = new char[size];
    size_t read;

    while ((read = inFile->read(buf, size)) != 0) {
        outFile->write(buf, read);
    }

    delete [] buf;
    delete outFile;
    delete inFile;

    return 0;
}

static int
command(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt_long(argc, argv, shortOptions, longOptions, NULL)) != -1) {
        switch (opt) {
        case 'h':
            usage();
            return 0;
        default:
            std::cerr << "error: unexpected option `" << (char)opt << "`\n";
            usage();
            return 1;
        }
    }

    if (argc != optind + 2) {
        std::cerr << "error: insufficient number of arguments\n";
        usage();
        return 1;
    }

    return repack(argv[optind], argv[optind + 1]);
}

const Command repack_command = {
    "repack",
    synopsis,
    usage,
    command
};
