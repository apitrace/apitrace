#include "trace_file.hpp"

#include <assert.h>
#include <string.h>

#include <zlib.h>
#include <snappy.h>

#include "os.hpp"

#include <iostream>
#include <set>

using namespace Trace;

#define SNAPPY_BYTE1 'a'
#define SNAPPY_BYTE2 't'

static void cleanupHandler(int sig);

class FileCleanup
{
public:
    FileCleanup()
    {
        OS::CatchInterrupts(cleanupHandler);
    }

    ~FileCleanup()
    {
        flush();
        m_files.clear();
    }

    void addFile(Trace::File *file)
    {
        m_files.insert(file);
    }
    void removeFile(Trace::File *file)
    {
        m_files.erase(file);
    }

    void flush()
    {
        std::set<Trace::File*>::const_iterator itr;
        for (itr = m_files.begin(); itr != m_files.end(); ++itr) {
            (*itr)->flush(File::FlushDeep);
        }
    }

private:
    std::set<Trace::File*> m_files;
};
static FileCleanup s_cleaner;

static void cleanupHandler(int sig)
{
    s_cleaner.flush();
}

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


File::~File()
{
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

    if (m_isOpened) {
        s_cleaner.addFile(this);
    }
    return m_isOpened;
}

bool File::write(const void *buffer, int length)
{
    if (!m_isOpened || m_mode != File::Write) {
        return false;
    }
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
        s_cleaner.removeFile(this);
    }
}

void File::flush(FlushType type)
{
    rawFlush(type);
}

int File::getc()
{
    if (!m_isOpened || m_mode != File::Read) {
        return 0;
    }
    return rawGetc();
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


bool File::isSnappyCompressed(const std::string &filename)
{
    std::fstream stream(filename.c_str(),
                        std::fstream::binary | std::fstream::in);
    if (!stream.is_open())
        return false;

    unsigned char byte1, byte2;
    stream >> byte1;
    stream >> byte2;
    stream.close();

    return (byte1 == SNAPPY_BYTE1 && byte2 == SNAPPY_BYTE2);
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

void ZLibFile::rawFlush(FlushType type)
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

        m_stream << compressedLength;
        m_stream.write(m_compressedCache, compressedLength);
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
