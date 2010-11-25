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
#include <map>
#include <string>

#include <zlib.h>

#include "trace_format.hpp"
#include "trace_model.hpp"


namespace Trace {


class Parser
{
protected:
   gzFile file;

   typedef std::map<size_t, std::string> namemap;
   namemap names;

public:
   Parser() {
      file = NULL;
   }

   ~Parser() {
      close();
   }

   bool open(const char *filename) {
      unsigned long long version;

      file = gzopen(filename, "rb");
      if (!file) {
         return false;
      }

      version = read_uint();
      if (version != TRACE_VERSION) {
         std::cerr << "error: unsupported format version" << version << "\n";
         return false;
      }

      return true;
   }

   void close(void) {
      if (file) {
         gzclose(file);
         file = NULL;
      }
   }

   Call *parse_call(void) {
      Call *call = new Call;
      call->name = read_name();
      do {
         int c = read_byte();
         switch(c) {
         case Trace::CALL_END:
            return call;
         case Trace::CALL_ARG:
            parse_arg(call);
            break;
         case Trace::CALL_RET:
            call->ret = parse_value();
            break;
         default:
            std::cerr << "error: unknown call detail " << c << "\n";
            assert(0);
            /* fallthrough */
         case -1:
            delete call;
            return NULL;
         }
      } while(true);
   }
   
   void parse_arg(Call *call) {
      unsigned index = read_uint();
      std::string name = read_name();
      Value *value = parse_value();
      if (index >= call->args.size()) {
          call->args.resize(index + 1);
      }
      call->args[index] = Arg(name, value);
   }
   
   Value *parse_value(void) {
      int c;
      c = read_byte();
      switch(c) {
      case Trace::TYPE_NULL:
         return new Null;
      case Trace::TYPE_FALSE:
         return new Bool(false);
      case Trace::TYPE_TRUE:
         return new Bool(true);
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
      case Trace::TYPE_STRUCT:
         return parse_struct();
      case Trace::TYPE_BLOB:
         return parse_blob();
      case Trace::TYPE_OPAQUE:
         return parse_opaque();
      default:
         std::cerr << "error: unknown type " << c << "\n";
         assert(0);
         return NULL;
      }
   }

   Value *parse_sint() {
      return new SInt(-(signed long long)read_uint());
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
      std::string name = read_name();
      Value *value = parse_value();
      return new Const(name, value);
   }
   
   Value *parse_bitmask() {
      unsigned long long value = 0;
      int c;
      do {
         c = read_byte();
         switch(c) {
         case Trace::TYPE_SINT:
            value |= -(signed long long)read_uint();
            break;
         case Trace::TYPE_UINT:
            value |= read_uint();
            break;
         case Trace::TYPE_CONST:
            read_name();
            break;
         case Trace::TYPE_NULL:
            goto done;
         default:
            std::cerr << "error: uexpected type " << c << "\n";
            assert(0);
            return NULL;
         }
      } while(true);
done:
      return new UInt(value);
   }

   Value *parse_array(void) {
      size_t len = read_uint();
      Array *array = new Array(len);
      for (size_t i = 0; i < len; ++i) {
         array->values[i] = parse_value();
      }
      return array;
   }
   
   Value *parse_blob(void) {
      size_t size = read_uint();
      Blob *blob = new Blob(size);
      if (size) {
          gzread(file, blob->buf, size);
      }
      return blob;
   }
   
   Value *parse_struct() {
      size_t length = read_uint();
      /* XXX */
      for (size_t i = 0; i < length; ++i) {
         std::string name = read_name();
         Value *value = parse_value();
         std::cout << "  " << name << " = " << value << "\n";
      }
      return NULL;
   }
   
   Value *parse_opaque() {
      unsigned long long addr;
      addr = read_uint();
      /* XXX */
      return new UInt(addr);
   }

   std::string read_name(void) {
       std::string name;
       size_t id = read_uint();
       if (id >= names.size()) {
           name = read_string();
           names[id] = name;
           return name;
       } else {
           return names[id];
       }
   }
   
   std::string read_string(void) {
      size_t len = read_uint();
      if (!len) {
         return std::string();
      }
      char * buf = new char[len];
      gzread(file, buf, len);
      std::string value(buf, len);
      delete [] buf;
#ifdef TRACE_VERBOSE
      std::cerr << '"' << value << '"' << "\n";
#endif
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
#ifdef TRACE_VERBOSE
      std::cerr << value << "\n";
#endif
      return value;
   }

   int read_byte(void) {
      int c = gzgetc(file);
#ifdef TRACE_VERBOSE
      if (c < 0)
         std::cerr << "EOF" << "\n";
      else
         std::cerr << "0x" << std::hex << c << "\n";
#endif
      return c;
   }
};


} /* namespace Trace */

#endif /* _TRACE_PARSER_HPP_ */
