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


#include <string.h>

#include "image.hpp"
#include "retrace.hpp"
#include "glproc.hpp"
#include "glstate.hpp"
#include "glretrace.hpp"


namespace glretrace {

bool double_buffer = true;
bool insideGlBeginEnd = false;
Trace::Parser parser;
glws::WindowSystem *ws = NULL;
glws::Visual *visual = NULL;
glws::Drawable *drawable = NULL;
glws::Context *context = NULL;

unsigned frame = 0;
long long startTime = 0;
bool wait = false;

bool benchmark = false;
const char *compare_prefix = NULL;
const char *snapshot_prefix = NULL;
enum frequency snapshot_frequency = FREQUENCY_NEVER;

unsigned dump_state = ~0;

void
checkGlError(Trace::Call &call) {
    GLenum error = glGetError();
    if (error == GL_NO_ERROR) {
        return;
    }

    if (retrace::verbosity == 0) {
        std::cout << call;
        std::cout.flush();
    }

    std::cerr << call.no << ": ";
    std::cerr << "warning: glGetError(";
    std::cerr << call.name();
    std::cerr << ") = ";

    switch (error) {
    case GL_INVALID_ENUM:
        std::cerr << "GL_INVALID_ENUM";
        break;
    case GL_INVALID_VALUE:
        std::cerr << "GL_INVALID_VALUE";
        break;
    case GL_INVALID_OPERATION:
        std::cerr << "GL_INVALID_OPERATION";
        break;
    case GL_STACK_OVERFLOW:
        std::cerr << "GL_STACK_OVERFLOW";
        break;
    case GL_STACK_UNDERFLOW:
        std::cerr << "GL_STACK_UNDERFLOW";
        break;
    case GL_OUT_OF_MEMORY:
        std::cerr << "GL_OUT_OF_MEMORY";
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION";
        break;
    case GL_TABLE_TOO_LARGE:
        std::cerr << "GL_TABLE_TOO_LARGE";
        break;
    default:
        std::cerr << error;
        break;
    }
    std::cerr << "\n";
}


void snapshot(unsigned call_no) {
    if (!drawable ||
        (!snapshot_prefix && !compare_prefix)) {
        return;
    }

    Image::Image *ref = NULL;

    if (compare_prefix) {
        char filename[PATH_MAX];
        snprintf(filename, sizeof filename, "%s%010u.png", compare_prefix, call_no);
        ref = Image::readPNG(filename);
        if (!ref) {
            return;
        }
        if (retrace::verbosity >= 0) {
            std::cout << "Read " << filename << "\n";
        }
    }

    Image::Image *src = glstate::getDrawBufferImage(GL_RGBA);
    if (!src) {
        return;
    }

    if (snapshot_prefix) {
        char filename[PATH_MAX];
        snprintf(filename, sizeof filename, "%s%010u.png", snapshot_prefix, call_no);
        if (src->writePNG(filename) && retrace::verbosity >= 0) {
            std::cout << "Wrote " << filename << "\n";
        }
    }

    if (ref) {
        std::cout << "Snapshot " << call_no << " average precision of " << src->compare(*ref) << " bits\n";
        delete ref;
    }

    delete src;
}


void frame_complete(unsigned call_no) {
    ++frame;

    if (snapshot_frequency == FREQUENCY_FRAME ||
        snapshot_frequency == FREQUENCY_FRAMEBUFFER) {
        snapshot(call_no);
    }
}


static void display(void) {
    startTime = OS::GetTime();
    Trace::Call *call;

    while ((call = parser.parse_call())) {
        const char *name = call->name();

        if (retrace::verbosity >= 1) {
            std::cout << *call;
            std::cout.flush();
        }

        if (name[0] == 'C' && name[1] == 'G' && name[2] == 'L') {
            glretrace::retrace_call_cgl(*call);
        }
        else if (name[0] == 'w' && name[1] == 'g' && name[2] == 'l') {
            glretrace::retrace_call_wgl(*call);
        }
        else if (name[0] == 'g' && name[1] == 'l' && name[2] == 'X') {
            glretrace::retrace_call_glx(*call);
        } else {
            retrace::retrace_call(*call);
        }

        if (!insideGlBeginEnd &&
            drawable && context &&
            call->no >= dump_state) {
            glstate::dumpCurrentContext(std::cout);
            exit(0);
        }

        delete call;
    }

    // Reached the end of trace
    glFlush();

    long long endTime = OS::GetTime();
    float timeInterval = (endTime - startTime) * 1.0E-6;

    if (retrace::verbosity >= -1) { 
        std::cout << 
            "Rendered " << frame << " frames"
            " in " <<  timeInterval << " secs,"
            " average of " << (frame/timeInterval) << " fps\n";
    }

    if (wait) {
        while (ws->processEvents()) {}
    } else {
        exit(0);
    }
}


static void usage(void) {
    std::cout << 
        "Usage: glretrace [OPTION] TRACE\n"
        "Replay TRACE.\n"
        "\n"
        "  -b           benchmark mode (no error checking or warning messages)\n"
        "  -c PREFIX    compare against snapshots\n"
        "  -db          use a double buffer visual (default)\n"
        "  -sb          use a single buffer visual\n"
        "  -s PREFIX    take snapshots\n"
        "  -S FREQUENCY snapshot frequency: frame (default), framebuffer, or draw\n"
        "  -v           verbose output\n"
        "  -D CALLNO    dump state at specific call no\n"
        "  -w           wait on final frame\n";
}

extern "C"
int main(int argc, char **argv)
{

    int i;
    for (i = 1; i < argc; ++i) {
        const char *arg = argv[i];

        if (arg[0] != '-') {
            break;
        }

        if (!strcmp(arg, "--")) {
            break;
        } else if (!strcmp(arg, "-b")) {
            benchmark = true;
            retrace::verbosity = -1;
        } else if (!strcmp(arg, "-c")) {
            compare_prefix = argv[++i];
            if (snapshot_frequency == FREQUENCY_NEVER) {
                snapshot_frequency = FREQUENCY_FRAME;
            }
        } else if (!strcmp(arg, "-D")) {
            dump_state = atoi(argv[++i]);
            retrace::verbosity = -2;
        } else if (!strcmp(arg, "-db")) {
            double_buffer = true;
        } else if (!strcmp(arg, "-sb")) {
            double_buffer = false;
        } else if (!strcmp(arg, "--help")) {
            usage();
            return 0;
        } else if (!strcmp(arg, "-s")) {
            snapshot_prefix = argv[++i];
            if (snapshot_frequency == FREQUENCY_NEVER) {
                snapshot_frequency = FREQUENCY_FRAME;
            }
        } else if (!strcmp(arg, "-S")) {
            arg = argv[++i];
            if (!strcmp(arg, "frame")) {
                snapshot_frequency = FREQUENCY_FRAME;
            } else if (!strcmp(arg, "framebuffer")) {
                snapshot_frequency = FREQUENCY_FRAMEBUFFER;
            } else if (!strcmp(arg, "draw")) {
                snapshot_frequency = FREQUENCY_DRAW;
            } else {
                std::cerr << "error: unknown frequency " << arg << "\n";
                usage();
                return 1;
            }
            if (snapshot_prefix == NULL) {
                snapshot_prefix = "";
            }
        } else if (!strcmp(arg, "-v")) {
            ++retrace::verbosity;
        } else if (!strcmp(arg, "-w")) {
            wait = true;
        } else {
            std::cerr << "error: unknown option " << arg << "\n";
            usage();
            return 1;
        }
    }

    ws = glws::createNativeWindowSystem();
    visual = ws->createVisual(double_buffer);

    for ( ; i < argc; ++i) {
        if (!parser.open(argv[i])) {
            std::cerr << "error: failed to open " << argv[i] << "\n";
            return 1;
        }

        display();

        parser.close();
    }

    return 0;
}

} /* namespace glretrace */
