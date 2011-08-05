#include "trace_file.hpp"

#include <zlib.h>

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
    return gzwrite(m_gzFile, buffer, length);
}

bool ZLibFile::rawRead(void *buffer, int length)
{
    return gzread(m_gzFile, buffer, length);
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
