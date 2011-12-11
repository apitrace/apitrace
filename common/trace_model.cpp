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


#include "formatter.hpp"
#include "trace_model.hpp"


namespace trace {


Call::~Call() {
    for (unsigned i = 0; i < args.size(); ++i) {
        delete args[i];
    }

    if (ret) {
        delete ret;
    }
}


String::~String() {
    delete [] value;
}


Struct::~Struct() {
    for (std::vector<Value *>::iterator it = members.begin(); it != members.end(); ++it) {
        delete *it;
    }
}


Array::~Array() {
    for (std::vector<Value *>::iterator it = values.begin(); it != values.end(); ++it) {
        delete *it;
    }
}

Blob::~Blob() {
    // Blobs are often bound and referred during many calls, so we can't delete
    // them here in that case.
    //
    // Once bound there is no way to know when they were unbound, which
    // effectively means we have to leak them.  A better solution would be to
    // keep a list of bound pointers, and defer the destruction to when the
    // trace in question has been fully processed.
    if (!bound) {
        delete [] buf;
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
bool Struct ::toBool(void) const { return true; }
bool Array  ::toBool(void) const { return true; }
bool Blob   ::toBool(void) const { return true; }
bool Pointer::toBool(void) const { return value != 0; }


// signed integer cast
signed long long Value  ::toSInt(void) const { assert(0); return 0; }
signed long long Null   ::toSInt(void) const { return 0; }
signed long long Bool   ::toSInt(void) const { return static_cast<signed long long>(value); }
signed long long SInt   ::toSInt(void) const { return value; }
signed long long UInt   ::toSInt(void) const { assert(static_cast<signed long long>(value) >= 0); return static_cast<signed long long>(value); }
signed long long Float  ::toSInt(void) const { return static_cast<signed long long>(value); }
signed long long Double ::toSInt(void) const { return static_cast<signed long long>(value); }


// unsigned integer cast
unsigned long long Value  ::toUInt(void) const { assert(0); return 0; }
unsigned long long Null   ::toUInt(void) const { return 0; }
unsigned long long Bool   ::toUInt(void) const { return static_cast<unsigned long long>(value); }
unsigned long long SInt   ::toUInt(void) const { assert(value >= 0); return static_cast<signed long long>(value); }
unsigned long long UInt   ::toUInt(void) const { return value; }
unsigned long long Float  ::toUInt(void) const { return static_cast<unsigned long long>(value); }
unsigned long long Double ::toUInt(void) const { return static_cast<unsigned long long>(value); }


// floating point cast
float Value  ::toFloat(void) const { assert(0); return 0; }
float Null   ::toFloat(void) const { return 0; }
float Bool   ::toFloat(void) const { return static_cast<float>(value); }
float SInt   ::toFloat(void) const { return static_cast<float>(value); }
float UInt   ::toFloat(void) const { return static_cast<float>(value); }
float Float  ::toFloat(void) const { return value; }
float Double ::toFloat(void) const { return value; }


// floating point cast
double Value  ::toDouble(void) const { assert(0); return 0; }
double Null   ::toDouble(void) const { return 0; }
double Bool   ::toDouble(void) const { return static_cast<double>(value); }
double SInt   ::toDouble(void) const { return static_cast<double>(value); }
double UInt   ::toDouble(void) const { return static_cast<double>(value); }
double Float  ::toDouble(void) const { return value; }
double Double ::toDouble(void) const { return value; }


// pointer cast
void * Value  ::toPointer(void) const { assert(0); return NULL; }
void * Null   ::toPointer(void) const { return NULL; }
void * Blob   ::toPointer(void) const { return buf; }
void * Pointer::toPointer(void) const { return (void *)value; }

void * Value  ::toPointer(bool bind) { assert(0); return NULL; }
void * Null   ::toPointer(bool bind) { return NULL; }
void * Blob   ::toPointer(bool bind) { if (bind) bound = true; return buf; }
void * Pointer::toPointer(bool bind) { return (void *)value; }


// pointer cast
unsigned long long Value  ::toUIntPtr(void) const { assert(0); return 0; }
unsigned long long Null   ::toUIntPtr(void) const { return 0; }
unsigned long long Pointer::toUIntPtr(void) const { return value; }


// string cast
const char * Value ::toString(void) const { assert(0); return NULL; }
const char * Null  ::toString(void) const { return NULL; }
const char * String::toString(void) const { return value; }


// virtual Value::visit()
void Null   ::visit(Visitor &visitor) { visitor.visit(this); }
void Bool   ::visit(Visitor &visitor) { visitor.visit(this); }
void SInt   ::visit(Visitor &visitor) { visitor.visit(this); }
void UInt   ::visit(Visitor &visitor) { visitor.visit(this); }
void Float  ::visit(Visitor &visitor) { visitor.visit(this); }
void Double ::visit(Visitor &visitor) { visitor.visit(this); }
void String ::visit(Visitor &visitor) { visitor.visit(this); }
void Enum   ::visit(Visitor &visitor) { visitor.visit(this); }
void Bitmask::visit(Visitor &visitor) { visitor.visit(this); }
void Struct ::visit(Visitor &visitor) { visitor.visit(this); }
void Array  ::visit(Visitor &visitor) { visitor.visit(this); }
void Blob   ::visit(Visitor &visitor) { visitor.visit(this); }
void Pointer::visit(Visitor &visitor) { visitor.visit(this); }


void Visitor::visit(Null *) { assert(0); }
void Visitor::visit(Bool *) { assert(0); }
void Visitor::visit(SInt *) { assert(0); }
void Visitor::visit(UInt *) { assert(0); }
void Visitor::visit(Float *) { assert(0); }
void Visitor::visit(Double *) { assert(0); }
void Visitor::visit(String *) { assert(0); }
void Visitor::visit(Enum *node) { assert(0); }
void Visitor::visit(Bitmask *node) { visit(static_cast<UInt *>(node)); }
void Visitor::visit(Struct *) { assert(0); }
void Visitor::visit(Array *) { assert(0); }
void Visitor::visit(Blob *) { assert(0); }
void Visitor::visit(Pointer *) { assert(0); }


class Dumper : public Visitor
{
protected:
    std::ostream &os;
    formatter::Formatter *formatter;
    formatter::Attribute *normal;
    formatter::Attribute *bold;
    formatter::Attribute *italic;
    formatter::Attribute *strike;
    formatter::Attribute *red;
    formatter::Attribute *pointer;
    formatter::Attribute *literal;

public:
    Dumper(std::ostream &_os, bool color) : os(_os) {
        formatter = formatter::defaultFormatter(color);
        normal = formatter->normal();
        bold = formatter->bold();
        italic = formatter->italic();
        strike = formatter->strike();
        red = formatter->color(formatter::RED);
        pointer = formatter->color(formatter::GREEN);
        literal = formatter->color(formatter::BLUE);
    }

    ~Dumper() {
        delete normal;
        delete bold;
        delete italic;
        delete strike;
        delete red;
        delete pointer;
        delete literal;
        delete formatter;
    }

    void visit(Null *) {
        os << "NULL";
    }

    void visit(Bool *node) {
        os << literal << (node->value ? "true" : "false") << normal;
    }

    void visit(SInt *node) {
        os << literal << node->value << normal;
    }

    void visit(UInt *node) {
        os << literal << node->value << normal;
    }

    void visit(Float *node) {
        os << literal << node->value << normal;
    }

    void visit(Double *node) {
        os << literal << node->value << normal;
    }

    void visit(String *node) {
        os << literal << "\"";
        for (const char *it = node->value; *it; ++it) {
            unsigned char c = (unsigned char) *it;
            if (c == '\"')
                os << "\\\"";
            else if (c == '\\')
                os << "\\\\";
            else if (c >= 0x20 && c <= 0x7e)
                os << c;
            else if (c == '\t') {
                os << "\t";
            } else if (c == '\r') {
                // Ignore carriage-return
            } else if (c == '\n') {
                // Reset formatting so that it looks correct with 'less -R'
                os << normal << '\n' << literal;
            } else {
                unsigned octal0 = c & 0x7;
                unsigned octal1 = (c >> 3) & 0x7;
                unsigned octal2 = (c >> 3) & 0x7;
                os << "\\";
                if (octal2)
                    os << octal2;
                if (octal1)
                    os << octal1;
                os << octal0;
            }
        }
        os << "\"" << normal;
    }

    void visit(Enum *node) {
        const EnumSig *sig = node->sig;
        for (const EnumValue *it = sig->values; it != sig->values + sig->num_values; ++it) {
            if (it->value == node->value) {
                os << literal << it->name << normal;
                return;
            }
        }
        os << literal << node->value << normal;
    }

    void visit(Bitmask *bitmask) {
        unsigned long long value = bitmask->value;
        const BitmaskSig *sig = bitmask->sig;
        bool first = true;
        for (const BitmaskFlag *it = sig->flags; value != 0 && it != sig->flags + sig->num_flags; ++it) {
            if ((it->value && (value & it->value) == it->value) ||
                (!it->value && value == 0)) {
                if (!first) {
                    os << " | ";
                }
                os << literal << it->name << normal;
                value &= ~it->value;
                first = false;
            }
        }
        if (value || first) {
            if (!first) {
                os << " | ";
            }
            os << literal << "0x" << std::hex << value << std::dec << normal;
        }
    }

    void visit(Struct *s) {
        const char *sep = "";
        os << "{";
        for (unsigned i = 0; i < s->members.size(); ++i) {
            os << sep << italic << s->sig->member_names[i] << normal << " = ";
            _visit(s->members[i]);
            sep = ", ";
        }
        os << "}";
    }

    void visit(Array *array) {
        if (array->values.size() == 1) {
            os << "&";
            _visit(array->values[0]);
        }
        else {
            const char *sep = "";
            os << "{";
            for (std::vector<Value *>::iterator it = array->values.begin(); it != array->values.end(); ++it) {
                os << sep;
                _visit(*it);
                sep = ", ";
            }
            os << "}";
        }
    }

    void visit(Blob *blob) {
        os << pointer << "blob(" << blob->size << ")" << normal;
    }

    void visit(Pointer *p) {
        os << pointer << "0x" << std::hex << p->value << std::dec << normal;
    }

    void visit(Call *call) {
        CallFlags flags = call->flags;

        if (flags & CALL_FLAG_NON_REPRODUCIBLE) {
            os << strike;
        } else if (flags & (CALL_FLAG_FAKE | CALL_FLAG_NO_SIDE_EFFECTS)) {
            os << normal;
        } else {
            os << bold;
        }
        os << call->sig->name << normal;

        os << "(";
        const char *sep = "";
        for (unsigned i = 0; i < call->args.size(); ++i) {
            os << sep << italic << call->sig->arg_names[i] << normal << " = ";
            if (call->args[i]) {
                _visit(call->args[i]);
            } else {
               os << "?";
            }
            sep = ", ";
        }
        os << ")";

        if (call->ret) {
            os << " = ";
            _visit(call->ret);
        }
        
        if (flags & CALL_FLAG_INCOMPLETE) {
            os << " // " << red << "incomplete" << normal;
        }
        
        os << "\n";

        if (flags & CALL_FLAG_END_FRAME) {
            os << "\n";
        }
    }
};


void Value::dump(std::ostream &os, bool color) {
    Dumper d(os, color);
    visit(d);
}


static Null null;

const Value & Value::operator[](size_t index) const {
    const Array *array = dynamic_cast<const Array *>(this);
    if (array) {
        if (index < array->values.size()) {
            return *array->values[index];
        }
    }
    return null;
}

void Call::dump(std::ostream &os, bool color) {
    Dumper d(os, color);
    os << no << " ";
    d.visit(this);
}


} /* namespace trace */
