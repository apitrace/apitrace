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


/* XXX: We still use our own implementation:
 *
 * - MSVC's C++11 threads implementation are hardcoded to use C++ exceptions
 *
 * - MinGW's C++11 threads implementation is often either missing or relies on
 *   winpthreads
 */

#if !defined(_WIN32)
#define HAVE_CXX11_THREADS
#endif


#ifdef HAVE_CXX11_THREADS

#include <thread>
#include <mutex>
#include <condition_variable>

namespace os {

    using std::mutex;
    using std::recursive_mutex;
    using std::unique_lock;
    using std::condition_variable;
    using std::thread;

} /* namespace os */


#else /* !HAVE_CXX11_THREADS */


#include <assert.h>
#ifdef _WIN32
#  include <process.h>
#  include <windows.h>
#  if _WIN32_WINNT >= 0x0600
#    define HAVE_WIN32_CONDITION_VARIABLES
#  endif
#else
#  include <pthread.h>
#  include <unistd.h>
#endif

#include <functional>


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

    protected:
        _base_mutex(void) {
        }

    public:
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
#ifdef _WIN32
#  ifdef HAVE_WIN32_CONDITION_VARIABLES
        // Only supported on Vista an higher. Not yet supported by WINE.
        typedef CONDITION_VARIABLE native_handle_type;
        native_handle_type _native_handle;
#else
        // http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
        LONG cWaiters;
        enum {
            EVENT_ONE = 0,
            EVENT_ALL,
            EVENT_COUNT
        };
        HANDLE hEvents[EVENT_COUNT];
#endif
#else
        typedef pthread_cond_t native_handle_type;
        native_handle_type _native_handle;
#endif

    public:
        condition_variable() {
#ifdef _WIN32
#  ifdef HAVE_WIN32_CONDITION_VARIABLES
            InitializeConditionVariable(&_native_handle);
#  else
            cWaiters = 0;
            hEvents[EVENT_ONE] = CreateEvent(NULL, FALSE, FALSE, NULL);
            hEvents[EVENT_ALL] = CreateEvent(NULL, TRUE, FALSE, NULL);
#  endif
#else
            pthread_cond_init(&_native_handle, NULL);
#endif
        }

        ~condition_variable() {
#ifdef _WIN32
#  ifdef HAVE_WIN32_CONDITION_VARIABLES
            /* No-op */
#  else
            CloseHandle(hEvents[EVENT_ALL]);
            CloseHandle(hEvents[EVENT_ONE]);
#  endif
#else
            pthread_cond_destroy(&_native_handle);
#endif
        }

        inline void
        notify_one(void) {
#ifdef _WIN32
#  ifdef HAVE_WIN32_CONDITION_VARIABLES
            WakeConditionVariable(&_native_handle);
#  else
            if (cWaiters) {
                SetEvent(hEvents[EVENT_ONE]);
            }
#  endif
#else
            pthread_cond_signal(&_native_handle);
#endif
        }

        inline void
        notify_all(void) {
#ifdef _WIN32
#  ifdef HAVE_WIN32_CONDITION_VARIABLES
            WakeAllConditionVariable(&_native_handle);
#  else
            if (cWaiters) {
                SetEvent(hEvents[EVENT_ALL]);
            }
#  endif
#else
            pthread_cond_broadcast(&_native_handle);
#endif
        }

        inline void
        wait(unique_lock<mutex> & lock) {
            mutex::native_handle_type & mutex_native_handle = lock.mutex()->native_handle();
#ifdef _WIN32
#  ifdef HAVE_WIN32_CONDITION_VARIABLES
            SleepConditionVariableCS(&_native_handle, &mutex_native_handle, INFINITE);
#  else
            InterlockedIncrement(&cWaiters);
            LeaveCriticalSection(&mutex_native_handle);
            DWORD dwResult;
            dwResult = WaitForMultipleObjects(EVENT_COUNT, hEvents, FALSE, INFINITE);
            EnterCriticalSection(&mutex_native_handle);
            if (InterlockedDecrement(&cWaiters) == 0 &&
                dwResult == WAIT_OBJECT_0 + EVENT_ALL) {
                ResetEvent(hEvents[EVENT_ALL]);
            }
#  endif
#else
            pthread_cond_wait(&_native_handle, &mutex_native_handle);
#endif
        }

        inline void
        wait(unique_lock<mutex> & lock, std::function<bool()> pred) {
            while (!pred) {
                wait(lock);
            }
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
    private:
        static_assert(sizeof(T) == sizeof(void *), "Size mismatch");

#ifdef _WIN32
        DWORD dwTlsIndex;
#else
        pthread_key_t key;
#endif

    public:
        thread_specific(void) {
#ifdef _WIN32
            dwTlsIndex = TlsAlloc();
#else
            pthread_key_create(&key, NULL);
#endif
        }

        ~thread_specific() {
#ifdef _WIN32
            TlsFree(dwTlsIndex);
#else
            pthread_key_delete(key);
#endif
        }

        inline T
        get(void) const {
            void *ptr;
#ifdef _WIN32
            ptr = TlsGetValue(dwTlsIndex);
#else
            ptr = pthread_getspecific(key);
#endif
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
#ifdef _WIN32
            TlsSetValue(dwTlsIndex, new_ptr);
#else
            pthread_setspecific(key, new_ptr);
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
        thread(thread &&other) :
            _native_handle(other._native_handle)
        {
            other._native_handle = 0;
        }

        thread(const thread &other) = delete;

        inline
        ~thread() {
        }

        static unsigned
        hardware_concurrency(void) {
#ifdef _WIN32
            SYSTEM_INFO si;
            GetSystemInfo(&si);
            return si.dwNumberOfProcessors;
#else
            return sysconf(_SC_NPROCESSORS_ONLN);
#endif
        }

        template< class Function, class... Args >
        explicit thread(Function &&f, Args&&... args) {
            auto bound = std::bind(std::forward<Function>(f), std::forward<Args>(args)...);
            auto data = new decltype(bound) (std::move(bound));
            _native_handle = _create(data);
        }

        inline thread &
        operator =(thread &&other) {
            assert(_native_handle == 0);
            _native_handle = other._native_handle;
            other._native_handle = 0;
            return *this;
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

        template< typename Param >
        static
#ifdef _WIN32
        unsigned __stdcall
#else
        void *
#endif
        _callback(void *lpParameter) {
            Param *pParam = static_cast<Param *>(lpParameter);
            (*pParam)();
            delete pParam;
            return 0;
        }

        template< typename Param >
        static inline native_handle_type
        _create(Param *function) {
#ifdef _WIN32
            uintptr_t handle =_beginthreadex(NULL, 0, &_callback<Param>, function, 0, NULL);
            return reinterpret_cast<HANDLE>(handle);
#else
            pthread_t t;
            pthread_create(&t, NULL, &_callback<Param>, function);
            return t;
#endif
        }
    };

} /* namespace os */


#endif  /* !HAVE_CXX11_THREADS */


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
#  error
#endif

#if defined(_WIN32_WINNT) && _WIN32_WINNT < 0x0600
#  define OS_THREAD_SPECIFIC(_type) os::thread_specific< _type >
#else
#  define OS_THREAD_SPECIFIC(_type) OS_THREAD_LOCAL _type
#endif

#define OS_THREAD_SPECIFIC_PTR(_type) OS_THREAD_SPECIFIC(_type *)
