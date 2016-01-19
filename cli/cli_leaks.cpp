/*********************************************************************
 *
 * Copyright 2016 VMware, Inc.
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
#include "os_process.hpp"
#include "cli_resources.hpp"

static const char *synopsis = "Check trace for object leaks.";

static os::String
find_command(void)
{
    return findScript("leaks.py");
}

static void
usage(void)
{
    os::String command = find_command();

    char *args[4];
    args[0] = (char *) APITRACE_PYTHON_EXECUTABLE;
    args[1] = (char *) command.str();
    args[2] = (char *) "--help";
    args[3] = NULL;

    os::execute(args);
}

static int
command(int argc, char *argv[])
{
    int i;

    os::String command = find_command();

    os::String apitracePath = os::getProcessName();

    std::vector<const char *> args;
    args.push_back("python");
    args.push_back(command.str());
    args.push_back("--apitrace");
    args.push_back(apitracePath.str());
    for (i = 1; i < argc; i++) {
        args.push_back(argv[i]);
    }
    args.push_back(NULL);

    return os::execute((char * const *)&args[0]);
}

const Command leaks_command = {
    "leaks",
    synopsis,
    usage,
    command
};
