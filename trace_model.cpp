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


#include "trace_model.hpp"


namespace Trace {


void Void::visit(Visitor &visitor) {
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


class Dumper : public Visitor
{
public:
   std::ostream &os;

   Dumper() : os(std::cout) {}

   Dumper(std::ostream &_os) : os(_os) {}

   void visit(Void *node) {
   }

   void visit(Bool *node) {
      os << (node->value ? "true" : "false");
   }

   void visit(SInt *node) {
      os << node->value;
   }

   void visit(UInt *node) {
      os << node->value;
   }

   void visit(Float *node) {
      os << node->value;
   }

   void visit(String *node) {
      os << '"' << node->value << '"';
   }

   void visit(Const *node) {
      os << node->name;
   }

   void visit(Array *node) {
      const char *sep = "";
      os << "{";
      for (std::vector<Value *>::iterator it = node->values.begin(); it != node->values.end(); ++it) {
         os << sep;
         (*it)->visit(*this);
         sep = ", ";
      }
      os << "}";
   }
};


std::ostream & operator <<(std::ostream &os, Value *value) {
   Dumper d(os);
   if (value) {
      value->visit(d);
   }
   return os;
}


static const Value *unwrap(const Value *node) {
   const Const *c = dynamic_cast<const Const *>(node);
   if (c)
      return c->value;
   return node;
}

signed long long asSInt(const Value *node) {
   node = unwrap(node);
   const SInt *sint = dynamic_cast<const SInt *>(node);
   if (sint)
      return sint->value;
   const UInt *uint = dynamic_cast<const UInt *>(node);
   if (uint)
      return uint->value;
   assert(0);
   return 0;
}

unsigned long long asUInt(const Value *node) {
   node = unwrap(node);
   const UInt *uint = dynamic_cast<const UInt *>(node);
   if (uint)
      return uint->value;
   assert(0);
   return 0;
}


double asFloat(const Value *node) {
   node = unwrap(node);
   const Float *fl = dynamic_cast<const Float *>(node);
   assert(fl);
   return fl->value;
}


std::ostream & operator <<(std::ostream &os, Call &call) {
   const char *sep = "";
   os << call.name << "(";
   for (std::list<Arg>::iterator it = call.args.begin(); it != call.args.end(); ++it) {
      os << sep << it->first << " = " << it->second;
      sep = ", ";
   }
   os << ")";
   if (call.ret) {
      os << " = " << call.ret;
   }
   os << "\n";
   return os;
}


} /* namespace Trace */
