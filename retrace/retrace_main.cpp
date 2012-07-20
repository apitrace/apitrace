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
#include <iostream>

#include "os_binary.hpp"
#include "os_time.hpp"
#include "image.hpp"
#include "trace_callset.hpp"
#include "trace_dump.hpp"
#include "retrace.hpp"


static bool waitOnFinish = false;

static const char *comparePrefix = NULL;
static const char *snapshotPrefix = NULL;
static trace::CallSet snapshotFrequency;
static trace::CallSet compareFrequency;

static unsigned dumpStateCallNo = ~0;


namespace retrace {


trace::Parser parser;
trace::Profiler profiler;


int verbosity = 0;
bool debug = true;
bool profiling = false;
bool profileGPU = false;
bool dumpingState = false;


bool doubleBuffer = true;
bool coreProfile = false;


unsigned frameNo = 0;


void
frameComplete(trace::Call &call) {
    ++frameNo;
}


static void
takeSnapshot(unsigned call_no) {
    assert(snapshotPrefix || comparePrefix);

    image::Image *ref = NULL;

    if (comparePrefix) {
        os::String filename = os::String::format("%s%010u.png", comparePrefix, call_no);
        ref = image::readPNG(filename);
        if (!ref) {
            return;
        }
        if (retrace::verbosity >= 0) {
            std::cout << "Read " << filename << "\n";
        }
    }

    image::Image *src = getSnapshot();
    if (!src) {
        return;
    }

    if (snapshotPrefix) {
        if (snapshotPrefix[0] == '-' && snapshotPrefix[1] == 0) {
            char comment[21];
            snprintf(comment, sizeof comment, "%u", call_no);
            src->writePNM(std::cout, comment);
        } else {
            os::String filename = os::String::format("%s%010u.png", snapshotPrefix, call_no);
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

    return;
}


static void
mainLoop() {
    retrace::Retracer retracer;

    addCallbacks(retracer);

    long long startTime = 0; 
    frameNo = 0;

    startTime = os::getTime();
    trace::Call *call;

    while ((call = retrace::parser.parse_call())) {
        bool swapRenderTarget = call->flags & trace::CALL_FLAG_SWAP_RENDERTARGET;
        bool doSnapshot =
            snapshotFrequency.contains(*call) ||
            compareFrequency.contains(*call)
        ;

        // For calls which cause rendertargets to be swaped, we take the
        // snapshot _before_ swapping the rendertargets.
        if (doSnapshot && swapRenderTarget) {
            if (call->flags & trace::CALL_FLAG_END_FRAME) {
                // For swapbuffers/presents we still use this call number,
                // spite not have been executed yet.
                takeSnapshot(call->no);
            } else {
                // Whereas for ordinate fbo/rendertarget changes we use the
                // previous call's number.
                takeSnapshot(call->no - 1);
            }
        }

        retracer.retrace(*call);

        if (doSnapshot && !swapRenderTarget) {
            takeSnapshot(call->no);
        }

        if (call->no >= dumpStateCallNo &&
            dumpState(std::cout)) {
            exit(0);
        }

        delete call;
    }

    // Reached the end of trace
    flushRendering();

    long long endTime = os::getTime();
    float timeInterval = (endTime - startTime) * (1.0 / os::timeFrequency);

    if ((retrace::verbosity >= -1) || (retrace::profiling)) {
        std::cout << 
            "Rendered " << frameNo << " frames"
            " in " <<  timeInterval << " secs,"
            " average of " << (frameNo/timeInterval) << " fps\n";
    }

    if (waitOnFinish) {
        waitForInput();
    } else {
        return;
    }
}


} /* namespace retrace */


static void
usage(const char *argv0) {
    std::cout << 
        "Usage: " << argv0 << " [OPTION] TRACE [...]\n"
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
        "  -w           waitOnFinish on final frame\n";
}


extern "C"
int main(int argc, char **argv)
{
    using namespace retrace;

    assert(compareFrequency.empty());
    assert(snapshotFrequency.empty());

    int i;
    for (i = 1; i < argc; ++i) {
        const char *arg = argv[i];

        if (arg[0] != '-') {
            break;
        }

        if (!strcmp(arg, "--")) {
            break;
        } else if (!strcmp(arg, "-b")) {
            retrace::debug = false;
            retrace::verbosity = -1;
        } else if (!strcmp(arg, "-p")) {
            retrace::debug = false;
            retrace::profiling = true;
            retrace::verbosity = -1;
        } else if (!strcmp(arg, "-pgpu")) {
            retrace::profileGPU = true;
        } else if (!strcmp(arg, "-c")) {
            comparePrefix = argv[++i];
            if (compareFrequency.empty()) {
                compareFrequency = trace::CallSet(trace::FREQUENCY_FRAME);
            }
        } else if (!strcmp(arg, "-C")) {
            compareFrequency = trace::CallSet(argv[++i]);
            if (comparePrefix == NULL) {
                comparePrefix = "";
            }
        } else if (!strcmp(arg, "-D")) {
            dumpStateCallNo = atoi(argv[++i]);
            dumpingState = true;
            retrace::verbosity = -2;
        } else if (!strcmp(arg, "-core")) {
            retrace::coreProfile = true;
        } else if (!strcmp(arg, "-db")) {
            retrace::doubleBuffer = true;
        } else if (!strcmp(arg, "-sb")) {
            retrace::doubleBuffer = false;
        } else if (!strcmp(arg, "--help")) {
            usage(argv[0]);
            return 0;
        } else if (!strcmp(arg, "-s")) {
            snapshotPrefix = argv[++i];
            if (snapshotFrequency.empty()) {
                snapshotFrequency = trace::CallSet(trace::FREQUENCY_FRAME);
            }
            if (snapshotPrefix[0] == '-' && snapshotPrefix[1] == 0) {
                os::setBinaryMode(stdout);
                retrace::verbosity = -2;
            }
        } else if (!strcmp(arg, "-S")) {
            snapshotFrequency = trace::CallSet(argv[++i]);
            if (snapshotPrefix == NULL) {
                snapshotPrefix = "";
            }
        } else if (!strcmp(arg, "-v")) {
            ++retrace::verbosity;
        } else if (!strcmp(arg, "-w")) {
            waitOnFinish = true;
        } else {
            std::cerr << "error: unknown option " << arg << "\n";
            usage(argv[0]);
            return 1;
        }
    }

    retrace::setUp();

    for ( ; i < argc; ++i) {
        if (!retrace::parser.open(argv[i])) {
            std::cerr << "error: failed to open " << argv[i] << "\n";
            return 1;
        }

        retrace::mainLoop();

        retrace::parser.close();
    }

    // XXX: X often hangs on XCloseDisplay
    //retrace::cleanUp();

    return 0;
}

