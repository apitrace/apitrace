#ifndef TRACE_FILE_HPP
#define TRACE_FILE_HPP

#include <string>

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
    char getc();

protected:
    virtual bool rawOpen(const std::string &filename, File::Mode mode) = 0;
    virtual bool rawWrite(const void *buffer, int length) = 0;
    virtual bool rawRead(void *buffer, int length) = 0;
    virtual char rawGetc() = 0;
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
    virtual char rawGetc();
    virtual void rawClose();
    virtual void rawFlush();
private:
    void *m_gzFile;
};

}

#endif
