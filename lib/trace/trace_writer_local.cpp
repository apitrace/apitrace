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
#include "trace_option.hpp"
#include "trace_ostream.hpp"
#include "trace_writer_local.hpp"
#include "trace_format.hpp"
#include "os_backtrace.hpp"

#ifdef _WIN32
#include <shlobj.h>
#endif


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
    acquired(0),
    sharedPtrThis(std::make_shared<LocalWriter*>(this))
{
    os::String process = os::getProcessName();
    os::log("apitrace: loaded into %s\n", process.str());

    // Install the signal handlers as early as possible, to prevent
    // interfering with the application's signal handling.
    os::setExceptionCallback(exceptionCallback);
}

static void FlushLocalWriterThread(const std::weak_ptr<LocalWriter*> writerWeakPtr,
                                   const uint32_t intervalMs)
{
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));

        const auto locked = writerWeakPtr.lock();
        if (!locked)
            break;
        const auto writer = *locked;

        writer->flush();
    }
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
    char suffix[17] = "";
    bool hasTimestamp;

    hasTimestamp = boolOption(getenv("TRACE_TIMESTAMP"), false);
    if (hasTimestamp) {
        std::time_t time = std::time({});
        strftime(suffix, sizeof(suffix), ".%Y%m%dT%H%M%S", std::localtime(&time));
    }

    lpFileName = getenv("TRACE_FILE");
    if (!lpFileName) {
        static unsigned dwCounter = 0;

        os::String process = os::getProcessName();
#ifdef _WIN32
        process.trimExtension();
#endif
        process.trimDirectory();

        os::String prefix = os::getCurrentDir();
#ifdef _WIN32
        // Default to user's desktop, as current directory often points to
        // system directory or program files, which are not generally writable.
        char szDesktopPath[MAX_PATH];
        HRESULT hr = SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, szDesktopPath);
        if (SUCCEEDED(hr)) {
            prefix = szDesktopPath;
        }
#endif
        prefix.join(process);

        for (;;) {
            FILE *file;

            if (dwCounter)
                szFileName = os::String::format("%s.%s%u.trace", prefix.str(), suffix, dwCounter);
            else
                szFileName = os::String::format("%s%s.trace", prefix.str(), suffix);

            lpFileName = szFileName;
            file = fopen(lpFileName, "rb");
            if (file == NULL)
                break;

            fclose(file);

            ++dwCounter;
        }
    } else if (hasTimestamp) {
        const char *ext = strrchr(lpFileName, '.');
        int length = ext ? ext - lpFileName : strlen(lpFileName);

        szFileName = os::String::format("%.*s%s%s", length, lpFileName, suffix, ext ? ext : "");
        lpFileName = szFileName;
    }

    os::log("apitrace: tracing to %s\n", lpFileName);

    Properties properties;
    os::String processName = os::getProcessName();
    properties["process.name"] = processName;
    os::String processCommandLine = os::getProcessCommandLine();
    properties["process.commandLine"] = processCommandLine;

    if (!Writer::open(lpFileName, TRACE_VERSION, properties)) {
        os::log("apitrace: error: failed to open %s\n", lpFileName);
        os::abort();
    }

    pid = os::getCurrentProcessId();

    const auto flushIntervalStr = getenv("FLUSH_EVERY_MS");
    if (flushIntervalStr) {
        const auto intervalMs = atoi(flushIntervalStr);
        if (intervalMs < 0) {
            os::log("apitrace: error: invalid FLUSH_EVERY_MS: %s\n", flushIntervalStr);
            os::abort();
        }
        std::thread(FlushLocalWriterThread, this->sharedPtrThis, uint32_t(intervalMs)).detach();
    }

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
    if (fake) {
        writeFlags(FLAG_FAKE);
    } else if (os::backtrace_is_needed(sig->name)) {
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
        os::log("apitrace: warning: %u: clamping size from %zu to %zu\n", _call, size, maxSize);
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

