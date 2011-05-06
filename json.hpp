/**************************************************************************
 *
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

/*
 * Trace writing functions.
 */

#ifndef _JSON_HPP_
#define _JSON_HPP_

#include <assert.h>
#include <stddef.h>
#include <wchar.h>

#include <ostream>
#include <iomanip>


class JSONWriter
{
private:
    std::ostream &os;

    int level;
    bool value;
    char space;

    void newline(void) {
        os << "\n";
        for (int i = 0; i < level; ++i) 
            os << "  ";
    }

    void separator(void) {
        if (value) {
            os << ",";
            switch (space) {
            case '\0':
                break;
            case '\n':
                newline();
                break;
            default:
                os << space;
                break;
            }
        } else {
            if (space == '\n') {
                newline();
            }
        }
    }

    void escapeAsciiString(const char *str) {
        os << "\"";

        const unsigned char *src = (const unsigned char *)str;
        unsigned char c;
        while ((c = *src++)) {
            if ((c == '\"') ||
                (c == '\\')) {
                // escape character
                os << '\\' << (unsigned char)c;
            } else if ((c >= 0x20 && c <= 0x7e) ||
                        c == '\t' ||
                        c == '\r' ||
                        c == '\n') {
                // pass-through character
                os << (unsigned char)c;
            } else {
                assert(0);
                os << "?";
            }
        }

        os << "\"";
    }

    void escapeUnicodeString(const char *str) {
        os << "\"";

        const char *locale = setlocale(LC_CTYPE, "");
        const char *src = str;
        mbstate_t state;

        memset(&state, 0, sizeof state);

        do {
            // Convert characters one at a time in order to recover from
            // conversion errors
            wchar_t c;
            size_t written = mbsrtowcs(&c, &src, 1, &state);
            if (written == 0) {
                // completed
                break;
            } if (written == (size_t)-1) {
                // conversion error -- skip 
                os << "?";
                do {
                    ++src;
                } while (*src & 0x80);
            } else if ((c == '\"') ||
                       (c == '\\')) {
                // escape character
                os << '\\' << (unsigned char)c;
            } else if ((c >= 0x20 && c <= 0x7e) ||
                        c == '\t' ||
                        c == '\r' ||
                        c == '\n') {
                // pass-through character
                os << (unsigned char)c;
            } else {
                // unicode
                os << "\\u" << std::setfill('0') << std::hex << std::setw(4) << (unsigned)c;
                os << std::dec;
            }
        } while (src);

        setlocale(LC_CTYPE, locale);

        os << "\"";
    }

    void encodeBase64String(const unsigned char *bytes, size_t size) {
        const char *table64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        unsigned char c0, c1, c2, c3;
        char buf[4];
        unsigned written;

        os << "\"";

        written = 0;
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
            ++written;

            if (written >= 76/4 && size) {
                os << "\n";
                written = 0;
            }
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

        os << "\"";
    }

public:
    JSONWriter(std::ostream &_os) : 
        os(_os), 
        level(0),
        value(false),
        space(0)
    {
        beginObject();
    }

    ~JSONWriter() {
        endObject();
        newline();
    }

    inline void beginObject() {
        separator();
        os << "{";
        ++level;
        value = false;
    }

    inline void endObject() {
        --level;
        if (value)
            newline();
        os << "}";
        value = true;
        space = '\n';
    }

    inline void beginMember(const char * name) {
        space = 0;
        separator();
        newline();
        escapeAsciiString(name);
        os << ": ";
        value = false;
    }

    inline void endMember(void) {
        assert(value);
        value = true;
        space = 0;
    }

    inline void beginArray() {
        separator();
        os << "[";
        ++level;
        value = false;
        space = 0;
    }

    inline void endArray(void) {
        --level;
        if (space == '\n') {
            newline();
        }
        os << "]";
        value = true;
        space = '\n';
    }

    inline void writeString(const char *s) {
        separator();
        escapeUnicodeString(s);
        value = true;
        space = ' ';
    }

    inline void writeBase64(const void *bytes, size_t size) {
        separator();
        encodeBase64String((const unsigned char *)bytes, size);
        value = true;
        space = ' ';
    }

    inline void writeNull(void) {
        separator();
        os << "null";
        value = true;
        space = ' ';
    }

    inline void writeBool(bool b) {
        separator();
        os << (b ? "true" : "false");
        value = true;
        space = ' ';
    }

    template<class T>
    inline void writeNumber(T n) {
        if (n != n) {
            // NaN
            writeNull();
        } else {
            separator();
            os << std::dec << std::setprecision(9) << n;
            value = true;
            space = ' ';
        }
    }
    
    inline void writeStringMember(const char *name, const char *s) {
        beginMember(name);
        writeString(s);
        endMember();
    }

    inline void writeBoolMember(const char *name, bool b) {
        beginMember(name);
        writeBool(b);
        endMember();
    }

    template<class T>
    inline void writeNumberMember(const char *name, T n) {
        beginMember(name);
        writeNumber(n);
        endMember();
    }
};

#endif /* _JSON_HPP_ */
