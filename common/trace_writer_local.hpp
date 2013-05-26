/**************************************************************************
 *
 * Copyright 2007-2013 VMware, Inc.
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

/*
 * Trace writing functions, used to trace calls in the current process.
 */

#ifndef _TRACE_WRITER_LOCAL_HPP_
#define _TRACE_WRITER_LOCAL_HPP_


#include <stdint.h>

#include "os_thread.hpp"
#include "os_process.hpp"
#include "trace_writer.hpp"


namespace trace {

    extern const FunctionSig memcpy_sig;
    extern const FunctionSig malloc_sig;
    extern const FunctionSig free_sig;
    extern const FunctionSig realloc_sig;

    /**
     * A specialized Writer class, mean to trace the current process.
     *
     * In particular:
     * - it creates a trace file based on the current process name
     * - uses mutexes to allow tracing from multiple threades
     * - flushes the output to ensure the last call is traced in event of
     *   abnormal termination
     */
    class LocalWriter : public Writer {
    protected:
        /**
         * This mutex guarantees that only one thread writes to the trace file
         * at one given instance.
         *
         * We need a recursive mutex so that we dont't dead lock in the event
         * of a segfault happens while the mutex is held.
         *
         * To prevent deadlocks, the call for the real function (the one being
         * traced) should not be done with the mutex held. That is, it should
         * be done outside the beginEnter/endEnter and beginLeave/endLeave
         * pairs. Preferably between these two pairs.
         */
        os::recursive_mutex mutex;
        int acquired;

        /**
         * ID of the processed that opened the trace file.
         */
        os::ProcessId pid;

        void checkProcessId();

    public:
        /**
         * Should never called directly -- use localWriter singleton below
         * instead.
         */
        LocalWriter();
        ~LocalWriter();

        void open(void);

        /**
         * It will acquire the mutex.
         */
        unsigned beginEnter(const FunctionSig *sig, bool fake = false);

        /**
         * It will release the mutex.
         */
        void endEnter(void);

        /**
         * It will acquire the mutex.
         */
        void beginLeave(unsigned call);

        /**
         * It will release the mutex.
         */
        void endLeave(void);

        void flush(void);
    };

    /**
     * Singleton.
     */
    extern LocalWriter localWriter;

} /* namespace trace */

#endif /* _TRACE_WRITER_LOCAL_HPP_ */
