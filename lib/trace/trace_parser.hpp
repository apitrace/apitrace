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

#pragma once


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


// Parser interface
class AbstractParser
{
public:
    virtual ~AbstractParser() {}
    virtual  Call *parse_call(void) = 0;
    virtual void getBookmark(ParseBookmark &bookmark) = 0;
    virtual void setBookmark(const ParseBookmark &bookmark) = 0;
    virtual bool open(const char *filename) = 0;
    virtual void close(void) = 0;
    virtual unsigned long long getVersion(void) const = 0;
    virtual const Properties & getProperties(void) const = 0;

    const std::string & getProperty(const char *name) const;
};


/**
 * Parses the events of a trace file, in file order.
 *
 * Unlike Parser, it doesn't join the enter and leave events of each call
 * together.  Instead each event is parsed into its own Call object, and passed
 * to handleEnter() or handleLeave(), which subclasses must implement.
 */
class SimpleParser
{
protected:
    File *file = nullptr;

    enum Mode {
        FULL = 0,
        SCAN,
        SKIP
    };

    Properties properties;

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


    FunctionSig *glGetErrorSig = nullptr;

    int next_event_type = -1;
    unsigned next_call_no = 0;

    unsigned long long version = 0;
    unsigned long long semanticVersion = 0;

public:
    API api = API_UNKNOWN;

    SimpleParser();

    virtual ~SimpleParser();

    bool open(const char *filename);

    void close(void);

    /**
     * Parses the next event, and passes it to handleEnter() or handleLeave().
     *
     * Returns false at the end of the file.
     */
    bool parse_event(void) {
        return parse_event(FULL);
    }

    bool supportsOffsets() const
    {
        return file->supportsOffsets();
    }

    void getBookmark(ParseBookmark &bookmark);

    void setBookmark(const ParseBookmark &bookmark);

    unsigned long long getVersion(void) const {
        return semanticVersion;
    }

    const Properties & getProperties(void) const {
        return properties;
    }


    int percentRead() const {
        return file->percentRead();
    }

    size_t containerSizeInBytes() const {
        return file->containerSizeInBytes();
    }

    size_t containerBytesRead() const {
        return file->containerBytesRead();
    }

    size_t dataBytesRead() const {
        return file->dataBytesRead();
    }

    const char *containerType() const {
        return file->containerType();
    }

protected:
    /**
     * Called for every enter event, with a new Call object holding the
     * function signature, thread, call number, and the arguments in the event
     * (i.e., the input arguments).  Ownership of the call is transferred.
     *
     * If the end of the file was reached in the middle of the event, the call
     * has CALL_FLAG_INCOMPLETE set.
     */
    virtual void handleEnter(Call *call) = 0;

    /**
     * Called for every leave event, with a new Call object holding the call
     * number of the call being left, and the arguments (i.e., the output
     * arguments), return value, and flags in the event.  Its signature is a
     * placeholder (without name nor arguments), and its thread is 0, so it must
     * be paired with its enter event by the call number.  Ownership of the call
     * is transferred.
     *
     * If the end of the file was reached in the middle of the event, the call
     * has CALL_FLAG_INCOMPLETE set.
     */
    virtual void handleLeave(Call *call) = 0;

    bool parse_event(Mode mode);

    FunctionSigFlags *parse_function_sig(void);
    StructSig *parse_struct_sig();
    EnumSig *parse_old_enum_sig();
    EnumSig *parse_enum_sig();
    BitmaskSig *parse_bitmask_sig();
    
public:
    static CallFlags
    lookupCallFlags(const char *name);

protected:
    void parseProperties(void);

    void parse_enter(Mode mode);

    void parse_leave(Mode mode);

    bool parse_call_details(Call *call, Mode mode);

    bool parse_call_backtrace(Call *call, Mode mode);
    StackFrame * parse_backtrace_frame(Mode mode);

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

    Value *parse_wstring();
    void scan_wstring();

    char * read_string(void);
    void skip_string(void);

    signed long long read_sint(void);
    void skip_sint(void);

    unsigned long long read_uint(void);
    void skip_uint(void);

    inline int read_byte(void);
    inline void skip_byte(void);
};


/**
 * Parses the calls of a trace file, joining the enter and leave events of each
 * call together.
 *
 * Calls are returned when they are left, i.e., not necessarily in call number
 * order.  Calls not left by the end of the file are returned last, with
 * CALL_FLAG_INCOMPLETE set.
 */
class Parser: public AbstractParser, public SimpleParser
{
protected:
    typedef std::list<Call *> CallList;

    // Calls entered but not left yet
    CallList calls;

    // Call that was just left, to be returned by parse_call()
    Call *completed = nullptr;

public:
    Parser();

    ~Parser();

    bool open(const char *filename) override {
        return SimpleParser::open(filename);
    }

    void close(void) override;

    Call *parse_call(void) override {
        return parse_call(FULL);
    }

    void getBookmark(ParseBookmark &bookmark) override {
        SimpleParser::getBookmark(bookmark);
    }

    void setBookmark(const ParseBookmark &bookmark) override;

    unsigned long long getVersion(void) const override {
        return SimpleParser::getVersion();
    }

    const Properties & getProperties(void) const override {
        return SimpleParser::getProperties();
    }

    Call *scan_call() {
        return parse_call(SCAN);
    }

protected:
    Call *parse_call(Mode mode);

    void handleEnter(Call *call) override;

    void handleLeave(Call *call) override;

    void adjust_call_flags(Call *call);
};


AbstractParser *
lastFrameLoopParser(AbstractParser *parser, int loopCount);


} /* namespace trace */

