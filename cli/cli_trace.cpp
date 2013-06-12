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


#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <iostream>

#include "os_string.hpp"
#include "os_process.hpp"

#include "cli.hpp"
#include "cli_resources.hpp"


#if defined(__APPLE__)
#define TRACE_VARIABLE "DYLD_LIBRARY_PATH"
#define GL_TRACE_WRAPPER  "OpenGL"
#elif defined(_WIN32)
#define GL_TRACE_WRAPPER  "opengl32.dll"
#else
#define TRACE_VARIABLE "LD_PRELOAD"
#define GL_TRACE_WRAPPER  "glxtrace.so"
#define EGL_TRACE_WRAPPER  "egltrace.so"
#endif


static inline bool
copyWrapper(const os::String & wrapperPath,
            const char *programPath,
            bool verbose)
{
    os::String wrapperFilename(wrapperPath);
    wrapperFilename.trimDirectory();

    os::String tmpWrapper(programPath);
    tmpWrapper.trimFilename();
    tmpWrapper.join(wrapperFilename);

    if (verbose) {
        std::cerr << wrapperPath << " -> " << tmpWrapper << "\n";
    }

    if (tmpWrapper.exists()) {
        std::cerr << "error: not overwriting " << tmpWrapper << "\n";
        return false;
    }

    if (!os::copyFile(wrapperPath, tmpWrapper, false)) {
        std::cerr << "error: failed to copy " << wrapperPath << " into " << tmpWrapper << "\n";
        return false;
    }

    return true;
}


static int
traceProgram(trace::API api,
             char * const *argv,
             const char *output,
             bool verbose)
{
    const char *wrapperFilename;
    std::vector<const char *> args;
    int status = 1;

    /*
     * TODO: simplify code
     */

    bool useInject = false;
    switch (api) {
    case trace::API_GL:
        wrapperFilename = GL_TRACE_WRAPPER;
        break;
#ifdef EGL_TRACE_WRAPPER
    case trace::API_EGL:
        wrapperFilename = EGL_TRACE_WRAPPER;
        break;
#endif
#ifdef _WIN32
    case trace::API_D3D7:
        wrapperFilename = "ddraw.dll";
        break;
    case trace::API_D3D8:
        wrapperFilename = "d3d8.dll";
        break;
    case trace::API_D3D9:
        wrapperFilename = "d3d9.dll";
        break;
    case trace::API_DXGI:
        wrapperFilename = "dxgitrace.dll";
        useInject = true;
        break;
#endif
    default:
        std::cerr << "error: unsupported API\n";
        return 1;
    }

    os::String wrapperPath = findWrapper(wrapperFilename, verbose);
    if (!wrapperPath.length()) {
        std::cerr << "error: failed to find " << wrapperFilename << " wrapper\n";
        goto exit;
    }

#if defined(_WIN32)
    useInject = true;
    if (useInject) {
        args.push_back("inject");
        args.push_back(wrapperPath);
    } else {
        /* On Windows copy the wrapper to the program directory.
         */
        if (!copyWrapper(wrapperPath, argv[0], verbose)) {
            goto exit;
        }
    }
#else  /* !_WIN32 */
    (void)useInject;
#endif /* !_WIN32 */

#if defined(__APPLE__)
    /* On Mac OS X, using DYLD_LIBRARY_PATH, we actually set the
     * directory, not the file. */
    wrapperPath.trimFilename();
#endif

    /*
     * Spawn child process.
     */

    {
#if defined(TRACE_VARIABLE)
        const char *oldEnvVarValue = getenv(TRACE_VARIABLE);
        if (oldEnvVarValue) {
            wrapperPath.append(OS_PATH_SEP);
            wrapperPath.append(oldEnvVarValue);
        }

        /* FIXME: Don't modify our (ie parent) environment */
        os::setEnvironment(TRACE_VARIABLE, wrapperPath.str());

        if (verbose) {
            std::cerr << TRACE_VARIABLE << "=" << wrapperPath.str() << "\n";
        }
#endif /* TRACE_VARIABLE */

        if (output) {
            os::setEnvironment("TRACE_FILE", output);
        }

        for (char * const * arg = argv; *arg; ++arg) {
            args.push_back(*arg);
        }

        if (verbose) {
            const char *sep = "";
            for (unsigned i = 0; i < args.size(); ++i) {
                std::cerr << sep << args[i];
                sep = " ";
            }
            std::cerr << "\n";
        }

        args.push_back(NULL);

        status = os::execute((char * const *)&args[0]);

#if defined(TRACE_VARIABLE)
        if (oldEnvVarValue) {
            os::setEnvironment(TRACE_VARIABLE, oldEnvVarValue);
        } else {
            os::unsetEnvironment(TRACE_VARIABLE);
        }
#endif
    }

exit:
#if defined(_WIN32)
    if (!useInject) {
        os::String tmpWrapper(argv[0]);
        tmpWrapper.trimFilename();
        tmpWrapper.join(wrapperFilename);
        os::removeFile(tmpWrapper);
    }
#endif

    if (output) {
        os::unsetEnvironment("TRACE_FILE");
    }
    
    return status;

}


