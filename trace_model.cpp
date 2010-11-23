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


void Null::visit(Visitor &visitor) {
   visitor.visit(this);
}

void Bool::visit(Visitor &visitor) {
   visitor.visit(this);
}

void SInt::visit(Visitor &visitor) {
   visitor.visit(this);
}

void UInt::visit(Visitor &visitor) {
   visitor.visit(this);
}

void Float::visit(Visitor &visitor) {
   visitor.visit(this);
}

void String::visit(Visitor &visitor) {
   visitor.visit(this);
}

void Const::visit(Visitor &visitor) {
   visitor.visit(this);
}

void Array::visit(Visitor &visitor) {
   visitor.visit(this);
}

void Blob::visit(Visitor &visitor) {
   visitor.visit(this);
}


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

   void visit(Null *node) {
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

   void visit(Const *node) {
      os << literal << node->name << normal;
   }

   void visit(Array *array) {
      if (array->values.size() == 1) {
         os << "&";
         array->values[0]->visit(*this);
      }
      else {
         const char *sep = "";
         os << "{";
         for (std::vector<Value *>::iterator it = array->values.begin(); it != array->values.end(); ++it) {
            os << sep;
            (*it)->visit(*this);
            sep = ", ";
         }
         os << "}";
      }
   }
   
   void visit(Blob *blob) {
      os << pointer << "blob(" << blob->size << ")" << normal;
   }

   void visit(Call *call) {
      const char *sep = "";
      os << bold << call->name << normal << "(";
      for (std::list<Arg>::iterator it = call->args.begin(); it != call->args.end(); ++it) {
         os << sep << italic << it->first << normal << " = ";
         it->second->visit(*this);
         sep = ", ";
      }
      os << ")";
      if (call->ret) {
         os << " = ";
         call->ret->visit(*this);
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
   const Const *c = dynamic_cast<const Const *>(node);
   if (c)
      return c->value;
   return node;
}


Value::operator signed long long(void) const {
   const SInt *sint = dynamic_cast<const SInt *>(unwrap(this));
   if (sint)
      return sint->value;
   const UInt *uint = dynamic_cast<const UInt *>(unwrap(this));
   if (uint)
      return uint->value;
   assert(0);
   return 0;
}

Value::operator unsigned long long(void) const {
   const UInt *uint = dynamic_cast<const UInt *>(unwrap(this));
   if (uint)
      return uint->value;
   assert(0);
   return 0;
}


Value::operator double(void) const {
   const Float *fl = dynamic_cast<const Float *>(unwrap(this));
   assert(fl);
   return fl->value;
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

void * Value::blob(void) const {
   const Blob *blob = dynamic_cast<const Blob *>(unwrap(this));
   if (blob)
       return blob->buf;
   const Null *null = dynamic_cast<const Null *>(unwrap(this));
   if (null);
       return NULL;
   assert(0);
   return NULL;
}

const char * Value::string(void) const {
   const String *string = dynamic_cast<const String *>(unwrap(this));
   if (string)
       return string->value.c_str();
   const Null *null = dynamic_cast<const Null *>(unwrap(this));
   if (null);
       return NULL;
   assert(0);
   return NULL;
}

Value & Call::arg(const char *name) {
   for (std::list<Arg>::iterator it = args.begin(); it != args.end(); ++it) {
      if (it->first == name) {
         return *it->second;
      }
   }
   return null;
}

std::ostream & operator <<(std::ostream &os, Call &call) {
   Dumper d(os);
   d.visit(&call);
   return os;
}


} /* namespace Trace */
