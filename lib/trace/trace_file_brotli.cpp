/**************************************************************************
 *
 * Copyright 2016 VMware, Inc.
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


#include "trace_file.hpp"


#include <assert.h>
#include <string.h>

#include <iostream>

#include <brotli/decode.h>

#include "os.hpp"


using namespace trace;


class BrotliFile : public File {
public:
    BrotliFile(void);
    virtual ~BrotliFile();

protected:
    virtual bool rawOpen(const char *filename) override;
    virtual size_t rawRead(void *buffer, size_t length) override;
    virtual int rawGetc(void) override;
    virtual void rawClose(void) override;
    virtual bool rawSkip(size_t length) override;

    size_t containerSizeInBytes(void) const override;
    size_t containerBytesRead(void) const override;
    size_t dataBytesRead(void) const override;
    const char *containerType(void) const override;
private:
    BrotliDecoderState *state;
    mutable std::ifstream m_stream;
    static const size_t kFileBufferSize = 65536;
    uint8_t input[kFileBufferSize];
    const uint8_t* next_in;
    size_t available_in;
    std::streampos m_endPos = 0;
    size_t m_dataBytesRead = 0;
};

BrotliFile::BrotliFile(void)
{
    state = BrotliDecoderCreateInstance(nullptr, nullptr, nullptr);
    available_in = 0;
    next_in = input;
}

BrotliFile::~BrotliFile()
{
    close();
    BrotliDecoderDestroyInstance(state);
}

bool BrotliFile::rawOpen(const char *filename)
{
    std::ios_base::openmode fmode = std::fstream::binary
                                  | std::fstream::in;

    m_stream.open(filename, fmode);

    if (m_stream.is_open()) {
        m_stream.seekg(0, std::ios::end);
        m_endPos = m_stream.tellg();
        m_stream.seekg(0, std::ios::beg);
        m_dataBytesRead = 0;
    }

    return m_stream.is_open();
}

size_t BrotliFile::rawRead(void *buffer, size_t length)
{
    uint8_t* output = (uint8_t *)buffer;
    size_t total_out;
    size_t available_out = length;
    uint8_t* next_out = output;

    while (true) {
        BrotliDecoderResult result;
        result = BrotliDecoderDecompressStream(state,
                                               &available_in, &next_in,
                                               &available_out, &next_out, &total_out);
        if (result == BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT) {
            if (m_stream.fail()) {
                break;
            }
            m_stream.read((char *)input, kFileBufferSize);
            available_in = kFileBufferSize;
            if (m_stream.fail()) {
                available_in = m_stream.gcount();
                if (!available_in) {
                    break;
                }
            }
            next_in = input;
        } else {
            assert(result == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT ||
                   result == BROTLI_DECODER_RESULT_SUCCESS);
            break;
        }
    }

    assert(next_out - output <= length);

    m_dataBytesRead += static_cast<size_t>(next_out - output);

    return next_out - output;
}

int BrotliFile::rawGetc()
{
    unsigned char c;
    if (rawRead(&c, 1) != 1) {
        return -1;
    }
    return c;
}

void BrotliFile::rawClose()
{
    m_stream.close();
}

bool BrotliFile::rawSkip(size_t)
{
    return false;
}

size_t BrotliFile::containerSizeInBytes(void) const {
    return static_cast<size_t>(m_endPos);
}

size_t BrotliFile::containerBytesRead(void) const {
    return static_cast<size_t>(m_stream.tellg());
}

size_t BrotliFile::dataBytesRead(void) const {
    return m_dataBytesRead;
}

const char *BrotliFile::containerType(void) const {
    return "Brotli";
}

File * File::createBrotli(void) {
    return new BrotliFile;
}
