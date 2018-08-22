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
#include <regex>
#include <getopt.h>
#ifndef _WIN32
#include <unistd.h> // for isatty()
#endif
#ifdef _WIN32
#include <malloc.h> // _get_heap_handle
#include <new.h>
#include <windows.h>
#include <psapi.h>
#endif

#include "os_binary.hpp"
#include "os_crtdbg.hpp"
#include "os_time.hpp"
#include "os_thread.hpp"
#include "image.hpp"
#include "threaded_snapshot.hpp"
#include "trace_callset.hpp"
#include "trace_dump.hpp"
#include "trace_option.hpp"
#include "retrace.hpp"
#include "state_writer.hpp"
#include "ws.hpp"
#include "process_name.hpp"


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


trace::AbstractParser *parser;
trace::Profiler profiler;


int verbosity = 0;
int debug = 1;
bool markers = false;
bool snapshotMRT = false;
bool snapshotAlpha = false;
bool forceWindowed = true;
bool dumpingState = false;
bool dumpingSnapshots = false;

bool resolveMSAA = true;

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
bool profilingListMetrics = false;
bool profilingNumPasses = false;

bool profiling = false;
bool profilingGpuTimes = false;
bool profilingCpuTimes = false;
bool profilingPixelsDrawn = false;
bool profilingMemoryUsage = false;
bool useCallNos = true;
bool singleThread = false;
bool ignoreRetvals = false;
bool contextCheck = true;

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
    int
    getSnapshotCount(void) override {
        return 0;
    }

    image::Image *
    getSnapshot(int n) override {
        return NULL;
    }

    bool
    canDump(void) override {
        return false;
    }

    void
    dumpState(StateWriter &writer) override {
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
takeSnapshot(unsigned call_no, int mrt, unsigned snapshot_no) {

    assert(dumpingSnapshots);
    assert(snapshotPrefix);

    std::unique_ptr<image::Image> src(dumper->getSnapshot(mrt));
    if (!src) {
        /* TODO for mrt>0 we probably don't want to treat this as an error: */
        if (mrt == 0)
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
            os::String filename;
            unsigned no = useCallNos ? call_no : snapshot_no;

            if (!retrace::snapshotMRT) {
                assert(mrt == 0);
                filename = os::String::format("%s%010u.png", snapshotPrefix, no);
            } else if (mrt == -2) {
                /* stencil */
                filename = os::String::format("%s%010u-s.png", snapshotPrefix, no);
            } else if (mrt == -1) {
                /* depth */
                filename = os::String::format("%s%010u-z.png", snapshotPrefix, no);
            } else {
                filename = os::String::format("%s%010u-mrt%u.png", snapshotPrefix, no, mrt);
            }

            // Here we release our ownership on the Image, it is now the
            // responsibility of the snapshotter to delete it.
            snapshotter->writePNG(filename, src.release());
        }
    }

    return;
}

