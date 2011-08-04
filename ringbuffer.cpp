#include <string.h>
#include <assert.h>

#include "ringbuffer.hpp"

#include <iostream>

//#define DBG 1

using namespace OS;

Ringbuffer::Ringbuffer(long size)
    : m_size(size)
{
    m_buffer = new char[m_size];
    m_writePtr = m_buffer;
    m_readPtr = m_buffer;

    MutexInit(m_mutex);
}

Ringbuffer::~Ringbuffer()
{
    MutexDestroy(m_mutex);
    delete [] m_buffer;
}

long Ringbuffer::size() const
{
    return m_size;
}

int Ringbuffer::sizeToWrite() const
{
    if (m_writePtr > m_readPtr) {
        return m_size - (m_writePtr - m_readPtr);
    } else if (m_writePtr == m_readPtr) {
        return m_size;
    } else {
        return m_readPtr - m_writePtr;
    }
}

int Ringbuffer::sizeToRead() const
{
    return m_size - sizeToWrite();
}

void Ringbuffer::write(char *buffer, int size)
{
#if DBG
    std::cerr << "::write toRead = "<<sizeToRead()
              << ", toWrite = " << sizeToWrite()
              <<", bufSize = "<<size
              <<". "
              << std::endl;
#endif
    if (size > sizeToWrite()) {
        assert(0);
        return;
    }
    MutexLock(m_mutex);
    if (m_writePtr + size > m_buffer + m_size) {
        int endSize = (m_buffer + m_size) - m_writePtr;
        int beginSize = size - endSize;
        memcpy(m_writePtr, buffer, endSize);
        memcpy(m_buffer, buffer + endSize, beginSize);
        m_writePtr = m_buffer + beginSize;
    } else {
        memcpy(m_writePtr, buffer, size);
        m_writePtr += size;
    }
    MutexUnlock(m_mutex);
}

void Ringbuffer::read(char *buffer, int size)
{
    MutexLock(m_mutex);
    memcpy(buffer, m_readPtr, size);
    m_readPtr += size;
    MutexUnlock(m_mutex);
}

void Ringbuffer::reset()
{
    m_writePtr = m_buffer;
    m_readPtr = m_buffer;
}

char * Ringbuffer::readPointer() const
{
    return m_readPtr;
}

void Ringbuffer::readPointerAdvance(int size)
{
    MutexLock(m_mutex);
    if ((m_readPtr + size) > (m_buffer + m_size)) {
        m_readPtr = m_buffer + readOverflowsBy(size);
    } else {
        m_readPtr += size;
    }
    MutexUnlock(m_mutex);
}

bool Ringbuffer::readOverflows(int size) const
{
    return (m_readPtr + size) > (m_buffer + m_size);
}

int Ringbuffer::readOverflowsBy(int size) const
{
    return (m_readPtr + size) - (m_buffer + m_size);
}

char * Ringbuffer::buffer()
{
    return m_buffer;
}

