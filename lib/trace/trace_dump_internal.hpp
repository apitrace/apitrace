/**************************************************************************
 *
 * Copyright 2010-2016 VMware, Inc.
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


#pragma once

#include "highlight.hpp"
#include "trace_dump.hpp"


namespace trace {


class Dumper : public Visitor
{
protected:
    std::ostream &os;
    DumpFlags dumpFlags;
    const highlight::Highlighter & highlighter;
    const highlight::Attribute & normal;
    const highlight::Attribute & bold;
    const highlight::Attribute & italic;
    const highlight::Attribute & strike;
    const highlight::Attribute & red;
    const highlight::Attribute & pointer;
    const highlight::Attribute & literal;

public:
    Dumper(std::ostream &_os, DumpFlags _flags);
    virtual ~Dumper();

    virtual void visit(Null *) override;
    virtual void visit(Bool *node) override;
    virtual void visit(SInt *node) override;
    virtual void visit(UInt *node) override;
    virtual void visit(Float *node) override;
    virtual void visit(Double *node) override;

    template< typename C >
    void visitString(const C *value) {
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
    virtual void visit(String *node) override;
    virtual void visit(WString *node) override;
    virtual void visit(Enum *node) override;
    virtual void visit(Bitmask *bitmask) override;
    virtual const char *
    visitMembers(Struct *s, const char *sep = "");
    virtual void visit(Struct *s) override;
    virtual void visit(Array *array) override;
    virtual void visit(Blob *blob) override;
    virtual void visit(Pointer *p) override;
    virtual void visit(Repr *r) override;
    virtual void visit(StackFrame *frame);
    virtual void visit(Backtrace & backtrace);
    virtual void visit(Call *call);
};


} /* namespace trace */

