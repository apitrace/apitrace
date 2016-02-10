/**************************************************************************
 *
 * Copyright 2012 Jose Fonseca
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
 * Python pickle writer
 */

#pragma once

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <wchar.h>

#include <ostream>
#include <string>
#include <limits>


class PickleWriter
{
private:
    std::ostream &os;

    /*
     * Python pickle opcodes.  See pickle.py and pickletools.py from Python
     * standard library for details.
     */
    enum Opcode {
        MARK            = '(',
        STOP            = '.',
        POP             = '0',
        POP_MARK        = '1',
        DUP             = '2',
        FLOAT           = 'F',
        INT             = 'I',
        BININT          = 'J',
        BININT1         = 'K',
        LONG            = 'L',
        BININT2         = 'M',
        NONE            = 'N',
        PERSID          = 'P',
        BINPERSID       = 'Q',
        REDUCE          = 'R',
        STRING          = 'S',
        BINSTRING       = 'T',
        SHORT_BINSTRING = 'U',
        UNICODE         = 'V',
        BINUNICODE      = 'X',
        APPEND          = 'a',
        BUILD           = 'b',
        GLOBAL          = 'c',
        DICT            = 'd',
        EMPTY_DICT      = '}',
        APPENDS         = 'e',
        GET             = 'g',
        BINGET          = 'h',
        INST            = 'i',
        LONG_BINGET     = 'j',
        LIST            = 'l',
        EMPTY_LIST      = ']',
        OBJ             = 'o',
        PUT             = 'p',
        BINPUT          = 'q',
        LONG_BINPUT     = 'r',
        SETITEM         = 's',
        TUPLE           = 't',
        EMPTY_TUPLE     = ')',
        SETITEMS        = 'u',
        BINFLOAT        = 'G',

        PROTO           = '\x80',
        NEWOBJ          = '\x81',
        EXT1            = '\x82',
        EXT2            = '\x83',
        EXT4            = '\x84',
        TUPLE1          = '\x85',
        TUPLE2          = '\x86',
        TUPLE3          = '\x87',
        NEWTRUE         = '\x88',
        NEWFALSE        = '\x89',
        LONG1           = '\x8a',
        LONG4           = '\x8b',
    };

public:
    PickleWriter(std::ostream &_os) :
        os(_os) {
    }

    inline void begin() {
        os.put(PROTO);
        os.put(2);
    }

    inline void end() {
        os.put(STOP);
    }

    inline void beginDict() {
        os.put(EMPTY_DICT);
        os.put(BINPUT);
        os.put(1);
    }

    inline void endDict() {
    }

    inline void beginItem() {
    }

    inline void beginItem(const char * name) {
        writeString(name);
    }

    inline void beginItem(const std::string &name) {
        beginItem(name.c_str());
    }

    inline void endItem(void) {
        os.put(SETITEM);
    }

    inline void beginList() {
        os.put(EMPTY_LIST);
        os.put(BINPUT);
        os.put(1);
        os.put(MARK);
    }

    inline void endList(void) {
        os.put(APPENDS);
    }

    inline void beginTuple() {
        os.put(MARK);
    }

    inline void endTuple(void) {
        os.put(TUPLE);
    }

    inline void beginTuple(unsigned length) {
        if (length >= 4) {
            os.put(MARK);
        }
    }

    inline void endTuple(unsigned length) {
        static const Opcode ops[4] = {
            EMPTY_TUPLE,
            TUPLE1,
            TUPLE2,
            TUPLE3,
        };
        Opcode op = length < 4 ? ops[length] : TUPLE;
        os.put(op);
    }

    inline void writeString(const char *s, size_t length) {
        if (!s) {
            writeNone();
            return;
        }

        if (length < 256) {
            os.put(SHORT_BINSTRING);
            os.put(length);
        } else {
            os.put(BINSTRING);
            putInt32(length);
        }
        os.write(s, length);

        os.put(BINPUT);
        os.put(1);
    }

    inline void writeString(const char *s) {
        if (!s) {
            writeNone();
            return;
        }

        writeString(s, strlen(s));
    }

    inline void writeString(const std::string &s) {
        writeString(s.c_str(), s.size());
    }

