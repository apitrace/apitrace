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


#include <limits>

#include "formatter.hpp"
#include "trace_dump.hpp"


namespace trace {


class Dumper : public Visitor
{
protected:
    std::ostream &os;
    DumpFlags dumpFlags;
    formatter::Formatter *formatter;
    formatter::Attribute *normal;
    formatter::Attribute *bold;
    formatter::Attribute *italic;
    formatter::Attribute *strike;
    formatter::Attribute *red;
    formatter::Attribute *pointer;
    formatter::Attribute *literal;

public:
    Dumper(std::ostream &_os, DumpFlags _flags) : 
        os(_os),
        dumpFlags(_flags)
    {
        bool color = !(dumpFlags & DUMP_FLAG_NO_COLOR);
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
        os << literal << "NULL" << normal;
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
        std::streamsize oldPrecision = os.precision(std::numeric_limits<float>::digits10 + 1);
        os << literal << node->value << normal;
        os.precision(oldPrecision);
    }

    void visit(Double *node) {
        std::streamsize oldPrecision = os.precision(std::numeric_limits<double>::digits10 + 1);
        os << literal << node->value << normal;
        os.precision(oldPrecision);
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
        const EnumValue *it = node->lookup();
        if (it) {
            os << literal << it->name << normal;
            return;
        }
        os << literal << node->value << normal;
    }

    void visit(Bitmask *bitmask) {
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
    visitMembers(Struct *s, const char *sep = "") {
        for (unsigned i = 0; i < s->members.size(); ++i) {
            const char *memberName = s->sig->member_names[i];
            Value *memberValue = s->members[i];

            if (!memberName || !*memberName) {
                // Anonymous structure
                Struct *memberStruct = dynamic_cast<Struct *>(memberValue);
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

    void visit(Struct *s) {
        os << "{";
        visitMembers(s);
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

    void visit(Repr *r) {
        _visit(r->humanValue);
    }

    void visit(StackFrame *frame) {
        if (frame->module != NULL) {
            os << frame->module << " ";
        }
        if (frame->function != NULL) {
            os << "at " << frame->function << "() ";
        }
        if (frame->filename != NULL) {
            os << "at " << frame->filename;
            if (frame->linenumber >= 0) {
                os << ":" << frame->linenumber << " ";
            }
        }
        else {
            if (frame->offset >= 0) {
                os << "[" << "0x" << std::hex << frame->offset << std::dec << "]";
            }
        }
    }

    void visit(Backtrace & backtrace) {
        for (int i = 0; i < backtrace.size(); i ++) {
            visit(backtrace[i]);
            os << "\n";
        }
    }

    void visit(Call *call) {
        CallFlags callFlags = call->flags;
        
        if (!(dumpFlags & DUMP_FLAG_NO_CALL_NO)) {
            os << call->no << " ";
        }

        if (callFlags & CALL_FLAG_NON_REPRODUCIBLE) {
            os << strike;
        } else if (callFlags & (CALL_FLAG_FAKE | CALL_FLAG_NO_SIDE_EFFECTS)) {
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
        
        if (callFlags & CALL_FLAG_INCOMPLETE) {
            os << " // " << red << "incomplete" << normal;
        }
        
        os << "\n";

        if (call->backtrace != NULL) {
            os << bold << red << "Backtrace:\n" << normal;
            visit(*call->backtrace);
        }
        if (callFlags & CALL_FLAG_END_FRAME) {
            os << "\n";
        }
    }
};


void dump(Value *value, std::ostream &os, DumpFlags flags) {
    Dumper d(os, flags);
    value->visit(d);
}


void dump(Call &call, std::ostream &os, DumpFlags flags) {
    Dumper d(os, flags);
    d.visit(&call);
}


} /* namespace trace */
