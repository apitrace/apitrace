#ifndef TRACE_FILE_HPP
#define TRACE_FILE_HPP

#include <string>
#include <fstream>

namespace Trace {

class File {
public:
    enum Mode {
        Read,
        Write
    };
public:
    File(const std::string &filename = std::string(),
         File::Mode mode = File::Read);
    virtual ~File();

    bool isOpened() const;
    File::Mode mode() const;
    std::string filename() const;

    bool open(const std::string &filename, File::Mode mode);
    bool write(const void *buffer, int length);
    bool read(void *buffer, int length);
    void close();
    void flush();
    int getc();

protected:
    virtual bool rawOpen(const std::string &filename, File::Mode mode) = 0;
    virtual bool rawWrite(const void *buffer, int length) = 0;
    virtual bool rawRead(void *buffer, int length) = 0;
    virtual int rawGetc() = 0;
    virtual void rawClose() = 0;
    virtual void rawFlush() = 0;

protected:
    std::string m_filename;
    File::Mode m_mode;
    bool m_isOpened;
};

class ZLibFile : public File {
public:
    ZLibFile(const std::string &filename = std::string(),
             File::Mode mode = File::Read);
    virtual ~ZLibFile();

protected:
    virtual bool rawOpen(const std::string &filename, File::Mode mode);
    virtual bool rawWrite(const void *buffer, int length);
    virtual bool rawRead(void *buffer, int length);
    virtual int rawGetc();
    virtual void rawClose();
    virtual void rawFlush();
private:
    void *m_gzFile;
};

namespace snappy {
    class File;
}

#define SNAPPY_CHUNK_SIZE (1 * 1024 * 1024)
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
    virtual void rawFlush();

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

#endif
