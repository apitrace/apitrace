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

#include <zlib.h>

#include "os.hpp"
#include "trace_write.hpp"
#include "trace_format.hpp"


namespace Trace {


static gzFile g_gzFile = NULL;
static void _Close(void) {
    if(g_gzFile != NULL) {
        gzclose(g_gzFile);
        g_gzFile = NULL;
    }
}

static void _Open(const char *szExtension) {
    _Close();

    static unsigned dwCounter = 0;

    char szFileName[PATH_MAX];
    const char *lpFileName;

    lpFileName = getenv("TRACE_PATH");
    if (lpFileName) {
        strncpy(szFileName, lpFileName, PATH_MAX);
    }
    else {
        char szProcessName[PATH_MAX];
        char szCurrentDir[PATH_MAX];
        OS::GetProcessName(szProcessName, PATH_MAX);
        OS::GetCurrentDir(szCurrentDir, PATH_MAX);

        for (;;) {
            FILE *file;
 
            if (dwCounter)
                snprintf(szFileName, PATH_MAX, "%s%c%s.%u.%s", szCurrentDir, PATH_SEP, szProcessName, dwCounter, szExtension);
            else
                snprintf(szFileName, PATH_MAX, "%s%c%s.%s", szCurrentDir, PATH_SEP, szProcessName, szExtension);
 
            file = fopen(szFileName, "rb");
            if(file == NULL)
                break;
 
            fclose(file);
 
            ++dwCounter;
        }
    }

    OS::DebugMessage("apitrace: tracing to %s\n", szFileName);

    g_gzFile = gzopen(szFileName, "wb");
}

static inline void Write(const void *sBuffer, size_t dwBytesToWrite) {
    if (g_gzFile == NULL)
        return;

    gzwrite(g_gzFile, sBuffer, dwBytesToWrite);
}

static inline void 
WriteByte(char c) {
    Write(&c, 1);
}

void inline 
WriteUInt(unsigned long long value) {
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

    Write(buf, len);
}

static inline void 
WriteFloat(float value) {
    assert(sizeof value == 4);
    Write((const char *)&value, sizeof value);
}

static inline void 
WriteDouble(double value) {
    assert(sizeof value == 8);
    Write((const char *)&value, sizeof value);
}

static inline void 
WriteString(const char *str) {
    size_t len = strlen(str);
    WriteUInt(len);
    Write(str, len);
}

void Open(void) {
    if (!g_gzFile) {
        _Open("trace");
        WriteUInt(TRACE_VERSION);
    }
}

static unsigned call_no = 0;

inline bool lookup(std::vector<bool> &map, size_t index) {
    if (index >= map.size()) {
        map.resize(index + 1);
        return false;
    } else {
        return map[index];
    }
}

static std::vector<bool> functions;
static std::vector<bool> structs;
static std::vector<bool> enums;
static std::vector<bool> bitmasks;


void Close(void) {
    _Close();
    call_no = 0;
    functions = std::vector<bool>();
    structs = std::vector<bool>();
    enums = std::vector<bool>();
    bitmasks = std::vector<bool>();
}

unsigned BeginEnter(const FunctionSig &function) {
    OS::AcquireMutex();
    Open();
    WriteByte(Trace::EVENT_ENTER);
    WriteUInt(function.id);
    if (!lookup(functions, function.id)) {
        WriteString(function.name);
        WriteUInt(function.num_args);
        for (unsigned i = 0; i < function.num_args; ++i) {
            WriteString(function.args[i]);
        }
        functions[function.id] = true;
    }
    return call_no++;
}

void EndEnter(void) {
    WriteByte(Trace::CALL_END);
    gzflush(g_gzFile, Z_SYNC_FLUSH);
    OS::ReleaseMutex();
}

void BeginLeave(unsigned call) {
    OS::AcquireMutex();
    WriteByte(Trace::EVENT_LEAVE);
    WriteUInt(call);
}

void EndLeave(void) {
    WriteByte(Trace::CALL_END);
    gzflush(g_gzFile, Z_SYNC_FLUSH);
    OS::ReleaseMutex();
}

void BeginArg(unsigned index) {
    WriteByte(Trace::CALL_ARG);
    WriteUInt(index);
}

void BeginReturn(void) {
    WriteByte(Trace::CALL_RET);
}

void BeginArray(size_t length) {
    WriteByte(Trace::TYPE_ARRAY);
    WriteUInt(length);
}

void BeginStruct(const StructSig *sig) {
    WriteByte(Trace::TYPE_STRUCT);
    WriteUInt(sig->id);
    if (!lookup(structs, sig->id)) {
        WriteString(sig->name);
        WriteUInt(sig->num_members);
        for (unsigned i = 0; i < sig->num_members; ++i) {
            WriteString(sig->members[i]);
        }
        structs[sig->id] = true;
    }
}

void LiteralBool(bool value) {
    WriteByte(value ? Trace::TYPE_TRUE : Trace::TYPE_FALSE);
}

void LiteralSInt(signed long long value) {
    if (value < 0) {
        WriteByte(Trace::TYPE_SINT);
        WriteUInt(-value);
    } else {
        WriteByte(Trace::TYPE_UINT);
        WriteUInt(value);
    }
}

void LiteralUInt(unsigned long long value) {
    WriteByte(Trace::TYPE_UINT);
    WriteUInt(value);
}

void LiteralFloat(float value) {
    WriteByte(Trace::TYPE_FLOAT);
    WriteFloat(value);
}

void LiteralFloat(double value) {
    WriteByte(Trace::TYPE_DOUBLE);
    WriteDouble(value);
}

void LiteralString(const char *str) {
    if (!str) {
        LiteralNull();
        return;
    }
    WriteByte(Trace::TYPE_STRING);
    WriteString(str);
}

void LiteralString(const char *str, size_t len) {
    if (!str) {
        LiteralNull();
        return;
    }
    WriteByte(Trace::TYPE_STRING);
    WriteUInt(len);
    Write(str, len);
}

void LiteralWString(const wchar_t *str) {
    if (!str) {
        LiteralNull();
        return;
    }
    WriteByte(Trace::TYPE_STRING);
    WriteString("<wide-string>");
}

void LiteralBlob(const void *data, size_t size) {
    if (!data) {
        LiteralNull();
        return;
    }
    WriteByte(Trace::TYPE_BLOB);
    WriteUInt(size);
    if (size) {
        Write(data, size);
    }
}

void LiteralEnum(const EnumSig *sig) {
    WriteByte(Trace::TYPE_ENUM);
    WriteUInt(sig->id);
    if (!lookup(enums, sig->id)) {
        WriteString(sig->name);
        LiteralSInt(sig->value);
        enums[sig->id] = true;
    }
}

void LiteralBitmask(const BitmaskSig &bitmask, unsigned long long value) {
    WriteByte(Trace::TYPE_BITMASK);
    WriteUInt(bitmask.id);
    if (!lookup(bitmasks, bitmask.id)) {
        WriteUInt(bitmask.count);
        for (unsigned i = 0; i < bitmask.count; ++i) {
            if (i != 0 && bitmask.values[i].value == 0) {
                OS::DebugMessage("apitrace: bitmask %s is zero but is not first flag\n", bitmask.values[i].name);
            }
            WriteString(bitmask.values[i].name);
            WriteUInt(bitmask.values[i].value);
        }
        bitmasks[bitmask.id] = true;
    }
    WriteUInt(value);
}

void LiteralNull(void) {
    WriteByte(Trace::TYPE_NULL);
}

void LiteralOpaque(const void *addr) {
    if (!addr) {
        LiteralNull();
        return;
    }
    WriteByte(Trace::TYPE_OPAQUE);
    WriteUInt((size_t)addr);
}

void Abort(void) {
    Close();
    OS::Abort();
}

} /* namespace Trace */


#ifdef _WIN32

#if 0
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch(fdwReason) {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
        return TRUE;
    case DLL_THREAD_DETACH:
        return TRUE;
    case DLL_PROCESS_DETACH:
        Trace::Close();
        return TRUE;
    }
    (void)hinstDLL;
    (void)lpvReserved;
    return TRUE;
}
#endif

#else

static void _uninit(void) __attribute__((destructor));
static void _uninit(void) {
    Trace::Close();
}

#endif
