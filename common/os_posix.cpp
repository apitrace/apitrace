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
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#if defined(__linux__)
#include <linux/limits.h> // PATH_MAX
#endif

#ifdef __APPLE__
#include <sys/syslimits.h> // PATH_MAX
#include <mach-o/dyld.h>
#endif

#ifdef ANDROID
#include <android/log.h>
#endif

#ifndef PATH_MAX
#warning PATH_MAX undefined
#define PATH_MAX 4096
#endif

#include "os.hpp"
#include "os_string.hpp"


namespace os {


String
getProcessName(void)
{
    String path;
    size_t size = PATH_MAX;
    char *buf = path.buf(size);

    // http://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe
#ifdef __APPLE__
    uint32_t len = size;
    if (_NSGetExecutablePath(buf, &len) != 0) {
        *buf = 0;
        return path;
    }
    len = strlen(buf);
#else
    ssize_t len;
    len = readlink("/proc/self/exe", buf, size - 1);
    if (len == -1) {
        // /proc/self/exe is not available on setuid processes, so fallback to
        // /proc/self/cmdline.
        int fd = open("/proc/self/cmdline", O_RDONLY);
        if (fd >= 0) {
            len = read(fd, buf, size - 1);
            close(fd);
        }
    }
    if (len <= 0) {
        snprintf(buf, size, "%i", (int)getpid());
        return path;
    }
#endif
    path.truncate(len);

    return path;
}

String
getCurrentDir(void)
{
    String path;
    size_t size = PATH_MAX;
    char *buf = path.buf(size);

    getcwd(buf, size);
    buf[size - 1] = 0;
    
    path.truncate();
    return path;
}

bool
String::exists(void) const
{
    struct stat st;
    int err;

    err = stat(str(), &st);
    if (err) {
        return false;
    }

    if (!S_ISREG(st.st_mode))
        return false;

    return true;
}

int execute(char * const * args)
{
    pid_t pid = fork();
    if (pid == 0) {
        // child
        execvp(args[0], args);
        fprintf(stderr, "error: failed to execute %s\n", args[0]);
        exit(-1);
    } else {
        // parent
        if (pid == -1) {
            fprintf(stderr, "error: failed to fork\n");
            return -1;
        }
        int status = -1;
        waitpid(pid, &status, 0);
        return status;
    }
}

static volatile bool logging = false;

void
log(const char *format, ...)
{
    logging = true;
    va_list ap;
    va_start(ap, format);
    fflush(stdout);
#ifdef ANDROID
    __android_log_vprint(ANDROID_LOG_DEBUG, "apitrace", format, ap);
#else
    vfprintf(stderr, format, ap);
#endif
    va_end(ap);
    logging = false;
}

#if defined(__APPLE__)
long long timeFrequency = 0LL;
#endif

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
    /*
     * There are several signals that can happen when logging to stdout/stderr.
     * For example, SIGPIPE will be emitted if stderr is a pipe with no
     * readers.  Therefore ignore any signal while logging by returning
     * immediately, to prevent deadlocks.
     */
    if (logging) {
        return;
    }

    static int recursion_count = 0;

    log("apitrace: warning: caught signal %i\n", sig);

    if (recursion_count) {
        log("apitrace: warning: recursion handling signal %i\n", sig);
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
        log("error: unexpected signal %i\n", sig);
        raise(SIGKILL);
    }
    old_action = &old_actions[sig];

    if (old_action->sa_flags & SA_SIGINFO) {
        // Handler is in sa_sigaction
        old_action->sa_sigaction(sig, info, context);
    } else {
        if (old_action->sa_handler == SIG_DFL) {
            log("apitrace: info: taking default action for signal %i\n", sig);

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
            // SIGKILL and SIGSTOP can't be handled.
            if (sig == SIGKILL || sig == SIGSTOP) {
                continue;
            }

            /*
             * SIGPIPE can be emitted when writing to stderr that is redirected
             * to a pipe without readers.  It is also very unlikely to ocurr
             * inside graphics APIs, and most applications where it can occur
             * normally already ignore it.  In summary, it is unlikely that a
             * SIGPIPE will cause abnormal termination, which it is likely that
             * intercepting here will cause problems, so simple don't intercept
             * it here.
             */
            if (sig == SIGPIPE) {
                continue;
            }

            if (sigaction(sig,  NULL, &old_actions[sig]) >= 0) {
                sigaction(sig,  &new_action, NULL);
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

