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
 * Top-level application for accessing almost all of apitrace
 * functionality.
 */

#include <string.h>

#include <iomanip>
#include <iostream>

#include "cli.hpp"

#define ARRAY_SIZE(arr) (sizeof (arr) / sizeof (arr[0]))

static const char *help_synopsis = "Print detailed help for the given command.";

static void list_commands(void);

static void
help_usage()
{
    std::cout
        << "usage: apitrace help [<command>]\n"
        << help_synopsis << "\n"
        "\n";

    list_commands();
}

static int
do_help_command(int argc, char *argv[]);

const Command help_command = {
    "help",
    help_synopsis,
    help_usage,
    do_help_command
};

static const Command * commands[] = {
    &diff_command,
    &diff_state_command,
    &diff_images_command,
    &dump_command,
    &dump_images_command,
    &leaks_command,
    &pickle_command,
    &sed_command,
    &repack_command,
    &retrace_command,
    &trace_command,
    &trim_command,
    &trim_auto_command,
    &help_command
};

/* Aliases provide a mechanism to allow compatibility with old command
 * names (such as "retrace") for current commands (such as the replay
 * command). */
typedef struct {
    const char *name;
    const Command *command;
} Alias;

static const Alias aliases[] = {
    { "retrace", &retrace_command }
};

static void
usage(void)
{
    std::cout <<
        "Usage: apitrace <command> [<args> ...]\n"
        "Top-level command line frontend to apitrace.\n"
        "\n";

    list_commands();
}

static void
list_commands(void) {
    const Command *command;
    int i, max_width = 0;

    std::cout << "The available commands are as follows:\n\n";

    std::cout << std::setiosflags(std::ios::left);

    for (i = 0; i < ARRAY_SIZE(commands); i++) {
        command = commands[i];
        if (strlen(command->name) > max_width) {
            max_width = strlen(command->name);
        }
    }

    for (i = 0; i < ARRAY_SIZE(commands); i++) {
        command = commands[i];

        std::cout << "    " << std::setw(max_width + 2) << command->name
                  << " " << command->synopsis << "\n";
    }

    std::cout << "\n"
        "Use \"apitrace help <command>\" for more details on each command.\n";
}


static int
do_help_command(int argc, char *argv[])
{
    const Command *command;
    int i;

    if (argc != 2) {
        help_usage();
        return 0;
    }

    char *command_name = argv[1];

    for (i = 0; i < ARRAY_SIZE(commands); i++) {
        command = commands[i];

        if (strcmp(command_name, command->name) == 0) {
            (command->usage) ();
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
    const char *command_name;
    const Command *command;
    const Alias *alias;
    int i;

    for (i = 1; i < argc; ++i) {
        const char *arg = argv[i];

        if (arg[0] != '-') {
            break;
        }

        if (strcmp(arg, "--help") == 0) {
            return do_help_command(0, NULL);
        } else {
            std::cerr << "Error: unknown option " << arg << "\n";
            usage();
            return 1;
        }
    }

    if (i == argc) {
        usage();
        return 1;
    }

    command_name = argv[i];

    argc -= i;
    argv = &argv[i];

    for (i = 0; i < ARRAY_SIZE(commands); i++) {
        command = commands[i];

        if (strcmp(command_name, command->name) == 0)
            return (command->function) (argc, argv);
    }

    for (i = 0; i < ARRAY_SIZE(aliases); i++) {
        alias = &aliases[i];

        if (strcmp(command_name, alias->name) == 0)
            return (alias->command->function) (argc, argv);
    }

    std::cerr << "Error: unknown command " << command_name
              << " (see \"apitrace help\").\n";

   return 1;
}