static const char *synopsis = "Generate a new trace by executing the given program.";

static void
usage(void)
{
    std::cout << "usage: apitrace trace [OPTIONS] PROGRAM [ARGS ...]\n"
        << synopsis << "\n"
        "\n"
        "    The given program will be executed with the given arguments.\n"
        "    During execution, all OpenGL calls will be captured to a trace\n"
        "    file. That trace file can then be used\n"
        "    with other apitrace utilities for replay or analysis.\n"
        "\n"
        "    -v, --verbose       verbose output\n"
        "    -a, --api=API       specify API to trace ("
#ifdef _WIN32
                                                      "gl, d3d7, d3d8, d3d9, or dxgi (for d3d10 and higher) "
#else
                                                      "gl or egl"
#endif
                                                      ");\n"
        "                        default is `gl`\n"
        "    -o, --output=TRACE  specify output trace file;\n"
        "                        default is `PROGRAM.trace`\n";
}

const static char *
shortOptions = "+hva:o:";

const static struct option
longOptions[] = {
    {"help", no_argument, 0, 'h'},
    {"verbose", no_argument, 0, 'v'},
    {"api", required_argument, 0, 'a'},
    {"output", required_argument, 0, 'o'},
    {0, 0, 0, 0}
};

static int
command(int argc, char *argv[])
{
    bool verbose = false;
    trace::API api = trace::API_GL;
    const char *output = NULL;

    int opt;
    while ((opt = getopt_long(argc, argv, shortOptions, longOptions, NULL)) != -1) {
        switch (opt) {
        case 'h':
            usage();
            return 0;
        case 'v':
            verbose = true;
            break;
        case 'a':
            if (strcmp(optarg, "gl") == 0) {
                api = trace::API_GL;
            } else if (strcmp(optarg, "egl") == 0) {
                api = trace::API_EGL;
            } else if (strcmp(optarg, "d3d7") == 0) {
                api = trace::API_D3D7;
            } else if (strcmp(optarg, "d3d8") == 0) {
                api = trace::API_D3D8;
            } else if (strcmp(optarg, "d3d9") == 0) {
                api = trace::API_D3D9;
            } else if (strcmp(optarg, "dxgi") == 0 ||
                       strcmp(optarg, "d3d10") == 0 ||
                       strcmp(optarg, "d3d10_1") == 0 ||
                       strcmp(optarg, "d3d11") == 0 ||
                       strcmp(optarg, "d3d11_1") == 0) {
                api = trace::API_DXGI;
            } else {
                std::cerr << "error: unknown API `" << optarg << "`\n";
                usage();
                return 1;
            }
            break;
        case 'o':
            output = optarg;
            break;
        default:
            std::cerr << "error: unexpected option `" << opt << "`\n";
            usage();
            return 1;
        }
    }

    if (optind == argc) {
        std::cerr << "error: no command specified\n";
        usage();
        return 1;
    }

    assert(argv[argc] == 0);
    return traceProgram(api, argv + optind, output, verbose);
}

const Command trace_command = {
    "trace",
    synopsis,
    usage,
    command
};
