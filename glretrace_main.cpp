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

#include "os_string.hpp"
#include "os_time.hpp"
#include "image.hpp"
#include "retrace.hpp"
#include "trace_callset.hpp"
#include "glproc.hpp"
#include "glstate.hpp"
#include "glretrace.hpp"


namespace glretrace {

bool double_buffer = true;
bool insideGlBeginEnd = false;
trace::Parser parser;
glws::Profile defaultProfile = glws::PROFILE_COMPAT;
glws::Visual *visual[glws::PROFILE_MAX];
glws::Drawable *drawable = NULL;
glws::Context *context = NULL;

unsigned frame = 0;
long long startTime = 0;
bool wait = false;

bool benchmark = false;
static const char *compare_prefix = NULL;
static const char *snapshot_prefix = NULL;
static trace::CallSet snapshot_frequency;
static trace::CallSet compare_frequency;

unsigned dump_state = ~0;

void
checkGlError(trace::Call &call) {
    GLenum error = glGetError();
    if (error == GL_NO_ERROR) {
        return;
    }

    std::ostream & os = retrace::warning(call);

    os << "glGetError(";
    os << call.name();
    os << ") = ";

    switch (error) {
    case GL_INVALID_ENUM:
        os << "GL_INVALID_ENUM";
        break;
    case GL_INVALID_VALUE:
        os << "GL_INVALID_VALUE";
        break;
    case GL_INVALID_OPERATION:
        os << "GL_INVALID_OPERATION";
        break;
    case GL_STACK_OVERFLOW:
        os << "GL_STACK_OVERFLOW";
        break;
    case GL_STACK_UNDERFLOW:
        os << "GL_STACK_UNDERFLOW";
        break;
    case GL_OUT_OF_MEMORY:
        os << "GL_OUT_OF_MEMORY";
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        os << "GL_INVALID_FRAMEBUFFER_OPERATION";
        break;
    case GL_TABLE_TOO_LARGE:
        os << "GL_TABLE_TOO_LARGE";
        break;
    default:
        os << error;
        break;
    }
    os << "\n";
}

/**
 * Grow the current drawble.
 *
 * We need to infer the drawable size from GL calls because the drawable sizes
 * are specified by OS specific calls which we do not trace.
 */
void
updateDrawable(int width, int height) {
    if (!drawable) {
        return;
    }

    if (drawable->visible &&
        width  <= drawable->width &&
        height <= drawable->height) {
        return;
    }

    // Ignore zero area viewports
    if (width == 0 || height == 0) {
        return;
    }

    // Check for bound framebuffer last, as this may have a performance impact.
    GLint draw_framebuffer = 0;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &draw_framebuffer);
    if (draw_framebuffer != 0) {
        return;
    }

    drawable->resize(width, height);
    drawable->show();

    glScissor(0, 0, width, height);
}


static void
snapshot(unsigned call_no) {
    assert(snapshot_prefix || compare_prefix);

    if (!drawable) {
        return;
    }

    image::Image *ref = NULL;

    if (compare_prefix) {
        os::String filename = os::String::format("%s%010u.png", compare_prefix, call_no);
        ref = image::readPNG(filename);
        if (!ref) {
            return;
        }
        if (retrace::verbosity >= 0) {
            std::cout << "Read " << filename << "\n";
        }
    }

    image::Image *src = glstate::getDrawBufferImage();
    if (!src) {
        return;
    }

    if (snapshot_prefix) {
        if (snapshot_prefix[0] == '-' && snapshot_prefix[1] == 0) {
            char comment[21];
            snprintf(comment, sizeof comment, "%u", call_no);
            src->writePNM(std::cout, comment);
        } else {
            os::String filename = os::String::format("%s%010u.png", snapshot_prefix, call_no);
            if (src->writePNG(filename) && retrace::verbosity >= 0) {
                std::cout << "Wrote " << filename << "\n";
            }
        }
    }

    if (ref) {
        std::cout << "Snapshot " << call_no << " average precision of " << src->compare(*ref) << " bits\n";
        delete ref;
    }

    delete src;
}


void frame_complete(trace::Call &call) {
    ++frame;

    if (!drawable) {
        return;
    }

    if (!drawable->visible) {
        retrace::warning(call) << "could not infer drawable size (glViewport never called)\n";
    }
}


