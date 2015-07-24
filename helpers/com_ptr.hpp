/**************************************************************************
 *
 * Copyright 2014 VMware, Inc.
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

#pragma once


#include <assert.h>

#include <windows.h>


/**
 * Simple smart pointer template for COM interfaces.
 *
 * - https://msdn.microsoft.com/en-us/magazine/dn904668.aspx
 * - https://msdn.microsoft.com/en-us/library/417w8b3b.aspx
 * - https://msdn.microsoft.com/en-us/library/ezzw7k98.aspx
 */
template< typename T >
class com_ptr
{
private:
    T *p;

public:
    com_ptr(void) :
        p(nullptr)
    {
    }

    com_ptr(T *_p) :
        p(_p)
    {
    }

    ~com_ptr() {
        T *temp = p;
        p = nullptr;
        if (temp) {
            temp->Release();
        }
    }

    // Used when initializing
    T **
    operator & () {
        assert(p == nullptr);
        return &p;
    }

    // Implict cast to T*
    operator T * () const {
        return p;
    }

    struct no_ref_count : public T
    {
    private:
        ULONG STDMETHODCALLTYPE AddRef(void);
        ULONG STDMETHODCALLTYPE Release(void);
    };

    // Methods
    no_ref_count *
    operator -> () const {
        assert(p != nullptr);
        return static_cast< no_ref_count *>(p);
    }

    com_ptr &
    operator = (T *q) {
        if (p != q) {
            T *temp = p;
            p = q;
            if (temp) {
                temp->Release();
            }
            if (q) {
                q->AddRef();
            }
        }
        return *this;
    }

    com_ptr(const com_ptr &) = delete;
    com_ptr & operator= (const com_ptr &) = delete;
};



