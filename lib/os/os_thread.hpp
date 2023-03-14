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


#include <thread>
#include <mutex>
#include <condition_variable>


/**
 * Compiler TLS.
 *
 * It's not portable to use for DLLs on Windows XP, or non-POD types.
 *
 * See also:
 * - http://gcc.gnu.org/onlinedocs/gcc-4.6.3/gcc/Thread_002dLocal.html
 * - http://msdn.microsoft.com/en-us/library/9w1sdazb.aspx
 * - https://msdn.microsoft.com/en-us/library/y5f6w579.aspx
 */
#if defined(__GNUC__)
#  define OS_THREAD_LOCAL __thread
#elif defined(_MSC_VER)
#  define OS_THREAD_LOCAL __declspec(thread)
#else
#  error Unsupported C++ compiler
#endif