    inline void writeWString(const wchar_t *s, size_t length) {
        if (!s) {
            writeNone();
            return;
        }

        /* FIXME: emit UTF-8 */
        os.put(BINUNICODE);
        putInt32(length);
        for (size_t i = 0; i < length; ++i) {
            wchar_t wc = s[i];
            char c = wc >= 0 && wc < 0x80 ? (char)wc : '?';
            os.put(c);
        }

        os.put(BINPUT);
        os.put(1);
    }

    inline void writeWString(const wchar_t *s) {
        if (!s) {
            writeNone();
            return;
        }

        writeWString(s, wcslen(s));
    }

    inline void writeNone(void) {
        os.put(NONE);
    }

    inline void writeBool(bool b) {
        os.put(b ? NEWTRUE : NEWFALSE);
    }

    inline void writeInt(uint8_t i) {
        os.put(BININT1);
        os.put(i);
    }

    inline void writeInt(uint16_t i) {
        if (i < 0x100) {
            writeInt((uint8_t)i);
        } else {
            os.put(BININT2);
            putInt16(i);
        }
    }

    inline void writeInt(int32_t i) {
        if (0 <= i && i < 0x10000) {
            writeInt((uint16_t)i);
        } else {
            os.put(BININT);
            putInt32(i);
        }
    }

    inline void writeInt(uint32_t i) {
        if (i < 0x8000000) {
            writeInt((int32_t)i);
        } else {
            writeLong(i);
        }
    }

    inline void writeInt(long long i) {
        if (-0x8000000 <= i && i < 0x8000000) {
            writeInt((int32_t)i);
        } else {
            writeLong(i);
        }
    }

    inline void writeInt(unsigned long long i) {
        if (i < 0x8000000) {
            writeInt((int32_t)i);
        } else {
            writeLong(i);
        }
    }

    inline void writeFloat(double f) {
        union {
            double f;
            char c[8];
        } u;

        static_assert(sizeof u.f == sizeof u.c, "double is not 8 bytes");
        u.f = f;

        os.put(BINFLOAT);
        os.put(u.c[7]);
        os.put(u.c[6]);
        os.put(u.c[5]);
        os.put(u.c[4]);
        os.put(u.c[3]);
        os.put(u.c[2]);
        os.put(u.c[1]);
        os.put(u.c[0]);
    }

    inline void writeByteArray(const void *buf, size_t length) {
        os.put(GLOBAL);
        os << "__builtin__\nbytearray\n";
        os.put(BINPUT);
        os.put(1);
        writeString(static_cast<const char *>(buf), length);
        os.put(TUPLE1);
        os.put(REDUCE);
    }

    inline void writePointer(unsigned long long addr) {
        os.put(GLOBAL);
        os << "unpickle\nPointer\n";
        os.put(BINPUT);
        os.put(1);
        writeInt(addr);
        os.put(TUPLE1);
        os.put(REDUCE);
    }

protected:
    inline void putInt16(uint16_t i) {
        os.put( i        & 0xff);
        os.put( i >>  8        );
    }

    inline void putInt32(uint32_t i) {
        os.put( i        & 0xff);
        os.put((i >>  8) & 0xff);
        os.put((i >> 16) & 0xff);
        os.put( i >> 24        );
    }

    template< class T >
    inline void writeLong(T l) {
        os.put(LONG1);

        if (l == 0) {
            os.put(0);
            return;
        }

        // Same as l >> (8 * sizeof l), but without the warnings
        T sign;
        if (std::numeric_limits<T>::is_signed) {
            sign = l < 0 ? ~0 : 0;
        } else {
            sign = 0;
        }

        // Count how many bytes we need to represent the long integer.
        T sl = l;
        unsigned c = 0;
        do {
            ++c;
            sl >>= 8;
        } while (sl != sign);

        // Add an extra byte if sign bit doesn't match
        if (((l >> (8 * c - 1)) & 1) != ((l >> (8 * sizeof l - 1)) & 1)) {
            ++c;
        }
        os.put(c);

        for (unsigned i = 0; i < c; ++ i) {
            os.put(l & 0xff);
            l >>= 8;
        }
    }
};

