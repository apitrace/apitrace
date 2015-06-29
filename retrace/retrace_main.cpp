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
#include <iostream>
#include <getopt.h>
#ifndef _WIN32
#include <unistd.h> // for isatty()
#endif

#include "os_binary.hpp"
#include "os_crtdbg.hpp"
#include "os_time.hpp"
#include "os_thread.hpp"
#include "image.hpp"
#include "trace_callset.hpp"
#include "trace_dump.hpp"
#include "trace_option.hpp"
#include "retrace.hpp"
#include "state_writer.hpp"
#include "ws.hpp"


static bool waitOnFinish = false;
static int loopCount = 0;

static const char *snapshotPrefix = "";
static enum {
    PNM_FMT,
    RAW_RGB,
    RAW_MD5
} snapshotFormat = PNM_FMT;

static trace::CallSet snapshotFrequency;
static unsigned snapshotInterval = 0;
static trace::ParseBookmark lastFrameStart;

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

unsigned curPass = 0;
unsigned numPasses = 1;
bool profilingWithBackends = false;
char* profilingCallsMetricsString;
char* profilingFramesMetricsString;
char* profilingDrawCallsMetricsString;

bool profiling = false;
bool profilingGpuTimes = false;
bool profilingCpuTimes = false;
bool profilingPixelsDrawn = false;
bool profilingMemoryUsage = false;
bool useCallNos = true;
bool singleThread = false;

unsigned frameNo = 0;
unsigned callNo = 0;


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


/**
 * Take snapshots.
 */
static void
takeSnapshot(unsigned call_no) {
    static unsigned snapshot_no = 0;

    assert(dumpingSnapshots);
    assert(snapshotPrefix);

    image::Image *src = dumper->getSnapshot();
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

            // Alpha channel often has bogus data, so strip it when writing
            // PNG images to disk to simplify visualization.
            bool strip_alpha = true;

            if (src->writePNG(filename, strip_alpha) &&
                retrace::verbosity >= 0) {
                std::cout << "Wrote " << filename << "\n";
            }
        }
    }

    delete src;

    snapshot_no++;

    return;
}


/**
 * Retrace one call.
 *
 * Take snapshots before/after retracing (as appropriate) and dispatch it to
 * the respective handler.
 */
static void
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


class RelayRunner;


/**
 * Implement multi-threading by mimicking a relay race.
 */
class RelayRace
{
private:
    /**
     * Runners indexed by the leg they run (i.e, the thread_ids from the
     * trace).
     */
    std::vector<RelayRunner*> runners;

public:
    RelayRace();

    ~RelayRace();

    RelayRunner *
    getRunner(unsigned leg);

    inline RelayRunner *
    getForeRunner() {
        return getRunner(0);
    }

    void
    run(void);

    void
    passBaton(trace::Call *call);

    void
    finishLine();

    void
    stopRunners();
};


/**
 * Each runner is a thread.
 *
 * The fore runner doesn't have its own thread, but instead uses the thread
 * where the race started.
 */
class RelayRunner
{
private:
    friend class RelayRace;

    RelayRace *race;

    unsigned leg;
    
    os::mutex mutex;
    os::condition_variable wake_cond;

    /**
     * There are protected by the mutex.
     */
    bool finished;
    trace::Call *baton;

    os::thread thread;

    static void
    runnerThread(RelayRunner *_this);

public:
    RelayRunner(RelayRace *race, unsigned _leg) :
        race(race),
        leg(_leg),
        finished(false),
        baton(0)
    {
        /* The fore runner does not need a new thread */
        if (leg) {
            thread = os::thread(runnerThread, this);
        }
    }

    ~RelayRunner() {
        if (thread.joinable()) {
            thread.join();
        }
    }

