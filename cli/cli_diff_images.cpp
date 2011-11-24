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

#include <string.h>
#include <iostream>

#include "cli.hpp"
#include "os_string.hpp"
#include "trace_tools.hpp"

static const char *synopsis = "Identify differences between two image dumps.";

static os::String
find_command(void)
{
#define CLI_DIFF_IMAGES_COMMAND "snapdiff.py"

    return trace::findFile("scripts/" CLI_DIFF_IMAGES_COMMAND,
     APITRACE_SCRIPTS_INSTALL_DIR "/" CLI_DIFF_IMAGES_COMMAND,
                           true);

}

static void
usage(void)
{
    char *args[3];

    os::String command = find_command();

    args[0] = (char *) command.str();
    args[1] = (char *) "--help";
    args[2] = NULL;

#ifdef _WIN32
    std::cerr << "The 'apitrace diff-images' command is not yet supported on this O/S.\n";
#else
    execv(command.str(), args);
#endif

    std::cerr << "Error: Failed to execute " << args[0] << "\n";
}

static int
command(int argc, char *argv[])
{
    int i;
    char **args = new char* [argc+2];

    os::String command = find_command();

    args[0] = (char *) command.str();

    for (i = 0; i < argc; i++) {
        args[i+1] = argv[i];
    }

    args[i+1] = NULL;

#ifdef _WIN32
    std::cerr << "The 'apitrace diff-images' command is not yet supported on this O/S.\n";
#else
    execv(command.str(), args);
#endif

    std::cerr << "Error: Failed to execute " << args[0] << "\n";

    return 1;
}

const Command diff_images_command = {
    "diff-images",
    synopsis,
    usage,
    command
};
