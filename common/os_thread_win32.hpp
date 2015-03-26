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
 * OS native thread abstraction for Windows.
 *
 * Mimics C++11 threads.
 */

#pragma once

#include <process.h>
#include <windows.h>


/*
 * This feature is not supported on Windows XP
 */
#define USE_WIN32_CONDITION_VARIABLES 0


/**
 * Compiler TLS.
 *
 * See also:
 * - http://gcc.gnu.org/onlinedocs/gcc-4.6.3/gcc/Thread_002dLocal.html
 * - http://msdn.microsoft.com/en-us/library/9w1sdazb.aspx
 */
#define OS_THREAD_SPECIFIC(_type) os::thread_specific< _type >


namespace os {


    /**
     * Base class for mutex and recursive_mutex.
     */
    class _base_mutex
    {
    public:
        typedef CRITICAL_SECTION native_handle_type;

        _base_mutex(void) {
            InitializeCriticalSection(&_native_handle);
        }

        ~_base_mutex() {
            DeleteCriticalSection(&_native_handle);
        }

        inline void
        lock(void) {
            EnterCriticalSection(&_native_handle);
        }

        inline void
        unlock(void) {
            LeaveCriticalSection(&_native_handle);
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
            InitializeCriticalSection(&_native_handle);
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
            InitializeCriticalSection(&_native_handle);
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
        unique_lock(mutex_type &m) :
            _mutex(m)
        {
            _mutex.lock();
        }

        inline
        ~unique_lock() {
            _mutex.unlock();
        }

        inline void
        lock() {
            _mutex.lock();
        }

        inline void
        unlock() {
            _mutex.unlock();
        }

        mutex_type *
        mutex() const {
            return &_mutex;
        }

    protected:
        mutex_type &_mutex;
    };


    /**
     * Same interface as std::condition_variable
     */
    class condition_variable
    {
    private:
#  if USE_WIN32_CONDITION_VARIABLES
        // XXX: Only supported on Vista an higher. Not yet supported by WINE.
        typedef CONDITION_VARIABLE native_handle_type;
        native_handle_type _native_handle;
#else
        // http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
        LONG cWaiters;
        HANDLE hEvent;
#endif

    public:
        condition_variable() {
#  if USE_WIN32_CONDITION_VARIABLES
            InitializeConditionVariable(&_native_handle);
#  else
            cWaiters = 0;
            hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
#  endif
        }

        ~condition_variable() {
#  if USE_WIN32_CONDITION_VARIABLES
            /* No-op */
#  else
            CloseHandle(hEvent);
#  endif
        }

        inline void
        notify_one(void) {
#  if USE_WIN32_CONDITION_VARIABLES
            WakeConditionVariable(&_native_handle);
#  else
            if (cWaiters) {
                SetEvent(hEvent);
            }
#  endif
        }

        inline void
        wait(unique_lock<mutex> & lock) {
            mutex::native_handle_type & mutex_native_handle = lock.mutex()->native_handle();
#  if USE_WIN32_CONDITION_VARIABLES
            SleepConditionVariableCS(&_native_handle, &mutex_native_handle, INFINITE);
#  else
            InterlockedIncrement(&cWaiters);
            LeaveCriticalSection(&mutex_native_handle);
            WaitForSingleObject(hEvent, INFINITE);
            EnterCriticalSection(&mutex_native_handle);
            InterlockedDecrement(&cWaiters);
#  endif
        }
    };


    /**
     * Implement TLS through OS threading API.
     *
     * This will only work when T is a pointer, intptr_t, or uintptr_t.
     */
    template <typename T>
    class thread_specific
    {
        static_assert(sizeof(T) == sizeof(void *), "size mismatch");

    private:
        DWORD dwTlsIndex;

    public:
        thread_specific(void) {
            dwTlsIndex = TlsAlloc();
        }

        ~thread_specific() {
            TlsFree(dwTlsIndex);
        }

        inline T
        get(void) const {
            void *ptr;
            ptr = TlsGetValue(dwTlsIndex);
            return reinterpret_cast<T>(ptr);
        }

        inline
        operator T (void) const
        {
            return get();
        }

        inline T
        operator -> (void) const
        {
            return get();
        }

        inline T
        operator = (T new_value)
        {
            set(new_value);
            return new_value;
        }

        inline void
        set(T new_value) {
            void *new_ptr = reinterpret_cast<void *>(new_value);
            TlsSetValue(dwTlsIndex, new_ptr);
        }
    };


    /**
     * Same interface as std::thread
     */
    class thread {
    public:
        typedef HANDLE native_handle_type;

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

        inline
        ~thread() {
        }

        template< class Function, class Arg >
        explicit thread( Function &f, Arg arg ) {
            typedef CallbackParam< Function, Arg > Param;
            Param *pParam = new Param(f, arg);
            _native_handle = _create(pParam);
        }

        inline thread &
        operator =(const thread &other) {
            _native_handle = other._native_handle;
            return *this;
        }

        inline bool
        joinable(void) const {
            return _native_handle != 0;
        }

        inline void
        join() {
            WaitForSingleObject(_native_handle, INFINITE);
        }

    private:
        native_handle_type _native_handle;

        template< typename Function, typename Arg >
        struct CallbackParam {
            Function &f;
            Arg arg;

            inline
            CallbackParam(Function &_f, Arg _arg) :
                f(_f),
                arg(_arg)
            {}

            inline void
            operator () (void) {
                f(arg);
            }
        };

        template< typename Param >
        static
        unsigned __stdcall
        _callback(void *lpParameter) {
            Param *pParam = static_cast<Param *>(lpParameter);
            (*pParam)();
            delete pParam;
            return 0;
        }

        template< typename Param >
        static inline native_handle_type
        _create(Param *pParam) {
            uintptr_t handle =_beginthreadex(NULL, 0, &_callback<Param>, static_cast<void *>(pParam), 0, NULL);
            return reinterpret_cast<HANDLE>(handle);
        }
    };

} /* namespace os */
