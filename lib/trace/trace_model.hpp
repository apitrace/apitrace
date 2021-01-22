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
 * Object hierarchy for describing the traces in memory.
 */

#pragma once


#include <assert.h>
#include <stdlib.h>

#include <map>
#include <vector>
#include <ostream>


namespace trace {


// Should match Call::no
typedef unsigned CallNo;


typedef unsigned Id;


struct FunctionSig {
    Id id;
    const char *name;
    unsigned num_args;
    const char **arg_names;
};


struct StructSig {
    Id id;
    const char *name;
    unsigned num_members;
    const char **member_names;
};


struct EnumValue {
    const char *name;
    signed long long value;
};


struct EnumSig {
    Id id;
    unsigned num_values;
    const EnumValue *values;
};


struct BitmaskFlag {
    const char *name;
    unsigned long long value;
};


struct BitmaskSig {
    Id id;
    unsigned num_flags;
    const BitmaskFlag *flags;
};


class Visitor;
class Null;
class Struct;
class Array;
class Blob;


class Value
{
public:
    virtual ~Value() {}
    virtual void visit(Visitor &visitor) = 0;

    virtual bool toBool(void) const = 0;
    virtual signed long long toSInt(void) const;
    virtual unsigned long long toUInt(void) const;
    virtual float toFloat(void) const;
    virtual double toDouble(void) const;

    virtual void *toPointer(void) const;
    virtual void *toPointer(bool bind);
    virtual unsigned long long toUIntPtr(void) const;
    virtual const char *toString(void) const;

    virtual const Null *toNull(void) const { return NULL; }
    virtual Null *toNull(void) { return NULL; }

    virtual const Array *toArray(void) const { return NULL; }
    virtual Array *toArray(void) { return NULL; }

    virtual const Struct *toStruct(void) const { return NULL; }
    virtual Struct *toStruct(void) { return NULL; }

    virtual const Blob *toBlob(void) const { return NULL; }
    virtual Blob *toBlob(void) { return NULL; }

    Value & operator[](size_t index) const;
};


class Null : public Value
{
public:
    bool toBool(void) const override;
    signed long long toSInt(void) const override;
    unsigned long long toUInt(void) const override;
    virtual float toFloat(void) const override;
    virtual double toDouble(void) const override;
    void *toPointer(void) const override;
    void *toPointer(bool bind) override;
    unsigned long long toUIntPtr(void) const override;
    const char *toString(void) const override;
    void visit(Visitor &visitor) override;

    const Null *toNull(void) const override { return this; }
    Null *toNull(void) override { return this; }
};


class Bool : public Value
{
public:
    Bool(bool _value) : value(_value) {}

    bool toBool(void) const override;
    signed long long toSInt(void) const override;
    unsigned long long toUInt(void) const override;
    virtual float toFloat(void) const override;
    virtual double toDouble(void) const override;
    void visit(Visitor &visitor) override;

    bool value;
};


class SInt : public Value
{
public:
    SInt(signed long long _value) : value(_value) {}

    bool toBool(void) const override;
    signed long long toSInt(void) const override;
    unsigned long long toUInt(void) const override;
    virtual float toFloat(void) const override;
    virtual double toDouble(void) const override;
    void visit(Visitor &visitor) override;

    signed long long value;
};


class UInt : public Value
{
public:
    UInt(unsigned long long _value) : value(_value) {}

    bool toBool(void) const override;
    signed long long toSInt(void) const override;
    unsigned long long toUInt(void) const override;
    virtual float toFloat(void) const override;
    virtual double toDouble(void) const override;
    void visit(Visitor &visitor) override;

    unsigned long long value;
};


class Float : public Value
{
public:
    Float(float _value) : value(_value) {}

    bool toBool(void) const override;
    signed long long toSInt(void) const override;
    unsigned long long toUInt(void) const override;
    virtual float toFloat(void) const override;
    virtual double toDouble(void) const override;
    void visit(Visitor &visitor) override;

    float value;
};


class Double : public Value
{
public:
    Double(double _value) : value(_value) {}

    bool toBool(void) const override;
    signed long long toSInt(void) const override;
    unsigned long long toUInt(void) const override;
    virtual float toFloat(void) const override;
    virtual double toDouble(void) const override;
    void visit(Visitor &visitor) override;