static void display(void) {
    retrace::Retracer retracer;

    retracer.addCallbacks(gl_callbacks);
    retracer.addCallbacks(glx_callbacks);
    retracer.addCallbacks(wgl_callbacks);
    retracer.addCallbacks(cgl_callbacks);
    retracer.addCallbacks(egl_callbacks);

    startTime = os::getTime();
    trace::Call *call;

    while ((call = parser.parse_call())) {
        bool swapRenderTarget = call->flags & trace::CALL_FLAG_SWAP_RENDERTARGET;
        bool doSnapshot =
            snapshot_frequency.contains(*call) ||
            compare_frequency.contains(*call)
        ;

        // For calls which cause rendertargets to be swaped, we take the
        // snapshot _before_ swapping the rendertargets.
        if (doSnapshot && swapRenderTarget) {
            if (call->flags & trace::CALL_FLAG_END_FRAME) {
                // For swapbuffers/presents we still use this call number,
                // spite not have been executed yet.
                snapshot(call->no);
            } else {
                // Whereas for ordinate fbo/rendertarget changes we use the
                // previous call's number.
                snapshot(call->no - 1);
            }
        }

        retracer.retrace(*call);

        if (doSnapshot && !swapRenderTarget) {
            snapshot(call->no);
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

    long long endTime = os::getTime();
    float timeInterval = (endTime - startTime) * (1.0 / os::timeFrequency);

    if ((retrace::verbosity >= -1) || (retrace::profiling)) {
        std::cout << 
            "Rendered " << frame << " frames"
            " in " <<  timeInterval << " secs,"
            " average of " << (frame/timeInterval) << " fps\n";
    }

    if (wait) {
        while (glws::processEvents()) {}
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
        "  -p           profiling mode (run whole trace, dump profiling info)\n"
        "  -c PREFIX    compare against snapshots\n"
        "  -C CALLSET   calls to compare (default is every frame)\n"
        "  -core        use core profile\n"
        "  -db          use a double buffer visual (default)\n"
        "  -sb          use a single buffer visual\n"
        "  -s PREFIX    take snapshots; `-` for PNM stdout output\n"
        "  -S CALLSET   calls to snapshot (default is every frame)\n"
        "  -v           increase output verbosity\n"
        "  -D CALLNO    dump state at specific call no\n"
        "  -w           wait on final frame\n";
}

extern "C"
int main(int argc, char **argv)
{
    assert(compare_frequency.empty());
    assert(snapshot_frequency.empty());

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
            glws::debug = false;
        } else if (!strcmp(arg, "-p")) {
            retrace::profiling = true;
            retrace::verbosity = -1;
            glws::debug = false;
        } else if (!strcmp(arg, "-c")) {
            compare_prefix = argv[++i];
            if (compare_frequency.empty()) {
                compare_frequency = trace::CallSet(trace::FREQUENCY_FRAME);
            }
        } else if (!strcmp(arg, "-C")) {
            compare_frequency = trace::CallSet(argv[++i]);
            if (compare_prefix == NULL) {
                compare_prefix = "";
            }
        } else if (!strcmp(arg, "-D")) {
            dump_state = atoi(argv[++i]);
            retrace::verbosity = -2;
        } else if (!strcmp(arg, "-core")) {
            defaultProfile = glws::PROFILE_CORE;
        } else if (!strcmp(arg, "-db")) {
            double_buffer = true;
        } else if (!strcmp(arg, "-sb")) {
            double_buffer = false;
        } else if (!strcmp(arg, "--help")) {
            usage();
            return 0;
        } else if (!strcmp(arg, "-s")) {
            snapshot_prefix = argv[++i];
            if (snapshot_frequency.empty()) {
                snapshot_frequency = trace::CallSet(trace::FREQUENCY_FRAME);
            }
            if (snapshot_prefix[0] == '-' && snapshot_prefix[1] == 0) {
                retrace::verbosity = -2;
            }
        } else if (!strcmp(arg, "-S")) {
            snapshot_frequency = trace::CallSet(argv[++i]);
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

    glws::init();
    visual[glws::PROFILE_COMPAT] = glws::createVisual(double_buffer, glws::PROFILE_COMPAT);
    visual[glws::PROFILE_CORE] = glws::createVisual(double_buffer, glws::PROFILE_CORE);
    visual[glws::PROFILE_ES1] = glws::createVisual(double_buffer, glws::PROFILE_ES1);
    visual[glws::PROFILE_ES2] = glws::createVisual(double_buffer, glws::PROFILE_ES2);

    for ( ; i < argc; ++i) {
        if (!parser.open(argv[i])) {
            std::cerr << "error: failed to open " << argv[i] << "\n";
            return 1;
        }

        display();

        parser.close();
    }

    for (int n = 0; n < glws::PROFILE_MAX; n++) {
        delete visual[n];
    }

    glws::cleanup();

    return 0;
}

} /* namespace glretrace */
