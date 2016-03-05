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


#include <string.h>
#include <deque>

#include "trace_model.hpp"


namespace trace {


static Null null;


Call::~Call() {
    for (auto & arg : args) {
        delete arg.value;
    }

    if (ret) {
        delete ret;
    }
}

Value &
Call::argByName(const char *argName) {
    for (unsigned i = 0; i < sig->num_args; ++i) {
        if (strcmp(sig->arg_names[i], argName) == 0) {
            return arg(i);
        }
    }
    return null;
}


String::~String() {
    delete [] value;
}


WString::~WString() {
    delete [] value;
}


Struct::~Struct() {
    for (auto & member : members) {
        delete member;
    }
}


Array::~Array() {
    for (auto & value : values) {
        delete value;
    }
}


#define BLOB_MAX_BOUND_SIZE (1*1024*1024*1024)

class BoundBlob {
public:
    static size_t totalSize;

private:
    size_t size;
    char *buf;

public:
    inline
    BoundBlob(size_t _size, char *_buf) :
        size(_size),
        buf(_buf)
    {
        assert(totalSize + size >= totalSize);
        totalSize += size;
    }

    inline
    ~BoundBlob()  {
        assert(totalSize >= size);
        totalSize -= size;
        delete [] buf;
    }

    // Fake move constructor
    // std::deque:push_back with move semantics was added only from c++11.
    BoundBlob(const BoundBlob & other)
    {
        size = other.size;
        buf = other.buf;
        const_cast<BoundBlob &>(other).size = 0;
        const_cast<BoundBlob &>(other).buf = 0;
    }

