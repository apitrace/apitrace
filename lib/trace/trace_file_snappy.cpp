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


/*
 * Snappy file format.
 * -------------------
 *
 * Snappy at its core is just a compressoin algorithm so we're
 * creating a new file format which uses snappy compression
 * to hold the trace data.
 *
 * The file is composed of a number of chunks, they are:
 * chunk {
 *     uint32 - specifying the length of the compressed data
 *     compressed data, in little endian
 * }
 * File can contain any number of such chunks.
 * The default size of an uncompressed chunk is specified in
 * SNAPPY_CHUNK_SIZE.
 *
 * Note:
 * Currently the default size for a a to-be-compressed data is
 * 1mb, meaning that the compressed data will be <= 1mb.
 * The reason it's 1mb is because it seems
 * to offer a pretty good compression/disk io speed ratio
 * but that might change.
 *
 */


#include <snappy.h>
#include <snappy-sinksource.h>

#include <iostream>
#include <algorithm>

#include <assert.h>
#include <string.h>

#include "trace_file.hpp"
#include "trace_snappy.hpp"


#define SNAPPY_CHUNK_SIZE (1 * 1024 * 1024)



using namespace trace;


class SnappyFile : public File {
public:
    SnappyFile(void);
    virtual ~SnappyFile();

    virtual bool supportsOffsets(void) const override;
    virtual File::Offset currentOffset(void) const override;
    virtual void setCurrentOffset(const File::Offset &offset) override;
protected:
    virtual bool rawOpen(const char *filename) override;
    virtual size_t rawRead(void *buffer, size_t length) override;
    virtual int rawGetc(void) override;
    virtual void rawClose(void) override;
    virtual bool rawSkip(size_t length) override;
    virtual int rawPercentRead(void) override;

private:
    inline size_t usedCacheSize(void) const
    {
        assert(m_cachePtr >= m_cache);
        return m_cachePtr - m_cache;
    }
    inline size_t freeCacheSize(void) const
    {
        assert(m_cacheSize >= usedCacheSize());
        if (m_cacheSize > 0) {
            return m_cacheSize - usedCacheSize();
        } else {
            return 0;
        }
    }
    inline bool endOfData(void) const
    {
        return m_stream.eof() && freeCacheSize() == 0;
    }
    void flushWriteCache(void);
    void flushReadCache(size_t skipLength = 0);
    void createCache(size_t size);
    size_t readCompressedLength();
private:
    std::ifstream m_stream;
    size_t m_cacheMaxSize;
    size_t m_cacheSize;
    char *m_cache;
    char *m_cachePtr;

    char *m_compressedCache;

    uint64_t m_currentChunkOffset;
    std::streampos m_endPos;
};

SnappyFile::SnappyFile(void)
    : File(),
      m_cacheMaxSize(SNAPPY_CHUNK_SIZE),
      m_cacheSize(m_cacheMaxSize),
      m_cache(new char [m_cacheMaxSize]),
      m_cachePtr(m_cache)
{
    size_t maxCompressedLength =
        snappy::MaxCompressedLength(SNAPPY_CHUNK_SIZE);
    m_compressedCache = new char[maxCompressedLength];
}

SnappyFile::~SnappyFile()
{
    close();
    delete [] m_compressedCache;
    delete [] m_cache;
}

bool SnappyFile::rawOpen(const char *filename)
{
    std::ios_base::openmode fmode = std::fstream::binary
                                  | std::fstream::in;

    m_stream.open(filename, fmode);

    //read in the initial buffer if we're reading
    if (m_stream.is_open()) {
        m_stream.seekg(0, std::ios::end);
        m_endPos = m_stream.tellg();
        m_stream.seekg(0, std::ios::beg);

        // read the snappy file identifier
        unsigned char byte1, byte2;
        m_stream >> byte1;
        m_stream >> byte2;
        assert(byte1 == SNAPPY_BYTE1 && byte2 == SNAPPY_BYTE2);

        flushReadCache();
    }
    return m_stream.is_open();
}

size_t SnappyFile::rawRead(void *buffer, size_t length)
{
    if (endOfData()) {
        return 0;
    }

    if (freeCacheSize() >= length) {
        memcpy(buffer, m_cachePtr, length);
        m_cachePtr += length;
    } else {
        size_t sizeToRead = length;
        size_t offset = 0;
        while (sizeToRead) {
            size_t chunkSize = std::min(freeCacheSize(), sizeToRead);
            offset = length - sizeToRead;
            memcpy((char*)buffer + offset, m_cachePtr, chunkSize);
            m_cachePtr += chunkSize;
            sizeToRead -= chunkSize;
            if (sizeToRead > 0) {
                flushReadCache();
            }
            if (!m_cacheSize) {
                return length - sizeToRead;
            }
        }
    }

    return length;
}

