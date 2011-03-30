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

#include <ostream>


class JSONWriter
{
private:
    std::ostream &os;

    int level;
    bool value;

    void newline(void) {
        os << "\n";
        for (int i = 0; i < level; ++i) 
            os << "  ";
    }

    void separator(void) {
        if (value) {
            os << ",";
        }
    }

    void escapeString(const char *str) {
        const unsigned char *p = (const unsigned char *)str;
        os << "\"";
        unsigned char c;
        while ((c = *p++) != 0) {
            if (c == '\"')
                os << "\\\"";
            else if (c == '\\')
                os << "\\\\";
            else if (c >= 0x20 && c <= 0x7e)
                os << c;
            else if (c == '\t')
                os << "\\t";
            else if (c == '\r')
                os << "\\r";
            else if (c == '\n')
                os << "&#10;";
            else {
                unsigned octal0 = c & 0x7;
                unsigned octal1 = (c >> 3) & 0x7;
                unsigned octal2 = (c >> 3) & 0x7;
                os << "\\";
                if (octal2)
                    os << octal2;
                if (octal1)
                    os << octal1;
                os << octal0;
            }
        }
        os << "\"";
    }

public:
    JSONWriter(std::ostream &_os) : 
        os(_os), 
        level(0),
        value(false)
    {
        beginObject();
    }

    ~JSONWriter() {
        endObject();
        newline();
    }

    inline void beginObject() {
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
    }

    inline void beginMember(const char * name) {
        separator();
        newline();
        escapeString(name);
        os << ": ";
        value = false;
    }

    inline void endMember(void) {
        assert(value);
        value = true;
    }

    inline void beginArray() {
        separator();
        os << "[";
        value = false;
    }

    inline void endArray(void) {
        os << "]";
        value = true;
    }

    inline void writeString(const char *s) {
        separator();
        escapeString(s);
        value = false;
    }

    inline void writeNull(void) {
        separator();
        os << "null";
        value = true;
    }

    inline void writeBool(bool b) {
        separator();
        os << (b ? "true" : "false");
        value = true;
    }

    template<class T>
    void writeNumber(T n) {
        separator();
        os << n;
        value = true;
    }
};

#endif /* _JSON_HPP_ */
