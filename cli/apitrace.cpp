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

#include "apitrace_cli.hpp"

#define ARRAY_SIZE(arr) (sizeof (arr) / sizeof (arr[0]))

typedef void (*command_usage_t) (const char *argv0);
typedef int (*command_function_t) (int argc, char *argv[], int first_command_arg);

typedef struct {
    const char *name;
    const char *synopsis;
    command_usage_t usage;
    command_function_t function;
} Command;

#define APITRACE_HELP_SYNOPSIS "Print detailed help for the given command."

static void
apitrace_help_usage(const char *argv0)
{
    std::cout << argv0 << " [<command>]"
        "\n\n\t"
        APITRACE_HELP_SYNOPSIS
        "\n\n\t"
        "Except in this case, where this is all the help you will get."
        "\n\n";
}

static int
apitrace_help_command(int argc, char *argv[], int first_command_arg);

static Command commands[] = {
    { "dump",
      APITRACE_DUMP_SYNOPSIS,
      apitrace_dump_usage,
      apitrace_dump_command },
    { "help",
      APITRACE_HELP_SYNOPSIS,
      apitrace_help_usage,
      apitrace_help_command }
};

void
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
                  << " " << command->synopsis << "\n";
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
            (command->usage) ("apitrace");

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