static void
takeSnapshot(unsigned call_no) {
    static unsigned snapshot_no = 0;
    int cnt = dumper->getSnapshotCount();

    if (retrace::snapshotMRT) {
        for (int mrt = -2; mrt < cnt; mrt++) {
            takeSnapshot(call_no, mrt, snapshot_no);
        }
    } else {
        takeSnapshot(call_no, 0, snapshot_no);
    }

    snapshot_no++;
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

            assert(call);
            assert(call->thread_id == leg);

            retraceCall(call);
            delete call;
            call = parser->parse_call();

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
        delete runner;
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
    call = parser->parse_call();
    if (!call) {
        /* Nothing to do */
        return;
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
        while ((call = parser->parse_call())) {
            retraceCall(call);
            delete call;
        }
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
        "      --markers           insert call no markers in the command stream\n"
        "      --pcpu              cpu profiling (cpu times per call)\n"
        "      --pgpu              gpu profiling (gpu times per draw call)\n"
        "      --ppd               pixels drawn profiling (pixels drawn per draw call)\n"
        "      --pmem              memory usage profiling (vsize rss per call)\n"
        "      --pcalls            call profiling metrics selection\n"
        "      --pframes           frame profiling metrics selection\n"
        "      --pdrawcalls        draw call profiling metrics selection\n"
        "      --list-metrics      list all available metrics for TRACE\n"
        "      --gen-passes        generate profiling passes and output passes number\n"
        "      --call-nos[=BOOL]   use call numbers in snapshot filenames\n"
        "      --core              use core profile\n"
        "      --db                use a double buffer visual (default)\n"
        "      --samples=N         use GL_ARB_multisample (default is 1)\n"
        "      --driver=DRIVER     force driver type (`hw`, `sw`, `ref`, `null`, or driver module name)\n"
        "      --fullscreen        allow fullscreen\n"
        "      --headless          don't show windows\n"
        "      --sb                use a single buffer visual\n"
        "  -m, --mrt               dump all MRTs and depth/stencil\n"
        "      --msaa-no-resolve   dump raw sample images of multisampled texture instead of resolved texture\n"
        "  -s, --snapshot-prefix=PREFIX    take snapshots; `-` for PNM stdout output\n"
        "      --snapshot-alpha    Include alpha channel in snapshots.\n"
        "      --snapshot-format=FMT       use (PNM, RGB, or MD5; default is PNM) when writing to stdout output\n"
        "  -S, --snapshot=CALLSET  calls to snapshot (default is every frame)\n"
        "      --snapshot-interval=N    specify a frame interval when generating snaphots (default is 0)\n"
        "  -t, --snapshot-threaded encode screenshots on multiple threads\n"
        "  -v, --verbose           increase output verbosity\n"
        "  -D, --dump-state=CALL   dump state at specific call no\n"
        "      --dump-format=FORMAT dump state format (`json` or `ubjson`)\n"
        "  -w, --wait              waitOnFinish on final frame\n"
        "      --loop[=N]          loop N times (N<0 continuously) replaying final frame.\n"
        "      --singlethread      use a single thread to replay command stream\n"
        "      --ignore-retvals    ignore return values in wglMakeCurrent, etc\n"
        "      --no-context-check  don't check that the actual GL context version matches the requested version\n"
    ;
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
    PLMETRICS_OPT,
    GENPASS_OPT,
    MSAA_NO_RESOLVE_OPT,
    SB_OPT,
    LOOP_OPT,
    SINGLETHREAD_OPT,
    IGNORE_RETVALS_OPT,
    NO_CONTEXT_CHECK,
    SNAPSHOT_ALPHA_OPT,
    SNAPSHOT_FORMAT_OPT,
    SNAPSHOT_INTERVAL_OPT,
    DUMP_FORMAT_OPT,
    MARKERS_OPT
};

const static char *
shortOptions = "bdD:hms:S:vwt";

const static struct option
longOptions[] = {
    {"benchmark", no_argument, 0, 'b'},
    {"debug", no_argument, 0, 'd'},
    {"markers", no_argument, 0, MARKERS_OPT},
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
    {"mrt", no_argument, 0, 'm'},
    {"msaa-no-resolve", no_argument, 0, MSAA_NO_RESOLVE_OPT},
    {"pcpu", no_argument, 0, PCPU_OPT},
    {"pgpu", no_argument, 0, PGPU_OPT},
    {"ppd", no_argument, 0, PPD_OPT},
    {"pmem", no_argument, 0, PMEM_OPT},
    {"pcalls", required_argument, 0, PCALLS_OPT},
    {"pframes", required_argument, 0, PFRAMES_OPT},
    {"pdrawcalls", required_argument, 0, PDRAWCALLS_OPT},
    {"list-metrics", no_argument, 0, PLMETRICS_OPT},
    {"gen-passes", no_argument, 0, GENPASS_OPT},
    {"sb", no_argument, 0, SB_OPT},
    {"snapshot", required_argument, 0, 'S'},
    {"snapshot-alpha", no_argument, 0, SNAPSHOT_ALPHA_OPT},
    {"snapshot-format", required_argument, 0, SNAPSHOT_FORMAT_OPT},
    {"snapshot-interval", required_argument, 0, SNAPSHOT_INTERVAL_OPT},
    {"snapshot-prefix", required_argument, 0, 's'},
    {"snapshot-threaded", no_argument, 0, 't'},
    {"verbose", no_argument, 0, 'v'},
    {"wait", no_argument, 0, 'w'},
    {"loop", optional_argument, 0, LOOP_OPT},
    {"singlethread", no_argument, 0, SINGLETHREAD_OPT},
    {"ignore-retvals", no_argument, 0, IGNORE_RETVALS_OPT},
    {"no-context-check", no_argument, 0, NO_CONTEXT_CHECK},
    {0, 0, 0, 0}
};


static void exceptionCallback(void)
{
    std::cerr << retrace::callNo << ": error: caught an unhandled exception\n";
}


static bool
endsWith(const std::string &s1, const char *s2)
{
    size_t len = strlen(s2);
    return s1.length() >= len &&
           s1.compare(s1.length() - len, len, s2) == 0;
}


// Try to compensate for different OS
static void
adjustProcessName(const std::string &name)
{
    std::string adjustedName(name);

    if (adjustedName.length() > 2 && adjustedName[1] == ':') {
#ifndef _WIN32
        adjustedName.erase(0, 2);
#endif
    } else {
#ifdef _WIN32
        adjustedName.insert(0, "C:");
#endif
    }

    for (char &c: adjustedName) {
#ifdef _WIN32
        if (c == '/')  c = '\\';
#else
        if (c == '\\') c = '/';
#endif
    }

#ifndef _WIN32
    static const std::regex programFiles("/Program Files( \\(x86\\))?/", std::regex_constants::icase);
    adjustedName = std::regex_replace(adjustedName, programFiles, "/opt/");
#endif

    if (endsWith(adjustedName, ".exe")) {
#ifndef _WIN32
        adjustedName.resize(adjustedName.length() - strlen(".exe"));
#endif
    }  else {
#ifdef _WIN32
        adjustedName.append(".exe");
#endif
    }

    std::cerr << adjustedName << "\n";

    setProcessName(adjustedName.c_str());
}


#ifdef _WIN32

// Diagnose OOM
static int
new_failure_handler(size_t size)
{
    fprintf(stderr, "error: failed to allocate %zu bytes\n", size);

    // Describe the heap features:
    // 0 - default heap
    // 2 - low fragmentation heap
    // etc.
    ULONG HeapCompatibility;
    if (HeapQueryInformation(reinterpret_cast<HANDLE>(_get_heap_handle()),
                             HeapCompatibilityInformation,
                             &HeapCompatibility, sizeof HeapCompatibility, nullptr)) {
        fprintf(stderr, "info: heap features %lu\n", HeapCompatibility);
    }

#define MB (1024*1024)

    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof statex;
    if (GlobalMemoryStatusEx(&statex)) {
        fprintf(stderr, "info: %lu%% memory in use\n", statex.dwMemoryLoad);
        fprintf(stderr, "info: %llu total MB of physical memory\n", statex.ullTotalPhys/MB);
        fprintf(stderr, "info: %llu free  MB of physical memory\n", statex.ullAvailPhys/MB);
        fprintf(stderr, "info: %llu total MB of paging file\n", statex.ullTotalPageFile/MB);
        fprintf(stderr, "info: %llu free  MB of paging file\n", statex.ullAvailPageFile/MB);
        fprintf(stderr, "info: %llu total MB of virtual memory\n", statex.ullTotalVirtual/MB);
        fprintf(stderr, "info: %llu free  MB of virtual memory\n", statex.ullAvailVirtual/MB);
        fprintf(stderr, "info: %llu free  MB of extended memory\n", statex.ullAvailExtendedVirtual/MB);
    }

    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo( GetCurrentProcess(), &pmc, sizeof pmc)) {
        fprintf(stderr, "info: %lu page faults\n", pmc.PageFaultCount);
        fprintf(stderr, "info: %zu MB peak working set\n", size_t(pmc.PeakWorkingSetSize/MB));
        fprintf(stderr, "info: %zu MB working set\n", size_t(pmc.WorkingSetSize/MB));
        fprintf(stderr, "info: %zu MB quota peak paged pool usage\n", size_t(pmc.QuotaPeakPagedPoolUsage/MB));
        fprintf(stderr, "info: %zu MB quota paged pool usage\n", size_t(pmc.QuotaPagedPoolUsage/MB));
        fprintf(stderr, "info: %zu MB quota peak non-paged pool usage\n", size_t(pmc.QuotaPeakNonPagedPoolUsage/MB));
        fprintf(stderr, "info: %zu MB quota non-paged pool usage\n", size_t(pmc.QuotaNonPagedPoolUsage/MB));
        fprintf(stderr, "info: %zu MB page file used\n", size_t(pmc.PagefileUsage/MB));
        fprintf(stderr, "info: %zu MB peak page file used\n", size_t(pmc.PeakPagefileUsage/MB));
    }

    // Describe free virtual memory
    LPCVOID lpAddress = nullptr;
    MEMORY_BASIC_INFORMATION MBI;
    size_t NumFree = 0;
    size_t TotalFree = 0;
    size_t LargestFree = 0;
    while (VirtualQuery(lpAddress, &MBI, sizeof MBI) == sizeof MBI) {
        assert(MBI.RegionSize > 0);
        if (MBI.State == MEM_FREE) {
            ++NumFree;
            TotalFree += MBI.RegionSize;
            LargestFree = std::max(LargestFree, size_t(MBI.RegionSize));
        }
        if (LPCBYTE(lpAddress) >= (LPCBYTE)MBI.BaseAddress + MBI.RegionSize) {
            break;
        }
        lpAddress = (LPCBYTE)MBI.BaseAddress + MBI.RegionSize;
    }
    fprintf(stderr, "info: %zu MB virtual address free\n", TotalFree/MB);
    fprintf(stderr, "info: %zu free virtual address regions\n", NumFree);
    fprintf(stderr, "info: %zu MB largest free virtual address region\n", LargestFree/MB);

    return 0;
}

