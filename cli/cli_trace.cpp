/*********************************************************************
 *
 * Copyright 2011 Intel Corporation
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *********************************************************************/

#include <iostream>

#include "cli.hpp"

#include "os_path.hpp"


static int verbose = 1;

static const char *synopsis = "Generate a new trace by executing the given program.";

static void
usage(void)
{
    std::cout << "usage: apitrace trace <program> [<args> ...]\n"
        << synopsis << "\n"
        "\n"
        "    The given program will be executed with the given arguments.\n"
        "    During execution, all OpenGL calls will be captured to a trace\n"
        "    file named <program>.trace. That trace file can then be used\n"
        "    with other apitrace utilities for replay or analysis.\n";
}

/* We only support "apitrace trace" on POSIX-like systems (not WIN32) */
#ifndef _WIN32

#include <sys/stat.h>

#ifdef __APPLE__
#define CLI_TRACE_VARIABLE "DYLD_LIBRARY_PATH"
#define CLI_TRACE_WRAPPER  "OpenGL"
#else
#define CLI_TRACE_VARIABLE "LD_PRELOAD"
#define CLI_TRACE_WRAPPER  "glxtrace.so"
#endif

static int
file_exists(const char *path)
{
    struct stat st;
    int err;

    err = stat(path, &st);
    if (err)
        return 0;

    if (! S_ISREG(st.st_mode))
        return 0;

    return 1;
}

static os::Path
find_wrapper(const char *filename)
{
    os::Path complete;

    /* First look in the same directory from which this process is
     * running, (to support developers running a compiled program that
     * has not been installed. */
#if 1
    os::Path process_dir = os::getProcessName();

    process_dir.trimFilename();

    complete = process_dir;
    complete.join("wrappers");
    complete.join(filename);
#else
    complete = APITRACE_BINARY_DIR "/wrappers";
    complete.join(filename);
#endif

    if (file_exists(complete))
        return complete;

    /* Second, look in the directory for installed wrappers. */
    complete = APITRACE_WRAPPER_INSTALL_DIR;
    complete.join(filename);

    if (file_exists(complete))
        return complete;

    std::cerr << "error: cannot find " << filename << " (looked in " <<
        APITRACE_WRAPPER_INSTALL_DIR << ")\n";
    exit(1);

    return "";
}

static int
do_trace_posix(int argc, char *argv[])
{
    os::Path binary = find_wrapper(CLI_TRACE_WRAPPER);

    /* On Mac OS X, using DYLD_LIBRARY_PATH, we actually set the
     * directory, not the file. */
#ifdef __APPLE__
    binary.trimFilename();
#endif

    if (verbose) {
        std::cerr << CLI_TRACE_VARIABLE << "=" << binary.str() << "\n";
    }

    setenv(CLI_TRACE_VARIABLE, binary.str(), 1);

    if (verbose) {
        const char *sep = "";
        for (char **arg = argv; *arg; ++arg) {
            std::cerr << *arg << sep;
            sep = " ";
        }
        std::cerr << "\n";
    }

    execvp(argv[0], argv);

    std::cerr << "Error: Failed to execute " << argv[0] << "\n";

    return 1;
}

#endif

static int
command(int argc, char *argv[])
{

#ifdef _WIN32

    std::cerr <<
        "The 'apitrace trace' command is not supported for this operating system.\n"
        "Instead, you will need to copy opengl32.dll, d3d8.dll, or d3d9.dll from\n"
        APITRACE_WRAPPER_INSTALL_DIR "\n"
        "to the directory with the application to trace, then run the application.\n";
    return 1;

#else

    int i;

    for (i = 0; i < argc; ++i) {
        const char *arg = argv[i];

        if (arg[0] != '-') {
            break;
        }

        if (!strcmp(arg, "--")) {
            i++;
            break;
        } else if (!strcmp(arg, "--help")) {
            usage();
            return 0;
        } else {
            std::cerr << "error: unknown option " << arg << "\n";
            usage();
            return 1;
        }
    }

    if (i == argc) {
        std::cerr << "Error: Need a command name to execute (see 'apitrace trace --help')\n";
        return 1;
    }

    return do_trace_posix(argc - i, argv + i);

#endif
}

const Command trace_command = {
    "trace",
    synopsis,
    usage,
    command
};
