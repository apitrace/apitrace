#ifndef OS_RINGBUFFER_HPP
#define OS_RINGBUFFER_HPP

#include "os_thread.hpp"

namespace OS {

class Ringbuffer
{
public:
    Ringbuffer(long size = 32 * 1024 * 1024);
    ~Ringbuffer();

    char *buffer();

    long size() const;

    int sizeToWrite() const;
    int sizeToRead() const;

    void write(char *buffer, int size);
    void read(char *buffer, int size);

    void reset();

    char *readPointer() const;
    void readPointerAdvance(int size);
    bool readOverflows(int size) const;
    int readOverflowsBy(int size) const;

private:
    char *m_buffer;
    long m_size;

    char *m_writePtr;
    char *m_readPtr;

    OS::Mutex m_mutex;
};

}

#endif
