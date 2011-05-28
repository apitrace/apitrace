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
#include <stdlib.h>

#include <zlib.h>

#include "trace_parser.hpp"


#define TRACE_VERBOSE 0


namespace Trace {


Parser::Parser() {
    file = NULL;
    next_call_no = 0;
    version = 0;
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
        std::cerr << "error: unsupported trace format version " << version << "\n";
        return false;
    }

    return true;
}

template <typename Iter>
inline void
deleteAll(Iter begin, Iter end)
{
    while (begin != end) {
        delete *begin;
        ++begin;
    }
}

template <typename Container>
inline void
deleteAll(const Container &c)
{
    deleteAll(c.begin(), c.end());
}

void Parser::close(void) {
    if (file) {
        gzclose(file);
        file = NULL;
    }

    deleteAll(calls);
    deleteAll(functions);
    deleteAll(structs);
    deleteAll(enums);
    deleteAll(bitmasks);
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
        default:
            std::cerr << "error: unknown event " << c << "\n";
            exit(1);
        case -1:
            for (CallList::iterator it = calls.begin(); it != calls.end(); ++it) {
                std::cerr << "warning: incomplete call " << (*it)->name() << "\n";
                std::cerr << **it << "\n";
            }
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

    FunctionSig *sig = lookup(functions, id);
    if (!sig) {
        sig = new FunctionSig;
        sig->id = id;
        sig->name = read_string();
        sig->num_args = read_uint();
        const char **arg_names = new const char *[sig->num_args];
        for (unsigned i = 0; i < sig->num_args; ++i) {
            arg_names[i] = read_string();
        }
        sig->arg_names = arg_names;
        functions[id] = sig;
    }
    assert(sig);

    Call *call = new Call(sig);
    call->no = next_call_no++;

    if (parse_call_details(call)) {
        calls.push_back(call);
    } else {
        delete call;
    }
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
    if (!call) {
        return NULL;
    }

    if (parse_call_details(call)) {
        return call;
    } else {
        delete call;
        return NULL;
    }
}


bool Parser::parse_call_details(Call *call) {
    do {
        int c = read_byte();
        switch(c) {
        case Trace::CALL_END:
            return true;
        case Trace::CALL_ARG:
            parse_arg(call);
            break;
        case Trace::CALL_RET:
            call->ret = parse_value();
            break;
        default:
            std::cerr << "error: unknown call detail " << c << "\n";
            exit(1);
        case -1:
            return false;
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
    Value *value;
    c = read_byte();
    switch(c) {
    case Trace::TYPE_NULL:
        value = new Null;
        break;
    case Trace::TYPE_FALSE:
        value = new Bool(false);
        break;
    case Trace::TYPE_TRUE:
        value = new Bool(true);
        break;
    case Trace::TYPE_SINT:
        value = parse_sint();
        break;
    case Trace::TYPE_UINT:
        value = parse_uint();
        break;
    case Trace::TYPE_FLOAT:
        value = parse_float();
        break;
    case Trace::TYPE_DOUBLE:
        value = parse_double();
        break;
    case Trace::TYPE_STRING:
        value = parse_string();
        break;
    case Trace::TYPE_ENUM:
        value = parse_enum();
        break;
    case Trace::TYPE_BITMASK:
        value = parse_bitmask();
        break;
    case Trace::TYPE_ARRAY:
        value = parse_array();
        break;
    case Trace::TYPE_STRUCT:
        value = parse_struct();
        break;
    case Trace::TYPE_BLOB:
        value = parse_blob();
        break;
    case Trace::TYPE_OPAQUE:
        value = parse_opaque();
        break;
    default:
        std::cerr << "error: unknown type " << c << "\n";
        exit(1);
    case -1:
        value = NULL;
        break;
    }
#if TRACE_VERBOSE
    if (value) {
        std::cerr << "\tVALUE " << value << "\n";
    }
#endif
    return value;
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
    EnumSig *sig = lookup(enums, id);
    if (!sig) {
        sig = new EnumSig;
        sig->id = id;
        sig->name = read_string();
        Value *value = parse_value();
        sig->value = value->toSInt();
        delete value;
        enums[id] = sig;
    }
    assert(sig);
    return new Enum(sig);
}


Value *Parser::parse_bitmask() {
    size_t id = read_uint();
    BitmaskSig *sig = lookup(bitmasks, id);
    if (!sig) {
        sig = new BitmaskSig;
        sig->id = id;
        sig->num_flags = read_uint();
        BitmaskFlag *flags = new BitmaskFlag[sig->num_flags];
        for (BitmaskFlag *it = flags; it != flags + sig->num_flags; ++it) {
            it->name = read_string();
            it->value = read_uint();
            if (it->value == 0 && it != flags) {
                std::cerr << "warning: bitmask " << it->name << " is zero but is not first flag\n";
            }
        }
        sig->flags = flags;
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

    StructSig *sig = lookup(structs, id);
    if (!sig) {
        sig = new StructSig;
        sig->id = id;
        sig->name = read_string();
        sig->num_members = read_uint();
        const char **member_names = new const char *[sig->num_members];
        for (unsigned i = 0; i < sig->num_members; ++i) {
            member_names[i] = read_string();
        }
        sig->member_names = member_names;
        structs[id] = sig;
    }
    assert(sig);

    Struct *value = new Struct(sig);

    for (size_t i = 0; i < sig->num_members; ++i) {
        value->members[i] = parse_value();
    }

    return value;
}


Value *Parser::parse_opaque() {
    unsigned long long addr;
    addr = read_uint();
    return new Pointer(addr);
}


const char * Parser::read_string(void) {
    size_t len = read_uint();
    char * value = new char[len + 1];
    if (len) {
        gzread(file, value, (unsigned)len);
    }
    value[len] = 0;
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
