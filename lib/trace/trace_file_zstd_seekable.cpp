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


/*
 * Seekable Zstandard file format trace support.
 *
 * Uses the standard zstd seekable format which consists of:
 * - Standard ZStandard compressed frames
 * - A seek table stored in a skippable frame at the end
 *
 * The seek table contains offsets to each frame, allowing efficient
 * random access without decompressing the entire file.
 *
 * For format specification, see:
 * https://github.com/facebook/zstd/blob/dev/contrib/seekable_format/zstd_seekable_compression_format.md
 */


#include <zstd_seekable.h>

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


class ZstdSeekableFile : public File {
public:
    ZstdSeekableFile(void);
    virtual ~ZstdSeekableFile();

    virtual bool supportsOffsets(void) const override;
    virtual File::Offset currentOffset(void) const override;
    virtual void setCurrentOffset(const File::Offset &offset) override;

protected:
    virtual bool rawOpen(const char *filename) override;
    virtual size_t rawRead(void *buffer, size_t length) override;
    virtual void rawClose(void) override;
    virtual bool rawSkip(size_t length) override;

    size_t containerSizeInBytes(void) const override;
    size_t containerBytesRead(void) const override;
    size_t dataBytesRead(void) const override;
    const char* containerType() const override;

private:
    inline size_t cacheRemaining(void) const
    {
        return m_cacheSize - m_cachePos;
    }

    inline bool endOfData(void) const
    {
        return m_currentOffset >= m_uncompressedSize;
    }

    void reloadCache(void);

private:
    FILE* m_fp;
    ZSTD_seekable* m_seekable;

    // Small read buffer for efficient single-byte reads
    char* m_cache;
    size_t m_cacheSize;        // Amount of valid data in cache
    size_t m_cachePos;         // Current read position in cache

    uint64_t m_currentOffset;  // Current decompressed file offset
    uint64_t m_uncompressedSize; // Total uncompressed size
    uint64_t m_compressedSize;   // Total compressed size
};

ZstdSeekableFile::ZstdSeekableFile(void)
    : File(),
      m_fp(nullptr),
      m_seekable(nullptr),
      m_cache(new char[ZSTD_READ_BUFFER_SIZE]),
      m_cacheSize(0),
      m_cachePos(0),
      m_currentOffset(0),
      m_uncompressedSize(0),
      m_compressedSize(0)
{
}

ZstdSeekableFile::~ZstdSeekableFile()
{
    close();
    delete [] m_cache;
}

bool ZstdSeekableFile::rawOpen(const char *filename)
{
    m_fp = fopen(filename, "rb");
    if (!m_fp) {
        return false;
    }

    m_seekable = ZSTD_seekable_create();
    if (!m_seekable) {
        fclose(m_fp);
        m_fp = nullptr;
        return false;
    }

    size_t result = ZSTD_seekable_initFile(m_seekable, m_fp);
    if (ZSTD_isError(result)) {
        std::cerr << "error: failed to initialize seekable zstd: "
                  << ZSTD_getErrorName(result) << "\n";
        ZSTD_seekable_free(m_seekable);
        m_seekable = nullptr;
        fclose(m_fp);
        m_fp = nullptr;
        return false;
    }

    // Get file size information
    unsigned numFrames = ZSTD_seekable_getNumFrames(m_seekable);
    if (numFrames == 0) {
        std::cerr << "error: no frames in zstd file\n";
        ZSTD_seekable_free(m_seekable);
        m_seekable = nullptr;
        fclose(m_fp);
        m_fp = nullptr;
        return false;
    }

    // Calculate total sizes
    m_uncompressedSize = 0;
    m_compressedSize = 0;
    for (unsigned i = 0; i < numFrames; ++i) {
        m_uncompressedSize += ZSTD_seekable_getFrameDecompressedSize(m_seekable, i);
        m_compressedSize += ZSTD_seekable_getFrameCompressedSize(m_seekable, i);
    }

    m_currentOffset = 0;
    m_cacheSize = 0;
    m_cachePos = 0;

    return true;
}

size_t ZstdSeekableFile::rawRead(void *buffer, size_t length)
{
    size_t totalRead = 0;
    while (totalRead < length && !endOfData()) {
        // Copy from cache first
        size_t fromCache = std::min(cacheRemaining(), length - totalRead);
        if (fromCache > 0) {
            memcpy((char*)buffer + totalRead, m_cache + m_cachePos, fromCache);
            m_cachePos += fromCache;
            m_currentOffset += fromCache;
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

    return totalRead;
}

void ZstdSeekableFile::rawClose(void)
{
    if (m_seekable) {
        ZSTD_seekable_free(m_seekable);
        m_seekable = nullptr;
    }
    if (m_fp) {
        fclose(m_fp);
        m_fp = nullptr;
    }
}

void ZstdSeekableFile::reloadCache(void)
{
    size_t result = ZSTD_seekable_decompress(m_seekable, m_cache, ZSTD_READ_BUFFER_SIZE,
                                             m_currentOffset);

    if (ZSTD_isError(result)) {
        std::cerr << "error: zstd decompression failed: "
                  << ZSTD_getErrorName(result) << "\n";
        m_cacheSize = 0;
        m_cachePos = 0;
        return;
    }

    m_cacheSize = result;
    m_cachePos = 0;
}

// Seekable handling is done inside of ZSTD_seekable, so we don't need the
// chunk/offset bookmarking that apitrace does.  Just save/restore the
// decompressed offset.  We use chunk as the storage since it's u64.
bool ZstdSeekableFile::supportsOffsets(void) const
{
    return true;
}

File::Offset ZstdSeekableFile::currentOffset(void) const
{
    File::Offset offset;
    offset.chunk = m_currentOffset;
    offset.offsetInChunk = 0;
    return offset;
}

void ZstdSeekableFile::setCurrentOffset(const File::Offset &offset)
{
    m_currentOffset = offset.chunk;
    // Invalidate cache, force refill on next read
    m_cacheSize = 0;
    m_cachePos = 0;
}

bool ZstdSeekableFile::rawSkip(size_t length)
{
    if (endOfData()) {
        return false;
    }

    // For seekable files, just advance the offset
    if (m_currentOffset + length <= m_uncompressedSize) {
        m_currentOffset += length;

        // Adjust cache or invalidate it
        if (m_cachePos + length <= m_cacheSize) {
            m_cachePos += length;
        } else {
            m_cacheSize = 0;
            m_cachePos = 0;
        }
        return true;
    }

    return false;
}

size_t ZstdSeekableFile::containerSizeInBytes(void) const
{
    return m_compressedSize;
}

size_t ZstdSeekableFile::containerBytesRead(void) const
{
    // This is approximate since we don't track compressed bytes read
    if (m_uncompressedSize == 0) {
        return 0;
    }
    return (m_currentOffset * m_compressedSize) / m_uncompressedSize;
}

size_t ZstdSeekableFile::dataBytesRead(void) const
{
    return m_currentOffset;
}

const char* ZstdSeekableFile::containerType() const
{
    return "Zstandard (seekable)";
}

File* File::createZstdSeekable(void)
{
    return new ZstdSeekableFile();
}
