/**************************************************************************
 *
 * Copyright 2013 VMware, Inc.
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
#include <unistd.h>

#define GLX_GLXEXT_PROTOTYPES

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>
#include <GL/glxext.h>  // for glXGetProcAddressARB


int main(int argc, char **argv)
{
    const unsigned N = 100000;
    unsigned i;

    fprintf(stderr, "main\n");

    glXGetProcAddressARB((const GLubyte *)"__main");

    pid_t pid = fork();
    if (pid == 0) {
        fprintf(stderr, "child\n");
        fflush(stderr);
        for (i = 0; i < N; ++i) {
            glXGetProcAddressARB((const GLubyte *)"__child");
        }
    } else {
        // parent
        fprintf(stderr, "parent\n");
        fflush(stderr);
        if (pid == -1) {
            fprintf(stderr, "error: failed to fork\n");
            exit(-1);
        }
        for (i = 0; i < N; ++i) {
            glXGetProcAddressARB((const GLubyte *)"__parent");
        }
    }

    return 0;
}
