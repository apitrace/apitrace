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
#include "trace_file.hpp"
#include "trace_writer.hpp"
#include "trace_format.hpp"


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
    // Install the signal handlers as early as possible, to prevent
    // interfering with the application's signal handling.
    os::SetExceptionCallback(exceptionCallback);
}

LocalWriter::~LocalWriter()
{
    os::ResetExceptionCallback();
}

void
LocalWriter::open(void) {

    static unsigned dwCounter = 0;

    const char *szExtension = "trace";
    char szFileName[PATH_MAX];
    const char *lpFileName;

    lpFileName = getenv("TRACE_FILE");
    if (lpFileName) {
        strncpy(szFileName, lpFileName, PATH_MAX);
    }
    else {
        char szProcessName[PATH_MAX];
        char szCurrentDir[PATH_MAX];
        os::GetProcessName(szProcessName, PATH_MAX);
        os::GetCurrentDir(szCurrentDir, PATH_MAX);

        for (;;) {
            FILE *file;

            if (dwCounter)
                snprintf(szFileName, PATH_MAX, "%s%c%s.%u.%s", szCurrentDir, PATH_SEP, szProcessName, dwCounter, szExtension);
            else
                snprintf(szFileName, PATH_MAX, "%s%c%s.%s", szCurrentDir, PATH_SEP, szProcessName, szExtension);

            file = fopen(szFileName, "rb");
            if (file == NULL)
                break;

            fclose(file);

            ++dwCounter;
        }
    }

    os::DebugMessage("apitrace: tracing to %s\n", szFileName);

    Writer::open(szFileName);

#if 0
    // For debugging the exception handler
    *((int *)0) = 0;
#endif
}

unsigned LocalWriter::beginEnter(const FunctionSig *sig) {
    os::AcquireMutex();
    ++acquired;

    if (!m_file->isOpened()) {
        open();
    }

    return Writer::beginEnter(sig);
}

void LocalWriter::endEnter(void) {
    Writer::endEnter();
    --acquired;
    os::ReleaseMutex();
}

void LocalWriter::beginLeave(unsigned call) {
    os::AcquireMutex();
    ++acquired;
    Writer::beginLeave(call);
}

void LocalWriter::endLeave(void) {
    Writer::endLeave();
    --acquired;
    os::ReleaseMutex();
}

void LocalWriter::flush(void) {
    /*
     * Do nothing if the mutex is already acquired (e.g., if a segfault happen
     * while writing the file) to prevent dead-lock.
     */

    if (!acquired) {
        os::AcquireMutex();
        if (m_file->isOpened()) {
            os::DebugMessage("apitrace: flushing trace due to an exception\n");
            m_file->flush();
        }
        os::ReleaseMutex();
    }
}


LocalWriter localWriter;


} /* namespace trace */

