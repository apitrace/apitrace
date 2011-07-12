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

#ifndef _TRACE_MODEL_HPP_
#define _TRACE_MODEL_HPP_


#include <assert.h>

#include <map>
#include <vector>
#include <iostream>


namespace Trace {


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


struct EnumSig {
    Id id;
    const char *name;
    signed long long value;
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
    virtual unsigned long long toUIntPtr(void) const;
    virtual const char *toString(void) const;

    const Value & operator[](size_t index) const;
};


class Null : public Value
{
public:
    bool toBool(void) const;
    signed long long toSInt(void) const;
    unsigned long long toUInt(void) const;
    virtual float toFloat(void) const;
    virtual double toDouble(void) const;
    void *toPointer(void) const;
    unsigned long long toUIntPtr(void) const;
    const char *toString(void) const;
    void visit(Visitor &visitor);
};


class Bool : public Value
{
public:
    Bool(bool _value) : value(_value) {}

    bool toBool(void) const;
    signed long long toSInt(void) const;
    unsigned long long toUInt(void) const;
    virtual float toFloat(void) const;
    virtual double toDouble(void) const;
    void visit(Visitor &visitor);

    bool value;
};


class SInt : public Value
{
public:
    SInt(signed long long _value) : value(_value) {}

    bool toBool(void) const;
    signed long long toSInt(void) const;
    unsigned long long toUInt(void) const;
    virtual float toFloat(void) const;
    virtual double toDouble(void) const;
    void visit(Visitor &visitor);

    signed long long value;
};


class UInt : public Value
{
public:
    UInt(unsigned long long _value) : value(_value) {}

    bool toBool(void) const;
    signed long long toSInt(void) const;
    unsigned long long toUInt(void) const;
    virtual float toFloat(void) const;
    virtual double toDouble(void) const;
    void visit(Visitor &visitor);

    unsigned long long value;
};


class Float : public Value
{
public:
    Float(double _value) : value(_value) {}

    bool toBool(void) const;
    signed long long toSInt(void) const;
    unsigned long long toUInt(void) const;
    virtual float toFloat(void) const;
    virtual double toDouble(void) const;
    void visit(Visitor &visitor);

    double value;
};


class String : public Value
{
public:
    String(const char * _value) : value(_value) {}

    bool toBool(void) const;
    const char *toString(void) const;
    void visit(Visitor &visitor);

    const char * value;
};


class Enum : public Value
{
public:
    Enum(const EnumSig *_sig) : sig(_sig) {}

    bool toBool(void) const;
    signed long long toSInt(void) const;
    unsigned long long toUInt(void) const;
    virtual float toFloat(void) const;
    virtual double toDouble(void) const;
    void visit(Visitor &visitor);

    const EnumSig *sig;
};


class Bitmask : public UInt
{
public:
    Bitmask(const BitmaskSig *_sig, unsigned long long _value) : UInt(_value), sig(_sig) {}

    void visit(Visitor &visitor);

    const BitmaskSig *sig;
};


class Struct : public Value
{
public:
    Struct(StructSig *_sig) : sig(_sig), members(_sig->num_members) { }
    ~Struct();

    bool toBool(void) const;
    void visit(Visitor &visitor);

    const StructSig *sig;
    std::vector<Value *> members;
};


class Array : public Value
{
public:
    Array(size_t len) : values(len) {}
    ~Array();

    bool toBool(void) const;
    void visit(Visitor &visitor);

    std::vector<Value *> values;
};


class Blob : public Value
{
public:
    Blob(size_t _size) {
        size = _size;
        buf = new char[_size];
    }

    ~Blob();

    bool toBool(void) const;
    void *toPointer(void) const;
    void visit(Visitor &visitor);

    size_t size;
    char *buf;
};


class Pointer : public UInt
{
public:
    Pointer(unsigned long long value) : UInt(value) {}

    bool toBool(void) const;
    void *toPointer(void) const;
    unsigned long long toUIntPtr(void) const;
    void visit(Visitor &visitor);
};


class Visitor
{
public:
    virtual void visit(Null *);
    virtual void visit(Bool *);
    virtual void visit(SInt *);
    virtual void visit(UInt *);
    virtual void visit(Float *);
    virtual void visit(String *);
    virtual void visit(Enum *);
    virtual void visit(Bitmask *);
    virtual void visit(Struct *);
    virtual void visit(Array *);
    virtual void visit(Blob *);
    virtual void visit(Pointer *);

protected:
    inline void _visit(Value *value) {
        if (value) { 
            value->visit(*this); 
        }
    }
};


std::ostream & operator <<(std::ostream &os, Value *value);


class Call
{
public:
    unsigned no;
    const FunctionSig *sig;
    std::vector<Value *> args;
    Value *ret;

    Call(FunctionSig *_sig) : sig(_sig), args(_sig->num_args), ret(0) { }
    ~Call();

    inline const char * name(void) const {
        return sig->name;
    }

    inline Value & arg(unsigned index) {
        assert(index < args.size());
        return *(args[index]);
    }
};


std::ostream & operator <<(std::ostream &os, Call &call);


} /* namespace Trace */

#endif /* _TRACE_MODEL_HPP_ */
