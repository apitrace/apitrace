/**************************************************************************
 *
 * Copyright 2011-2012 Jose Fonseca
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
 * OS native thread abstraction.
 *
 * Mimics/leverages C++11 threads.
 */

#pragma once


#ifdef _WIN32

/* XXX: Use our own implementation on Windows:
 *
 * - MSVC's C++11 threads implementation relies on C++ exceptions
 *
 * - MinGW's C++11 threads implementation is often either missing or relies on
 *   winpthreads
 */
#include "os_thread_win32.hpp"

#else /* !_WIN32 */

#include <thread>
#include <mutex>
#include <condition_variable>

#define OS_THREAD_SPECIFIC(_type) __thread _type

namespace os {

    using std::mutex;
    using std::recursive_mutex;
    using std::unique_lock;
    using std::condition_variable;
    using std::thread;

} /* namespace os */

#endif /* !_WIN32 */

#define OS_THREAD_SPECIFIC_PTR(_type) OS_THREAD_SPECIFIC(_type *)
