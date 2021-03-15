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
#include <wchar.h>
#include <vector>

#include "os.hpp"
#include "trace_ostream.hpp"
#include "trace_writer.hpp"
#include "trace_format.hpp"

namespace trace {


Writer::Writer() :
    call_no(0)
{
    m_file = nullptr;
}

Writer::~Writer()
{
    close();
}

void
Writer::close(void) {
    delete m_file;
    m_file = nullptr;
}

bool
Writer::open(const char *filename,
             unsigned semanticVersion,
             const Properties &properties)
{
    close();

    m_file = createSnappyStream(filename);
    if (!m_file) {
        return false;
    }

    call_no = 0;
    functions.clear();
    structs.clear();
    enums.clear();
    bitmasks.clear();
    frames.clear();

    _writeUInt(TRACE_VERSION);

    assert(semanticVersion <= TRACE_VERSION);
    _writeUInt(semanticVersion);

    beginProperties();
    for (auto & kv : properties) {
        writeProperty(kv.first.c_str(), kv.second.c_str());
    }
    endProperties();

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
    static_assert(sizeof value == 4, "float is not 4 bytes");
    _write((const char *)&value, sizeof value);
}

void inline
Writer::_writeDouble(double value) {
    static_assert(sizeof value == 8, "double is not 8 bytes");
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

void
Writer::writeFlags(unsigned flags) {
    if (flags) {
        _writeByte(trace::CALL_FLAGS);
        _writeUInt(flags);
    }
}

void
Writer::writeProperty(const char *name, const char *value)
{
    assert(name);
    assert(strlen(name));
    assert(value);
    _writeString(name);
    _writeString(value);
}

void
Writer::endProperties(void)
{
    _writeUInt(0);  // zero-length string
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

void Writer::writeWString(const wchar_t *str, size_t len) {
    if (!str) {
        Writer::writeNull();
        return;
    }
    _writeByte(trace::TYPE_WSTRING);
    _writeUInt(len);
    for (size_t i = 0; i < len; ++i) {
        _writeUInt(str[i]);
    }
}

void Writer::writeWString(const wchar_t *str) {
    if (!str) {
        Writer::writeNull();
        return;
    }
    size_t len = wcslen(str);
    writeWString(str, len);
}

void Writer::writeBlob(const void *data, size_t size) {
    if (!data) {
        Writer::writeNull();
        return;
    }
    _writeByte(trace::TYPE_BLOB);
    _writeUInt((unsigned long long) data);
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

