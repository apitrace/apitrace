/**************************************************************************
 *
 * Copyright 2011-2014 Jose Fonseca
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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

#include <GLFW/glfw3.h>


int execute(char * const * args)
{
#ifdef _WIN32
    return _spawnv(_P_WAIT, args[0], (const char *const *)args);
#else
    pid_t pid = fork();
    if (pid == 0) {
        // child
        execv(args[0], args);
        exit(-1);
    } else {
        // parent
        if (pid == -1) {
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
#endif
}

int
main(int argc, char **argv)
{
    GLboolean child = GL_FALSE;
    int i;

    for (i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        if (strcmp(arg, "-child") == 0) {
            child = GL_TRUE;
        } else {
            fprintf(stderr, "error: unknown arg %s\n", arg);
            exit(1);
        }
    }

    fprintf(stdout, "%s\n", child ? "child" : "parent");
    fflush(stdout);

    if (child) {
        GLFWwindow* window = NULL;

        glfwInit();

        glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

        window = glfwCreateWindow(250, 250, argv[0], NULL, NULL);
        if (!window) {
             return EXIT_SKIP;
        }

        glfwMakeContextCurrent(window);

        glFlush();

        glfwDestroyWindow(window);
        glfwTerminate();

        return 0;
    } else {
        const char *args[3];

        args[0] = argv[0];
        args[1] = "-child";
        args[2] = NULL;
        return execute((char * const *)args);
    }
}
