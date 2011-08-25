#include "trace_snappyfile.hpp"

#include <snappy.h>

#include <assert.h>
#include <string.h>

using namespace Trace;

SnappyFile::SnappyFile(const std::string &filename,
                              File::Mode mode)
    : File(),
      m_cache(0),
      m_cachePtr(0),
      m_cacheSize(0)
{
    m_compressedCache = new char[SNAPPY_CHUNK_SIZE];
}

SnappyFile::~SnappyFile()
{
    delete [] m_compressedCache;
}

bool SnappyFile::rawOpen(const std::string &filename, File::Mode mode)
{
    std::ios_base::openmode fmode = std::fstream::binary;
    if (mode == File::Write) {
        fmode |= (std::fstream::out | std::fstream::trunc);
        createCache(SNAPPY_CHUNK_SIZE);
    } else if (mode == File::Read) {
        fmode |= std::fstream::in;
    }

    m_stream.open(filename.c_str(), fmode);

    //read in the initial buffer if we're reading
    if (m_stream.is_open() && mode == File::Read) {
        // read the snappy file identifier
        unsigned char byte1, byte2;
        m_stream >> byte1;
        m_stream >> byte2;
        assert(byte1 == SNAPPY_BYTE1 && byte2 == SNAPPY_BYTE2);

        flushCache();
    } else if (m_stream.is_open() && mode == File::Write) {
        // write the snappy file identifier
        m_stream << SNAPPY_BYTE1;
        m_stream << SNAPPY_BYTE2;
    }
    return m_stream.is_open();
}

bool SnappyFile::rawWrite(const void *buffer, int length)
{
    if (freeCacheSize() > length) {
        memcpy(m_cachePtr, buffer, length);
        m_cachePtr += length;
    } else if (freeCacheSize() == length) {
        memcpy(m_cachePtr, buffer, length);
        m_cachePtr += length;
        flushCache();
    } else {
        int sizeToWrite = length;

        while (sizeToWrite >= freeCacheSize()) {
            int endSize = freeCacheSize();
            int offset = length - sizeToWrite;
            memcpy(m_cachePtr, (char*)buffer + offset, endSize);
            sizeToWrite -= endSize;
            m_cachePtr += endSize;
            flushCache();
        }
        if (sizeToWrite) {
            int offset = length - sizeToWrite;
            memcpy(m_cachePtr, (char*)buffer + offset, sizeToWrite);
            m_cachePtr += sizeToWrite;
        }
    }

    return true;
}

bool SnappyFile::rawRead(void *buffer, int length)
{
    if (m_stream.eof()) {
        return false;
    }
    if (freeCacheSize() > length) {
        memcpy(buffer, m_cachePtr, length);
        m_cachePtr += length;
    } else if (freeCacheSize() == length) {
        memcpy(buffer, m_cachePtr, length);
        m_cachePtr += length;
        flushCache();
    } else {
        int sizeToRead = length;
        int offset = 0;
        while (sizeToRead) {
            int chunkSize = std::min(freeCacheSize(), sizeToRead);
            offset = length - sizeToRead;
            memcpy((char*)buffer + offset, m_cachePtr, chunkSize);
            m_cachePtr += chunkSize;
            sizeToRead -= chunkSize;
            if (sizeToRead > 0)
                flushCache();
            if (!m_cacheSize)
                break;
        }
    }

    return true;
}

int SnappyFile::rawGetc()
{
    int c = 0;
    if (!rawRead(&c, 1))
        return -1;
    return c;
}

void SnappyFile::rawClose()
{
    flushCache();
    m_stream.close();
    delete [] m_cache;
    m_cache = NULL;
    m_cachePtr = NULL;
}

void SnappyFile::rawFlush(FlushType type)
{
    if (type == FlushDeep) {
        flushCache();
    }
    m_stream.flush();
}

void SnappyFile::flushCache()
{
    if (m_mode == File::Write) {
        size_t compressedLength;

        ::snappy::RawCompress(m_cache, SNAPPY_CHUNK_SIZE - freeCacheSize(),
                              m_compressedCache, &compressedLength);

        writeCompressedLength(compressedLength);
        m_stream.write(m_compressedCache, compressedLength);
        m_cachePtr = m_cache;
    } else if (m_mode == File::Read) {
        if (m_stream.eof())
            return;
        //assert(m_cachePtr == m_cache + m_cacheSize);
        size_t compressedLength;
        compressedLength = readCompressedLength();
        m_stream.read((char*)m_compressedCache, compressedLength);
        ::snappy::GetUncompressedLength(m_compressedCache, compressedLength,
                                        &m_cacheSize);
        if (m_cache)
            delete [] m_cache;
        createCache(m_cacheSize);
        ::snappy::RawUncompress(m_compressedCache, compressedLength,
                                m_cache);
    }
}

void SnappyFile::createCache(size_t size)
{
    m_cache = new char[size];
    m_cachePtr = m_cache;
    m_cacheSize = size;
}

void SnappyFile::writeCompressedLength(size_t value)
{
    m_stream.write((char*)&value, sizeof value);
}

size_t SnappyFile::readCompressedLength()
{
    size_t len;
    m_stream.read((char*)&len, sizeof len);
    return len;
}
