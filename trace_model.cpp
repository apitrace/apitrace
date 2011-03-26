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


namespace Trace {


Call::~Call() {
    for (unsigned i = 0; i < args.size(); ++i) {
        delete args[i];
    }

    if (ret) {
        delete ret;
    }
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
    // TODO: Don't leak blobs.  Blobs are often bound and accessed during many
    // calls, so we can't delete them here.
    //delete [] buf;
}


// bool cast
Null   ::operator bool(void) const { return false; }
Bool   ::operator bool(void) const { return value; }
SInt   ::operator bool(void) const { return value != 0; }
UInt   ::operator bool(void) const { return value != 0; }
Float  ::operator bool(void) const { return value != 0; }
String ::operator bool(void) const { return true; }
Enum   ::operator bool(void) const { return static_cast<bool>(*sig->second); }
Struct ::operator bool(void) const { return true; }
Array  ::operator bool(void) const { return true; }
Blob   ::operator bool(void) const { return true; }
Pointer::operator bool(void) const { return value != 0; }


// signed integer cast
Value  ::operator signed long long (void) const { assert(0); return NULL; }
Null   ::operator signed long long (void) const { return 0; }
Bool   ::operator signed long long (void) const { return static_cast<signed long long>(value); }
SInt   ::operator signed long long (void) const { return value; }
UInt   ::operator signed long long (void) const { assert(static_cast<signed long long>(value) >= 0); return static_cast<signed long long>(value); }
Float  ::operator signed long long (void) const { return static_cast<signed long long>(value); }
Enum   ::operator signed long long (void) const { return static_cast<signed long long>(*sig->second); }


// unsigned integer cast
Value  ::operator unsigned long long (void) const { assert(0); return NULL; }
Null   ::operator unsigned long long (void) const { return 0; }
Bool   ::operator unsigned long long (void) const { return static_cast<unsigned long long>(value); }
SInt   ::operator unsigned long long (void) const { assert(value >= 0); return static_cast<signed long long>(value); }
UInt   ::operator unsigned long long (void) const { return value; }
Float  ::operator unsigned long long (void) const { return static_cast<unsigned long long>(value); }
Enum   ::operator unsigned long long (void) const { return static_cast<unsigned long long>(*sig->second); }


// floating point cast
Value  ::operator double (void) const { assert(0); return NULL; }
Null   ::operator double (void) const { return 0; }
Bool   ::operator double (void) const { return static_cast<double>(value); }
SInt   ::operator double (void) const { return static_cast<double>(value); }
UInt   ::operator double (void) const { return static_cast<double>(value); }
Float  ::operator double (void) const { return value; }
Enum   ::operator double (void) const { return static_cast<unsigned long long>(*sig->second); }


// blob cast
void * Value  ::blob(void) const { assert(0); return NULL; }
void * Null   ::blob(void) const { return NULL; }
void * Blob   ::blob(void) const { return buf; }
void * Pointer::blob(void) const { assert(value < 0x100000ULL); return (void *)value; }


// virtual Value::visit()
void Null   ::visit(Visitor &visitor) { visitor.visit(this); }
void Bool   ::visit(Visitor &visitor) { visitor.visit(this); }
void SInt   ::visit(Visitor &visitor) { visitor.visit(this); }
void UInt   ::visit(Visitor &visitor) { visitor.visit(this); }
void Float  ::visit(Visitor &visitor) { visitor.visit(this); }
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
void Visitor::visit(String *) { assert(0); }
void Visitor::visit(Enum *node) { _visit(node->sig->second); }
void Visitor::visit(Bitmask *node) { visit(static_cast<UInt *>(node)); }
void Visitor::visit(Struct *) { assert(0); }
void Visitor::visit(Array *) { assert(0); }
void Visitor::visit(Blob *) { assert(0); }
void Visitor::visit(Pointer *) { assert(0); }


class Dumper : public Visitor
{
protected:
    std::ostream &os;
    Formatter::Formatter *formatter;
    Formatter::Attribute *normal;
    Formatter::Attribute *bold;
    Formatter::Attribute *italic;
    Formatter::Attribute *red;
    Formatter::Attribute *pointer;
    Formatter::Attribute *literal;

public:
    Dumper(std::ostream &_os) : os(_os) {
        formatter = Formatter::defaultFormatter();
        normal = formatter->normal();
        bold = formatter->bold();
        italic = formatter->italic();
        red = formatter->color(Formatter::RED);
        pointer = formatter->color(Formatter::GREEN);
        literal = formatter->color(Formatter::BLUE);
    }

    ~Dumper() {
        delete normal;
        delete bold;
        delete italic;
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

    void visit(String *node) {
        os << literal << '"' << node->value << '"' << normal;
    }

    void visit(Enum *node) {
        os << literal << node->sig->first << normal;
    }

    void visit(Bitmask *bitmask) {
        unsigned long long value = bitmask->value;
        const Bitmask::Signature *sig = bitmask->sig;
        bool first = true;
        for (Bitmask::Signature::const_iterator it = sig->begin(); value != 0 && it != sig->end(); ++it) {
            assert(it->second);
            if ((value & it->second) == it->second) {
                if (!first) {
                    os << " | ";
                }
                os << literal << it->first << normal;
                value &= ~it->second;
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
        const char *sep = "";
        os << bold << call->sig->name << normal << "(";
        for (unsigned i = 0; i < call->args.size(); ++i) {
            os << sep << italic << call->sig->arg_names[i] << normal << " = ";
            _visit(call->args[i]);
            sep = ", ";
        }
        os << ")";
        if (call->ret) {
            os << " = ";
            _visit(call->ret);
        }
        os << "\n";
    }
};


std::ostream & operator <<(std::ostream &os, Value *value) {
    Dumper d(os);
    if (value) {
        value->visit(d);
    }
    return os;
}


static inline const Value *unwrap(const Value *node) {
    const Enum *c = dynamic_cast<const Enum *>(node);
    if (c)
        return c->sig->second;
    return node;
}


static Null null;

const Value & Value::operator[](size_t index) const {
    const Array *array = dynamic_cast<const Array *>(unwrap(this));
    if (array) {
        if (index < array->values.size()) {
            return *array->values[index];
        }
    }
    return null;
}

const char * Value::string(void) const {
    const String *string = dynamic_cast<const String *>(unwrap(this));
    if (string)
        return string->value.c_str();
    const Null *null = dynamic_cast<const Null *>(unwrap(this));
    if (null)
        return NULL;
    assert(0);
    return NULL;
}

std::ostream & operator <<(std::ostream &os, Call &call) {
    Dumper d(os);
    os << call.no << " ";
    d.visit(&call);
    return os;
}


} /* namespace Trace */
