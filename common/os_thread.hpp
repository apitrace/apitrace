/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
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
 * Simple OS abstraction.
 *
 * Mimics C++11 / boost threads.
 */

#ifndef _OS_THREAD_HPP_
#define _OS_THREAD_HPP_


#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
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
    public:
#ifdef _WIN32
        /* FIXME */
#else
        typedef pthread_cond_t native_handle_type;
#endif

        condition_variable() {
#ifdef _WIN32
            /* FIXME */
#else
            pthread_cond_init(&_native_handle, NULL);
#endif
        }

        ~condition_variable() {
#ifdef _WIN32
            /* FIXME */
#else
            pthread_cond_destroy(&_native_handle);
#endif
        }

        inline void
        signal(void) {
#ifdef _WIN32
            /* FIXME */
#else
            pthread_cond_signal(&_native_handle);
#endif
        }

        inline void
        wait(unique_lock<mutex> & lock) {
#ifdef _WIN32
            /* FIXME */
#else
            pthread_cond_wait(&_native_handle, &lock.mutex()->native_handle());
#endif
        }

    protected:
        native_handle_type _native_handle;
    };


    /**
     * Same interface as boost::thread_specific_ptr.
     */
    template <typename T>
    class thread_specific_ptr
    {
    private:
#ifdef _WIN32
        DWORD dwTlsIndex;
#else
        pthread_key_t key;

        static void destructor(void *ptr) {
            delete static_cast<T *>(ptr);
        }
#endif

    public:
        thread_specific_ptr(void) {
#ifdef _WIN32
            dwTlsIndex = TlsAlloc();
#else
            pthread_key_create(&key, &destructor);
#endif
        }

        ~thread_specific_ptr() {
#ifdef _WIN32
            TlsFree(dwTlsIndex);
#else
            pthread_key_delete(key);
#endif
        }

        T* get(void) const {
            void *ptr;
#ifdef _WIN32
            ptr = TlsGetValue(dwTlsIndex);
#else
            ptr = pthread_getspecific(key);
#endif
            return static_cast<T*>(ptr);
        }

        T* operator -> (void) const
        {
            return get();
        }

        T& operator * (void) const
        {
            return *get();
        }

        void reset(T* new_value=0) {
            T * old_value = get();
#ifdef _WIN32
            TlsSetValue(dwTlsIndex, new_value);
#else
            pthread_setspecific(key, new_value);
#endif
            if (old_value) {
                delete old_value;
            }
        }
    };


} /* namespace os */

#endif /* _OS_THREAD_HPP_ */
