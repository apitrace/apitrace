/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
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


#include <assert.h>

#include <zlib.h>

#include "trace_parser.hpp"


#define TRACE_VERBOSE 0


namespace Trace {


unsigned long long Parser::version = 0;


Parser::Parser() {
    file = NULL;
    next_call_no = 0;
}


Parser::~Parser() {
    close();
}


bool Parser::open(const char *filename) {
    file = gzopen(filename, "rb");
    if (!file) {
        return false;
    }

    version = read_uint();
    if (version > TRACE_VERSION) {
        std::cerr << "error: unsupported format version" << version << "\n";
        return false;
    }

    return true;
}


void Parser::close(void) {
    if (file) {
        gzclose(file);
        file = NULL;
    }
}


Call *Parser::parse_call(void) {
    do {
        int c = read_byte();
        switch(c) {
        case Trace::EVENT_ENTER:
            parse_enter();
            break;
        case Trace::EVENT_LEAVE:
            return parse_leave();
        case Trace::EVENT_MESSAGE:
            std::cerr << "message: " << read_string() << "\n";
            break;
        default:
            std::cerr << "error: unknown call detail " << c << "\n";
            assert(0);
            /* fallthrough */
        case -1:
            return NULL;
        }
    } while(true);
}


/**
 * Helper function to lookup an ID in a vector, resizing the vector if it doesn't fit.
 */
template<class T>
T *lookup(std::vector<T *> &map, size_t index) {
    if (index >= map.size()) {
        map.resize(index + 1);
        return NULL;
    } else {
        return map[index];
    }
}


void Parser::parse_enter(void) {
    size_t id = read_uint();

    Call::Signature *sig = lookup(functions, id);
    if (!sig) {
        sig = new Call::Signature;
        sig->name = read_string();
        unsigned size = read_uint();
        for (unsigned i = 0; i < size; ++i) {
            sig->arg_names.push_back(read_string());
        }
        functions[id] = sig;
    }
    assert(sig);

    Call *call = new Call(sig);
    call->no = next_call_no++;

    parse_call_details(call);

    calls.push_back(call);
}


Call *Parser::parse_leave(void) {
    unsigned call_no = read_uint();

    Call *call = NULL;
    for (CallList::iterator it = calls.begin(); it != calls.end(); ++it) {
        if ((*it)->no == call_no) {
            call = *it;
            calls.erase(it);
            break;
        }
    }
    assert(call);
    if (!call) {
        return NULL;
    }
    parse_call_details(call);
    return call;
}


void Parser::parse_call_details(Call *call) {
    do {
        int c = read_byte();
        switch(c) {
        case Trace::CALL_END:
            return;
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
            return;
        }
    } while(true);
}


void Parser::parse_arg(Call *call) {
    unsigned index = read_uint();
    Value *value = parse_value();
    if (index >= call->args.size()) {
        call->args.resize(index + 1);
    }
    call->args[index] = value;
}


Value *Parser::parse_value(void) {
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
    case Trace::TYPE_ENUM:
        return parse_enum();
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


Value *Parser::parse_sint() {
    return new SInt(-(signed long long)read_uint());
}


Value *Parser::parse_uint() {
    return new UInt(read_uint());
}


Value *Parser::parse_float() {
    float value;
    gzread(file, &value, sizeof value);
    return new Float(value);
}


Value *Parser::parse_double() {
    double value;
    gzread(file, &value, sizeof value);
    return new Float(value);
}


Value *Parser::parse_string() {
    return new String(read_string());
}


Value *Parser::parse_enum() {
    size_t id = read_uint();
    Enum::Signature *sig = lookup(enums, id);
    if (!sig) {
        std::string name = read_string();
        Value *value = parse_value();
        sig = new Enum::Signature(name, value);
        enums[id] = sig;
    }
    assert(sig);
    return new Enum(sig);
}


Value *Parser::parse_bitmask() {
    size_t id = read_uint();
    Bitmask::Signature *sig = lookup(bitmasks, id);
    if (!sig) {
        size_t size = read_uint();
        sig = new Bitmask::Signature(size);
        for (Bitmask::Signature::iterator it = sig->begin(); it != sig->end(); ++it) {
            it->first = read_string();
            it->second = read_uint();
            assert(it->second);
        }
        bitmasks[id] = sig;
    }
    assert(sig);

    unsigned long long value = read_uint();

    return new Bitmask(sig, value);
}


Value *Parser::parse_array(void) {
    size_t len = read_uint();
    Array *array = new Array(len);
    for (size_t i = 0; i < len; ++i) {
        array->values[i] = parse_value();
    }
    return array;
}


Value *Parser::parse_blob(void) {
    size_t size = read_uint();
    Blob *blob = new Blob(size);
    if (size) {
        gzread(file, blob->buf, (unsigned)size);
    }
    return blob;
}


Value *Parser::parse_struct() {
    size_t id = read_uint();

    Struct::Signature *sig = lookup(structs, id);
    if (!sig) {
        sig = new Struct::Signature;
        sig->name = read_string();
        unsigned size = read_uint();
        for (unsigned i = 0; i < size; ++i) {
            sig->member_names.push_back(read_string());
        }
        structs[id] = sig;
    }
    assert(sig);

    Struct *value = new Struct(sig);

    for (size_t i = 0; i < sig->member_names.size(); ++i) {
        value->members[i] = parse_value();
    }

    return value;
}


Value *Parser::parse_opaque() {
    unsigned long long addr;
    addr = read_uint();
    return new Pointer(addr);
}


std::string Parser::read_string(void) {
    size_t len = read_uint();
    if (!len) {
        return std::string();
    }
    char * buf = new char[len];
    gzread(file, buf, (unsigned)len);
    std::string value(buf, len);
    delete [] buf;
#if TRACE_VERBOSE
    std::cerr << "\tSTRING \"" << value << "\"\n";
#endif
    return value;
}


unsigned long long Parser::read_uint(void) {
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
#if TRACE_VERBOSE
    std::cerr << "\tUINT " << value << "\n";
#endif
    return value;
}


inline int Parser::read_byte(void) {
    int c = gzgetc(file);
#if TRACE_VERBOSE
    if (c < 0)
        std::cerr << "\tEOF" << "\n";
    else
        std::cerr << "\tBYTE 0x" << std::hex << c << std::dec << "\n";
#endif
    return c;
}


} /* namespace Trace */
