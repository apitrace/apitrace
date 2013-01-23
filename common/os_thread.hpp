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
 * Mimics C++11 threads.
 */

#ifndef _OS_THREAD_HPP_
#define _OS_THREAD_HPP_


#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif


#define USE_WIN32_CONDITION_VARIABLES 0


/**
 * Compiler TLS.
 *
 * See also:
 * - http://gcc.gnu.org/onlinedocs/gcc-4.6.3/gcc/Thread_002dLocal.html
 * - http://msdn.microsoft.com/en-us/library/9w1sdazb.aspx
 */
#if defined(_MSC_VER)
#  define thread_specific __declspec(thread)
#elif defined(__GNUC__)
#  define thread_specific __thread
#else
#  define thread_specific
#  error "Unsupported compiler"
#endif


namespace os {


    /**
     * Base class for mutex and recursive_mutex.
     */
    class _base_mutex
    {
    public:
#ifdef _WIN32
        typedef CRITICAL_SECTION native_handle_type;
#else
        typedef pthread_mutex_t native_handle_type;
#endif

        _base_mutex(void) {
#ifdef _WIN32
            InitializeCriticalSection(&_native_handle);
#else
            pthread_mutexattr_t attr;
            pthread_mutexattr_init(&attr);
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
            pthread_mutex_init(&_native_handle, &attr);
            pthread_mutexattr_destroy(&attr);
#endif
        }

        ~_base_mutex() {
#ifdef _WIN32
            DeleteCriticalSection(&_native_handle);
#else
            pthread_mutex_destroy(&_native_handle);
#endif
        }

        inline void
        lock(void) {
#ifdef _WIN32
            EnterCriticalSection(&_native_handle);
#else
            pthread_mutex_lock(&_native_handle);
#endif
        }

        inline void
        unlock(void) {
#ifdef _WIN32
            LeaveCriticalSection(&_native_handle);
#else
            pthread_mutex_unlock(&_native_handle);
#endif
        }

        native_handle_type & native_handle() {
            return _native_handle;
        }

    protected:
        native_handle_type _native_handle;
    };


    /**
     * Same interface as std::mutex.
     */
    class mutex : public _base_mutex
    {
    public:
        inline
        mutex(void) {
#ifdef _WIN32
            InitializeCriticalSection(&_native_handle);
#else
            pthread_mutex_init(&_native_handle, NULL);
#endif
        }
    };


    /**
     * Same interface as std::recursive_mutex.
     */
    class recursive_mutex : public _base_mutex
    {
    public:
        inline
        recursive_mutex(void) {
#ifdef _WIN32
            InitializeCriticalSection(&_native_handle);
#else
            pthread_mutexattr_t attr;
            pthread_mutexattr_init(&attr);
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
            pthread_mutex_init(&_native_handle, &attr);
            pthread_mutexattr_destroy(&attr);
#endif
        }
    };


    /**
     * Same interface as std::unique_lock;
     */
    template< class Mutex >
    class unique_lock
    {
    public:
        typedef Mutex mutex_type;

        inline explicit
        unique_lock(mutex_type & mutex) :
            _mutex(&mutex)
        {
            _mutex->lock();
        }

        inline
        ~unique_lock() {
            _mutex->unlock();
        }

        inline void
        lock() {
            _mutex->lock();
        }

        inline void
        unlock() {
            _mutex->unlock();
        }

        mutex_type *
        mutex() const {
            return _mutex;
        }

    protected:
        mutex_type *_mutex;
    };


    /**
     * Same interface as std::condition_variable
     */
    class condition_variable
    {
    private:
#ifdef _WIN32
#  if USE_WIN32_CONDITION_VARIABLES
        // XXX: Only supported on Vista an higher. Not yet supported by WINE.
        typedef CONDITION_VARIABLE native_handle_type;
        native_handle_type _native_handle;
#else
        // http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
        LONG cWaiters;
        HANDLE hEvent;
#endif
#else
        typedef pthread_cond_t native_handle_type;
        native_handle_type _native_handle;
#endif

    public:
        condition_variable() {
#ifdef _WIN32
#  if USE_WIN32_CONDITION_VARIABLES
            InitializeConditionVariable(&_native_handle);
#  else
            cWaiters = 0;
            hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
#  endif
#else
            pthread_cond_init(&_native_handle, NULL);
#endif
        }

        ~condition_variable() {
#ifdef _WIN32
#  if USE_WIN32_CONDITION_VARIABLES
            /* No-op */
#  else
            CloseHandle(hEvent);
#  endif
#else
            pthread_cond_destroy(&_native_handle);
#endif
        }

        inline void
        signal(void) {
#ifdef _WIN32
#  if USE_WIN32_CONDITION_VARIABLES
            WakeConditionVariable(&_native_handle);
#  else
            if (cWaiters) {
                SetEvent(hEvent);
            }
#  endif
#else
            pthread_cond_signal(&_native_handle);
#endif
        }

        inline void
        wait(unique_lock<mutex> & lock) {
            mutex::native_handle_type & mutex_native_handle = lock.mutex()->native_handle();
#ifdef _WIN32
#  if USE_WIN32_CONDITION_VARIABLES
            SleepConditionVariableCS(&_native_handle, &mutex_native_handle, INFINITE);
#  else
            InterlockedIncrement(&cWaiters);
            LeaveCriticalSection(&mutex_native_handle);
            WaitForSingleObject(hEvent, INFINITE);
            EnterCriticalSection(&mutex_native_handle);
            InterlockedDecrement(&cWaiters);
#  endif
#else
            pthread_cond_wait(&_native_handle, &mutex_native_handle);
#endif
        }
    };


    /**
     * Same interface as std::thread
     */
    class thread {
    public:
#ifdef _WIN32
        typedef HANDLE native_handle_type;
#else
        typedef pthread_t native_handle_type;
#endif

        inline
        thread() :
            _native_handle(0)
        {
        }

        inline
        thread(const thread &other) :
            _native_handle(other._native_handle)
        {
        }

        template< class Function, class Arg >
        explicit thread( Function& f, Arg arg ) {
#ifdef _WIN32
            DWORD id = 0;
            _native_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)f, (LPVOID)arg, 0, &id);
#else
            pthread_create(&_native_handle, NULL, (void *(*) (void *))f, (void *)arg);
#endif
        }

        inline bool
        joinable(void) const {
            return _native_handle != 0;
        }

        inline void
        join() {
#ifdef _WIN32
            WaitForSingleObject(_native_handle, INFINITE);
#else
            pthread_join(_native_handle, NULL);
#endif
        }

    private:
        native_handle_type _native_handle;

#if 0
#ifdef _WIN32
        template< class Function, class Arg >
        static DWORD WINAPI
        ThreadProc(LPVOID lpParameter) {

        );
#endif
#endif
    };

} /* namespace os */

#endif /* _OS_THREAD_HPP_ */
