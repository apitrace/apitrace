/**************************************************************************
 *
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


#include "trace_file.hpp"


#include <assert.h>
#include <string.h>

#include <zlib.h>
#include <gzguts.h>

#include "os.hpp"

#include <iostream>


using namespace trace;


class ZLibFile : public File {
public:
    ZLibFile(const std::string &filename = std::string(),
             File::Mode mode = File::Read);
    virtual ~ZLibFile();


    virtual bool supportsOffsets() const;
    virtual File::Offset currentOffset();
protected:
    virtual bool rawOpen(const std::string &filename, File::Mode mode);
    virtual bool rawWrite(const void *buffer, size_t length);
    virtual size_t rawRead(void *buffer, size_t length);
    virtual int rawGetc();
    virtual void rawClose();
    virtual void rawFlush();
    virtual bool rawSkip(size_t length);
    virtual int  rawPercentRead();
private:
    gzFile m_gzFile;
    double m_endOffset;
};

ZLibFile::ZLibFile(const std::string &filename,
                   File::Mode mode)
    : File(filename, mode),
      m_gzFile(NULL)
{
}

ZLibFile::~ZLibFile()
{
    close();
}

bool ZLibFile::rawOpen(const std::string &filename, File::Mode mode)
{
    m_gzFile = gzopen(filename.c_str(),
                      (mode == File::Write) ? "wb" : "rb");

    if (mode == File::Read && m_gzFile) {
        //XXX: unfortunately zlib doesn't support
        //     SEEK_END or we could've done:
        //m_endOffset = gzseek(m_gzFile, 0, SEEK_END);
        //gzrewind(m_gzFile);
        gz_state *state = (gz_state *)m_gzFile;
        off_t loc = lseek(state->fd, 0, SEEK_CUR);
        m_endOffset = lseek(state->fd, 0, SEEK_END);
        lseek(state->fd, loc, SEEK_SET);
    }

    return m_gzFile != NULL;
}

bool ZLibFile::rawWrite(const void *buffer, size_t length)
{
    return gzwrite(m_gzFile, buffer, length) != -1;
}

size_t ZLibFile::rawRead(void *buffer, size_t length)
{
    int ret = gzread(m_gzFile, buffer, length);
    return ret < 0 ? 0 : ret;
}

int ZLibFile::rawGetc()
{
    return gzgetc(m_gzFile);
}

void ZLibFile::rawClose()
{
    if (m_gzFile) {
        gzclose(m_gzFile);
        m_gzFile = NULL;
    }
}

void ZLibFile::rawFlush()
{
    gzflush(m_gzFile, Z_SYNC_FLUSH);
}

File::Offset ZLibFile::currentOffset()
{
    return File::Offset(gztell(m_gzFile));
}

bool ZLibFile::supportsOffsets() const
{
    return false;
}

bool ZLibFile::rawSkip(size_t)
{
    return false;
}

int ZLibFile::rawPercentRead()
{
    gz_state *state = (gz_state *)m_gzFile;
    return 100 * (lseek(state->fd, 0, SEEK_CUR) / m_endOffset);
}


File * File::createZLib(void) {
    return new ZLibFile;
}

bool File::isZLibCompressed(const std::string &filename)
{
    std::fstream stream(filename.c_str(),
                        std::fstream::binary | std::fstream::in);
    if (!stream.is_open())
        return false;

    unsigned char byte1, byte2;
    stream >> byte1;
    stream >> byte2;
    stream.close();

    return (byte1 == 0x1f && byte2 == 0x8b);
}

