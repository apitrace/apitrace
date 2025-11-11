/*
 * Copyright © 2025 Igalia SL
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */


/* Fallback support for reading Zstandard files without the seek table.
 *
 * If the application crashed and we didn't get to write our seek table, then
 * the trace will still be parseable by the base Zstd streaming library.
 */


#include <zstd.h>

#include <iostream>
#include <algorithm>
#include <stdio.h>

#include <assert.h>
#include <string.h>

#include "trace_file.hpp"

// Size of the decompressed data that we keep cached -- rawRead() is typically
// called on small lengths, so we need to be able to return quickly by
// decompressing in bulk up front.
#define ZSTD_READ_BUFFER_SIZE (2 * 1024 * 1024)

using namespace trace;


class ZstdFile : public File {
public:
    ZstdFile(void);
    virtual ~ZstdFile();

protected:
    virtual bool rawOpen(const char *filename) override;
    virtual size_t rawRead(void *buffer, size_t length) override;
    virtual void rawClose(void) override;

    size_t containerSizeInBytes(void) const override;
    size_t containerBytesRead(void) const override;
    size_t dataBytesRead(void) const override;
    const char* containerType() const override;

private:
    inline size_t cacheRemaining(void) const
    {
        return m_cacheSize - m_cachePos;
    }

    void reloadCache(void);

private:
    FILE* m_fp;
    ZSTD_DStream* m_dstream;

    // Decompressed data cache for efficient small reads
    char* m_cache;
    size_t m_cacheSize;        // Amount of valid data in cache
    size_t m_cachePos;         // Current read position in cache

    // Input buffer for compressed data
    size_t m_inputSize;
    char* m_inputBuffer;
    ZSTD_inBuffer m_in; // Persistent input buffer state

    uint64_t m_dataBytesRead;      // Total decompressed bytes read
    uint64_t m_compressedBytesRead; // Total compressed bytes read
    std::streampos m_fileSize;      // Total file size
    bool m_eof;                     // Whether we've hit EOF
};

ZstdFile::ZstdFile(void)
    : File(),
      m_fp(nullptr),
      m_dstream(nullptr),
      m_cache(new char[ZSTD_READ_BUFFER_SIZE]),
      m_cacheSize(0),
      m_cachePos(0),
      // Allocate enough to store even the worst-case compressed frame for our 2MB seekable decompressed frame size.
      m_inputSize(ZSTD_compressBound(2 * 1024 * 1024)),
      m_inputBuffer(new char[m_inputSize]),
      m_in({nullptr, 0, 0}),
      m_dataBytesRead(0),
      m_compressedBytesRead(0),
      m_fileSize(0),
      m_eof(false)
{
}

ZstdFile::~ZstdFile()
{
    close();
    delete [] m_cache;
    delete [] m_inputBuffer;
}

bool ZstdFile::rawOpen(const char *filename)
{
    m_fp = fopen(filename, "rb");
    if (!m_fp) {
        return false;
    }

    // Get file size
    fseek(m_fp, 0, SEEK_END);
    m_fileSize = ftell(m_fp);
    fseek(m_fp, 0, SEEK_SET);

    m_dstream = ZSTD_createDStream();
    if (!m_dstream) {
        fclose(m_fp);
        m_fp = nullptr;
        return false;
    }

    size_t result = ZSTD_initDStream(m_dstream);
    if (ZSTD_isError(result)) {
        std::cerr << "error: failed to initialize zstd stream: "
                  << ZSTD_getErrorName(result) << "\n";
        ZSTD_freeDStream(m_dstream);
        m_dstream = nullptr;
        fclose(m_fp);
        m_fp = nullptr;
        return false;
    }

    m_dataBytesRead = 0;
    m_compressedBytesRead = 0;
    m_cacheSize = 0;
    m_cachePos = 0;
    m_in.src = m_inputBuffer;
    m_in.pos = 0;
    m_in.size = 0;
    m_eof = false;

    return true;
}

size_t ZstdFile::rawRead(void *buffer, size_t length)
{
    size_t totalRead = 0;

    while (totalRead < length) {
        // Copy from cache first
        size_t fromCache = std::min(cacheRemaining(), length - totalRead);
        if (fromCache > 0) {
            memcpy((char*)buffer + totalRead, m_cache + m_cachePos, fromCache);
            m_cachePos += fromCache;
            totalRead += fromCache;
        }

        // Refill cache if needed
        if (totalRead < length) {
            reloadCache();
            if (m_cacheSize == 0) {
                // Reached EOF
                break;
            }
        }
    }

    m_dataBytesRead += totalRead;
    return totalRead;
}

void ZstdFile::rawClose(void)
{
    if (m_dstream) {
        ZSTD_freeDStream(m_dstream);
        m_dstream = nullptr;
    }
    if (m_fp) {
        fclose(m_fp);
        m_fp = nullptr;
    }
}

void ZstdFile::reloadCache(void)
{
    if (m_eof) {
        m_cacheSize = 0;
        m_cachePos = 0;
        return;
    }

    for (;;) {
        // Fill the input buffer if it's not full.
        if (m_in.size < m_inputSize) {
            size_t bytesRead = fread(m_inputBuffer + m_in.size, 1, m_inputSize - m_in.size, m_fp);
            if (bytesRead == 0) {
                m_eof = true;
            } else {
                m_in.size += bytesRead;
                m_compressedBytesRead += bytesRead;
            }
        }

        ZSTD_outBuffer output = { m_cache, ZSTD_READ_BUFFER_SIZE, 0 };
        size_t result = ZSTD_decompressStream(m_dstream, &output, &m_in);

        if (ZSTD_isError(result)) {
            std::cerr << "error: zstd decompression failed: "
                        << ZSTD_getErrorName(result) << "\n";
            m_eof = true;
            return;
        }

        if (output.pos == 0 && !m_eof) {
            // If no output was generated, then we need more input data even
            // though we haven't fully consumed the buffer.  Move the remainder
            // down and loop to refill the input and try again.
            size_t toMove = m_in.size - m_in.pos;
            memmove(m_inputBuffer, m_inputBuffer + m_in.pos, toMove);
            m_in.pos = 0;
            m_in.size = toMove;
        } else {
            // Return the data that we decompressed, regardless of whether the
            // output buffer is full.  This is also the path of EOF -- nothing
            // left to fread(), and no frames left to decompress produces a
            // cacheSize == 0.
            m_cachePos = 0;
            m_cacheSize = output.pos;
            return;
        }
    }
}

size_t ZstdFile::containerSizeInBytes(void) const
{
    return m_fileSize;
}

size_t ZstdFile::containerBytesRead(void) const
{
    return m_compressedBytesRead;
}

size_t ZstdFile::dataBytesRead(void) const
{
    return m_dataBytesRead;
}

const char* ZstdFile::containerType() const
{
    return "Zstandard";
}

File* File::createZstd(void)
{
    return new ZstdFile();
}