int SnappyFile::rawGetc(void)
{
    unsigned char c = 0;
    if (rawRead(&c, 1) != 1)
        return -1;
    return c;
}

void SnappyFile::rawClose(void)
{
    m_stream.close();
    delete [] m_cache;
    m_cache = NULL;
    m_cachePtr = NULL;
}

void SnappyFile::flushReadCache(size_t skipLength)
{
    //assert(m_cachePtr == m_cache + m_cacheSize);
    m_currentChunkOffset = m_stream.tellg();
    size_t compressedLength;
    compressedLength = readCompressedLength();
    if (!compressedLength) {
        // Reached end of file
        createCache(0);
        return;
    }

    m_stream.read((char*)m_compressedCache, compressedLength);
    if (m_stream.fail()) {
        std::cerr << "warning: unexpected end of file while reading trace\n";

        compressedLength = m_stream.gcount();
        if (!snappy::GetUncompressedLength(m_compressedCache, compressedLength,
                                           &m_cacheSize)) {
            createCache(0);
            return;
        }

        createCache(m_cacheSize);
        snappy::ByteArraySource source(m_compressedCache, compressedLength);

        snappy::UncheckedByteArraySink sink(m_cache);
        m_cacheSize = snappy::UncompressAsMuchAsPossible(&source, &sink);

        return;
    }

    if (!snappy::GetUncompressedLength(m_compressedCache, compressedLength,
                                       &m_cacheSize)) {
        createCache(0);
        return;
    }

    createCache(m_cacheSize);
    if (skipLength < m_cacheSize) {
        snappy::RawUncompress(m_compressedCache, compressedLength,
                              m_cache);
    }
}

void SnappyFile::createCache(size_t size)
{
    if (size > m_cacheMaxSize) {
        do {
            m_cacheMaxSize <<= 1;
        } while (size > m_cacheMaxSize);

        delete [] m_cache;
        m_cache = new char[size];
        m_cacheMaxSize = size;
    }

    m_cachePtr = m_cache;
    m_cacheSize = size;
}

size_t SnappyFile::readCompressedLength()
{
    unsigned char buf[4];
    size_t length;
    m_stream.read((char *)buf, sizeof buf);
    if (m_stream.fail()) {
        length = 0;
    } else {
        length  =  (size_t)buf[0];
        length |= ((size_t)buf[1] <<  8);
        length |= ((size_t)buf[2] << 16);
        length |= ((size_t)buf[3] << 24);
    }
    return length;
}

bool SnappyFile::supportsOffsets(void) const
{
    return true;
}

File::Offset SnappyFile::currentOffset(void) const
{
    File::Offset offset;
    offset.chunk = m_currentChunkOffset;
    offset.offsetInChunk = m_cachePtr - m_cache;
    return offset;
}

void SnappyFile::setCurrentOffset(const File::Offset &offset)
{
    // to remove eof bit
    m_stream.clear();
    // seek to the start of a chunk
    m_stream.seekg(offset.chunk, std::ios::beg);
    // load the chunk
    flushReadCache();
    assert(m_cacheSize >= offset.offsetInChunk);
    // seek within our cache to the correct location within the chunk
    m_cachePtr = m_cache + offset.offsetInChunk;

}

bool SnappyFile::rawSkip(size_t length)
{
    if (endOfData()) {
        return false;
    }

    if (freeCacheSize() >= length) {
        m_cachePtr += length;
    } else {
        size_t sizeToRead = length;
        while (sizeToRead) {
            size_t chunkSize = std::min(freeCacheSize(), sizeToRead);
            m_cachePtr += chunkSize;
            sizeToRead -= chunkSize;
            if (sizeToRead > 0) {
                flushReadCache(sizeToRead);
            }
            if (!m_cacheSize) {
                break;
            }
        }
    }

    return true;
}

int SnappyFile::rawPercentRead(void)
{
    return int(100 * (double(m_stream.tellg()) / double(m_endPos)));
}


File* File::createSnappy(void) {
    return new SnappyFile;
}
