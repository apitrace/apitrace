/**************************************************************************
 *
 * Copyright 2011 Zack Rusin
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/


#pragma once

#include <fstream>
#include <stdint.h>


namespace trace {

class File {
public:
    struct Offset {
        Offset(uint64_t _chunk = 0, uint32_t _offsetInChunk = 0)
            : chunk(_chunk),
              offsetInChunk(_offsetInChunk)
        {}
        uint64_t chunk;
        uint32_t offsetInChunk;
    };

public:
    static File *createZLib(void);
    static File *createBrotli(void);
    static File *createSnappy(void);
    static File *createForRead(const char *filename);
public:
    File(void);
    virtual ~File();

    bool isOpened(void) const;

    bool open(const char *filename);
    size_t read(void *buffer, size_t length);
    void close(void);
    int getc(void);
    bool skip(size_t length);
    int percentRead(void);

    virtual bool supportsOffsets(void) const;
    virtual File::Offset currentOffset(void) const;
    virtual void setCurrentOffset(const File::Offset &offset);
protected:
    virtual bool rawOpen(const char *filename) = 0;
    virtual size_t rawRead(void *buffer, size_t length) = 0;
    virtual int rawGetc(void) = 0;
    virtual void rawClose(void) = 0;
    virtual bool rawSkip(size_t length) = 0;
    virtual int rawPercentRead(void) = 0;

protected:
    bool m_isOpened = false;
};

inline bool File::isOpened(void) const
{
    return m_isOpened;
}

inline bool File::open(const char *filename)
{
    if (m_isOpened) {
        close();
    }
    m_isOpened = rawOpen(filename);

    return m_isOpened;
}

inline size_t File::read(void *buffer, size_t length)
{
    if (!m_isOpened) {
        return 0;
    }
    return rawRead(buffer, length);
}

inline int File::percentRead(void)
{
    if (!m_isOpened) {
        return 0;
    }
    return rawPercentRead();
}

inline void File::close(void)
{
    if (m_isOpened) {
        rawClose();
        m_isOpened = false;
    }
}

inline int File::getc(void)
{
    if (!m_isOpened) {
        return -1;
    }
    return rawGetc();
}

inline bool File::skip(size_t length)
{
    if (!m_isOpened) {
        return false;
    }
    return rawSkip(length);
}


inline bool
operator<(const File::Offset &one, const File::Offset &two)
{
    return one.chunk < two.chunk ||
            (one.chunk == two.chunk && one.offsetInChunk < two.offsetInChunk);
}

inline bool
operator==(const File::Offset &one, const File::Offset &two)
{
    return one.chunk == two.chunk &&
            one.offsetInChunk == two.offsetInChunk;
}

inline bool
operator>=(const File::Offset &one, const File::Offset &two)
{
    return one.chunk > two.chunk ||
            (one.chunk == two.chunk && one.offsetInChunk >= two.offsetInChunk);
}

inline bool
operator>(const File::Offset &one, const File::Offset &two)
{
    return two < one;
}

inline bool
operator<=(const File::Offset &one, const File::Offset &two)
{
    return two >= one;
}


} /* namespace trace */
