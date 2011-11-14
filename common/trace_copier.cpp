/*********************************************************************
 *
 * Copyright 2010 VMware, Inc.
 * Copyright 2011 Intel Corporation
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *********************************************************************/

#include "trace_copier.hpp"

namespace trace {

Copier::Copier(const char *filename) : writer() {
    writer.open(filename);
}

Copier::~Copier() {
    writer.close();
}

void Copier::visit(Null *) {
    writer.writeNull();
}

void Copier::visit(Bool *node) {
    writer.writeBool(node->value);
}

void Copier::visit(SInt *node) {
    writer.writeSInt(node->value);
}

void Copier::visit(UInt *node) {
    writer.writeUInt(node->value);
}

void Copier::visit(Float *node) {
    writer.writeFloat(node->value);
}

void Copier::visit(Double *node) {
    writer.writeDouble(node->value);
}

void Copier::visit(String *node) {
    writer.writeString(node->value);
}

void Copier::visit(Enum *node) {
    writer.writeEnum(node->sig);
}

void Copier::visit(Bitmask *bitmask) {
    writer.writeBitmask(bitmask->sig, bitmask->value);
}

void Copier::visit(Struct *s) {
    writer.beginStruct(s->sig);
    for (unsigned i = 0; i < s->members.size(); ++i) {
        _visit(s->members[i]);
    }
}

void Copier::visit(Array *array) {
    if (array->values.size()) {
        writer.beginArray(array->values.size());
        for (std::vector<Value *>::iterator it = array->values.begin(); it != array->values.end(); ++it) {
            _visit(*it);
        }
        writer.endArray();
    } else {
        writer.writeNull();
    }
}

void Copier::visit(Blob *blob) {
    writer.writeBlob(blob->buf, blob->size);
}

void Copier::visit(Pointer *p) {
    writer.writeOpaque((void *)p->value);
}

void Copier::visit(Call *call) {
    unsigned __call = writer.beginEnter(call->sig);
    for (unsigned i = 0; i < call->args.size(); ++i) {
        writer.beginArg(i);
        _visit(call->args[i]);
        writer.endArg();
    }
    writer.endEnter();
    writer.beginLeave(__call);
    if (call->ret) {
        writer.beginReturn();
        _visit(call->ret);
        writer.endReturn();
    }
    writer.endLeave();
}

}
