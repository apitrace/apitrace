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


#include "trace_ostream.hpp"

#include <stdio.h>
#include <iostream>

#include <assert.h>
#include <string.h>

#include <zstd.h>
#include <zstd_seekable.h>

#include "os.hpp"


// Default frame size: 2MB (recommendation of Zstandard documentation)
#define ZSTD_FRAME_SIZE (2 * 1024 * 1024)

// Default compression level (3 is good balance of speed/ratio)
#define ZSTD_COMPRESSION_LEVEL 3


using namespace trace;


class ZstdOutStream : public OutStream {
public:
    ZstdOutStream(const char *filename,
                  int compressionLevel = ZSTD_COMPRESSION_LEVEL,
                  unsigned maxFrameSize = ZSTD_FRAME_SIZE);
    ~ZstdOutStream();

    bool write(const void *buffer, size_t length) override;
    void flush(void) override;
    bool isOpen(void) {
        return m_fp != nullptr;
    }

private:
    void close(void);
    bool flushOutput(void);

private:
    FILE* m_fp;
    ZSTD_seekable_CStream* m_cstream;
    char* m_outputBuffer;
    size_t m_outputBufferSize;
};

ZstdOutStream::ZstdOutStream(const char *filename,
                             int compressionLevel,
                             unsigned maxFrameSize)
    : m_fp(nullptr),
      m_cstream(nullptr),
      m_outputBuffer(nullptr),
      m_outputBufferSize(0)
{
    m_fp = fopen(filename, "wb");
    if (!m_fp) {
        return;
    }

    m_cstream = ZSTD_seekable_createCStream();
    if (!m_cstream) {
        fclose(m_fp);
        m_fp = nullptr;
        return;
    }

    // Initialize seekable compression stream
    // Parameters: compression level, add checksums, max frame size
    size_t result = ZSTD_seekable_initCStream(m_cstream,
                                              compressionLevel,
                                              0, // Add checksums for data integrity
                                              maxFrameSize);
    if (ZSTD_isError(result)) {
        std::cerr << "error: failed to initialize zstd compression: "
                  << ZSTD_getErrorName(result) << "\n";
        ZSTD_seekable_freeCStream(m_cstream);
        m_cstream = nullptr;
        fclose(m_fp);
        m_fp = nullptr;
        return;
    }

    // Allocate output buffer
    m_outputBufferSize = ZSTD_CStreamOutSize();
    m_outputBuffer = new char[m_outputBufferSize];
}

ZstdOutStream::~ZstdOutStream()
{
    close();
    delete [] m_outputBuffer;
}

bool ZstdOutStream::write(const void *buffer, size_t length)
{
    if (!m_fp || !m_cstream) {
        return false;
    }

    ZSTD_inBuffer input = { buffer, length, 0 };

    while (input.pos < input.size) {
        ZSTD_outBuffer output = { m_outputBuffer, m_outputBufferSize, 0 };

        size_t result = ZSTD_seekable_compressStream(m_cstream, &output, &input);

        if (ZSTD_isError(result)) {
            std::cerr << "error: zstd compression failed: "
                      << ZSTD_getErrorName(result) << "\n";
            return false;
        }

        if (output.pos > 0) {
            size_t written = fwrite(m_outputBuffer, 1, output.pos, m_fp);
            if (written != output.pos) {
                std::cerr << "error: failed to write compressed data\n";
                return false;
            }
        }
    }

    return true;
}

void ZstdOutStream::close(void)
{
    if (m_cstream && m_fp) {
        // Flush remaining data and write seek table
        size_t remaining;

        do {
            ZSTD_outBuffer output = { m_outputBuffer, m_outputBufferSize, 0 };
            remaining = ZSTD_seekable_endStream(m_cstream, &output);

            if (ZSTD_isError(remaining)) {
                std::cerr << "error: zstd stream finalization failed: "
                        << ZSTD_getErrorName(remaining) << "\n";
            }

            if (output.pos > 0) {
                size_t written = fwrite(m_outputBuffer, 1, output.pos, m_fp);
                if (written != output.pos) {
                    std::cerr << "error: failed to write final compressed data\n";
                }
            }
        } while (remaining > 0);
    }

    if (m_cstream) {
        ZSTD_seekable_freeCStream(m_cstream);
        m_cstream = nullptr;
    }

    if (m_fp) {
        fclose(m_fp);
        m_fp = nullptr;
    }
}

void ZstdOutStream::flush(void)
{
    if (!m_fp || !m_cstream) {
        return;
    }

    // On a flush, we want to end the current frame, but not write out the seek
    // table -- flush()es can happen by TraceLocalWriter's FLUSH_EVERY_MS.
    ZSTD_outBuffer output = { m_outputBuffer, m_outputBufferSize, 0 };
    ZSTD_seekable_endFrame(m_cstream, nullptr);
    if (output.pos > 0) {
        size_t written = fwrite(m_outputBuffer, 1, output.pos, m_fp);
        if (written != output.pos) {
            std::cerr << "error: failed to write final compressed data\n";
        }
    }

    fflush(m_fp);
}


OutStream *
trace::createZstdStream(const char *filename, int compressionLevel)
{
    ZstdOutStream *outStream = new ZstdOutStream(filename, compressionLevel);
    if (!outStream->isOpen()) {
        os::log("error: could not open %s for writing\n", filename);
        delete outStream;
        outStream = nullptr;
    }

    return outStream;
}
