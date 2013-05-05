/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
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


#include "trace_writer.hpp"


namespace trace {


class ModelWriter : public Visitor
{
protected:
    Writer &writer;

public:
    ModelWriter(Writer &_writer) :
        writer(_writer) {
    }

    void visit(Null *) {
        writer.writeNull();
    }

    void visit(Bool *node) {
        writer.writeBool(node->value);
    }

    void visit(SInt *node) {
        writer.writeSInt(node->value);
    }

    void visit(UInt *node) {
        writer.writeUInt(node->value);
    }

    void visit(Float *node) {
        writer.writeFloat(node->value);
    }

    void visit(Double *node) {
        writer.writeDouble(node->value);
    }

    void visit(String *node) {
        writer.writeString(node->value);
    }

    void visit(Enum *node) {
        writer.writeEnum(node->sig, node->value);
    }

    void visit(Bitmask *node) {
        writer.writeBitmask(node->sig, node->value);
    }

    void visit(Struct *node) {
        writer.beginStruct(node->sig);
        for (unsigned i = 0; i < node->sig->num_members; ++i) {
            _visit(node->members[i]);
        }
        writer.endStruct();
    }

    void visit(Array *node) {
        writer.beginArray(node->values.size());
        for (std::vector<Value *>::iterator it = node->values.begin(); it != node->values.end(); ++it) {
            _visit(*it);
        }
        writer.endArray();
    }

    void visit(Blob *node) {
        writer.writeBlob(node->buf, node->size);
    }

    void visit(Pointer *node) {
        writer.writePointer(node->value);
    }

    void visit(Repr *node) {
        writer.beginRepr();
        _visit(node->humanValue);
        _visit(node->machineValue);
        writer.endRepr();
    }

    void visit(Call *call) {
        unsigned call_no = writer.beginEnter(call->sig, call->thread_id);
        if (call->backtrace != NULL) {
            writer.beginBacktrace(call->backtrace->size());
            for (unsigned i = 0; i < call->backtrace->size(); ++i) {
                writer.writeStackFrame((*call->backtrace)[i]);
            }
            writer.endBacktrace();
        }
        for (unsigned i = 0; i < call->args.size(); ++i) {
            if (call->args[i].value) {
                writer.beginArg(i);
                _visit(call->args[i].value);
                writer.endArg();
            }
        }
        writer.endEnter();
        writer.beginLeave(call_no);
        if (call->ret) {
            writer.beginReturn();
            _visit(call->ret);
            writer.endReturn();
        }
        writer.endLeave();
    }
};


void Writer::writeCall(Call *call) {
    ModelWriter visitor(*this);
    visitor.visit(call);
}


} /* namespace trace */