    /**
     * Thread main loop.
     */
    void
    runRace(void) {
        os::unique_lock<os::mutex> lock(mutex);

        while (1) {
            while (!finished && !baton) {
                wake_cond.wait(lock);
            }

            if (finished) {
                break;
            }

            assert(baton);
            trace::Call *call = baton;
            baton = 0;

            runLeg(call);
        }

        if (0) std::cerr << "leg " << leg << " actually finishing\n";

        if (leg == 0) {
            race->stopRunners();
        }
    }

    /**
     * Interpret successive calls.
     */
    void
    runLeg(trace::Call *call) {

        /* Consume successive calls for this thread. */
        do {
            bool callEndsFrame = false;
            static trace::ParseBookmark frameStart;

            assert(call);
            assert(call->thread_id == leg);

            if (loopCount && call->flags & trace::CALL_FLAG_END_FRAME) {
                callEndsFrame = true;
                parser.getBookmark(frameStart);
            }

            retraceCall(call);
            delete call;
            call = parser.parse_call();

            /* Restart last frame if looping is requested. */
            if (loopCount) {
                if (!call) {
                    parser.setBookmark(lastFrameStart);
                    call = parser.parse_call();
                    if (loopCount > 0) {
                        --loopCount;
                    }
                } else if (callEndsFrame) {
                    lastFrameStart = frameStart;
                }
            }

        } while (call && call->thread_id == leg);

        if (call) {
            /* Pass the baton */
            assert(call->thread_id != leg);
            flushRendering();
            race->passBaton(call);
        } else {
            /* Reached the finish line */
            if (0) std::cerr << "finished on leg " << leg << "\n";
            if (leg) {
                /* Notify the fore runner */
                race->finishLine();
            } else {
                /* We are the fore runner */
                finished = true;
            }
        }
    }

    /**
     * Called by other threads when relinquishing the baton.
     */
    void
    receiveBaton(trace::Call *call) {
        assert (call->thread_id == leg);

        mutex.lock();
        baton = call;
        mutex.unlock();

        wake_cond.notify_one();
    }

    /**
     * Called by the fore runner when the race is over.
     */
    void
    finishRace() {
        if (0) std::cerr << "notify finish to leg " << leg << "\n";

        mutex.lock();
        finished = true;
        mutex.unlock();

        wake_cond.notify_one();
    }
};


void
RelayRunner::runnerThread(RelayRunner *_this) {
    _this->runRace();
}


RelayRace::RelayRace() {
    runners.push_back(new RelayRunner(this, 0));
}


RelayRace::~RelayRace() {
    assert(runners.size() >= 1);
    std::vector<RelayRunner*>::const_iterator it;
    for (it = runners.begin(); it != runners.end(); ++it) {
        RelayRunner* runner = *it;
        if (runner) {
            delete runner;
        }
    }
}


/**
 * Get (or instantiate) a runner for the specified leg.
 */
RelayRunner *
RelayRace::getRunner(unsigned leg) {
    RelayRunner *runner;

    if (leg >= runners.size()) {
        runners.resize(leg + 1);
        runner = 0;
    } else {
        runner = runners[leg];
    }
    if (!runner) {
        runner = new RelayRunner(this, leg);
        runners[leg] = runner;
    }
    return runner;
}


/**
 * Start the race.
 */
void
RelayRace::run(void) {
    trace::Call *call;
    call = parser.parse_call();
    if (!call) {
        /* Nothing to do */
        return;
    }

    /* If the user wants to loop we need to get a bookmark target. We
     * usually get this after replaying a call that ends a frame, but
     * for a trace that has only one frame we need to get it at the
     * beginning. */
    if (loopCount) {
        parser.getBookmark(lastFrameStart);
    }

    RelayRunner *foreRunner = getForeRunner();
    if (call->thread_id == 0) {
        /* We are the forerunner thread, so no need to pass baton */
        foreRunner->baton = call;
    } else {
        passBaton(call);
    }

    /* Start the forerunner thread */
    foreRunner->runRace();
}


/**
 * Pass the baton (i.e., the call) to the appropriate thread.
 */