    double value;
};


class String : public Value
{
public:
    String(const char * _value) : value(_value) {}
    ~String();

    bool toBool(void) const override;
    const char *toString(void) const override;
    void visit(Visitor &visitor) override;

    const char * value;
};


class WString : public Value
{
public:
    WString(const wchar_t * _value) : value(_value) {}
    ~WString();

    bool toBool(void) const override;
    void visit(Visitor &visitor) override;

    const wchar_t * value;
};


class Enum : public SInt
{
public:
    Enum(const EnumSig *_sig, signed long long _value) : SInt(_value), sig(_sig) {}

    void visit(Visitor &visitor) override;

    const EnumSig *sig;

    const EnumValue *
    lookup() {
        // TODO: use a std::map
        for (const EnumValue *it = sig->values; it != sig->values + sig->num_values; ++it) {
            if (it->value == value) {
                return it;
            }
        }
        return NULL;
    }
};


class Bitmask : public UInt
{
public:
    Bitmask(const BitmaskSig *_sig, unsigned long long _value) : UInt(_value), sig(_sig) {}

    void visit(Visitor &visitor) override;

    const BitmaskSig *sig;
};


class Struct : public Value
{
public:
    Struct(StructSig *_sig) : sig(_sig), members(_sig->num_members) { }
    ~Struct();

    bool toBool(void) const override;
    void visit(Visitor &visitor) override;

    const Struct *toStruct(void) const override { return this; }
    Struct *toStruct(void) override { return this; }

    const StructSig *sig;
    std::vector<Value *> members;
};


class Array : public Value
{
public:
    Array(size_t len) : values(len) {}
    ~Array();

    bool toBool(void) const override;
    void visit(Visitor &visitor) override;

    const Array *toArray(void) const override { return this; }
    Array *toArray(void) override { return this; }

    std::vector<Value *> values;

    inline size_t
    size(void) const {
        return values.size();
    }
};


class Blob : public Value
{
public:
    Blob(size_t _size) {
        size = _size;
        buf = new char[_size];
        bound = false;
    }

    ~Blob();

    bool toBool(void) const override;
    void *toPointer(void) const override;
    void *toPointer(bool bind) override;

    void visit(Visitor &visitor) override;

    const Blob *toBlob(void) const override { return this; }
    Blob *toBlob(void) override { return this; }

    size_t size;
    char *buf;
    bool bound;
};


class Pointer : public UInt
{
public:
    Pointer(unsigned long long value) : UInt(value) {}

    bool toBool(void) const override;
    void *toPointer(void) const override;
    void *toPointer(bool bind) override;
    unsigned long long toUIntPtr(void) const override;
    void visit(Visitor &visitor) override;
};


class Repr : public Value
{
public:
    Repr(Value *human, Value *machine) :
        humanValue(human),
        machineValue(machine)
    {}

    /** Human-readible value */
    Value *humanValue;

    /** Machine-readible value */
    Value *machineValue;
    
    virtual bool toBool(void) const override;
    virtual signed long long toSInt(void) const override;
    virtual unsigned long long toUInt(void) const override;
    virtual float toFloat(void) const override;
    virtual double toDouble(void) const override;

    virtual void *toPointer(void) const override;
    virtual void *toPointer(bool bind) override;
    virtual unsigned long long toUIntPtr(void) const override;
    virtual const char *toString(void) const override;

    void visit(Visitor &visitor) override;
};

struct RawStackFrame {
    Id id;
    const char * module;
    const char * function;
    const char * filename;
    int linenumber;
    long long offset;
    RawStackFrame() :
        module(0),
        function(0),
        filename(0),
        linenumber(-1),
        offset(-1)
    {
    }

