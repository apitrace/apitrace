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
 * TLS Wrapper for Platforms without a thread_specific variable modifier
 */

#ifndef _TLS_WRAPPER_HPP_
#define _TLS_WRAPPER_HPP_

#if !defined NO_TLS && !defined __APPLE__
#error Not supported on this platform
#endif

#include <pthread.h>
#include <stdint.h>

template<typename T>
class TLSWrapper {
public:
    TLSWrapper() {
        pthread_key_create(&m_key, NULL);
    }
    ~TLSWrapper() {
        pthread_key_delete(m_key);
    }
    
    T Get() {
        return (T)(uintptr_t)pthread_getspecific(m_key);
    }
    
    void Set(T value) {
        pthread_setspecific(m_key, (const void *)value);
    }
private:
    pthread_key_t m_key;
};

#endif /* _TLS_WRAPPER_HPP_ */