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
#include "trace_file.hpp"
#include "trace_writer_local.hpp"
#include "trace_format.hpp"
#include "trace_backtrace.hpp"


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
    os::log("apitrace: loaded\n");

    // Install the signal handlers as early as possible, to prevent
    // interfering with the application's signal handling.
    os::setExceptionCallback(exceptionCallback);
}

LocalWriter::~LocalWriter()
{
    os::resetExceptionCallback();
    checkProcessId();
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
	os::String prefix = "/data";
#else
	os::String prefix = os::getCurrentDir();
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

#if 0
    // For debugging the exception handler
    *((int *)0) = 0;
#endif
}

static uintptr_t next_thread_num = 1;

static OS_THREAD_SPECIFIC_PTR(void)
thread_num;

void LocalWriter::checkProcessId(void) {
    if (m_file->isOpened() &&
        os::getCurrentProcessId() != pid) {
        // We are a forked child process that inherited the trace file, so
        // create a new file.  We can't call any method of the current
        // file, as it may cause it to flush and corrupt the parent's
        // trace, so we effectively leak the old file object.
        m_file = File::createSnappy();
        // Don't want to open the same file again
        os::unsetEnvironment("TRACE_FILE");
        open();
    }
}

unsigned LocalWriter::beginEnter(const FunctionSig *sig, bool fake) {
    mutex.lock();
    ++acquired;

    checkProcessId();
    if (!m_file->isOpened()) {
        open();
    }

    // Although thread_num is a void *, we actually use it as a uintptr_t
    uintptr_t this_thread_num =
        reinterpret_cast<uintptr_t>(static_cast<void *>(thread_num));
    if (!this_thread_num) {
        this_thread_num = next_thread_num++;
        thread_num = reinterpret_cast<void *>(this_thread_num);
    }

    assert(this_thread_num);
    unsigned thread_id = this_thread_num - 1;
    unsigned call_no = Writer::beginEnter(sig, thread_id);
    if (!fake && backtrace_is_needed(sig->name)) {
        std::vector<RawStackFrame> backtrace = get_backtrace();
        beginBacktrace(backtrace.size());
        for (unsigned i = 0; i < backtrace.size(); ++i) {
            writeStackFrame(&backtrace[i]);
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
        os::log("apitrace: ignoring exception while tracing\n");
    } else {
        ++acquired;
        if (m_file->isOpened()) {
            if (os::getCurrentProcessId() != pid) {
                os::log("apitrace: ignoring exception in child process\n");
            } else {
                os::log("apitrace: flushing trace due to an exception\n");
                m_file->flush();
            }
        }
        --acquired;
    }
    mutex.unlock();
}


LocalWriter localWriter;


} /* namespace trace */