#endif


extern "C"
int main(int argc, char **argv)
{
    using namespace retrace;
    int loopCount = 0;
    int i;
    bool snapshotThreaded = false;

    os::setDebugOutput(os::OUTPUT_STDERR);

#ifdef _WIN32
#ifdef _MSC_VER
    // Hook new and malloc failures
    _set_new_handler(new_failure_handler);
    _set_new_mode(1);
#else
    (void)new_failure_handler;
#endif
#endif

    assert(snapshotFrequency.empty());

    int opt;
    while  ((opt = getopt_long_only(argc, argv, shortOptions, longOptions, NULL)) != -1) {
        switch (opt) {
        case 'h':
            usage(argv[0]);
            return 0;
        case 'b':
            retrace::debug = -1;
            retrace::verbosity = -1;
            break;
        case MARKERS_OPT:
            retrace::markers = true;
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
        case 'm':
            retrace::snapshotMRT = true;
            break;
        case MSAA_NO_RESOLVE_OPT:
            retrace::resolveMSAA = false;
            break;
        case SB_OPT:
            retrace::doubleBuffer = false;
            break;
        case SINGLETHREAD_OPT:
            retrace::singleThread = true;
            break;
        case IGNORE_RETVALS_OPT:
            retrace::ignoreRetvals = true;
            break;
        case NO_CONTEXT_CHECK:
            retrace::contextCheck = false;
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
        case SNAPSHOT_ALPHA_OPT:
            retrace::snapshotAlpha = true;
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
        case PLMETRICS_OPT:
            retrace::debug = 0;
            retrace::profiling = true;
            retrace::verbosity = -1;
            retrace::profilingWithBackends = true;
            retrace::profilingListMetrics = true;
            break;
        case GENPASS_OPT:
            retrace::debug = 0;
            retrace::profiling = true;
            retrace::verbosity = -1;
            retrace::profilingWithBackends = true;
            retrace::profilingNumPasses = true;
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
    if (retrace::profiling && !retrace::profilingWithBackends) {
        retrace::profiler.setup(retrace::profilingCpuTimes,
                                retrace::profilingGpuTimes,
                                retrace::profilingPixelsDrawn,
                                retrace::profilingMemoryUsage);
    }

    os::setExceptionCallback(exceptionCallback);

    for (retrace::curPass = 0; retrace::curPass < retrace::numPasses;
         retrace::curPass++)
    {
        for (i = optind; i < argc; ++i) {
            parser = new trace::Parser;
            if (loopCount) {
                parser = lastFrameLoopParser(parser, loopCount);
            }

            if (!parser->open(argv[i])) {
                return 1;
            }

            auto &properties = parser->getProperties();
            auto processNameIt = properties.find("process.name");
            if (processNameIt != properties.end()) {
                adjustProcessName(processNameIt->second);
            }

            retrace::mainLoop();

            parser->close();

            delete parser;
            parser = NULL;
        }
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


/*
 * Direct NVIDIA Optimus driver to use the High Performance Graphics.
 *
 * If we invoked glGetString(GL_VENDOR) or glGetString(GL_RENDERER) this
 * wouldn't be necessary, but glretrace skips such calls.
 *
 * See also:
 * - http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
 */
#ifdef _WIN32
extern "C" {
     __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif
