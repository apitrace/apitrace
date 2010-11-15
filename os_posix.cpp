/**************************************************************************
 *
 * Copyright 2010 VMware, Inc.
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
#include <unistd.h>
#include <pthread.h>

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
    if (readlink("/proc/self/exe", szProcessPath, sizeof(szProcessPath) - 1) == -1) {
        *str = 0;
        return false;
    }

    lpProcessName = strrchr(szProcessPath, '/');
    lpProcessName = lpProcessName ? lpProcessName + 1 : szProcessPath;
    
    strncpy(str, lpProcessName, size);

    return true;
}


} /* namespace OS */
