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

#include "trace_file.hpp"
#include "trace_format.hpp"
#include "trace_model.hpp"
#include "trace_api.hpp"


namespace trace {


struct ParseBookmark
{
    File::Offset offset;
    unsigned next_call_no;
};


class Parser
{
protected:
    File *file;

    enum Mode {
        FULL = 0,
        SCAN,
        SKIP
    };

    typedef std::list<Call *> CallList;
    CallList calls;

    struct FunctionSigFlags : public FunctionSig {
        CallFlags flags;
    };

    // Helper template that extends a base signature structure, with additional
    // parsing information.
    template< class T >
    struct SigState : public T {
        // Offset in the file of where signature was defined.  It is used when
        // reparsing to determine whether the signature definition is to be
        // expected next or not.
        File::Offset fileOffset;
    };

    typedef SigState<FunctionSigFlags> FunctionSigState;
    typedef SigState<StructSig> StructSigState;
    typedef SigState<EnumSig> EnumSigState;
    typedef SigState<BitmaskSig> BitmaskSigState;
    typedef SigState<StackFrame> StackFrameState;

    typedef std::vector<FunctionSigState *> FunctionMap;
    typedef std::vector<StructSigState *> StructMap;
    typedef std::vector<EnumSigState *> EnumMap;
    typedef std::vector<BitmaskSigState *> BitmaskMap;
    typedef std::vector<StackFrameState *> StackFrameMap;

    FunctionMap functions;
    StructMap structs;
    EnumMap enums;
    BitmaskMap bitmasks;
    StackFrameMap frames;

    FunctionSig *glGetErrorSig;

    unsigned next_call_no;

public:
    unsigned long long version;
    API api;

    Parser();

    ~Parser();

    bool open(const char *filename);

    void close(void);

    Call *parse_call(void) {
        return parse_call(FULL);
    }

    bool supportsOffsets() const
    {
        return file->supportsOffsets();
    }

    void getBookmark(ParseBookmark &bookmark);

    void setBookmark(const ParseBookmark &bookmark);

    int percentRead()
    {
        return file->percentRead();
    }

    Call *scan_call() {
        return parse_call(SCAN);
    }

protected:
    Call *parse_call(Mode mode);

    FunctionSigFlags *parse_function_sig(void);
    StructSig *parse_struct_sig();
    EnumSig *parse_old_enum_sig();
    EnumSig *parse_enum_sig();
    BitmaskSig *parse_bitmask_sig();
    
    static CallFlags
    lookupCallFlags(const char *name);

    Call *parse_Call(Mode mode);

    void parse_enter(Mode mode);

    Call *parse_leave(Mode mode);

    bool parse_call_details(Call *call, Mode mode);

    bool parse_call_backtrace(Call *call, Mode mode);
    StackFrame * parse_backtrace_frame(Mode mode);

    void adjust_call_flags(Call *call);

    void parse_arg(Call *call, Mode mode);

    Value *parse_value(void);
    void scan_value(void);
    inline Value *parse_value(Mode mode) {
        if (mode == FULL) {
            return parse_value();
        } else {
            scan_value();
            return NULL;
        }
    }

    Value *parse_sint();
    void scan_sint();

    Value *parse_uint();
    void scan_uint();

    Value *parse_float();
    void scan_float();

    Value *parse_double();
    void scan_double();

    Value *parse_string();
    void scan_string();

    Value *parse_enum();
    void scan_enum();

    Value *parse_bitmask();
    void scan_bitmask();

    Value *parse_array(void);
    void scan_array(void);

    Value *parse_blob(void);
    void scan_blob(void);

    Value *parse_struct();
    void scan_struct();

    Value *parse_opaque();
    void scan_opaque();

    Value *parse_repr();
    void scan_repr();

    const char * read_string(void);
    void skip_string(void);

    signed long long read_sint(void);
    void skip_sint(void);

    unsigned long long read_uint(void);
    void skip_uint(void);

    inline int read_byte(void);
    inline void skip_byte(void);
};


} /* namespace trace */

#endif /* _TRACE_PARSER_HPP_ */
