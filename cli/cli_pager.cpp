/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
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


/*
 * Pager abstraction.
 */


#include "cli.hpp"


#if defined(_WIN32)


void
pipepager(void) {
    /* no-op */
}


#else /* !defined(_WIN32) */


#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>


static pid_t pid = -1;


/*
 * Wait for pager process on exit.
 */
static void
on_exit(void)
{
    fflush(stdout);
    fflush(stderr);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    waitpid(pid, NULL, 0);
}


/*
 * Wait for page process on signal.
 */
static void
on_signal(int sig)
{
    fprintf(stderr, "on_signal\n");
    on_exit();
    signal(sig, SIG_DFL); 
    raise(sig);
}


/*
 * Spawn a pager process and pipe standard output into it.
 */
void
pipepager(void) {
    if (!isatty(STDOUT_FILENO)) {
        return;
    }

    union {
        int pipe[2];
        struct {
            int read;
            int write;
        };
    } fd;

    int ret;
    const char *pager;

    ret = pipe(fd.pipe);
    assert(ret == 0);
    if (ret != 0) {
        return;
    }

    pid = fork();
    switch (pid) {
    case -1:
        // failed to fork
        close(fd.read);
        close(fd.write);
        return;

    case 0:
        // child
        close(fd.write);

        dup2(fd.read, STDIN_FILENO);

        pager = getenv("PAGER");
        if (!pager) {
            pager = "less";
        }

        if (!getenv("PAGER")) {
            putenv((char *)"LESS=FRXn");
        }

        execlp(pager, pager, NULL);

        // This line should never be reached
        abort();

    default:
        // parent
        close(fd.read);

        dup2(fd.write, STDOUT_FILENO);
        if (isatty(STDERR_FILENO)) {
            dup2(fd.write, STDERR_FILENO);
        }
        close(fd.write);

        // Ensure we wait for the pager before terminating
        signal(SIGINT, on_signal);
        signal(SIGHUP, on_signal);
        signal(SIGTERM, on_signal);
        signal(SIGQUIT, on_signal);
        signal(SIGPIPE, on_signal);
        atexit(on_exit);
    }
}


#endif /* !defined(_WIN32) */
