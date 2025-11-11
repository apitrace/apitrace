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


#include <fstream>

#include "os.hpp"
#include "trace_file.hpp"
#include "trace_snappy.hpp"
#include "zstd.h"
#include "zstd_seekable.h"


using namespace trace;


File *
File::createForRead(const char *filename)
{
    std::ifstream stream(filename, std::ifstream::binary | std::ifstream::in);
    if (!stream.is_open()) {
        os::log("error: failed to open %s\n", filename);
        return NULL;
    }

    // Read first 4 bytes (magic number)
    unsigned char magic[4];
    for (int i = 0; i < 4; ++i)
        stream >> magic[i];

    // Read last 4 bytes to check for seekable zstd magic
    stream.seekg(-4, std::ios::end);
    unsigned char last_magic[4];
    for (int i = 0; i < 4; ++i)
        stream >> last_magic[i];

    stream.close();

    File *file;

    if (magic[0] == SNAPPY_BYTE1 && magic[1] == SNAPPY_BYTE2) {
        file = File::createSnappy();
    } else if (magic[0] == 0x1f && magic[1] == 0x8b) {
        file = File::createZLib();
    } else if (((magic[0] << 0) | (magic[1] << 8) | (magic[2] << 16) | (magic[3] << 24)) == ZSTD_MAGICNUMBER) {
        unsigned int lastMagicValue = (last_magic[0] << 0) | (last_magic[1] << 8) | (last_magic[2] << 16) | (last_magic[3] << 24);
        if (lastMagicValue == ZSTD_SEEKABLE_MAGICNUMBER) {
            file = File::createZstdSeekable();
        } else {
            file = File::createZstd();
        }
    } else  {
        // XXX: Brotli has no magic header
        file = File::createBrotli();
    }
    if (!file) {
        return NULL;
    }

    if (!file->open(filename)) {
        os::log("error: could not open %s for reading\n", filename);
        delete file;
        return NULL;
    }

    return file;
}
