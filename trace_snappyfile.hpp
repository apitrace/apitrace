#ifndef TRACE_SNAPPYFILE_HPP
#define TRACE_SNAPPYFILE_HPP

#include "trace_file.hpp"

#include <string>
#include <fstream>

namespace snappy {
    class File;
}

namespace Trace {

#define SNAPPY_CHUNK_SIZE (1 * 1024 * 1024)

#define SNAPPY_BYTE1 'a'
#define SNAPPY_BYTE2 't'


class SnappyFile : public File {
public:
    SnappyFile(const std::string &filename = std::string(),
               File::Mode mode = File::Read);
    virtual ~SnappyFile();

protected:
    virtual bool rawOpen(const std::string &filename, File::Mode mode);
    virtual bool rawWrite(const void *buffer, int length);
    virtual bool rawRead(void *buffer, int length);
    virtual int rawGetc();
    virtual void rawClose();
    virtual void rawFlush(FlushType type);

private:
    inline int freeCacheSize() const
    {
        if (m_cacheSize > 0)
            return m_cacheSize - (m_cachePtr - m_cache);
        else
            return 0;
    }
    void flushCache();
    void createCache(size_t size);
private:
    std::fstream m_stream;
    char *m_cache;
    char *m_cachePtr;
    size_t m_cacheSize;

    char *m_compressedCache;
};

}

#endif // TRACE_SNAPPYFILE_HPP
