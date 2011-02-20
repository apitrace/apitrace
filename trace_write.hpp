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

#ifndef _TRACE_WRITE_HPP_
#define _TRACE_WRITE_HPP_

#include <stddef.h>

namespace Trace {

    typedef unsigned Id;

    struct FunctionSig {
        Id id;
        const char *name;
        unsigned num_args;
        const char **args;
    };

    struct StructSig {
        Id id;
        const char *name;
        unsigned num_members;
        const char **members;
    };

    struct EnumSig {
        Id id;
        const char *name;
        signed long long value;
    };

    struct BitmaskVal {
        const char *name;
        unsigned long long value;
    };

    struct BitmaskSig {
        Id id;
        unsigned count;
        const BitmaskVal *values;
    };

    void Open(void);
    void Close(void);

    unsigned BeginEnter(const FunctionSig &function);
    void EndEnter(void);

    void BeginLeave(unsigned call);
    void EndLeave(void);

    void BeginArg(unsigned index);
    inline void EndArg(void) {}

    void BeginReturn(void);
    inline void EndReturn(void) {}

    void BeginArray(size_t length);
    inline void EndArray(void) {}

    inline void BeginElement(void) {}
    inline void EndElement(void) {}

    void BeginStruct(const StructSig *sig);
    inline void EndStruct(void) {}

    void LiteralBool(bool value);
    void LiteralSInt(signed long long value);
    void LiteralUInt(unsigned long long value);
    void LiteralFloat(float value);
    void LiteralFloat(double value);
    void LiteralString(const char *str);
    void LiteralString(const char *str, size_t size);
    void LiteralWString(const wchar_t *str);
    void LiteralBlob(const void *data, size_t size);
    void LiteralEnum(const EnumSig *sig);
    void LiteralBitmask(const BitmaskSig &bitmask, unsigned long long value);
    void LiteralNull(void);
    void LiteralOpaque(const void *ptr);

    void Abort(void);
}

#endif /* _TRACE_WRITE_HPP_ */
