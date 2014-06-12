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


using namespace trace;


File *
File::createForRead(const char *filename)
{
    std::ifstream stream(filename, std::ifstream::binary | std::ifstream::in);
    if (!stream.is_open()) {
        os::log("error: failed to open %s\n", filename);
        return NULL;
    }
    unsigned char byte1, byte2;
    stream >> byte1;
    stream >> byte2;
    stream.close();

    File *file;
    if (byte1 == SNAPPY_BYTE1 && byte2 == SNAPPY_BYTE2) {
        file = File::createSnappy();
    } else if (byte1 == 0x1f && byte2 == 0x8b) {
        file = File::createZLib();
    } else  {
        os::log("error: %s: unkwnown compression\n", filename);
        return NULL;
    }
    if (!file) {
        return NULL;
    }

    if (!file->open(filename, File::Read)) {
        os::log("error: could not open %s for reading\n", filename);
        delete file;
        return NULL;
    }

    return file;
}