    // Disallow assignment operator
    BoundBlob& operator = (const BoundBlob &);
};

size_t BoundBlob::totalSize = 0;

typedef std::deque<BoundBlob> BoundBlobQueue;
static BoundBlobQueue boundBlobQueue;


Blob::~Blob() {
    // Blobs are often bound and referred during many calls, so we can't delete
    // them here in that case.
    //
    // Once bound there is no way to know when they were unbound, which
    // effectively means we have to leak them.  But some applications
    // (particularly OpenGL applications that use vertex arrays in user memory)
    // we can easily exhaust all memory.  So instead we maintain a queue of
    // bound blobs and keep the total size bounded.

    if (!bound) {
        delete [] buf;
        return;
    }

    while (!boundBlobQueue.empty() &&
           BoundBlob::totalSize + size > BLOB_MAX_BOUND_SIZE) {
        boundBlobQueue.pop_front();
    }

    boundBlobQueue.push_back(BoundBlob(size, buf));
}

StackFrame::~StackFrame() {
    if (module != NULL) {
        delete [] module;
    }
    if (function != NULL) {
        delete [] function;
    }
    if (filename != NULL) {
        delete [] filename;
    }
}


// bool cast
bool Null   ::toBool(void) const { return false; }
bool Bool   ::toBool(void) const { return value; }
bool SInt   ::toBool(void) const { return value != 0; }
bool UInt   ::toBool(void) const { return value != 0; }
bool Float  ::toBool(void) const { return value != 0; }
bool Double ::toBool(void) const { return value != 0; }
bool String ::toBool(void) const { return true; }
bool WString::toBool(void) const { return true; }
bool Struct ::toBool(void) const { return true; }
bool Array  ::toBool(void) const { return true; }
bool Blob   ::toBool(void) const { return true; }
bool Pointer::toBool(void) const { return value != 0; }
bool Repr   ::toBool(void) const { return static_cast<bool>(machineValue); }


// signed integer cast
signed long long Value  ::toSInt(void) const { assert(0); return 0; }
signed long long Null   ::toSInt(void) const { return 0; }
signed long long Bool   ::toSInt(void) const { return static_cast<signed long long>(value); }
signed long long SInt   ::toSInt(void) const { return value; }
signed long long UInt   ::toSInt(void) const { assert(static_cast<signed long long>(value) >= 0); return static_cast<signed long long>(value); }
signed long long Float  ::toSInt(void) const { return static_cast<signed long long>(value); }
signed long long Double ::toSInt(void) const { return static_cast<signed long long>(value); }
signed long long Repr   ::toSInt(void) const { return machineValue->toSInt(); }


// unsigned integer cast
unsigned long long Value  ::toUInt(void) const { assert(0); return 0; }
unsigned long long Null   ::toUInt(void) const { return 0; }
unsigned long long Bool   ::toUInt(void) const { return static_cast<unsigned long long>(value); }
unsigned long long SInt   ::toUInt(void) const { assert(value >= 0); return static_cast<signed long long>(value); }
unsigned long long UInt   ::toUInt(void) const { return value; }
unsigned long long Float  ::toUInt(void) const { return static_cast<unsigned long long>(value); }
unsigned long long Double ::toUInt(void) const { return static_cast<unsigned long long>(value); }
unsigned long long Repr   ::toUInt(void) const { return machineValue->toUInt(); }


// floating point cast
float Value  ::toFloat(void) const { assert(0); return 0; }
float Null   ::toFloat(void) const { return 0; }
float Bool   ::toFloat(void) const { return static_cast<float>(value); }
float SInt   ::toFloat(void) const { return static_cast<float>(value); }
float UInt   ::toFloat(void) const { return static_cast<float>(value); }
float Float  ::toFloat(void) const { return value; }
float Double ::toFloat(void) const { return value; }
float Repr   ::toFloat(void) const { return machineValue->toFloat(); }


// floating point cast
double Value  ::toDouble(void) const { assert(0); return 0; }
double Null   ::toDouble(void) const { return 0; }
double Bool   ::toDouble(void) const { return static_cast<double>(value); }
double SInt   ::toDouble(void) const { return static_cast<double>(value); }
double UInt   ::toDouble(void) const { return static_cast<double>(value); }
double Float  ::toDouble(void) const { return value; }
double Double ::toDouble(void) const { return value; }
double Repr   ::toDouble(void) const { return machineValue->toDouble(); }


// pointer cast
void * Value  ::toPointer(void) const { assert(0); return NULL; }
void * Null   ::toPointer(void) const { return NULL; }
void * Blob   ::toPointer(void) const { return buf; }
void * Pointer::toPointer(void) const { return (void *)value; }
void * Repr   ::toPointer(void) const { return machineValue->toPointer(); }

void * Value  ::toPointer(bool bind) { assert(0); return NULL; }
void * Null   ::toPointer(bool bind) { return NULL; }
void * Blob   ::toPointer(bool bind) { if (bind) bound = true; return buf; }
void * Pointer::toPointer(bool bind) { return (void *)value; }
void * Repr   ::toPointer(bool bind) { return machineValue->toPointer(bind); }


// unsigned int pointer cast
unsigned long long Value  ::toUIntPtr(void) const { assert(0); return 0; }
unsigned long long Null   ::toUIntPtr(void) const { return 0; }
unsigned long long Pointer::toUIntPtr(void) const { return value; }
unsigned long long Repr   ::toUIntPtr(void) const { return machineValue->toUIntPtr(); }


// string cast
const char * Value ::toString(void) const { assert(0); return NULL; }
const char * Null  ::toString(void) const { return NULL; }
const char * String::toString(void) const { return value; }
const char * Repr  ::toString(void) const { return machineValue->toString(); }


// virtual Value::visit()
void Null   ::visit(Visitor &visitor) { visitor.visit(this); }
void Bool   ::visit(Visitor &visitor) { visitor.visit(this); }
void SInt   ::visit(Visitor &visitor) { visitor.visit(this); }
void UInt   ::visit(Visitor &visitor) { visitor.visit(this); }
void Float  ::visit(Visitor &visitor) { visitor.visit(this); }
void Double ::visit(Visitor &visitor) { visitor.visit(this); }
void String ::visit(Visitor &visitor) { visitor.visit(this); }
void WString::visit(Visitor &visitor) { visitor.visit(this); }
void Enum   ::visit(Visitor &visitor) { visitor.visit(this); }
void Bitmask::visit(Visitor &visitor) { visitor.visit(this); }
void Struct ::visit(Visitor &visitor) { visitor.visit(this); }
void Array  ::visit(Visitor &visitor) { visitor.visit(this); }
void Blob   ::visit(Visitor &visitor) { visitor.visit(this); }
void Pointer::visit(Visitor &visitor) { visitor.visit(this); }
void Repr   ::visit(Visitor &visitor) { visitor.visit(this); }


void Visitor::visit(Null *) { assert(0); }
void Visitor::visit(Bool *) { assert(0); }
void Visitor::visit(SInt *) { assert(0); }
void Visitor::visit(UInt *) { assert(0); }
void Visitor::visit(Float *) { assert(0); }
void Visitor::visit(Double *) { assert(0); }
void Visitor::visit(String *) { assert(0); }
void Visitor::visit(WString *) { assert(0); }
void Visitor::visit(Enum *node) { assert(0); }
void Visitor::visit(Bitmask *node) { visit(static_cast<UInt *>(node)); }
void Visitor::visit(Struct *) { assert(0); }
void Visitor::visit(Array *) { assert(0); }
void Visitor::visit(Blob *) { assert(0); }
void Visitor::visit(Pointer *) { assert(0); }
void Visitor::visit(Repr *node) { node->machineValue->visit(*this); }


Value &
Value::operator[] (size_t index) const {
    const Array *array = toArray();
    if (array) {
        if (index < array->values.size()) {
            return *array->values[index];
        }
    }
    return null;
}

} /* namespace trace */
