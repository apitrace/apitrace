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
#include <set>

#include "trace_file.hpp"
#include "trace_format.hpp"
#include "trace_model.hpp"


namespace Trace {

class Parser
{
protected:
    File *file;

    typedef std::list<Call *> CallList;
    CallList calls;

    // Helper template that extends a base signature structure, with additional
    // parsing information.
    template< class T >
    struct SigState : public T {
        // Offset in the file of where signature was defined.  It is used when
        // reparsing to determine whether the signature definition is to be
        // expected next or not.
        File::Offset offset;
    };

    typedef SigState<FunctionSig> FunctionSigState;
    typedef SigState<StructSig> StructSigState;
    typedef SigState<EnumSig> EnumSigState;
    typedef SigState<BitmaskSig> BitmaskSigState;

    typedef std::vector<FunctionSigState *> FunctionMap;
    typedef std::vector<StructSigState *> StructMap;
    typedef std::vector<EnumSigState *> EnumMap;
    typedef std::vector<BitmaskSigState *> BitmaskMap;

    FunctionMap functions;
    StructMap structs;
    EnumMap enums;
    BitmaskMap bitmasks;

    bool m_supportsSeeking;

    unsigned next_call_no;

public:
    unsigned long long version;

    Parser();

    ~Parser();

    bool open(const char *filename);

    void close(void);

    Call *parse_call(void);

    bool supportsOffsets() const
    {
        return file->supportsOffsets();
    }

    File::Offset currentOffset()
    {
        return file->currentOffset();
    }

    void setCurrentOffset(const File::Offset &offset)
    {
        file->setCurrentOffset(offset);
    }

    unsigned currentCallNumber() const
    {
        return next_call_no;
    }

    void setCurrentCallNumber(unsigned num)
    {
        next_call_no = num;
    }

    int percentRead()
    {
        return file->percentRead();
    }

    Call *scan_call();

protected:
    FunctionSig *parse_function_sig(void);
    StructSig *parse_struct_sig();
    EnumSig *parse_enum_sig();
    BitmaskSig *parse_bitmask_sig();

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

protected:
    void scan_enter(void);

    Call *scan_leave(void);

    bool scan_call_details(Call *call);

    void scan_arg(Call *call);

    void scan_value(void);

    void scan_sint();

    void scan_uint();

    void scan_float();

    void scan_double();

    void scan_string();

    void scan_enum();

    void scan_bitmask();

    void scan_array(void);

    void scan_blob(void);

    void scan_struct();

    void scan_opaque();

    void skip_string(void);

    void skip_uint(void);

    inline void skip_byte(void);
};


} /* namespace Trace */

#endif /* _TRACE_PARSER_HPP_ */
