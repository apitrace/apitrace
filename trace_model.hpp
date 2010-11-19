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

#ifndef _TRACE_MODEL_HPP_
#define _TRACE_MODEL_HPP_


#include <cassert>

#include <string>
#include <map>
#include <list>
#include <vector>
#include <iostream>


namespace Trace {


class Visitor;
class Dumper;
class UInt;


class Value
{
public:
   virtual void visit(Visitor &visitor) = 0;
};


class Void : public Value
{
public:
   void visit(Visitor &visitor);
};


class Bool : public Value
{
public:
   Bool(bool _value) : value(_value) {}

   void visit(Visitor &visitor);

   bool value;
};


class SInt : public Value
{
public:
   SInt(signed long long _value) : value(_value) {}

   void visit(Visitor &visitor);

   signed long long value;
};


class UInt : public Value
{
public:
   UInt(unsigned long long _value) : value(_value) {}

   void visit(Visitor &visitor);

   unsigned long long value;
};


class Float : public Value
{
public:
   Float(double _value) : value(_value) {}

   void visit(Visitor &visitor);

   double value;
};


class String : public Value
{
public:
   String(std::string _value) : value(_value) {}

   void visit(Visitor &visitor);

   std::string value;
};


class Const : public Value
{
public:
   Const(std::string _name, Value *_value) : name(_name), value(_value) {}

   void visit(Visitor &visitor);

   std::string name;
   Value *value;
};


class Array : public Value
{
public:
   Array(size_t len) : values(len) {}

   void visit(Visitor &visitor);

   std::vector<Value *> values;
};


class Visitor
{
public:
   virtual void visit(Void *) {assert(0);}
   virtual void visit(Bool *) {assert(0);}
   virtual void visit(SInt *) {assert(0);}
   virtual void visit(UInt *) {assert(0);}
   virtual void visit(Float *) {assert(0);}
   virtual void visit(String *) {assert(0);}
   virtual void visit(Const *) {assert(0);}
   virtual void visit(Array *) {assert(0);}
};


std::ostream & operator <<(std::ostream &os, Value *value);


signed long long asSInt(const Value *node);
unsigned long long asUInt(const Value *node);
double asFloat(const Value *node);


typedef std::pair<std::string, Value *> Arg;

class Call
{
public:
   std::string name;
   std::list<Arg> args;
   Value *ret;

   Call() : ret(0) { }

   Value * get_arg(const char *name) {
      for (std::list<Arg>::iterator it = args.begin(); it != args.end(); ++it) {
         if (it->first == name) {
            return it->second;
         }
      }
      return NULL;
   }
};



std::ostream & operator <<(std::ostream &os, Call &call);


} /* namespace Trace */

#endif /* _TRACE_MODEL_HPP_ */
