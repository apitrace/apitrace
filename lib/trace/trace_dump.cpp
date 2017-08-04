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


#include "trace_dump_internal.hpp"

#include <limits>

#include <assert.h>
#include <string.h>

#include "highlight.hpp"
#include "guids.hpp"


namespace trace {


Dumper::Dumper(std::ostream &_os, DumpFlags _flags) :
    os(_os),
    dumpFlags(_flags),
    highlighter(highlight::defaultHighlighter(!(dumpFlags & DUMP_FLAG_NO_COLOR))),
    normal(highlighter.normal()),
    bold(highlighter.bold()),
    italic(highlighter.italic()),
    strike(highlighter.strike()),
    red(highlighter.color(highlight::RED)),
    pointer(highlighter.color(highlight::GREEN)),
    literal(highlighter.color(highlight::BLUE))
{
}

Dumper::~Dumper() {
}

void Dumper::visit(Null *) {
    os << literal << "NULL" << normal;
}

void Dumper::visit(Bool *node) {
    os << literal << (node->value ? "true" : "false") << normal;
}

void Dumper::visit(SInt *node) {
    os << literal << node->value << normal;
}

void Dumper::visit(UInt *node) {
    os << literal << node->value << normal;
}

void Dumper::visit(Float *node) {
    std::streamsize oldPrecision = os.precision(std::numeric_limits<float>::digits10 + 1);
    os << literal << node->value << normal;
    os.precision(oldPrecision);
}

void Dumper::visit(Double *node) {
    std::streamsize oldPrecision = os.precision(std::numeric_limits<double>::digits10 + 1);
    os << literal << node->value << normal;
    os.precision(oldPrecision);
}

template< typename C >
void Dumper::visitString(const C *value) {
    os << literal << "\"";
    for (const C *it = value; *it; ++it) {
        unsigned c = (unsigned) *it;
        if (c == '\"')
            os << "\\\"";
        else if (c == '\\')
            os << "\\\\";
        else if (c >= 0x20 && c <= 0x7e)
            os << (char)c;
        else if (c == '\t') {
            os << "\t";
        } else if (c == '\r') {
            // Ignore carriage-return
        } else if (c == '\n') {
            if (dumpFlags & DUMP_FLAG_NO_MULTILINE) {
                os << "\\n";
            } else {
                // Reset formatting so that it looks correct with 'less -R'
                os << normal << '\n' << literal;
            }
        } else {
            // FIXME: handle wchar_t octals properly
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

void Dumper::visit(String *node) {
    visitString(node->value);
}

void Dumper::visit(WString *node) {
    os << literal << "L";
    visitString(node->value);
}

void Dumper::visit(Enum *node) {
    const EnumValue *it = node->lookup();
    if (it) {
        os << literal << it->name << normal;
        return;
    }
    os << literal << node->value << normal;
}

void Dumper::visit(Bitmask *bitmask) {
    unsigned long long value = bitmask->value;
    const BitmaskSig *sig = bitmask->sig;
    bool first = true;
    for (const BitmaskFlag *it = sig->flags; it != sig->flags + sig->num_flags; ++it) {
        assert(it->value || first);
        if ((it->value && (value & it->value) == it->value) ||
            (!it->value && value == 0)) {
            if (!first) {
                os << " | ";
            }
            os << literal << it->name << normal;
            value &= ~it->value;
            first = false;
        }
        if (value == 0) {
            break;
        }
    }
    if (value || first) {
        if (!first) {
            os << " | ";
        }
        os << literal << "0x" << std::hex << value << std::dec << normal;
    }
}

const char *
Dumper::visitMembers(Struct *s, const char *sep) {
    for (unsigned i = 0; i < s->members.size(); ++i) {
        const char *memberName = s->sig->member_names[i];
        Value *memberValue = s->members[i];

        if (!memberName || !*memberName) {
            // Anonymous structure
            Struct *memberStruct = memberValue->toStruct();
            assert(memberStruct);
            if (memberStruct) {
                sep = visitMembers(memberStruct, sep);
                continue;
            }
        }

        os << sep << italic << memberName << normal << " = ",
        _visit(memberValue);
        sep = ", ";
    }
    return sep;
}

void Dumper::visit(Struct *s) {
    // Replace GUIDs with their symbolic name
    // TODO: Move this to parsing, so it can be shared everywhere
    if (s->members.size() == 4 &&
        strcmp(s->sig->name, "GUID") == 0) {
        GUID guid;
        guid.Data1 = s->members[0]->toUInt();
        guid.Data2 = s->members[1]->toUInt();
        guid.Data3 = s->members[2]->toUInt();
        Array *data4 = s->members[3]->toArray();
        assert(data4);
        assert(data4->values.size() == 8);
        for (int i = 0; i < sizeof guid.Data4; ++i) {
            guid.Data4[i] = data4->values[i]->toUInt();
        }
        const char *name = getGuidName(guid);
        os << literal << name << normal;
        return;
    }

    os << "{";
    visitMembers(s);
    os << "}";
}

void Dumper::visit(Array *array) {
    if (array->values.size() == 1) {
        os << "&";
        _visit(array->values[0]);
    }
    else {
        const char *sep = "";
        os << "{";
        for (auto & value : array->values) {
            os << sep;
            _visit(value);
            sep = ", ";
        }
        os << "}";
    }
}

void Dumper::visit(Blob *blob) {
    os << pointer << "blob(" << blob->size << ")" << normal;
}

void Dumper::visit(Pointer *p) {
    os << pointer << "0x" << std::hex << p->value << std::dec << normal;
}

void Dumper::visit(Repr *r) {
    _visit(r->humanValue);
}

void Dumper::visit(StackFrame *frame) {
    frame->dump(os);
}

void Dumper::visit(Backtrace & backtrace) {
    for (auto & frame : backtrace) {
        visit(frame);
        os << "\n";
    }
}

void Dumper::visit(Call *call)
{
    CallFlags callFlags = call->flags;

    if (!(dumpFlags & DUMP_FLAG_NO_CALL_NO)) {
        os << call->no << " ";
    }
    if (dumpFlags & DUMP_FLAG_THREAD_IDS) {
        os << "@" << std::hex << call->thread_id << std::dec << " ";
    }

    if (callFlags & CALL_FLAG_NON_REPRODUCIBLE) {
        os << strike;
    } else if (callFlags & CALL_FLAG_NO_SIDE_EFFECTS) {
        os << normal;
    } else {
        os << bold;
    }
    os << call->sig->name << normal;

    os << "(";
    const char *sep = "";
    for (unsigned i = 0; i < call->args.size(); ++i) {
        os << sep;
        if (!(dumpFlags & DUMP_FLAG_NO_ARG_NAMES)) {
            os << italic << call->sig->arg_names[i] << normal << " = ";
        }
        if (call->args[i].value) {
            _visit(call->args[i].value);
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

    if (callFlags & (CALL_FLAG_FAKE |
                     CALL_FLAG_INCOMPLETE)) {
        os << " //";
        if (callFlags & CALL_FLAG_FAKE) {
            os << " " << "fake";
        }
        if (callFlags & CALL_FLAG_INCOMPLETE) {
            os << " " << red << "incomplete" << normal;
        }
    }

    if (!(dumpFlags & DUMP_FLAG_NO_MULTILINE)) {
        os << "\n";

        if (call->backtrace != NULL) {
            os << bold << red << "Backtrace:\n" << normal;
            visit(*call->backtrace);
        }
        if (callFlags & CALL_FLAG_END_FRAME) {
            os << "\n";
        }
    }
}


void dump(Value *value, std::ostream &os, DumpFlags flags) {
    Dumper d(os, flags);
    value->visit(d);
}


void dump(Call &call, std::ostream &os, DumpFlags flags) {
    Dumper d(os, flags);
    d.visit(&call);
}


} /* namespace trace */
