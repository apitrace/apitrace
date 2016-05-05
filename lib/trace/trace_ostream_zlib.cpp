/**************************************************************************
 *
 * Copyright 2015 VMware, Inc.
 * Copyright 2011 Zack Rusin
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


#include "trace_ostream.hpp"


#include <assert.h>
#include <string.h>

#include <zlib.h>

#include "os.hpp"

#include <iostream>


using namespace trace;


class ZLibOutStream : public OutStream {
public:
    ZLibOutStream(gzFile file);
    virtual ~ZLibOutStream();

protected:
    virtual bool write(const void *buffer, size_t length) override;
    virtual void flush(void) override;
private:
    gzFile m_gzFile;
};

ZLibOutStream::ZLibOutStream(gzFile file)
    : m_gzFile(file)
{
}

ZLibOutStream::~ZLibOutStream()
{
    if (m_gzFile) {
        gzclose(m_gzFile);
        m_gzFile = nullptr;
    }
}

bool ZLibOutStream::write(const void *buffer, size_t length)
{
    return gzwrite(m_gzFile, buffer, unsigned(length)) != -1;
}

void ZLibOutStream::flush(void)
{
    gzflush(m_gzFile, Z_SYNC_FLUSH);
}


OutStream *
trace::createZLibStream(const char *filename)
{
    gzFile file = gzopen(filename, "wb");
    if (!file) {
        return nullptr;
    }

    // Currently we only use gzip for offline compression, so aim for maximum
    // compression
    gzsetparams(file, Z_BEST_COMPRESSION, Z_DEFAULT_STRATEGY);

    return new ZLibOutStream(file);
}
