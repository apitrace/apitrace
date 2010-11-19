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


#include <cassert>

#include <iostream>

#include <zlib.h>

#include "trace_format.hpp"
#include "trace_model.hpp"


namespace Trace {


class Parser
{
protected:
   gzFile file;
public:
   Parser() {
      file = NULL;
   }

   bool parse(const char *filename) {
      unsigned long long version;

      file = gzopen(filename, "rb");
      if (!file) {
         return false;
      }

      version = read_uint();
      if (version != TRACE_VERSION) {
         std::cerr << "Unsupported format version" << version << "\n";
         return false;
      }

      while (!gzeof(file)) {
         parse_call();
      }

      return true;
   }

   void parse_call(void) {
      Call call;
      call.name = read_string();
      int c;
      do {
         c = gzgetc(file);
         if (c == Trace::CALL_END || c == -1) {
            break;
         }
         switch(c) {
         case Trace::CALL_END:
            return;
         case Trace::CALL_ARG:
            call.args.push_back(parse_arg());
            break;
         case Trace::CALL_RET:
            call.ret = parse_value();
            break;
         default:
            assert(0);
            std::cerr << "Unknown call detail " << c << "\n";
            break;
         }
      } while(true);
      handle_call(call);
   }
   
   virtual void handle_call(Call &call) {
      std::cout << call;
   }

   Arg parse_arg(void) {
      std::string name = read_string();
      Value *value = parse_value();
      return Arg(name, value);
   }
   
   Value *parse_value(void) {
      int c;
      c = gzgetc(file);
      switch(c) {
      case Trace::TYPE_BOOL:
         return parse_bool();
      case Trace::TYPE_SINT:
         return parse_sint();
      case Trace::TYPE_UINT:
         return parse_uint();
      case Trace::TYPE_FLOAT:
         return parse_float();
      case Trace::TYPE_DOUBLE:
         return parse_double();
      case Trace::TYPE_STRING:
         return parse_string();
      case Trace::TYPE_CONST:
         return parse_const();
      case Trace::TYPE_BITMASK:
         return parse_bitmask();
      case Trace::TYPE_ARRAY:
         return parse_array();
      case Trace::TYPE_POINTER:
         return parse_pointer();
      case Trace::TYPE_VOID:
         return NULL;
      default:
         std::cerr << "Unknown type " << c << "\n";
         assert(0);
         return NULL;
      }
   }

   Value *parse_bool() {
      int c;
      c = gzgetc(file);
      return new Bool(c);
   }
   
   Value *parse_sint() {
      return new SInt(-read_uint());
   }
   
   Value *parse_uint() {
      return new UInt(read_uint());
   }
   
   Value *parse_float() {
      float value;
      gzread(file, &value, sizeof value);
      return new Float(value);
   }
   
   Value *parse_double() {
      double value;
      gzread(file, &value, sizeof value);
      return new Float(value);
   }
   
   Value *parse_string() {
      return new String(read_string());
   }
   
   Value *parse_const() {
      std::string name = read_string();
      Value *value = parse_value();
      return new Const(name, value);
   }
   
   Value *parse_bitmask() {
      unsigned long long value = 0;
      int c;
      do {
         c = gzgetc(file);
         switch(c) {
         case Trace::TYPE_SINT:
            value |= -read_uint();
            break;
         case Trace::TYPE_UINT:
            value |= read_uint();
            break;
         case Trace::TYPE_CONST:
            read_string();
            break;
         case Trace::TYPE_VOID:
            goto done;
         default:
            std::cerr << "Unexpected type " << c << "\n";
            assert(0);
            return NULL;
         }
      } while(true);
done:
      return new UInt(value);
   }

   Value *parse_array() {
      size_t len = read_uint();
      Array *array = new Array(len);
      for (size_t i = 0; i < len; ++i) {
         array->values[i] = parse_value();
      }
      return array;
   }
   
   Value *parse_pointer() {
      unsigned long long addr;
      Value *value;
      addr = read_uint();
      value = parse_value();
      if (!value)
         value = new UInt(addr);
      return value;
   }
   
   std::string read_string(void) {
      size_t len = read_uint();
      char * buf = new char[len];
      gzread(file, buf, len);
      std::string value(buf, len);
      delete [] buf;
      return value;
   }

   unsigned long long read_uint(void) {
      unsigned long long value = 0;
      int c;
      unsigned shift = 0;
      do {
         c = gzgetc(file);
         if (c == -1) {
            break;
         }
         value |= (unsigned long long)(c & 0x7f) << shift;
         shift += 7;
      } while(c & 0x80);
      return value;
   }
};


} /* namespace Trace */

#endif /* _TRACE_PARSER_HPP_ */
