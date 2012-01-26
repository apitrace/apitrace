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

#ifndef _PICKLE_HPP_
#define _PICKLE_HPP_

#include <assert.h>
#include <stddef.h>

#include <ostream>
#include <string>


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

        PROTO           = 0x80,
        NEWOBJ          = 0x81,
        EXT1            = 0x82,
        EXT2            = 0x83,
        EXT4            = 0x84,
        TUPLE1          = 0x85,
        TUPLE2          = 0x86,
        TUPLE3          = 0x87,
        NEWTRUE         = 0x88,
        NEWFALSE        = 0x89,
        LONG1           = 0x8a,
        LONG4           = 0x8b,
    };

public:
    PickleWriter(std::ostream &_os) : 
        os(_os)
    {
        os.put(PROTO);
        os.put(2);
    }

    ~PickleWriter() {
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

    inline void putInt(int i) {
        os.put( i        & 0xff);
        os.put((i >>  8) & 0xff);
        os.put((i >> 16) & 0xff);
        os.put((i >> 24) & 0xff);
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
            putInt(length);
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

    inline void writeNone(void) {
        os.put(NONE);
    }

    inline void writeBool(bool b) {
        os.put(b ? NEWTRUE : NEWFALSE);
    }

    inline void writeInt(long i) {
        // TODO: binary
        os.put(INT);
        os << i << '\n'; 
    }

    inline void writeFloat(float f) {
        // TODO: binary
        os.put(FLOAT);
        os << f << '\n';
    }
};

#endif /* _Pickle_HPP_ */
