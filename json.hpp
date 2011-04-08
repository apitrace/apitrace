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
    }

    inline void endArray(void) {
        --level;
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
    void writeNumber(T n) {
        separator();
        os << std::dec << n;
        value = true;
        space = ' ';
    }
};

#endif /* _JSON_HPP_ */
