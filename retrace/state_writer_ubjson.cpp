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


static void
encodeBase64String(std::ostream &os, const unsigned char *bytes, size_t size) {
    const char *table64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    unsigned char c0, c1, c2, c3;
    char buf[4];

    while (size >= 3) {
        c0 = bytes[0] >> 2;
        c1 = ((bytes[0] & 0x03) << 4) | ((bytes[1] & 0xf0) >> 4);
        c2 = ((bytes[1] & 0x0f) << 2) | ((bytes[2] & 0xc0) >> 6);
        c3 = bytes[2] & 0x3f;

        buf[0] = table64[c0];
        buf[1] = table64[c1];
        buf[2] = table64[c2];
        buf[3] = table64[c3];

        os.write(buf, 4);

        bytes += 3;
        size -= 3;
    }

    if (size > 0) {
        c0 = bytes[0] >> 2;
        c1 = ((bytes[0] & 0x03) << 4);
        buf[2] = '=';
        buf[3] = '=';

        if (size > 1) {
            c1 |= ((bytes[1] & 0xf0) >> 4);
            c2 = ((bytes[1] & 0x0f) << 2);
            if (size > 2) {
                c2 |= ((bytes[2] & 0xc0) >> 6);
                c3 = bytes[2] & 0x3f;
                buf[3] = table64[c3];
            }
            buf[2] = table64[c2];
        }
        buf[1] = table64[c1];
        buf[0] = table64[c0];

        os.write(buf, 4);
    }
}


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
    beginObject(void) {
        os.put(MARKER_OBJECT_BEGIN);
    }

    void
    endObject(void) {
        os.put(MARKER_OBJECT_END);
    }

    void
    beginMember(const char * name) {
        writeString(name);
    }

    void
    endMember(void) {
    }

    void
    beginArray(void) {
        os.put(MARKER_ARRAY_BEGIN);
    }

    void
    endArray(void) {
        os.put(MARKER_ARRAY_END);
    }

    void
    writeString(const char *s) {
        os.put(MARKER_STRING);
        size_t len = strlen(s);
        writeUInt(len);
        if (true) {
            for (size_t i = 0; i < len; ++i) {
                char c = s[i];
                os.put((signed char)c >= 0 ? c : '?');
            }
        } else {
            os.write(s, len);
        }
    }

    void
    writeBlob(const void *bytes, size_t size) {
        if (true) {
            beginArray();
            os.put(MARKER_TYPE);
            os.put(MARKER_UINT8);
            os.put(MARKER_COUNT);
            writeUInt(size);
            os.write((const char *)bytes, size);
            endArray();
        } else {
            // XXX: Only for testing with simpleubjson
            os.put(MARKER_STRING);
            size_t len = (size + 2)/3*4;
            writeUInt(len);
            encodeBase64String(os, (const unsigned char *)bytes, size);
        }
    }

    void
    writeNull(void) {
        os.put(MARKER_NULL);
    }

    void
    writeBool(bool b) {
        os.put(b ? MARKER_TRUE : MARKER_FALSE);
    }

    void
    writeSInt(signed long long i) {
        if (INT8_MIN <= i && i <= INT8_MAX) {
            os.put(MARKER_INT8);
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
    writeUInt(unsigned long long u) {
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
    writeFloat(float f) {
        os.put(MARKER_FLOAT32);
        Float32 u;
        u.f = f;
        u.i = bigEndian32(u.i);
        os.write((const char *)&u.i, sizeof u.i);
    }

    void
    writeFloat(double f) {
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
