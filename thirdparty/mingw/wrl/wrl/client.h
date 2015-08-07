/**************************************************************************
 *
 * Copyright 2014-2015 VMware, Inc.
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


namespace Microsoft {
namespace WRL {

/**
 * See
 * - https://msdn.microsoft.com/en-us/library/br244983.aspx
 * - https://msdn.microsoft.com/en-us/magazine/dn904668.aspx
 */
template< typename T >
class ComPtr
{
private:
    T *p;

public:
    ComPtr(void) :
        p(nullptr)
    {
    }

    ComPtr(T *_p) :
        p(_p)
    {
    }

    ~ComPtr() {
        T *temp = p;
        p = nullptr;
        if (temp) {
            temp->Release();
        }
    }

    T **
    GetAddressOf(void) {
        assert(p == nullptr);
        return &p;
    }

    T *
    Get(void) const {
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

    ComPtr &
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

    ComPtr(const ComPtr &) = delete;
    ComPtr & operator= (const ComPtr &) = delete;
};

} // namespace WRL
} // namespace Microsoft
