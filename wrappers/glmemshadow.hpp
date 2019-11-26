/*
 * Copyright © 2019 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once

#include "glimports.hpp"

#include <stdlib.h>
#include <stdint.h>

#include <memory>
#include <vector>

namespace gltrace
{
class Context;
class ShareableContextResources;
}

class GLMemoryShadow
{
private:
    typedef std::weak_ptr<gltrace::ShareableContextResources>  shared_context_res_wptr_t;
    typedef std::shared_ptr<gltrace::ShareableContextResources>  shared_context_res_ptr_t;

    shared_context_res_wptr_t sharedRes;

    GLbitfield flags = 0;

    uint8_t *glMemory = nullptr;
    uint8_t *shadowMemory = nullptr;

    size_t mappedStart = 0;
    size_t mappedSize = 0;

    size_t nPages = 0;

    size_t mappedStartPage = 0;
    size_t mappedEndPage = 0;

    bool isDirty = false;
    std::vector<uint32_t> dirtyPages;
    uint32_t pagesToDirtyOnConsecutiveWrites = 1;
    uint32_t lastDirtiedRelativePage = UINT32_MAX - 1;

public:

    typedef void (*Callback)(const void *ptr, size_t size);

    ~GLMemoryShadow();

    bool init(const void *data, size_t size);

    void *map(gltrace::Context *_ctx, void *_glMemory, GLbitfield _flags, size_t start, size_t size);
    void unmap(Callback callback);

    void commitWrites(Callback callback);
    void updateForReads();

    void onAddressWrite(uintptr_t addr, size_t page);

    GLbitfield getMapFlags() const;

    static void commitAllWrites(gltrace::Context *_ctx, Callback callback);
    static void syncAllForReads(gltrace::Context *_ctx);

private:

    void setPageDirty(size_t relativePage);
    bool isPageDirty(size_t relativePage);
};
