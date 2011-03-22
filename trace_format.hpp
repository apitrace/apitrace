/**************************************************************************
 *
 * Copyright 2010 VMware, Inc.
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
 * Binary trace format decription.
 */

#ifndef _TRACE_FORMAT_HPP_
#define _TRACE_FORMAT_HPP_

namespace Trace {

#define TRACE_VERSION 1

enum Event {
    EVENT_ENTER = 0,
    EVENT_LEAVE,
    EVENT_MESSAGE
};

enum CallDetail {
    CALL_END = 0,
    CALL_ARG,
    CALL_RET,
    CALL_THREAD,
};

enum Type {
    TYPE_NULL = 0,
    TYPE_FALSE,
    TYPE_TRUE,
    TYPE_SINT,
    TYPE_UINT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_STRING, // Null terminated, human readible string
    TYPE_BLOB, // Block of bytes
    TYPE_ENUM,
    TYPE_BITMASK,
    TYPE_ARRAY,
    TYPE_STRUCT,
    TYPE_OPAQUE,
};

/*
 * XXX: Update grammar.
 *
 * trace = call* EOF
 *
 * call = name (detail)* END
 *
 * detail = ARG name value
 *        | RET value
 *        | ...
 *
 * value = VOID
 *       | BOOL BOOL_VALUE
 *       | SINT INT_VALUE
 *       | UINT INT_VALUE
 *       | FLOAT FLOAT_VALUE
 *       | STRING string
 *       | BLOB string
 *       | CONST name value
 *       | BITMASK value+
 *       | ARRAY length 
 *
 * bool = 0 | 1
 *
 * name = string
 *
 * string = length (BYTE)*
 *
 * length = INT_VALUE
 */


} /* namespace Trace */

#endif /* _TRACE_FORMAT_HPP_ */
