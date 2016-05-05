/**************************************************************************
 *
 * Copyright 2015 VMware, Inc
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

#include <fstream>

#include <assert.h>
#include <string.h>

#include <snappy.h>

#include "os.hpp"
#include "trace_snappy.hpp"


#define SNAPPY_CHUNK_SIZE (1 * 1024 * 1024)


using namespace trace;


class SnappyOutStream : public OutStream {
public:
    SnappyOutStream(const char *filename);
    ~SnappyOutStream();

    SnappyOutStream(void);
    bool write(const void *buffer, size_t length) override;
    void flush(void) override;
    bool isOpen(void) {
        return m_stream.is_open();
    }


private:
    void close(void);

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
    void createCache(size_t size);
    void writeCompressedLength(size_t length);
private:
    std::ofstream m_stream;
    size_t m_cacheMaxSize;
    size_t m_cacheSize;
    char *m_cache;
    char *m_cachePtr;

    char *m_compressedCache;
};

SnappyOutStream::SnappyOutStream(const char *filename)
    : m_cacheMaxSize(SNAPPY_CHUNK_SIZE),
      m_cacheSize(m_cacheMaxSize),
      m_cache(new char [m_cacheMaxSize]),
      m_cachePtr(m_cache)
{
    size_t maxCompressedLength =
        snappy::MaxCompressedLength(SNAPPY_CHUNK_SIZE);
    m_compressedCache = new char[maxCompressedLength];
    
    std::ios_base::openmode fmode = std::fstream::binary
                                  | std::fstream::out
                                  | std::fstream::trunc;
    m_stream.open(filename, fmode);
    if (m_stream.is_open()) {
        m_stream << SNAPPY_BYTE1;
        m_stream << SNAPPY_BYTE2;
        m_stream.flush();
    }
}

SnappyOutStream::~SnappyOutStream()
{
    close();
    delete [] m_compressedCache;
    delete [] m_cache;
}

bool SnappyOutStream::write(const void *buffer, size_t length)
{
    if (freeCacheSize() > length) {
        memcpy(m_cachePtr, buffer, length);
        m_cachePtr += length;
    } else if (freeCacheSize() == length) {
        memcpy(m_cachePtr, buffer, length);
        m_cachePtr += length;
        flushWriteCache();
    } else {
        size_t sizeToWrite = length;

        while (sizeToWrite >= freeCacheSize()) {
            size_t endSize = freeCacheSize();
            size_t offset = length - sizeToWrite;
            memcpy(m_cachePtr, (const char*)buffer + offset, endSize);
            sizeToWrite -= endSize;
            m_cachePtr += endSize;
            flushWriteCache();
        }
        if (sizeToWrite) {
            size_t offset = length - sizeToWrite;
            memcpy(m_cachePtr, (const char*)buffer + offset, sizeToWrite);
            m_cachePtr += sizeToWrite;
        }
    }

    return true;
}

void SnappyOutStream::close(void)
{
    flushWriteCache();
    m_stream.close();
    delete [] m_cache;
    m_cache = NULL;
    m_cachePtr = NULL;
}

void SnappyOutStream::flush(void)
{
    flushWriteCache();
    m_stream.flush();
}

void SnappyOutStream::flushWriteCache(void)
{
    size_t inputLength = usedCacheSize();

    if (inputLength) {
        size_t compressedLength;

        ::snappy::RawCompress(m_cache, inputLength,
                              m_compressedCache, &compressedLength);

        writeCompressedLength(compressedLength);
        m_stream.write(m_compressedCache, compressedLength);
        m_cachePtr = m_cache;
    }
    assert(m_cachePtr == m_cache);
}

void SnappyOutStream::writeCompressedLength(size_t length)
{
    unsigned char buf[4];
    buf[0] = length & 0xff; length >>= 8;
    buf[1] = length & 0xff; length >>= 8;
    buf[2] = length & 0xff; length >>= 8;
    buf[3] = length & 0xff; length >>= 8;
    assert(length == 0);
    m_stream.write((const char *)buf, sizeof buf);
}


OutStream *
trace::createSnappyStream(const char *filename)
{
    SnappyOutStream *outStream = new SnappyOutStream(filename);
    if (!outStream->isOpen()) {
        os::log("error: could not open %s for writing\n", filename);
        delete outStream;
        outStream = nullptr;
    }

    return outStream;
}
