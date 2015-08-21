#pragma once

#include <cstddef>
#include <string>
#include <iostream>
#include <memory>
#include <list>

#ifdef __unix__

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>

#define ALLOC_CHUNK_SIZE 64 * 1024 * 1024L

/*
 * Allocator that backs up memory with mmaped file
 * File is grown by ALLOC_CHUNK_SIZE, this new region is mmaped then
 * Nothing is deallocated
*/

class MmapedFileBuffer
{
private:
    int fd;
    off_t curChunkSize;
    const size_t chunkSize;
    std::list<void*> mmaps;
    void* vptr;
    std::string fileName;

    MmapedFileBuffer(MmapedFileBuffer const&) = delete;

    void operator=(MmapedFileBuffer const&) = delete;

    void newMmap() {
        ftruncate(fd, chunkSize * (mmaps.size() + 1));
        vptr = mmap(NULL, chunkSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                    chunkSize * mmaps.size());
        mmaps.push_front(vptr);
        curChunkSize = 0;
    }

public:
    MmapedFileBuffer()
        : curChunkSize(0),
          chunkSize(ALLOC_CHUNK_SIZE & ~(sysconf(_SC_PAGE_SIZE) - 1))
    {
        char templ[] = ".pbtmpXXXXXX";
        fd = mkstemp(templ);
        fileName = templ;
        newMmap();
    }

    ~MmapedFileBuffer() {
        close(fd);
        for (auto &m : mmaps) {
            munmap(m, chunkSize);
        }
        unlink(fileName.c_str());
    }

    void* allocate(size_t size) {
        if ((curChunkSize + size) > chunkSize) {
            newMmap();
        }
        void* addr = static_cast<char*>(vptr) + curChunkSize;
        curChunkSize += size;
        return addr;
    }
};

template <class T>
class MmapAllocator
{
private:
    std::shared_ptr<MmapedFileBuffer> file;
    void* vptr;

    template<class U>
    friend class MmapAllocator;

public:
    typedef T value_type;

    MmapAllocator()
    {
        file = std::make_shared<MmapedFileBuffer>();
    };

    ~MmapAllocator() {
    };

    template <class U> MmapAllocator(const MmapAllocator<U>& other)
        : file(other.file), vptr(other.vptr) {
    };

    T* allocate(std::size_t n) {
        return reinterpret_cast<T*>(file->allocate(n * sizeof(T)));
    };

    void deallocate(T* p, std::size_t n) {};
};
template <class T, class U>
bool operator==(const MmapAllocator<T>&, const MmapAllocator<U>&) {
    return true;
}
template <class T, class U>
bool operator!=(const MmapAllocator<T>& a, const MmapAllocator<U>& b) {
    return !(a==b);
}

#else
// default allocator
template<class T>
using MmapAllocator = std::allocator<T>;

#endif
