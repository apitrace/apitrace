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

#ifndef _LOG_HPP_
#define _LOG_HPP_

namespace Log {

    void Open(void);
    void Close(void);
    
    unsigned BeginEnter(const char *function);
    void EndEnter(void);
    
    void BeginLeave(unsigned call);
    void EndLeave(void);
    
    void BeginArg(unsigned index, const char *name);
    inline void EndArg(void) {}

    void BeginReturn(void);
    inline void EndReturn(void) {}

    void BeginArray(size_t length);
    inline void EndArray(void) {}

    inline void BeginElement(void) {}
    inline void EndElement(void) {}

    void BeginStruct(size_t length);
    inline void EndStruct(void) {}

    void BeginMember(const char *name);
    inline void EndMember(void) {}

    void BeginBitmask(void);
    void EndBitmask(void);

    void LiteralBool(bool value);
    void LiteralSInt(signed long long value);
    void LiteralUInt(unsigned long long value);
    void LiteralFloat(float value);
    void LiteralFloat(double value);
    void LiteralString(const char *str);
    void LiteralString(const char *str, size_t size);
    void LiteralWString(const wchar_t *str);
    void LiteralBlob(const void *data, size_t size);
    void LiteralNamedConstant(const char *name, long long value);
    void LiteralNull(void);
    void LiteralOpaque(const void *ptr);

    void Abort(void);
}

#endif /* _LOG_HPP_ */
