/**************************************************************************
 *
 * Copyright 2015 VMware, Inc
 * Copyright 2011 Jose Fonseca
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


#include "state_writer.hpp"

#include <string.h>
#include <stdint.h>

#include "ubjson.hpp"


namespace {


using namespace ubjson;


class UBJSONStateWriter : public StateWriter
{
private:
    std::ostream &os;

public:
    UBJSONStateWriter(std::ostream &_os) :
        os(_os)
    {
        beginObject();
    }

    ~UBJSONStateWriter()
    {
        endObject();
    }

    void
    beginObject(void) override {
        os.put(MARKER_OBJECT_BEGIN);
    }

    void
    endObject(void) override {
        os.put(MARKER_OBJECT_END);
    }

    inline void
    _writeString(const char *s, size_t len) {
        writeUInt(len);
        // TODO: convert string from locale encoding to UTF-8
        for (size_t i = 0; i < len; ++i) {
            char c = s[i];
            os.put((signed char)c >= 0 ? c : '?');
        }
    }

    void
    _writeString(const char *s) {
    }

    void
    beginMember(const char * name) override {
        size_t len = strlen(name);
        _writeString(name, len);
    }

    void
    endMember(void) override {
    }

    void
    beginArray(void) override {
        os.put(MARKER_ARRAY_BEGIN);
    }

    void
    endArray(void) override {
        os.put(MARKER_ARRAY_END);
    }

    void
    writeString(const char *s) override {
        size_t len = strlen(s);
        if (len == 1 && (signed char)s[0] >= 0) {
            os.put(MARKER_CHAR);
            os.put(s[0]);
            return;
        }
        os.put(MARKER_STRING);
        _writeString(s, len);
    }

    void
    writeBlob(const void *bytes, size_t size) override {
        // Encode as a strongly-typed array of uint8 values
        // http://ubjson.org/type-reference/binary-data/
        // http://ubjson.org/type-reference/container-types/#optimized-format
        beginArray();
        os.put(MARKER_TYPE);
        os.put(MARKER_UINT8);
        os.put(MARKER_COUNT);
        writeUInt(size);
        os.write((const char *)bytes, size);
    }

    void
    writeNull(void) override {
        os.put(MARKER_NULL);
    }

    void
    writeBool(bool b) override {
        os.put(b ? MARKER_TRUE : MARKER_FALSE);
    }

    void
    writeSInt(signed long long i) override {
        if (INT8_MIN <= i && i <= INT8_MAX) {
            os.put(MARKER_INT8);
            os.put((char)i);
            return;
        }
        if (0 <= i && i <= UINT8_MAX) {
            os.put(MARKER_UINT8);
            os.put((char)i);
            return;
        }
        if (INT16_MIN <= i && i <= INT16_MAX) {
            os.put(MARKER_INT16);
            uint16_t u16 = bigEndian16((int16_t)i);
            os.write((const char *)&u16, sizeof u16);
            return;
        }
        if (INT32_MIN <= i && i <= INT32_MAX) {
            os.put(MARKER_INT32);
            uint32_t u32 = bigEndian32((int32_t)i);
            os.write((const char *)&u32, sizeof u32);
            return;
        }
        os.put(MARKER_INT64);
        uint64_t u64 = bigEndian64(i);
        os.write((const char *)&u64, sizeof u64);
    }

    void
    writeUInt(unsigned long long u) override {
        if (u <= UINT8_MAX) {
            os.put(MARKER_UINT8);
            uint8_t u8 = u;
            os.put(u8);
            return;
        }
        if (u <= INT16_MAX) {
            os.put(MARKER_INT16);
            uint16_t u16 = bigEndian16(u);
            os.write((const char *)&u16, sizeof u16);
            return;
        }
        if (u <= INT32_MAX) {
            os.put(MARKER_INT32);
            uint32_t u32 = bigEndian32(u);
            os.write((const char *)&u32, sizeof u32);
            return;
        }
        os.put(MARKER_INT64);
        u = bigEndian64(u);
        // XXX: We should fall back to high-precision when INT64_MAX < u <= UINT64_MAX?
        os.write((const char *)&u, sizeof u);
    }

    void
    writeFloat(float f) override {
        os.put(MARKER_FLOAT32);
        Float32 u;
        u.f = f;
        u.i = bigEndian32(u.i);
        os.write((const char *)&u.i, sizeof u.i);
    }

    void
    writeFloat(double f) override {
        os.put(MARKER_FLOAT64);
        Float64 u;
        u.f = f;
        u.i = bigEndian64(u.i);
        os.write((const char *)&u.i, sizeof u.i);
    }
};


}


StateWriter *
createUBJSONStateWriter(std::ostream &os)
{
    return new UBJSONStateWriter(os);
}
