/**************************************************************************
 *
 * Copyright 2007-2009 VMware, Inc.
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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "os.hpp"
#include "trace_file.hpp"
#include "trace_writer.hpp"
#include "trace_format.hpp"
#include "trace_backtrace.hpp"

namespace trace {


Writer::Writer() :
    call_no(0)
{
    m_file = File::createSnappy();
    close();
}

Writer::~Writer()
{
    close();
    delete m_file;
    m_file = NULL;
}

void
Writer::close(void) {
    m_file->close();
}

bool
Writer::open(const char *filename) {
    close();

    if (!m_file->open(filename, File::Write)) {
        return false;
    }

    call_no = 0;
    functions.clear();
    structs.clear();
    enums.clear();
    bitmasks.clear();
    frames.clear();

    _writeUInt(TRACE_VERSION);

    return true;
}

void inline
Writer::_write(const void *sBuffer, size_t dwBytesToWrite) {
    m_file->write(sBuffer, dwBytesToWrite);
}

void inline
Writer::_writeByte(char c) {
    _write(&c, 1);
}

void inline
Writer::_writeUInt(unsigned long long value) {
    char buf[2 * sizeof value];
    unsigned len;

    len = 0;
    do {
        assert(len < sizeof buf);
        buf[len] = 0x80 | (value & 0x7f);
        value >>= 7;
        ++len;
    } while (value);

    assert(len);
    buf[len - 1] &= 0x7f;

    _write(buf, len);
}

void inline
Writer::_writeFloat(float value) {
    assert(sizeof value == 4);
    _write((const char *)&value, sizeof value);
}

void inline
Writer::_writeDouble(double value) {
    assert(sizeof value == 8);
    _write((const char *)&value, sizeof value);
}

void inline
Writer::_writeString(const char *str) {
    size_t len = strlen(str);
    _writeUInt(len);
    _write(str, len);
}

inline bool lookup(std::vector<bool> &map, size_t index) {
    if (index >= map.size()) {
        map.resize(index + 1);
        return false;
    } else {
        return map[index];
    }
}

void Writer::beginBacktrace(unsigned num_frames) {
    if (num_frames) {
        _writeByte(trace::CALL_BACKTRACE);
        _writeUInt(num_frames);
    }
}

void Writer::writeStackFrame(const RawStackFrame *frame) {
    _writeUInt(frame->id);
    if (!lookup(frames, frame->id)) {
        if (frame->module != NULL) {
            _writeByte(trace::BACKTRACE_MODULE);
            _writeString(frame->module);
        }
        if (frame->function != NULL) {
            _writeByte(trace::BACKTRACE_FUNCTION);
            _writeString(frame->function);
        }
        if (frame->filename != NULL) {
            _writeByte(trace::BACKTRACE_FILENAME);
            _writeString(frame->filename);
        }
        if (frame->linenumber >= 0) {
            _writeByte(trace::BACKTRACE_LINENUMBER);
            _writeUInt(frame->linenumber);
        }
        if (frame->offset >= 0) {
            _writeByte(trace::BACKTRACE_OFFSET);
            _writeUInt(frame->offset);
        }
        _writeByte(trace::BACKTRACE_END);
        frames[frame->id] = true;
    }
}

unsigned Writer::beginEnter(const FunctionSig *sig, unsigned thread_id) {
    _writeByte(trace::EVENT_ENTER);
    _writeUInt(thread_id);
    _writeUInt(sig->id);
    if (!lookup(functions, sig->id)) {
        _writeString(sig->name);
        _writeUInt(sig->num_args);
        for (unsigned i = 0; i < sig->num_args; ++i) {
            _writeString(sig->arg_names[i]);
        }
        functions[sig->id] = true;
    }

    return call_no++;
}

void Writer::endEnter(void) {
    _writeByte(trace::CALL_END);
}

void Writer::beginLeave(unsigned call) {
    _writeByte(trace::EVENT_LEAVE);
    _writeUInt(call);
}

void Writer::endLeave(void) {
    _writeByte(trace::CALL_END);
}

void Writer::beginArg(unsigned index) {
    _writeByte(trace::CALL_ARG);
    _writeUInt(index);
}

void Writer::beginReturn(void) {
    _writeByte(trace::CALL_RET);
}

void Writer::beginArray(size_t length) {
    _writeByte(trace::TYPE_ARRAY);
    _writeUInt(length);
}

void Writer::beginStruct(const StructSig *sig) {
    _writeByte(trace::TYPE_STRUCT);
    _writeUInt(sig->id);
    if (!lookup(structs, sig->id)) {
        _writeString(sig->name);
        _writeUInt(sig->num_members);
        for (unsigned i = 0; i < sig->num_members; ++i) {
            _writeString(sig->member_names[i]);
        }
        structs[sig->id] = true;
    }
}

void Writer::beginRepr(void) {
    _writeByte(trace::TYPE_REPR);
}

void Writer::writeBool(bool value) {
    _writeByte(value ? trace::TYPE_TRUE : trace::TYPE_FALSE);
}

void Writer::writeSInt(signed long long value) {
    if (value < 0) {
        _writeByte(trace::TYPE_SINT);
        _writeUInt(-value);
    } else {
        _writeByte(trace::TYPE_UINT);
        _writeUInt(value);
    }
}

void Writer::writeUInt(unsigned long long value) {
    _writeByte(trace::TYPE_UINT);
    _writeUInt(value);
}

void Writer::writeFloat(float value) {
    _writeByte(trace::TYPE_FLOAT);
    _writeFloat(value);
}

void Writer::writeDouble(double value) {
    _writeByte(trace::TYPE_DOUBLE);
    _writeDouble(value);
}

void Writer::writeString(const char *str) {
    if (!str) {
        Writer::writeNull();
        return;
    }
    _writeByte(trace::TYPE_STRING);
    _writeString(str);
}

void Writer::writeString(const char *str, size_t len) {
    if (!str) {
        Writer::writeNull();
        return;
    }
    _writeByte(trace::TYPE_STRING);
    _writeUInt(len);
    _write(str, len);
}

void Writer::writeWString(const wchar_t *str) {
    if (!str) {
        Writer::writeNull();
        return;
    }
    _writeByte(trace::TYPE_STRING);
    _writeString("<wide-string>");
}

void Writer::writeBlob(const void *data, size_t size) {
    if (!data) {
        Writer::writeNull();
        return;
    }
    _writeByte(trace::TYPE_BLOB);
    _writeUInt(size);
    if (size) {
        _write(data, size);
    }
}

void Writer::writeEnum(const EnumSig *sig, signed long long value) {
    _writeByte(trace::TYPE_ENUM);
    _writeUInt(sig->id);
    if (!lookup(enums, sig->id)) {
        _writeUInt(sig->num_values);
        for (unsigned i = 0; i < sig->num_values; ++i) {
            _writeString(sig->values[i].name);
            writeSInt(sig->values[i].value);
        }
        enums[sig->id] = true;
    }
    writeSInt(value);
}

void Writer::writeBitmask(const BitmaskSig *sig, unsigned long long value) {
    _writeByte(trace::TYPE_BITMASK);
    _writeUInt(sig->id);
    if (!lookup(bitmasks, sig->id)) {
        _writeUInt(sig->num_flags);
        for (unsigned i = 0; i < sig->num_flags; ++i) {
            if (i != 0 && sig->flags[i].value == 0) {
                os::log("apitrace: warning: sig %s is zero but is not first flag\n", sig->flags[i].name);
            }
            _writeString(sig->flags[i].name);
            _writeUInt(sig->flags[i].value);
        }
        bitmasks[sig->id] = true;
    }
    _writeUInt(value);
}

void Writer::writeNull(void) {
    _writeByte(trace::TYPE_NULL);
}

void Writer::writePointer(unsigned long long addr) {
    if (!addr) {
        Writer::writeNull();
        return;
    }
    _writeByte(trace::TYPE_OPAQUE);
    _writeUInt(addr);
}


} /* namespace trace */

