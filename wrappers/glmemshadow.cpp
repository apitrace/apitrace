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

#include "glmemshadow.hpp"

#include <unordered_map>
#include <algorithm>

#include <assert.h>

#ifdef _WIN32

#include <windows.h>

#else

#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>

#endif

#include "gltrace.hpp"
#include "os_thread.hpp"
#include "os.hpp"

static bool sInitialized = false;

static std::unordered_map<size_t, GLMemoryShadow*> sPages;
static size_t sPageSize;

static os::mutex mutex;

enum class MemProtection {
#ifdef _WIN32
    NO_ACCESS = PAGE_NOACCESS,
    READ_ONLY = PAGE_READONLY,
    READ_WRITE = PAGE_READWRITE,
#else
    NO_ACCESS = PROT_NONE,
    READ_ONLY = PROT_READ,
    READ_WRITE = PROT_READ | PROT_WRITE,
#endif
};

size_t getSystemPageSize() {
#ifdef _WIN32
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwPageSize;
#else
    return sysconf(_SC_PAGESIZE);
#endif
}

void memProtect(void *addr, size_t size, MemProtection protection) {
#ifdef _WIN32
    DWORD flOldProtect;
    BOOL bRet = VirtualProtect(addr, size, static_cast<DWORD>(protection), &flOldProtect);
    if (!bRet) {
        DWORD dwLastError = GetLastError();
        os::log("apitrace: error: VirtualProtect failed with error 0x%lx\n", dwLastError);
        os::abort();
    }
#else
    const int err = mprotect(addr, size, static_cast<int>(protection));
    if (err) {
        const char *errorStr = strerror(err);
        os::log("apitrace: error: mprotect failed with error \"%s\"\n", errorStr);
        os::abort();
    }
#endif
}

template<typename T, typename U>
auto divRoundUp(T a, U b) -> decltype(a / b) {
    return (a + b - 1) / b;
}

