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


namespace Trace {


static void exceptionCallback(void)
{
    OS::DebugMessage("apitrace: flushing trace due to an exception\n");
    localWriter.flush();
}


LocalWriter::LocalWriter() :
    acquired(0)
{}

LocalWriter::~LocalWriter()
{
    OS::ResetExceptionCallback();
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
        OS::GetProcessName(szProcessName, PATH_MAX);
        OS::GetCurrentDir(szCurrentDir, PATH_MAX);

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

    OS::DebugMessage("apitrace: tracing to %s\n", szFileName);

    Writer::open(szFileName);

    OS::SetExceptionCallback(exceptionCallback);

#if 0
    // For debugging the exception handler
    *((int *)0) = 0;
#endif
}

unsigned LocalWriter::beginEnter(const FunctionSig *sig) {
    OS::AcquireMutex();
    ++acquired;

    if (!m_file->isOpened()) {
        open();
    }

    return Writer::beginEnter(sig);
}

void LocalWriter::endEnter(void) {
    Writer::endEnter();
    --acquired;
    OS::ReleaseMutex();
}

void LocalWriter::beginLeave(unsigned call) {
    OS::AcquireMutex();
    ++acquired;
    Writer::beginLeave(call);
}

void LocalWriter::endLeave(void) {
    Writer::endLeave();
    --acquired;
    OS::ReleaseMutex();
}

void LocalWriter::flush(void) {
    /*
     * Do nothing if the mutex is already acquired (e.g., if a segfault happen
     * while writing the file) to prevent dead-lock.
     */

    if (!acquired) {
        OS::AcquireMutex();
        if (m_file->isOpened()) {
            m_file->flush();
        }
        OS::ReleaseMutex();
    }
}


LocalWriter localWriter;


} /* namespace Trace */