    void dump(std::ostream &os) {
        os << (this->module ? this->module : "?");
        if (this->function != NULL) {
            os << ": " << this->function;
        }
        if (this->offset >= 0) {
            os << "+0x" << std::hex << this->offset << std::dec;
        }
        if (this->filename != NULL) {
            os << ": " << this->filename;
            if (this->linenumber >= 0) {
                os << ":" << this->linenumber;
            }
        }
    }
};

class StackFrame : public RawStackFrame {
public:
    ~StackFrame();
};

typedef std::vector<StackFrame *> Backtrace;

class Visitor
{
public:
    virtual void visit(Null *);
    virtual void visit(Bool *);
    virtual void visit(SInt *);
    virtual void visit(UInt *);
    virtual void visit(Float *);
    virtual void visit(Double *);
    virtual void visit(String *);
    virtual void visit(WString *);
    virtual void visit(Enum *);
    virtual void visit(Bitmask *);
    virtual void visit(Struct *);
    virtual void visit(Array *);
    virtual void visit(Blob *);
    virtual void visit(Pointer *);
    virtual void visit(Repr *);
protected:
    inline void _visit(Value *value) {
        if (value) { 
            value->visit(*this); 
        }
    }
};


typedef unsigned CallFlags;

/**
 * Call flags.
 *
 * TODO: It might be better to to record some of these (but not all) into the
 * trace file.
 */
enum {

    /**
     * Whether a call was really done by the application or not.
     *
     * This flag is set for fake calls -- calls not truly done by the application
     * but emitted and recorded for completeness, to provide contextual information
     * necessary for retracing, that would not be available through other ways.
     *
     * XXX: This one definetely needs to go into the trace file.
     */
    CALL_FLAG_FAKE                      = (1 << 0),

    /**
     * Whether this call should be retraced or ignored.
     *
     * This flag is set for calls which can't be safely replayed (due to incomplete
     * information) or that have no sideffects.
     *
     * Some incomplete calls are unreproduceable, but not all.
     */
    CALL_FLAG_NON_REPRODUCIBLE         = (1 << 1),
    
    /**
     * Whether this call has no side-effects, therefore don't need to be
     * retraced.
     *
     * This flag is set for calls that merely query information which is not
     * needed for posterior calls.
     */
    CALL_FLAG_NO_SIDE_EFFECTS            = (1 << 2),

    /**
     * Whether this call renders into the bound rendertargets.
     */
    CALL_FLAG_RENDER                    = (1 << 3),

    /**
     * Whether this call causes render target to be swapped.
     *
     * This does not mark frame termination by itself -- that's solely the
     * responsibility of `endOfFrame` bit. 
     *
     * This mean that snapshots should be take prior to the call, and not
     * after.
     */
    CALL_FLAG_SWAP_RENDERTARGET         = (1 << 4),
        
    /**
     * Whether this call terminates a frame.
     *
     * XXX: This can't always be determined by the function name, so it should also
     * go into the trace file eventually.
     */
    CALL_FLAG_END_FRAME                 = (1 << 5),

    /**
     * Whether this call is incomplete, i.e., it never returned.
     */
    CALL_FLAG_INCOMPLETE                = (1 << 6),

    /**
     * Whether this call is verbose (i.e., not usually interesting).
     */
    CALL_FLAG_VERBOSE                  = (1 << 7),

    /**
     * String markers.
     */
    CALL_FLAG_MARKER                    = (1 << 8),
    CALL_FLAG_MARKER_PUSH               = (1 << 9),
    CALL_FLAG_MARKER_POP                = (1 << 10),
};


struct Arg
{
    Value *value;
};


class Call
{
public:
    unsigned thread_id;
    unsigned no;
    const FunctionSig *sig;
    std::vector<Arg> args;
    Value *ret;

    CallFlags flags;
    Backtrace* backtrace;
    bool reuse_call;

    Call(const FunctionSig *_sig, const CallFlags &_flags, unsigned _thread_id) :
        thread_id(_thread_id), 
        sig(_sig), 
        args(_sig->num_args), 
        ret(0),
        flags(_flags),
        backtrace(0),
        reuse_call(false) {
    }

    ~Call();

    inline const char *
    name(void) const {
        return sig->name;
    }

    inline Value &
    arg(unsigned index) {
        assert(index < args.size());
        return *(args[index].value);
    }

    inline const Value &
    arg(unsigned index) const {
        assert(index < args.size());
        return *(args[index].value);
    }

    Value &
    argByName(const char *argName);
};


typedef std::map<std::string, std::string> Properties;


} /* namespace trace */

