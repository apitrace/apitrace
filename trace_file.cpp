#include "trace_file.hpp"

#include "trace_snappyfile.hpp"

#include <assert.h>
#include <string.h>

#include <zlib.h>

#include "os.hpp"

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
