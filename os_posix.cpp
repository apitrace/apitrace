/**************************************************************************
 *
 * Copyright 2010-2011 VMware, Inc.
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


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include "os.hpp"


namespace OS {


static pthread_mutex_t 
mutex = PTHREAD_MUTEX_INITIALIZER;


void
AcquireMutex(void)
{
    pthread_mutex_lock(&mutex);
}


void
ReleaseMutex(void)
{
    pthread_mutex_unlock(&mutex);
}


bool
GetProcessName(char *str, size_t size)
{
    char szProcessPath[PATH_MAX + 1];
    char *lpProcessName;

    // http://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe
#ifdef __APPLE__
    uint32_t len = sizeof szProcessPath;
    if (_NSGetExecutablePath(szProcessPath, &len) != 0) {
        *str = 0;
        return false;
    }
#else
    ssize_t len;
    len = readlink("/proc/self/exe", szProcessPath, sizeof(szProcessPath) - 1);
    if (len == -1) {
        snprintf(str, size, "%i", (int)getpid());
        return true;
    }
#endif
    szProcessPath[len] = 0;

    lpProcessName = strrchr(szProcessPath, '/');
    lpProcessName = lpProcessName ? lpProcessName + 1 : szProcessPath;

    strncpy(str, lpProcessName, size);
    if (size)
        str[size - 1] = 0;

    return true;
}

bool
GetCurrentDir(char *str, size_t size)
{
    char *ret;
    ret = getcwd(str, size);
    str[size - 1] = 0;
    return ret ? true : false;
}

void
DebugMessage(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    fflush(stdout);
    vfprintf(stderr, format, ap);
    va_end(ap);
}

long long GetTime(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_usec + tv.tv_sec*1000000LL;
}

void
Abort(void)
{
    exit(0);
}


class MapReader
{
protected:
    int fd;
    char buf[512];
    int pos;
    int len;
    int lookahead;
    void *brk;

public:
    MapReader() {
        fd = open("/proc/self/maps", O_RDONLY);
        pos = 0;
        len = 0;
        lookahead = 0;
    }

    ~MapReader() {
        close(fd);
    }

    bool lookup(uintptr_t address, MemoryInfo *info) {
        if (lseek(fd, 0, SEEK_SET)) {
            return false;
        }

        slurp();
        
        brk = sbrk(0);

        while (lookahead) {
            if (read_entry(address, info)) {
                return true;
            }
        }

        return false;
    }

    bool read_entry(uintptr_t address, MemoryInfo *info) {
        uintptr_t start, stop;
        read_address(start, stop);
#if 0
        read_perms();
        read_offset();
        read_dev();
        read_inode();
        read_pathname();
#else
        while (lookahead != '\n') {
            consume();
        }
        consume(); // '\n'
#endif
        //DebugMessage("%08lx-%08lx\n", (unsigned long)start, (unsigned long)stop);

        if (start <= address && address < stop) {
            info->start = (const void *)start;
            info->stop  = (const void *)stop;

            //DebugMessage("   sbrk = %08lx\n", (unsigned long)brk);

            return true;
        }

        return false;
    }
    
    inline void read_address(uintptr_t &start, uintptr_t &stop) {
        start = read_hex();
        consume(); // '-'
        stop = read_hex();
    }

    uintptr_t read_hex() {
        uintptr_t val = 0;
        while (true) {
            if (lookahead >= '0' && lookahead <= '9') {
                val <<= 4;
                val |= lookahead - '0';
            } else if (lookahead >= 'A' && lookahead <= 'F') {
                val <<= 4;
                val |= lookahead - 'A' + 10;
            } else if (lookahead >= 'a' && lookahead <= 'f') {
                val <<= 4;
                val |= lookahead - 'a' + 10;
            } else {
                break;
            }
            consume();
        }
        return val;
    }

    inline void consume() {
        ++pos;
        if (pos < len) {
            lookahead = buf[pos];
        } else {
            slurp();
        }
    }

    void slurp(void) {
        pos = 0;
        ssize_t nread = read(fd, buf, sizeof buf);
        if (nread > 0) {
            len = nread;
            lookahead = buf[0];
        } else {
            lookahead = 0;
        }
    }
};



bool queryVirtualAddress(const void *address, MemoryInfo *info)
{
    MapReader reader;

    return reader.lookup((uintptr_t)address, info); 
}


} /* namespace OS */

