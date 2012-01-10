/**************************************************************************
 *
 * Copyright 2007-2010 VMware, Inc.
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
 * Trace writing functions.
 */

#ifndef _TRACE_WRITER_LOCAL_HPP_
#define _TRACE_WRITER_LOCAL_HPP_


#include "os_thread.hpp"
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
         * We need a recursive mutex so that it doesn't dead lock when a segfault happens when the mutex is held.
         */
        os::recursive_mutex mutex;
        int acquired;

    public:
        /**
         * Should never called directly -- use localWriter singleton below instead.
         */
        LocalWriter();
        ~LocalWriter();

        void open(void);

        unsigned beginEnter(const FunctionSig *sig);
        void endEnter(void);

        void beginLeave(unsigned call);
        void endLeave(void);

        void flush(void);
    };

    /**
     * Singleton.
     */
    extern LocalWriter localWriter;

} /* namespace trace */

#endif /* _TRACE_WRITER_LOCAL_HPP_ */
