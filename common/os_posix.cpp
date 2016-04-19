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

#ifndef _WIN32

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
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

#ifdef __QNXNTO__
#define SA_RESTART 0 // QNX does not have SA_RESTART
#endif

#ifndef __has_feature
#  define __has_feature(x) 0
#endif
#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
#  include <sanitizer/asan_interface.h>
#endif

#include "os.hpp"
#include "os_string.hpp"
#include "os_backtrace.hpp"


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
        // grow buf and retry
        buf = path.buf(len);
        _NSGetExecutablePath(buf, &len);
    }
    len = strlen(buf);
#else
    ssize_t len;

#ifdef ANDROID
    // On Android, we are almost always interested in the actual process title
    // rather than path to the VM kick-off executable
    // (/system/bin/app_process).
    len = 0;
#else
    len = readlink("/proc/self/exe", buf, size - 1);
#endif

    if (len <= 0) {
        // /proc/self/exe is not available on setuid processes, so fallback to
        // /proc/self/cmdline.
        int fd = open("/proc/self/cmdline", O_RDONLY);
        if (fd >= 0) {
            // buf already includes trailing zero
            len = read(fd, buf, size);
            close(fd);
            if (len >= 0) {
                len = strlen(buf);
            }
        }
    }

#ifdef __GLIBC__
    // fallback to `program_invocation_name`
    if (len <= 0) {
        len = strlen(program_invocation_name);
        buf = path.buf(len + 1);
        strcpy(buf, program_invocation_name);
    }
#endif

    // fallback to process ID
    if (len <= 0) {
        len = snprintf(buf, size, "%i", (int)getpid());
        if (len >= size) {
            len = size - 1;
        }
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

    if (getcwd(buf, size)) {
        buf[size - 1] = 0;
        path.truncate();
    } else {
        path.truncate(0);
    }
    
    return path;
}

String
getConfigDir(void)
{
    String path;

#ifdef __APPLE__
    // Library/Preferences
    const char *homeDir = getenv("HOME");
    assert(homeDir);
    if (homeDir) {
        path = homeDir;
        path.join("Library/Preferences");
    }
#else
    // http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
    const char *configHomeDir = getenv("XDG_CONFIG_HOME");
    if (configHomeDir) {
        path = configHomeDir;
    } else {
        const char *homeDir = getenv("HOME");
        if (!homeDir) {
            struct passwd *user = getpwuid(getuid());
            if (user != NULL) {
                homeDir = user->pw_dir;
            }
        }
        assert(homeDir);
        if (homeDir) {
            path = homeDir;
#if !defined(ANDROID)
            path.join(".config");
#endif
        }
    }
#endif

    return path;
}

bool
createDirectory(const String &path)
{
    return mkdir(path, 0777) == 0;
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

    return true;
}

int execute(char * const * args)
{
    pid_t pid = fork();
    if (pid == 0) {
        // child
        execvp(args[0], args);
        fprintf(stderr, "error: failed to execute:");
        for (unsigned i = 0; args[i]; ++i) {
            fprintf(stderr, " %s", args[i]);
        }
        fprintf(stderr, "\n");
        exit(-1);
    } else {
        // parent
        if (pid == -1) {
            fprintf(stderr, "error: failed to fork\n");
            return -1;
        }
        int status = -1;
        int ret;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            ret = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            // match shell return code
            ret = WTERMSIG(status) + 128;
        } else {
            ret = 128;
        }
        return ret;
    }
}

static volatile bool logging = false;

#ifndef HAVE_EXTERNAL_OS_LOG
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
    static FILE *log = NULL;
    if (!log) {
        // Duplicate stderr file descriptor, to prevent applications from
        // redirecting our debug messages to somewhere else.
        //
        // Another alternative would be to log to /dev/tty when available.
        log = fdopen(dup(STDERR_FILENO), "at");
    }
    vfprintf(log, format, ap);
    fflush(log);
#endif
    va_end(ap);
    logging = false;
}
#endif /* !HAVE_EXTERNAL_OS_LOG */

#if defined(__APPLE__)
long long timeFrequency = 0LL;
#endif

void
abort(void)
{
    _exit(1);
}


void
breakpoint(void)
{
#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
    asm("int3");
#else
    kill(getpid(), SIGTRAP);
#endif
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
        ++recursion_count;
        if (gCallback)
            gCallback();
        os::dump_backtrace();
        --recursion_count;
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

#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
    __asan_set_death_callback(callback);
#endif
    }
}

void
resetExceptionCallback(void)
{
    gCallback = NULL;
#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
    __asan_set_death_callback(NULL);
#endif
}

#ifdef __ANDROID__
#include "os_memory.hpp"
#include <cassert>
#include <cstdio>

#include <fcntl.h>
#include <unistd.h>

char statmBuff[256];
static __uint64_t pageSize = sysconf(_SC_PAGESIZE);

static long size, resident;

static inline void parseStatm()
{
    int fd = open("/proc/self/statm", O_RDONLY, 0);
    int sz = read(fd, statmBuff, 255);
    close(fd);
    statmBuff[sz] = 0;
    sz = sscanf(statmBuff, "%ld %ld",
               &size, &resident);
    assert(sz == 2);
}

long long getVsize()
{
    parseStatm();
    return pageSize * size;
}

long long getRss()
{
    parseStatm();
    return pageSize * resident;
}
#endif

} /* namespace os */

#endif // !defined(_WIN32)
