/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
 * Copyright (C) 2013 Intel Corporation. All rights reversed.
 * Author: Shuang He <shuang.he@intel.com>
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
#include <limits.h> // for CHAR_MAX
#include <memory> // for unique_ptr
#include <iostream>
#include <getopt.h>
#ifndef _WIN32
#include <unistd.h> // for isatty()
#endif

#include "os_binary.hpp"
#include "os_crtdbg.hpp"
#include "os_time.hpp"
#include "image.hpp"
#include "threaded_snapshot.hpp"
#include "trace_callset.hpp"
#include "trace_dump.hpp"
#include "trace_option.hpp"
#include "relay_race.hpp"
#include "retrace.hpp"
#include "state_writer.hpp"
#include "ws.hpp"


static bool waitOnFinish = false;

static const char *snapshotPrefix = "";
static enum {
    PNM_FMT,
    RAW_RGB,
    RAW_MD5
} snapshotFormat = PNM_FMT;

static trace::CallSet snapshotFrequency;
static unsigned snapshotInterval = 0;

static unsigned dumpStateCallNo = ~0;

retrace::Retracer retracer;


namespace retrace {


trace::Parser parser;
trace::Profiler profiler;


int verbosity = 0;
unsigned debug = 1;
bool forceWindowed = true;
bool dumpingState = false;
bool dumpingSnapshots = false;

Driver driver = DRIVER_DEFAULT;
const char *driverModule = NULL;

bool doubleBuffer = true;
unsigned samples = 1;

bool profiling = false;
bool profilingGpuTimes = false;
bool profilingCpuTimes = false;
bool profilingPixelsDrawn = false;
bool profilingMemoryUsage = false;
bool useCallNos = true;
bool singleThread = false;

unsigned frameNo = 0;
unsigned callNo = 0;
int loopCount = 0;


static void
takeSnapshot(unsigned call_no);


void
frameComplete(trace::Call &call) {
    ++frameNo;

    if (!(call.flags & trace::CALL_FLAG_END_FRAME) &&
        snapshotFrequency.contains(call)) {
        // This call doesn't have the end of frame flag, so take any snapshot
        // now.
        takeSnapshot(call.no);
    }
}


class DefaultDumper: public Dumper
{
public:
    image::Image *
    getSnapshot(void) {
        return NULL;
    }

    bool
    canDump(void) {
        return false;
    }