void
RelayRace::passBaton(trace::Call *call) {
    if (0) std::cerr << "switching to thread " << call->thread_id << "\n";
    RelayRunner *runner = getRunner(call->thread_id);
    runner->receiveBaton(call);
}


/**
 * Called when a runner other than the forerunner reaches the finish line.
 *
 * Only the fore runner can finish the race, so inform him that the race is
 * finished.
 */
void
RelayRace::finishLine(void) {
    RelayRunner *foreRunner = getForeRunner();
    foreRunner->finishRace();
}


/**
 * Called by the fore runner after finish line to stop all other runners.
 */
void
RelayRace::stopRunners(void) {
    std::vector<RelayRunner*>::const_iterator it;
    for (it = runners.begin() + 1; it != runners.end(); ++it) {
        RelayRunner* runner = *it;
        if (runner) {
            runner->finishRace();
        }
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
        "      --pcalls            call profiling metrics selection\n"
        "      --pframes           frame profiling metrics selection\n"
        "      --pdrawcalls        draw call profiling metrics selection\n"
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
    PCALLS_OPT,
    PFRAMES_OPT,
    PDRAWCALLS_OPT,
    SB_OPT,
    SNAPSHOT_FORMAT_OPT,
    LOOP_OPT,
    SINGLETHREAD_OPT,
    SNAPSHOT_INTERVAL_OPT,
    DUMP_FORMAT_OPT,
};

const static char *
shortOptions = "bdD:hs:S:vw";

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
    {"pcalls", required_argument, 0, PCALLS_OPT},
    {"pframes", required_argument, 0, PFRAMES_OPT},
    {"pdrawcalls", required_argument, 0, PDRAWCALLS_OPT},
    {"sb", no_argument, 0, SB_OPT},
    {"snapshot-prefix", required_argument, 0, 's'},
    {"snapshot-format", required_argument, 0, SNAPSHOT_FORMAT_OPT},
    {"snapshot", required_argument, 0, 'S'},
    {"snapshot-interval", required_argument, 0, SNAPSHOT_INTERVAL_OPT},
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
        case 'v':
            ++retrace::verbosity;
            break;
        case 'w':
            waitOnFinish = true;
            break;
        case LOOP_OPT:
            loopCount = trace::intOption(optarg, -1);
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
        case PCALLS_OPT:
            retrace::debug = 0;
            retrace::profiling = true;
            retrace::verbosity = -1;
            retrace::profilingWithBackends = true;
            retrace::profilingCallsMetricsString = optarg;
            break;
        case PFRAMES_OPT:
            retrace::debug = 0;
            retrace::profiling = true;
            retrace::verbosity = -1;
            retrace::profilingWithBackends = true;
            retrace::profilingFramesMetricsString = optarg;
            break;
        case PDRAWCALLS_OPT:
            retrace::debug = 0;
            retrace::profiling = true;
            retrace::verbosity = -1;
            retrace::profilingWithBackends = true;
            retrace::profilingDrawCallsMetricsString = optarg;
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

    retrace::setUp();
    if (retrace::profiling && !retrace::profilingWithBackends) {
        retrace::profiler.setup(retrace::profilingCpuTimes, retrace::profilingGpuTimes, retrace::profilingPixelsDrawn, retrace::profilingMemoryUsage);
    }

    os::setExceptionCallback(exceptionCallback);

    for (retrace::curPass = 0; retrace::curPass < retrace::numPasses;
         retrace::curPass++)
    {
        for (i = optind; i < argc; ++i) {
            if (!retrace::parser.open(argv[i])) {
                return 1;
            }

            retrace::mainLoop();

            retrace::parser.close();
        }
    }

    os::resetExceptionCallback();

    // XXX: X often hangs on XCloseDisplay
    //retrace::cleanUp();

#ifdef _WIN32
    if (mmRes == MMSYSERR_NOERROR) {
        timeEndPeriod(tc.wPeriodMin);
    }
#endif

    return 0;
}

