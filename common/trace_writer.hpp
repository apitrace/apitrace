/**************************************************************************
 *
 * Copyright 2007-2010 VMware, Inc.
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

#ifndef _TRACE_WRITER_HPP_
#define _TRACE_WRITER_HPP_


#include <stddef.h>

#include <vector>

#include "trace_model.hpp"


namespace Trace {
    class File;

    class Writer {
    protected:
        File *m_file;
        unsigned call_no;

        std::vector<bool> functions;
        std::vector<bool> structs;
        std::vector<bool> enums;
        std::vector<bool> bitmasks;

    public:
        Writer();
        ~Writer();

        bool open(const char *filename);
        void close(void);

        unsigned beginEnter(const FunctionSig *sig);
        void endEnter(void);

        void beginLeave(unsigned call);
        void endLeave(void);

        void beginArg(unsigned index);
        inline void endArg(void) {}

        void beginReturn(void);
        inline void endReturn(void) {}

        void beginArray(size_t length);
        inline void endArray(void) {}

        inline void beginElement(void) {}
        inline void endElement(void) {}

        void beginStruct(const StructSig *sig);
        inline void endStruct(void) {}

        void writeBool(bool value);
        void writeSInt(signed long long value);
        void writeUInt(unsigned long long value);
        void writeFloat(float value);
        void writeDouble(double value);
        void writeString(const char *str);
        void writeString(const char *str, size_t size);
        void writeWString(const wchar_t *str);
        void writeBlob(const void *data, size_t size);
        void writeEnum(const EnumSig *sig);
        void writeBitmask(const BitmaskSig *sig, unsigned long long value);
        void writeNull(void);
        void writeOpaque(const void *ptr);
        void writeCall(Call *call);

    protected:
        void inline _write(const void *sBuffer, size_t dwBytesToWrite);
        void inline _writeByte(char c);
        void inline _writeUInt(unsigned long long value);
        void inline _writeFloat(float value);
        void inline _writeDouble(double value);
        void inline _writeString(const char *str);

    };

    extern const FunctionSig memcpy_sig;
    extern const FunctionSig malloc_sig;
    extern const FunctionSig free_sig;
    extern const FunctionSig realloc_sig;

    /**
     * A specialized Writer class, mean to trace the current process.
     *
     * In particular:
     * - it creates a trace file based on the current process name
     * - uses mutexes to allow tracing from multiple threades
     * - flushes the output to ensure the last call is traced in event of
     *   abnormal termination
     */
    class LocalWriter : public Writer {
    protected:
        int acquired;

        std::vector<bool> regions;

    public:
        /**
         * Should never called directly -- use localWriter singleton below instead.
         */
        LocalWriter();
        ~LocalWriter();

        void open(void);

        unsigned beginEnter(const FunctionSig *sig);
        void endEnter(void);

        void beginLeave(unsigned call);
        void endLeave(void);

        void updateRegion(const void *ptr, size_t size = 0);

        void flush(void);
    };

    /**
     * Singleton.
     */
    extern LocalWriter localWriter;
}

#endif /* _TRACE_WRITER_HPP_ */
