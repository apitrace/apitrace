/**************************************************************************
 *
 * Copyright 2007-2011 VMware, Inc.
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

#include "os.hpp"
#include "os_thread.hpp"
#include "os_string.hpp"
#include "os_version.hpp"
#include "trace_ostream.hpp"
#include "trace_writer_local.hpp"
#include "trace_format.hpp"
#include "os_backtrace.hpp"


namespace trace {


static const char *memcpy_args[3] = {"dest", "src", "n"};
const FunctionSig memcpy_sig = {0, "memcpy", 3, memcpy_args};

static const char *malloc_args[1] = {"size"};
const FunctionSig malloc_sig = {1, "malloc", 1, malloc_args};

static const char *free_args[1] = {"ptr"};
const FunctionSig free_sig = {2, "free", 1, free_args};

static const char *realloc_args[2] = {"ptr", "size"};
const FunctionSig realloc_sig = {3, "realloc", 2, realloc_args};


static void exceptionCallback(void)
{
    localWriter.flush();
}


LocalWriter::LocalWriter() :
    acquired(0)
{
    os::String process = os::getProcessName();
    os::log("apitrace: loaded into %s\n", process.str());

    // Install the signal handlers as early as possible, to prevent
    // interfering with the application's signal handling.
    os::setExceptionCallback(exceptionCallback);
}

LocalWriter::~LocalWriter()
{
    os::resetExceptionCallback();
    checkProcessId();

    os::String process = os::getProcessName();
    os::log("apitrace: unloaded from %s\n", process.str());
}

void
LocalWriter::open(void) {
    os::String szFileName;

    const char *lpFileName;

    lpFileName = getenv("TRACE_FILE");
    if (!lpFileName) {
        static unsigned dwCounter = 0;

        os::String process = os::getProcessName();
#ifdef _WIN32
        process.trimExtension();
#endif
        process.trimDirectory();

#ifdef ANDROID
        os::String prefix = "/data/data";
        prefix.join(process);
#else
        os::String prefix = os::getCurrentDir();
#ifdef _WIN32
        // Avoid writing into Windows' system directory as quite often access
        // will be denied.
        if (IsWindows8OrGreater()) {
            char szDirectory[MAX_PATH + 1];
            GetSystemDirectoryA(szDirectory, sizeof szDirectory);
            if (stricmp(prefix, szDirectory) == 0) {
                GetTempPathA(sizeof szDirectory, szDirectory);
                prefix = szDirectory;
            }
        }
#endif
#endif
        prefix.join(process);

        for (;;) {
            FILE *file;

            if (dwCounter)
                szFileName = os::String::format("%s.%u.trace", prefix.str(), dwCounter);
            else
                szFileName = os::String::format("%s.trace", prefix.str());

            lpFileName = szFileName;
            file = fopen(lpFileName, "rb");
            if (file == NULL)
                break;

            fclose(file);

            ++dwCounter;
        }
    }

    os::log("apitrace: tracing to %s\n", lpFileName);

    if (!Writer::open(lpFileName)) {
        os::log("apitrace: error: failed to open %s\n", lpFileName);
        os::abort();
    }

    pid = os::getCurrentProcessId();

    Writer::beginProperties();
    os::String processName = os::getProcessName();
    Writer::writeProperty("process.name", processName);
    Writer::endProperties();

#if 0
    // For debugging the exception handler
    *((int *)0) = 0;
#endif
}

static uintptr_t next_thread_num = 1;

static OS_THREAD_LOCAL uintptr_t thread_num;

void LocalWriter::checkProcessId(void) {
    if (m_file &&
        os::getCurrentProcessId() != pid) {
        // We are a forked child process that inherited the trace file, so
        // create a new file.  We can't call any method of the current
        // file, as it may cause it to flush and corrupt the parent's
        // trace, so we effectively leak the old file object.
        close();
        // Don't want to open the same file again
        os::unsetEnvironment("TRACE_FILE");
        open();
    }
}

unsigned LocalWriter::beginEnter(const FunctionSig *sig, bool fake) {
    mutex.lock();
    ++acquired;

    checkProcessId();
    if (!m_file) {
        open();
    }

    uintptr_t this_thread_num = thread_num;
    if (!this_thread_num) {
        this_thread_num = next_thread_num++;
        thread_num = this_thread_num;
    }

    assert(this_thread_num);
    unsigned thread_id = this_thread_num - 1;
    unsigned call_no = Writer::beginEnter(sig, thread_id);
    if (!fake && os::backtrace_is_needed(sig->name)) {
        std::vector<RawStackFrame> backtrace = os::get_backtrace();
        beginBacktrace(backtrace.size());
        for (auto & frame : backtrace) {
            writeStackFrame(&frame);
        }
        endBacktrace();
    }
    return call_no;
}

void LocalWriter::endEnter(void) {
    Writer::endEnter();
    --acquired;
    mutex.unlock();
}

void LocalWriter::beginLeave(unsigned call) {
    mutex.lock();
    ++acquired;
    Writer::beginLeave(call);
}

void LocalWriter::endLeave(void) {
    Writer::endLeave();
    --acquired;
    mutex.unlock();
}

void LocalWriter::flush(void) {
    /*
     * Do nothing if the mutex is already acquired (e.g., if a segfault happen
     * while writing the file) as state could be inconsistent, therefore yield
     * inconsistent trace files and/or repeated segfaults till infinity.
     */

    mutex.lock();
    if (acquired) {
        os::log("apitrace: ignoring recurrent flush\n");
    } else {
        ++acquired;
        if (m_file) {
            if (os::getCurrentProcessId() != pid) {
                os::log("apitrace: ignoring flush in child process\n");
            } else {
                os::log("apitrace: flushing trace\n");
                m_file->flush();
            }
        }
        --acquired;
    }
    mutex.unlock();
}


LocalWriter localWriter;


void fakeMemcpy(const void *ptr, size_t size) {
    assert(ptr);
    if (!size) {
        return;
    }

    unsigned _call = localWriter.beginEnter(&memcpy_sig, true);

#if defined(_WIN32) && !defined(NDEBUG)
    size_t maxSize = 0;
    MEMORY_BASIC_INFORMATION mi;
    while (VirtualQuery((const uint8_t *)ptr + maxSize, &mi, sizeof mi) == sizeof mi &&
           mi.Protect & (PAGE_READONLY|PAGE_READWRITE)) {
        maxSize = (const uint8_t *)mi.BaseAddress + mi.RegionSize - (const uint8_t *)ptr;
    }
    if (maxSize < size) {
        os::log("apitrace: warning: %u: clamping size from %Iu to %Iu\n", _call, size, maxSize);
        size = maxSize;
    }
#endif

    localWriter.beginArg(0);
    localWriter.writePointer((uintptr_t)ptr);
    localWriter.endArg();
    localWriter.beginArg(1);
    localWriter.writeBlob(ptr, size);
    localWriter.endArg();
    localWriter.beginArg(2);
    localWriter.writeUInt(size);
    localWriter.endArg();
    localWriter.endEnter();
    localWriter.beginLeave(_call);
    localWriter.endLeave();
}


} /* namespace trace */