    void
    dumpState(StateWriter &writer) {
        assert(0);
    }
};


static DefaultDumper defaultDumper;

Dumper *dumper = &defaultDumper;


typedef StateWriter *(*StateWriterFactory)(std::ostream &);
static StateWriterFactory stateWriterFactory = createJSONStateWriter;


static Snapshotter *snapshotter;


/**
 * Take snapshots.
 */
static void
takeSnapshot(unsigned call_no) {
    static unsigned snapshot_no = 0;

    assert(dumpingSnapshots);
    assert(snapshotPrefix);

    std::unique_ptr<image::Image> src(dumper->getSnapshot());
    if (!src) {
        std::cerr << call_no << ": warning: failed to get snapshot\n";
        return;
    }

    if ((snapshotInterval == 0 ||
        (snapshot_no % snapshotInterval) == 0)) {

        if (snapshotPrefix[0] == '-' && snapshotPrefix[1] == 0) {
            char comment[21];
            snprintf(comment, sizeof comment, "%u",
                     useCallNos ? call_no : snapshot_no);
            switch (snapshotFormat) {
            case PNM_FMT:
                src->writePNM(std::cout, comment);
                break;
            case RAW_RGB:
                src->writeRAW(std::cout);
                break;
            case RAW_MD5:
                src->writeMD5(std::cout);
                break;
            default:
                assert(0);
                break;
            }
        } else {
            os::String filename = os::String::format("%s%010u.png",
                                                     snapshotPrefix,
                                                     useCallNos ? call_no : snapshot_no);

            // Here we release our ownership on the Image, it is now the
            // responsibility of the snapshotter to delete it.
            snapshotter->writePNG(filename, src.release());
        }
    }

    snapshot_no++;

    return;
}


/**
 * Retrace one call.
 *
 * Take snapshots before/after retracing (as appropriate) and dispatch it to
 * the respective handler.
 */
void
retraceCall(trace::Call *call) {
    callNo = call->no;

    bool swapRenderTarget = call->flags &
        trace::CALL_FLAG_SWAP_RENDERTARGET;
    bool doSnapshot = snapshotFrequency.contains(*call);

    // For calls which cause rendertargets to be swaped, we take the
    // snapshot _before_ swapping the rendertargets.
    if (doSnapshot && swapRenderTarget) {
        if (call->flags & trace::CALL_FLAG_END_FRAME) {
            // For swapbuffers/presents we still use this
            // call number, spite not have been executed yet.
            takeSnapshot(call->no);
        } else {
            // Whereas for ordinate fbo/rendertarget changes we
            // use the previous call's number.
            takeSnapshot(call->no - 1);
        }
    }

    retracer.retrace(*call);

    if (doSnapshot) {
        if (!swapRenderTarget) {
            takeSnapshot(call->no);
        }
        if (call->no >= snapshotFrequency.getLast()) {
            exit(0);
        }
    }

    if (call->no >= dumpStateCallNo &&
        dumper->canDump()) {
        StateWriter *writer = stateWriterFactory(std::cout);
        dumper->dumpState(*writer);
        delete writer;
        exit(0);
    }
}


static void
mainLoop() {
    addCallbacks(retracer);

    long long startTime = 0; 
    frameNo = 0;

    startTime = os::getTime();

    if (singleThread) {
        trace::Call *call;
        while ((call = parser.parse_call())) {
            retraceCall(call);
            delete call;
        };
    } else {
        RelayRace race;
        race.run();
    }
    finishRendering();

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
        "  -b, --benchmark         benchmark mode (no error checking or warning messages)\n"
        "  -d, --debug             increase debugging checks\n"
        "      --pcpu              cpu profiling (cpu times per call)\n"
        "      --pgpu              gpu profiling (gpu times per draw call)\n"
        "      --ppd               pixels drawn profiling (pixels drawn per draw call)\n"
        "      --pmem              memory usage profiling (vsize rss per call)\n"
        "      --call-nos[=BOOL]   use call numbers in snapshot filenames\n"
        "      --core              use core profile\n"
        "      --db                use a double buffer visual (default)\n"
        "      --samples=N         use GL_ARB_multisample (default is 1)\n"
        "      --driver=DRIVER     force driver type (`hw`, `sw`, `ref`, `null`, or driver module name)\n"
        "      --fullscreen        allow fullscreen\n"
        "      --headless          don't show windows\n"
        "      --sb                use a single buffer visual\n"
        "  -s, --snapshot-prefix=PREFIX    take snapshots; `-` for PNM stdout output\n"
        "      --snapshot-format=FMT       use (PNM, RGB, or MD5; default is PNM) when writing to stdout output\n"
        "  -S, --snapshot=CALLSET  calls to snapshot (default is every frame)\n"
        "      --snapshot-interval=N    specify a frame interval when generating snaphots (default is 0)\n"
        "  -t, --snapshot-threaded encode screenshots on multiple threads\n"
        "  -v, --verbose           increase output verbosity\n"
        "  -D, --dump-state=CALL   dump state at specific call no\n"
        "      --dump-format=FORMAT dump state format (`json` or `ubjson`)\n"
        "  -w, --wait              waitOnFinish on final frame\n"
        "      --loop[=N]          loop N times (N<0 continuously) replaying final frame.\n"
        "      --singlethread      use a single thread to replay command stream\n";
}

enum {
    CALL_NOS_OPT = CHAR_MAX + 1,
    CORE_OPT,
    DB_OPT,
    SAMPLES_OPT,
    DRIVER_OPT,
    FULLSCREEN_OPT,
    HEADLESS_OPT,
    PCPU_OPT,
    PGPU_OPT,
    PPD_OPT,
    PMEM_OPT,
    SB_OPT,
    SNAPSHOT_FORMAT_OPT,
    LOOP_OPT,
    SINGLETHREAD_OPT,
    SNAPSHOT_INTERVAL_OPT,
    DUMP_FORMAT_OPT,
};

const static char *
shortOptions = "bdD:hs:S:vwt";

const static struct option
longOptions[] = {
    {"benchmark", no_argument, 0, 'b'},
    {"debug", no_argument, 0, 'd'},
    {"call-nos", optional_argument, 0, CALL_NOS_OPT },
    {"core", no_argument, 0, CORE_OPT},
    {"db", no_argument, 0, DB_OPT},
    {"samples", required_argument, 0, SAMPLES_OPT},
    {"driver", required_argument, 0, DRIVER_OPT},
    {"dump-state", required_argument, 0, 'D'},
    {"dump-format", required_argument, 0, DUMP_FORMAT_OPT},
    {"fullscreen", no_argument, 0, FULLSCREEN_OPT},
    {"headless", no_argument, 0, HEADLESS_OPT},
    {"help", no_argument, 0, 'h'},
    {"pcpu", no_argument, 0, PCPU_OPT},
    {"pgpu", no_argument, 0, PGPU_OPT},
    {"ppd", no_argument, 0, PPD_OPT},
    {"pmem", no_argument, 0, PMEM_OPT},
    {"sb", no_argument, 0, SB_OPT},
    {"snapshot-prefix", required_argument, 0, 's'},
    {"snapshot-format", required_argument, 0, SNAPSHOT_FORMAT_OPT},
    {"snapshot", required_argument, 0, 'S'},
    {"snapshot-interval", required_argument, 0, SNAPSHOT_INTERVAL_OPT},
    {"snapshot-threaded", no_argument, 0, 't'},
    {"verbose", no_argument, 0, 'v'},
    {"wait", no_argument, 0, 'w'},
    {"loop", optional_argument, 0, LOOP_OPT},
    {"singlethread", no_argument, 0, SINGLETHREAD_OPT},
    {0, 0, 0, 0}
};


static void exceptionCallback(void)
{
    std::cerr << retrace::callNo << ": error: caught an unhandled exception\n";
}


extern "C"
int main(int argc, char **argv)
{
    using namespace retrace;
    int i;
    bool snapshotThreaded = false;

    os::setDebugOutput(os::OUTPUT_STDERR);

    assert(snapshotFrequency.empty());

    int opt;
    while  ((opt = getopt_long_only(argc, argv, shortOptions, longOptions, NULL)) != -1) {
        switch (opt) {
        case 'h':
            usage(argv[0]);
            return 0;
        case 'b':
            retrace::debug = 0;
            retrace::verbosity = -1;
            break;
        case 'd':
            ++retrace::debug;
            break;
        case CALL_NOS_OPT:
            useCallNos = trace::boolOption(optarg);
            break;
        case 'D':
            dumpStateCallNo = atoi(optarg);
            dumpingState = true;
            retrace::verbosity = -2;
            break;
        case DUMP_FORMAT_OPT:
            if (strcasecmp(optarg, "json") == 0) {
                stateWriterFactory = &createJSONStateWriter;
            } else if (strcasecmp(optarg, "ubjson") == 0) {
                os::setBinaryMode(stdout);
                stateWriterFactory = &createUBJSONStateWriter;
            } else {
                std::cerr << "error: unsupported dump format `" << optarg << "`\n";
                return EXIT_FAILURE;
            }
            break;
        case CORE_OPT:
            retrace::setFeatureLevel("3_2_core");
            break;
        case DB_OPT:
            retrace::doubleBuffer = true;
            break;
        case SAMPLES_OPT:
            retrace::samples = atoi(optarg);
            break;
        case DRIVER_OPT:
            if (strcasecmp(optarg, "hw") == 0) {
                driver = DRIVER_HARDWARE;
            } else if (strcasecmp(optarg, "sw") == 0) {
                driver = DRIVER_SOFTWARE;
            } else if (strcasecmp(optarg, "ref") == 0) {
                driver = DRIVER_REFERENCE;
            } else if (strcasecmp(optarg, "null") == 0) {
                driver = DRIVER_NULL;
            } else {
                driver = DRIVER_MODULE;
                driverModule = optarg;
            }
            break;
        case FULLSCREEN_OPT:
            retrace::forceWindowed = false;
            break;
        case HEADLESS_OPT:
            ws::headless = true;
            break;
        case SB_OPT:
            retrace::doubleBuffer = false;
            break;
        case SINGLETHREAD_OPT:
            retrace::singleThread = true;
            break;
        case 's':
            dumpingSnapshots = true;
            snapshotPrefix = optarg;
            if (snapshotFrequency.empty()) {
                snapshotFrequency = trace::CallSet(trace::FREQUENCY_FRAME);
            }
            if (snapshotPrefix[0] == '-' && snapshotPrefix[1] == 0) {
                os::setBinaryMode(stdout);
                retrace::verbosity = -2;
            } else {
                /*
                 * Create the snapshot directory if it does not exist.
                 *
                 * We can't just use trimFilename() because when applied to
                 * "/foo/boo/" it would merely return "/foo".
                 *
                 * XXX: create nested directories.
                 */
                os::String prefix(snapshotPrefix);
                os::String::iterator sep = prefix.rfindSep(false);
                if (sep != prefix.end()) {
                    prefix.erase(sep, prefix.end());
                    if (!prefix.exists() && !os::createDirectory(prefix)) {
                        std::cerr << "error: failed to create `" << prefix.str() << "` directory\n";
                    }
                }
            }
            break;
        case SNAPSHOT_FORMAT_OPT:
            if (strcmp(optarg, "RGB") == 0)
                snapshotFormat = RAW_RGB;
            else if (strcmp(optarg, "MD5") == 0)
                snapshotFormat = RAW_MD5;
            else
                snapshotFormat = PNM_FMT;
            break;
        case 'S':
            dumpingSnapshots = true;
            snapshotFrequency.merge(optarg);
            break;
        case SNAPSHOT_INTERVAL_OPT:
            snapshotInterval = atoi(optarg);
            break;
        case 't':
            snapshotThreaded = true;
            break;
        case 'v':
            ++retrace::verbosity;
            break;
        case 'w':
            waitOnFinish = true;
            break;
        case LOOP_OPT:
            retrace::loopCount = trace::intOption(optarg, -1);
            break;
        case PGPU_OPT:
            retrace::debug = 0;
            retrace::profiling = true;
            retrace::verbosity = -1;

            retrace::profilingGpuTimes = true;
            break;
        case PCPU_OPT:
            retrace::debug = 0;
            retrace::profiling = true;
            retrace::verbosity = -1;

            retrace::profilingCpuTimes = true;
            break;
        case PPD_OPT:
            retrace::debug = 0;
            retrace::profiling = true;
            retrace::verbosity = -1;

            retrace::profilingPixelsDrawn = true;
            break;
        case PMEM_OPT:
            retrace::debug = 0;
            retrace::profiling = true;
            retrace::verbosity = -1;

            retrace::profilingMemoryUsage = true;
            break;
        default:
            std::cerr << "error: unknown option " << opt << "\n";
            usage(argv[0]);
            return 1;
        }
    }

#ifndef _WIN32
    if (!isatty(STDOUT_FILENO)) {
        dumpFlags |= trace::DUMP_FLAG_NO_COLOR;
    }
#endif

#ifdef _WIN32
    // Set Windows timer resolution to the minimum period supported by the
    // system
    TIMECAPS tc;
    MMRESULT mmRes = timeGetDevCaps(&tc, sizeof tc);
    if (mmRes == MMSYSERR_NOERROR) {
        mmRes = timeBeginPeriod(tc.wPeriodMin);
    }
#endif

    if (snapshotThreaded) {
        snapshotter = new ThreadedSnapshotter(os::thread::hardware_concurrency());
    } else {
        snapshotter = new Snapshotter();
    }

    retrace::setUp();
    if (retrace::profiling) {
        retrace::profiler.setup(retrace::profilingCpuTimes, retrace::profilingGpuTimes, retrace::profilingPixelsDrawn, retrace::profilingMemoryUsage);
    }

    os::setExceptionCallback(exceptionCallback);

    for (i = optind; i < argc; ++i) {
        if (!retrace::parser.open(argv[i])) {
            return 1;
        }

        retrace::mainLoop();

        retrace::parser.close();
    }
    
    os::resetExceptionCallback();

    delete snapshotter;

    // XXX: X often hangs on XCloseDisplay
    //retrace::cleanUp();

#ifdef _WIN32
    if (mmRes == MMSYSERR_NOERROR) {
        timeEndPeriod(tc.wPeriodMin);
    }
#endif

    return 0;
}

