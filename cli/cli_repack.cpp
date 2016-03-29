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
#include <memory>

#include "cli.hpp"

#include <brotli/enc/encode.h>
#include <zlib.h>  // for crc32

#include "trace_file.hpp"
#include "trace_ostream.hpp"


static const char *synopsis = "Repack a trace file with different compression.";

static void
usage(void)
{
    std::cout
        << "usage: apitrace repack [options] <in-trace-file> <out-trace-file>\n"
        << synopsis << "\n"
        << "\n"
        << "Snappy compression allows for faster replay and smaller memory footprint,\n"
        << "at the expense of a slightly smaller compression ratio than zlib\n"
        << "\n"
        << "    -b,--brotli  Use Brotli compression\n"
        << "    -z,--zlib    Use ZLib compression\n"
        << "\n";
}

const static char *
shortOptions = "hbz";

const static struct option
longOptions[] = {
    {"help", no_argument, 0, 'h'},
    {"brotli", optional_argument, 0, 'b'},
    {"zlib", no_argument, 0, 'z'},
    {0, 0, 0, 0}
};

enum Format {
    FORMAT_SNAPPY = 0,
    FORMAT_ZLIB,
    FORMAT_BROTLI,
};


class BrotliTraceIn : public brotli::BrotliIn
{
private:
    trace::File *stream;
    char buf[1 << 16];
    bool eof = false;

public:
    uLong crc;

    BrotliTraceIn(trace::File *s) :
        stream(s)
    {
        crc = crc32(0L, Z_NULL, 0);
    }

    ~BrotliTraceIn() {
    }

    const void *
    Read(size_t n, size_t* bytes_read) override
    {
        if (n > sizeof buf) {
            n = sizeof buf;
        } else if (n == 0) {
            return eof ? nullptr : buf;
        }
        *bytes_read = stream->read(buf, n);
        if (*bytes_read == 0) {
            eof = true;
            return nullptr;
        }
        crc32(crc, reinterpret_cast<const Bytef *>(buf), *bytes_read);
        return buf;
    }
};


static int
repack_generic(trace::File *inFile, trace::OutStream *outFile)
{
    const size_t size = 8192;
    char *buf = new char[size];
    size_t read;

    while ((read = inFile->read(buf, size)) != 0) {
        outFile->write(buf, read);
    }

    delete [] buf;

    return EXIT_SUCCESS;
}


static int
repack_brotli(trace::File *inFile, const char *outFileName, int quality)
{
    brotli::BrotliParams params;

    // Brotli default quality is 11, but there are problems using quality
    // higher than 9:
    //
    // - Some traces cause compression to be extremely slow.  Possibly the same
    //   issue as https://github.com/google/brotli/issues/330
    // - Some traces get lower compression ratio with 11 than 9.  Possibly the
    //   same issue as https://github.com/google/brotli/issues/222
    params.quality = 9;

    // The larger the window, the higher the compression ratio and
    // decompression speeds, so choose the maximum.
    params.lgwin = 24;

    if (quality > 0) {
        params.quality = quality;
    }

    BrotliTraceIn in(inFile);
    FILE *fout = fopen(outFileName, "wb");
    if (!fout) {
        return EXIT_FAILURE;
    }
    assert(fout);
    brotli::BrotliFileOut out(fout);
    if (!BrotliCompress(params, &in, &out)) {
        std::cerr << "error: brotli compression failed\n";
        return EXIT_FAILURE;
    }
    fclose(fout);

    std::unique_ptr<trace::File> outFileIn(trace::File::createBrotli());
    if (!outFileIn->open(outFileName)) {
        std::cerr << "error: failed to open " << outFileName << " for reading\n";
        return EXIT_FAILURE;
    }
    BrotliTraceIn outIn(outFileIn.get());
    size_t bytes_read;
    do {
        outIn.Read(65536, &bytes_read);
    } while (bytes_read > 0);

    if (in.crc != outIn.crc) {
        std::cerr << "error: CRC mismatch reading " << outFileName << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static int
repack(const char *inFileName, const char *outFileName, Format format, int quality)
{
    int ret = EXIT_FAILURE;

    trace::File *inFile = trace::File::createForRead(inFileName);
    if (!inFile) {
        return 1;
    }

    trace::OutStream *outFile = nullptr;
    if (format == FORMAT_SNAPPY) {
        outFile = trace::createSnappyStream(outFileName);
    } else if (format == FORMAT_BROTLI) {
        ret = repack_brotli(inFile, outFileName, quality);
        delete inFile;
        return ret;
    } else if (format == FORMAT_ZLIB) {
        outFile = trace::createZLibStream(outFileName);
    }
    if (outFile) {
        ret = repack_generic(inFile, outFile);
        delete outFile;
    }

    delete inFile;

    return ret;
}

static int
command(int argc, char *argv[])
{
    Format format = FORMAT_SNAPPY;
    int opt;
    int quality = -1;
    while ((opt = getopt_long(argc, argv, shortOptions, longOptions, NULL)) != -1) {
        switch (opt) {
        case 'h':
            usage();
            return 0;
        case 'b':
            format = FORMAT_BROTLI;
            if (optarg) {
                quality = atoi(optarg);
            }
            break;
        case 'z':
            format = FORMAT_ZLIB;
            break;
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

    return repack(argv[optind], argv[optind + 1], format, quality);
}

const Command repack_command = {
    "repack",
    synopsis,
    usage,
    command
};
