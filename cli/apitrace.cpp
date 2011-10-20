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


/*
 * Top-level application for accessing most all apitrace
 * functionality.
 */

#include <iostream>
#include <iomanip>

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "config.h"

#define ARRAY_SIZE(arr) (sizeof (arr) / sizeof (arr[0]))

typedef int (*command_function_t) (int argc, char *argv[], int first_command_arg);

typedef struct {
    const char *name;
    command_function_t function;
    const char *arguments;
    const char *summary;
    const char *documentation;
} Command;

static int
apitrace_help_command(int argc, char *argv[], int first_command_arg);

static Command commands[] = {
    { "help", apitrace_help_command,
      "[<command>]",
      "Print detailed help for the given command.",
      "\tExcept in this case, where this is all the help you will get." }
};

static void
usage(void)
{
    Command *command;
    int i, max_width = 0;

    std::cout <<
        "Usage: apitrace <command> [<args> ...]\n\n"
        "The available commands are as follows:\n\n";

    std::cout << std::setiosflags(std::ios::left);

    for (i = 0; i < ARRAY_SIZE(commands); i++) {
        command = &commands[i];
        if (strlen(command->name) > max_width)
            max_width = strlen(command->name);
    }

    for (i = 0; i < ARRAY_SIZE(commands); i++) {
        command = &commands[i];

        std::cout << " " << std::setw(max_width+2) << command->name
                  << " " << command->summary << "\n";
    }

    std::cout << "\n"
        "Use \"apitrace help <command>\" for more details on each command.\n";
}

static int
apitrace_help_command(int argc, char *argv[], int first_arg_command)
{
    Command *command;
    int i;

    if(first_arg_command == argc) {
        usage();
        return 0;
    }

    char *command_name = argv[first_arg_command];

    for (i = 0; i < ARRAY_SIZE(commands); i++) {
        command = &commands[i];

        if (strcmp(command_name, command->name) == 0) {
            std::cout << "Help for \"apitrace " << command_name << "\":\n\n";
            std::cout << command->name;
            if (command->arguments)
                std::cout << " " << command->arguments
                          << "\n\n\t" << command->summary;
            else
                std::cout << "\t" << command->summary;
            std::cout << "\n\n" << command->documentation << "\n\n";

            return 0;
        }
    }

    std::cerr << "Error: Unknown command: " << command_name
              << " (see \"apitrace help\").\n";

    return 1;
}

int
main(int argc, char **argv)
{
    const char *command_name = "trace";
    Command *command;
    int i, first_command_arg;

    if (argc == 1) {
        usage();
        return 1;
    }

    for (i = 1; i < argc; ++i) {
        const char *arg = argv[i];

        if (arg[0] != '-') {
            break;
        }

        if (strcmp(arg, "--help") == 0) {
            return apitrace_help_command (0, NULL, 0);
        } else {
            std::cerr << "Error: unknown option " << arg << "\n";
            usage();
            return 1;
        }
    }
    first_command_arg = i;

    if (first_command_arg < argc) {
        command_name = argv[first_command_arg];
        first_command_arg++;
    }

    for (i = 0; i < ARRAY_SIZE(commands); i++) {
        command = &commands[i];

        if (strcmp(command_name, command->name) == 0)
            return (command->function) (argc, argv, first_command_arg);
    }

    std::cerr << "Error: unknown command " << command_name
              << " (see \"apitrace help\").\n";

    return 1;
}
