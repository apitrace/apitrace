#include "trace_file.hpp"

#include <assert.h>
#include <string.h>

#include <zlib.h>
#include <snappy.h>

#include <iostream>

using namespace Trace;

File::File(const std::string &filename,
           File::Mode mode)
    : m_filename(filename),
      m_mode(mode),
      m_isOpened(false)
{
    if (!m_filename.empty()) {
        open(m_filename, m_mode);
    }
}

static size_t writtenRef = 0;

File::~File()
{
    std::cerr << "written ref = "<<writtenRef << std::endl;
    close();
}

bool File::isOpened() const
{
    return m_isOpened;
}

File::Mode File::mode() const
{
    return m_mode;
}

std::string File::filename() const
{
    return m_filename;
}

bool File::open(const std::string &filename, File::Mode mode)
{
    if (m_isOpened) {
        close();
    }
    m_isOpened = rawOpen(filename, mode);
    m_mode = mode;
    return m_isOpened;
}

bool File::write(const void *buffer, int length)
{
    if (!m_isOpened || m_mode != File::Write) {
        return false;
    }
    writtenRef += length;
    return rawWrite(buffer, length);
}

bool File::read(void *buffer, int length)
{
    if (!m_isOpened || m_mode != File::Read) {
        return false;
    }
    return rawRead(buffer, length);
}

void File::close()
{
    if (m_isOpened) {
        rawClose();
        m_isOpened = false;
    }
}

void File::flush()
{
    rawFlush();
}

char File::getc()
{
    if (!m_isOpened || m_mode != File::Read) {
        return 0;
    }
    return rawGetc();
}

ZLibFile::ZLibFile(const std::string &filename,
                   File::Mode mode)
    : File(filename, mode),
      m_gzFile(NULL)
{
}

ZLibFile::~ZLibFile()
{
}

bool ZLibFile::rawOpen(const std::string &filename, File::Mode mode)
{
    m_gzFile = gzopen(filename.c_str(),
                      (mode == File::Write) ? "wb" : "rb");
    return m_gzFile != NULL;
}

bool ZLibFile::rawWrite(const void *buffer, int length)
{
    return gzwrite(m_gzFile, buffer, length) != -1;
}

bool ZLibFile::rawRead(void *buffer, int length)
{
    return gzread(m_gzFile, buffer, length) != -1;
}

char ZLibFile::rawGetc()
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

static size_t written = 0;
static size_t writtenComp = 0;

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
        flushCache();
    }
    return m_stream.is_open();
}

bool SnappyFile::rawWrite(const void *buffer, int length)
{
    static int bufWritten = 0;
    if (bufWritten < 500) {
        const char *cbuffer = (const char*)buffer;
        for (int i = 0; i < length; ++i) {
            std::cerr << "--- "<<bufWritten << ") "<< (int) cbuffer[i]
                      << std::endl;
            ++ bufWritten;
        }
    }
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
    std::cerr << "Reading byte = "<< (m_cachePtr - m_cache) << std::endl;
    if (freeCacheSize() > length) {
        memcpy(buffer, m_cachePtr, length);
        m_cachePtr += length;
    } else if (freeCacheSize() == length) {
        memcpy(buffer, m_cachePtr, length);
        m_cachePtr += length;
        assert(0);
        flushCache();
    } else {
        int sizeToRead = length;
        int offset = 0;
        assert(0);
        while (sizeToRead) {
            int chunkSize = std::min(freeCacheSize(), sizeToRead);
            offset = length - sizeToRead;
            memcpy((char*)buffer + offset, m_cachePtr, chunkSize);
            m_cachePtr += chunkSize;
            sizeToRead -= chunkSize;
            if (sizeToRead > 0)
                flushCache();
        }
    }

    return true;
}

char SnappyFile::rawGetc()
{
    char c;
    rawRead(&c, 1);
    return c;
}

void SnappyFile::rawClose()
{
    flushCache();
    std::cerr << "written = "<< written
              <<", comp = "<< writtenComp
              << std::endl;
    m_stream.close();
    delete [] m_cache;
    m_cache = NULL;
    m_cachePtr = NULL;
}

void SnappyFile::rawFlush()
{
    m_stream.flush();
}

void SnappyFile::flushCache()
{
    if (m_mode == File::Write) {
        size_t compressedLength;

        static bool first = true;
        if (first) {
            std::cerr << "Buffer = [";
            for (int i = 0; i < 512; ++i) {
                std::cerr << i << " ) "<< (int)m_cache[i] << std::endl;
            }
            std::cerr << "]"<<std::endl;
            first = false;
        }

        ::snappy::RawCompress(m_cache, SNAPPY_CHUNK_SIZE - freeCacheSize(),
                              m_compressedCache, &compressedLength);

        m_stream << compressedLength;
        m_stream.write(m_compressedCache, compressedLength);
        std::cerr << "compressed length = "<<compressedLength
                  <<" cache size = "<<(SNAPPY_CHUNK_SIZE - freeCacheSize())
                  <<" (ref = " << SNAPPY_CHUNK_SIZE <<")"
                  << std::endl;
        written += SNAPPY_CHUNK_SIZE - freeCacheSize();
        writtenComp += compressedLength;
        m_cachePtr = m_cache;
    } else if (m_mode == File::Read) {
        if (m_stream.eof())
            return;
        //assert(m_cachePtr == m_cache + m_cacheSize);
        size_t compressedLength;
        m_stream >> compressedLength;
        m_stream.read((char*)m_compressedCache, compressedLength);
        ::snappy::GetUncompressedLength(m_compressedCache, compressedLength,
                                        &m_cacheSize);
        std::cerr << "compressed length = "<<compressedLength
                  <<" cache size = "<<m_cacheSize
                  <<" (ref = " << SNAPPY_CHUNK_SIZE <<")"
                  << std::endl;
        if (m_cache)
            delete [] m_cache;
        createCache(m_cacheSize);
        ::snappy::RawUncompress(m_compressedCache, compressedLength,
                                m_cache);
        static bool first = true;
        if (first) {
            std::cerr << "Buffer = [";
            for (int i = 0; i < 512; ++i) {
                std::cerr << i << " ) "<< (int)m_cache[i] << std::endl;
            }
            std::cerr << "]"<<std::endl;
            first = false;
        }
    }
}

void SnappyFile::createCache(size_t size)
{
    m_cache = new char[size];
    m_cachePtr = m_cache;
    m_cacheSize = size;
}
