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


#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include "os.hpp"


namespace os {


static pthread_mutex_t 
mutex = PTHREAD_MUTEX_INITIALIZER;


void
acquireMutex(void)
{
    pthread_mutex_lock(&mutex);
}


void
releaseMutex(void)
{
    pthread_mutex_unlock(&mutex);
}


Path
getProcessName(void)
{
    Path path;

    char *szProcessPath = path.buf(PATH_MAX);

    // http://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe
#ifdef __APPLE__
    uint32_t len = PATH_MAX;
    if (_NSGetExecutablePath(szProcessPath, &len) != 0) {
        *szProcessPath = 0;
        return path;
    }
#else
    ssize_t len;
    len = readlink("/proc/self/exe", szProcessPath, PATH_MAX - 1);
    if (len == -1) {
        // /proc/self/exe is not available on setuid processes, so fallback to
        // /proc/self/cmdline.
        int fd = open("/proc/self/cmdline", O_RDONLY);
        if (fd >= 0) {
            len = read(fd, szProcessPath, PATH_MAX - 1);
            close(fd);
        }
    }
    if (len <= 0) {
        snprintf(szProcessPath, PATH_MAX, "%i", (int)getpid());
        return path;
    }
#endif
    path.truncate(len);

    return path;
}

Path
getCurrentDir(void)
{
    Path path;
    size_t size = PATH_MAX;
    char *str = path.buf(size);
    getcwd(str, size);
    str[size - 1] = 0;
    path.truncate();
    return path;
}

void
log(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    fflush(stdout);
    vfprintf(stderr, format, ap);
    va_end(ap);
}

long long
getTime(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_usec + tv.tv_sec*1000000LL;
}

void
abort(void)
{
    exit(0);
}


static void (*gCallback)(void) = NULL;

#define NUM_SIGNALS 16

struct sigaction old_actions[NUM_SIGNALS];


/*
 * See also:
 * - http://sourceware.org/git/?p=glibc.git;a=blob;f=debug/segfault.c
 * - http://ggi.cvs.sourceforge.net/viewvc/ggi/ggi-core/libgg/gg/cleanup.c?view=markup
 */
static void
signalHandler(int sig, siginfo_t *info, void *context)
{
    static int recursion_count = 0;

    fprintf(stderr, "apitrace: warning: caught signal %i\n", sig);

    if (recursion_count) {
        fprintf(stderr, "apitrace: warning: recursion handling signal %i\n", sig);
    } else {
        if (gCallback) {
            ++recursion_count;
            gCallback();
            --recursion_count;
        }
    }

    struct sigaction *old_action;
    if (sig >= NUM_SIGNALS) {
        /* This should never happen */
        fprintf(stderr, "error: unexpected signal %i\n", sig);
        raise(SIGKILL);
    }
    old_action = &old_actions[sig];

    if (old_action->sa_flags & SA_SIGINFO) {
        // Handler is in sa_sigaction
        old_action->sa_sigaction(sig, info, context);
    } else {
        if (old_action->sa_handler == SIG_DFL) {
            fprintf(stderr, "apitrace: info: taking default action for signal %i\n", sig);

#if 1
            struct sigaction dfl_action;
            dfl_action.sa_handler = SIG_DFL;
            sigemptyset (&dfl_action.sa_mask);
            dfl_action.sa_flags = 0;
            sigaction(sig, &dfl_action, NULL);

            raise(sig);
#else
            raise(SIGKILL);
#endif
        } else if (old_action->sa_handler == SIG_IGN) {
            /* ignore */
        } else {
            /* dispatch to handler */
            old_action->sa_handler(sig);
        }
    }
}

void
setExceptionCallback(void (*callback)(void))
{
    assert(!gCallback);
    if (!gCallback) {
        gCallback = callback;

        struct sigaction new_action;
        new_action.sa_sigaction = signalHandler;
        sigemptyset(&new_action.sa_mask);
        new_action.sa_flags = SA_SIGINFO | SA_RESTART;


        for (int sig = 1; sig < NUM_SIGNALS; ++sig) {
            // SIGKILL and SIGSTOP can't be handled
            if (sig != SIGKILL && sig != SIGSTOP) {
                if (sigaction(sig,  NULL, &old_actions[sig]) >= 0) {
                    sigaction(sig,  &new_action, NULL);
                }
            }
        }
    }
}

void
resetExceptionCallback(void)
{
    gCallback = NULL;
}

} /* namespace os */

