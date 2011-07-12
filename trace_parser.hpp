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

#ifndef _TRACE_PARSER_HPP_
#define _TRACE_PARSER_HPP_


#include <iostream>
#include <list>

#include "trace_format.hpp"
#include "trace_model.hpp"


namespace Trace {


class Parser
{
protected:
    void *file;

    typedef std::list<Call *> CallList;
    CallList calls;

    typedef std::vector<FunctionSig *> FunctionMap;
    FunctionMap functions;

    typedef std::vector<StructSig *> StructMap;
    StructMap structs;

    typedef std::vector<EnumSig *> EnumMap;
    EnumMap enums;

    typedef std::vector<BitmaskSig *> BitmaskMap;
    BitmaskMap bitmasks;

    unsigned next_call_no;

public:
    unsigned long long version;

    Parser();

    ~Parser();

    bool open(const char *filename);

    void close(void);

    Call *parse_call(void);

protected:
    void parse_enter(void);

    Call *parse_leave(void);

    bool parse_call_details(Call *call);

    void parse_arg(Call *call);

    Value *parse_value(void);

    Value *parse_sint();

    Value *parse_uint();

    Value *parse_float();

    Value *parse_double();

    Value *parse_string();

    Value *parse_enum();

    Value *parse_bitmask();

    Value *parse_array(void);

    Value *parse_blob(void);

    Value *parse_struct();

    Value *parse_opaque();

    const char * read_string(void);

    unsigned long long read_uint(void);

    inline int read_byte(void);
};


} /* namespace Trace */

#endif /* _TRACE_PARSER_HPP_ */