#ifdef _WIN32
static LONG CALLBACK
VectoredHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
    PEXCEPTION_RECORD pExceptionRecord = pExceptionInfo->ExceptionRecord;
    DWORD ExceptionCode = pExceptionRecord->ExceptionCode;

    if (ExceptionCode == EXCEPTION_ACCESS_VIOLATION &&
        pExceptionRecord->NumberParameters >= 2 &&
        pExceptionRecord->ExceptionInformation[0] == 1) { // writing

        const uintptr_t addr = static_cast<uintptr_t>(pExceptionRecord->ExceptionInformation[1]);
        const size_t page = addr / sPageSize;

        os::unique_lock<os::mutex> lock(mutex);

        const auto it = sPages.find(page);
        if (it != sPages.end()) {
            GLMemoryShadow *shadow = it->second;
            shadow->onAddressWrite(addr, page);
            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

#else

static struct sigaction sPrevSigAction;

void PageGuardExceptionHandler(int sig, siginfo_t *si, void *context) {
    if (sig == SIGSEGV && si->si_code == SEGV_ACCERR) {
        const uintptr_t addr = reinterpret_cast<uintptr_t>(si->si_addr);
        const size_t page = addr / sPageSize;

        os::unique_lock<os::mutex> lock(mutex);

        const auto it = sPages.find(page);
        if (it != sPages.end()) {
            GLMemoryShadow *shadow = it->second;
            shadow->onAddressWrite(addr, page);
            return;
        }
    }

    if (sPrevSigAction.sa_flags & SA_SIGINFO) {
        sPrevSigAction.sa_sigaction(sig, si, context);
    } else {
        if (sPrevSigAction.sa_handler == SIG_DFL) {
            signal(sig, SIG_DFL);
            raise(sig);
        } else if (sPrevSigAction.sa_handler == SIG_IGN) {
            // Ignore
        } else {
            sPrevSigAction.sa_handler(sig);
        }
    }
}
#endif

void initializeGlobals()
{
    sPageSize = getSystemPageSize();

#ifdef _WIN32
    if (AddVectoredExceptionHandler(1, VectoredHandler) == NULL) {
        os::log("apitrace: error: %s: add vectored exception handler failed\n", __FUNCTION__);
    }
#else
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = PageGuardExceptionHandler;
    if (sigaction(SIGSEGV, &sa, &sPrevSigAction) == -1) {
        os::log("apitrace: error: %s: set page guard exception handler failed\n", __FUNCTION__);
    }
#endif
}

GLMemoryShadow::~GLMemoryShadow()
{
    os::unique_lock<os::mutex> lock(mutex);

    const size_t startPage = reinterpret_cast<uintptr_t>(shadowMemory) / sPageSize;
    for (size_t i = 0; i < nPages; i++) {
        sPages.erase(startPage + i);
    }

#ifdef _WIN32
    VirtualFree(shadowMemory, nPages * sPageSize, MEM_RELEASE);
#else
    munmap(shadowMemory, nPages * sPageSize);
#endif
}

bool GLMemoryShadow::init(const void *data, size_t size)
{
    if (!sInitialized) {
        initializeGlobals();
        sInitialized = true;
    }

    nPages = divRoundUp(size, sPageSize);
    const size_t adjustedSize = nPages * sPageSize;

#ifdef _WIN32
    shadowMemory = reinterpret_cast<uint8_t*>(VirtualAlloc(nullptr, adjustedSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
#else
    shadowMemory = reinterpret_cast<uint8_t*>(mmap(nullptr, adjustedSize, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
#endif

    if (!shadowMemory) {
        os::log("apitrace: error: %s: Failed to allocate shadow memory!\n", __FUNCTION__);
        return false;
    }

    if (data != nullptr) {
        memcpy(shadowMemory, data, size);
    }

    memProtect(shadowMemory, adjustedSize, MemProtection::NO_ACCESS);

    {
        os::unique_lock<os::mutex> lock(mutex);

        const size_t startPage = reinterpret_cast<uintptr_t>(shadowMemory) / sPageSize;
        for (size_t i = 0; i < nPages; i++) {
            sPages.emplace(startPage + i, this);
        }
    }

    dirtyPages.resize(divRoundUp(nPages, 32));

    return true;
}

void *GLMemoryShadow::map(gltrace::Context *_ctx, void *_glMemory, GLbitfield _flags, size_t start, size_t size)
{
    sharedRes = _ctx->sharedRes;
    glMemory = reinterpret_cast<uint8_t*>(_glMemory);
    flags = _flags;
    mappedStart = start;
    mappedSize = size;

    mappedStartPage = start / sPageSize;
    mappedEndPage = divRoundUp(start + size, sPageSize);

    uint8_t *protectStart = shadowMemory + mappedStartPage * sPageSize;
    const size_t protectSize = (mappedEndPage - mappedStartPage) * sPageSize;

    // The buffer may have been updated before the mapping.
    // TODO: handle write only buffers
    if (flags & GL_MAP_READ_BIT) {
        memProtect(protectStart, protectSize, MemProtection::READ_WRITE);
        memcpy(shadowMemory + start, glMemory, size);
    }

    memProtect(protectStart, protectSize, MemProtection::READ_ONLY);

    return shadowMemory + start;
}

void GLMemoryShadow::unmap(Callback callback)
{
    if (isDirty) {
        os::unique_lock<os::mutex> lock(mutex);
        commitWrites(callback);
    }

    {
        os::unique_lock<os::mutex> lock(mutex);

        shared_context_res_ptr_t res = sharedRes.lock();
        if (res) {
            auto it = std::find(res->dirtyShadows.begin(), res->dirtyShadows.end(), this);
            if (it != res->dirtyShadows.end()) {
                res->dirtyShadows.erase(it);
            }
        } else {
            os::log("apitrace: error: %s: context(s) are destroyed!\n", __FUNCTION__);
        }
    }

    memProtect(shadowMemory, nPages * sPageSize, MemProtection::NO_ACCESS);

    sharedRes.reset();
    glMemory = nullptr;
    flags = 0;
    mappedStart = 0;
    mappedSize = 0;
    pagesToDirtyOnConsecutiveWrites = 1;
}

void GLMemoryShadow::onAddressWrite(uintptr_t addr, size_t page)
{
    const size_t relativePage = (addr - reinterpret_cast<uintptr_t>(shadowMemory)) / sPageSize;
    if (isPageDirty(relativePage)) {
        // It is possible if writing to the same buffer from two threads
        return;
    }

    if ((relativePage == lastDirtiedRelativePage + 1) && isPageDirty(relativePage - 1)) {
        /* Ensure that we would have log(n) page exceptions if traced application writes
         * to n consecutive pages.
         */
        pagesToDirtyOnConsecutiveWrites *= 2;
    } else {
        pagesToDirtyOnConsecutiveWrites = 1;
    }

    const size_t endPageToDirty = std::min(relativePage + pagesToDirtyOnConsecutiveWrites, nPages);
    for (size_t pageToDirty = relativePage; pageToDirty < endPageToDirty; pageToDirty++) {
        setPageDirty(pageToDirty);
    }

    lastDirtiedRelativePage = endPageToDirty - 1;

    memProtect(reinterpret_cast<void*>(page * sPageSize),
               (endPageToDirty - relativePage) * sPageSize, MemProtection::READ_WRITE);
}

GLbitfield GLMemoryShadow::getMapFlags() const
{
    return flags;
}

void GLMemoryShadow::setPageDirty(size_t relativePage)
{
    assert(relativePage < nPages);
    dirtyPages[relativePage / 32] |= 1U << (relativePage % 32);

    if (!isDirty) {
        shared_context_res_ptr_t res = sharedRes.lock();
        if (res) {
            res->dirtyShadows.push_back(this);
            isDirty = true;
        } else {
            os::log("apitrace: error: %s: context(s) are destroyed!\n", __FUNCTION__);
        }
    }
}

bool GLMemoryShadow::isPageDirty(size_t relativePage)
{
    assert(relativePage < nPages);
    return dirtyPages[relativePage / 32] & (1U << (relativePage % 32));
}

void GLMemoryShadow::commitWrites(Callback callback)
{
    assert(isDirty);

    uint8_t *shadowSlice = shadowMemory + mappedStartPage * sPageSize;
    const size_t glStartOffset = mappedStart % sPageSize;

    /* Other thread may write to the buffers at this very moment
     * so we need to protect pages before we read from them.
     * The other thread will have to wait until we commit all writes we want.
     */
    for (size_t i = mappedStartPage; i < mappedEndPage; i++) {
        if (isPageDirty(i)) {
            memProtect(shadowMemory + i * sPageSize, sPageSize, MemProtection::READ_ONLY);
        }
    }

    for (size_t i = mappedStartPage; i < mappedEndPage; i++) {
        if (isPageDirty(i)) {
            // We coalesce consecutive writes into one
            size_t firstDirty = i;
            while (++i < mappedEndPage && isPageDirty(i)) { }

            const size_t pages = i - firstDirty;
            if (firstDirty != mappedStartPage) {
                const size_t shadowOffset = (firstDirty - mappedStartPage) * sPageSize;
                const size_t glOffset = shadowOffset - glStartOffset;
                const size_t size = std::min(glStartOffset + mappedSize - shadowOffset, sPageSize * pages);

                memcpy(glMemory + glOffset, shadowSlice + shadowOffset, size);
                callback(shadowSlice + shadowOffset, size);
            } else {
                const size_t size = std::min(sPageSize * pages - glStartOffset, mappedSize);

                memcpy(glMemory, shadowSlice + glStartOffset, size);
                callback(shadowSlice + glStartOffset, size);
            }
        }
    }

    std::fill(dirtyPages.begin(), dirtyPages.end(), 0);
    isDirty = false;
    pagesToDirtyOnConsecutiveWrites = 1;
    lastDirtiedRelativePage = UINT32_MAX - 1;
}

void GLMemoryShadow::updateForReads()
{
    uint8_t *protectStart = shadowMemory + mappedStartPage * sPageSize;
    const size_t protectSize = (mappedEndPage - mappedStartPage) * sPageSize;

    memProtect(protectStart, protectSize, MemProtection::READ_WRITE);

    memcpy(shadowMemory + mappedStart, glMemory, mappedSize);

    memProtect(protectStart, protectSize, MemProtection::READ_ONLY);
}

void GLMemoryShadow::commitAllWrites(gltrace::Context *_ctx, Callback callback)
{
    if (!_ctx->sharedRes->dirtyShadows.empty()) {
        os::unique_lock<os::mutex> lock(mutex);

        for (GLMemoryShadow *memoryShadow : _ctx->sharedRes->dirtyShadows) {
            memoryShadow->commitWrites(callback);
        }

        _ctx->sharedRes->dirtyShadows.clear();
    }
}

void GLMemoryShadow::syncAllForReads(gltrace::Context *_ctx)
{
    if (!_ctx->sharedRes->bufferToShadowMemory.empty()) {
        os::unique_lock<os::mutex> lock(mutex);

        for (auto& it : _ctx->sharedRes->bufferToShadowMemory) {
            GLMemoryShadow* memoryShadow = it.second.get();
            if (memoryShadow->getMapFlags() & GL_MAP_READ_BIT) {
                memoryShadow->updateForReads();
            }
        }
    }
}
