/**************************************************************************
 *
 * Copyright 2010 VMware, Inc.
 * Copyright 2011 Intel corporation
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
#include <getopt.h>

#include <GL/gl.h>
#include <GL/glext.h>

#include <set>

#include "cli.hpp"

#include "os_string.hpp"

#include "trace_callset.hpp"
#include "trace_parser.hpp"
#include "trace_writer.hpp"

static const char *synopsis = "Create a new trace by trimming an existing trace.";

static void
usage(void)
{
    std::cout
        << "usage: apitrace trim [OPTIONS] TRACE_FILE...\n"
        << synopsis << "\n"
        "\n"
        "    -h, --help               Show detailed help for trim options and exit\n"
        "        --calls=CALLSET      Include specified calls in the trimmed output.\n"
        "        --deps               Include additional calls to satisfy dependencies\n"
        "        --no-deps            Do not include calls from dependency analysis\n"
        "        --prune              Omit uninteresting calls from the trace output\n"
        "        --no-prune           Do not prune uninteresting calls from the trace.\n"
        "    -x, --exact              Include exactly the calls specified in --calls\n"
        "                             Equivalent to both --no-deps and --no-prune\n"
        "        --thread=THREAD_ID   Only retain calls from specified thread\n"
        "    -o, --output=TRACE_FILE  Output trace file\n"
    ;
}

static void
help()
{
    std::cout
        << "usage: apitrace trim [OPTIONS] TRACE_FILE...\n"
        << synopsis << "\n"
        "\n"
        "    -h, --help               Show this help message and exit\n"
        "\n"
        "        --calls=CALLSET      Include specified calls in the trimmed output.\n"
        "                             Note that due to dependency analysis and pruning\n"
        "                             of uninteresting calls the resulting trace may\n"
        "                             include more and less calls than specified.\n"
        "                             See --no-deps, --no-prune, and --exact to change\n"
        "                             this behavior.\n"
        "\n"
        "        --deps               Perform dependency analysis and include dependent\n"
        "                             calls as needed, (even if those calls were not\n"
        "                             explicitly requested with --calls). This is the\n"
        "                             default behavior. See --no-deps and --exact.\n"
        "\n"
        "        --no-deps            Do not perform dependency analysis. In this mode\n"
        "                             the trimmed trace will never include calls from\n"
        "                             outside the range specified in --calls.\n"
        "\n"
        "        --prune              Omit calls that have no side effects, even if the\n"
        "                             call is within the range specified by --calls.\n"
        "                             This is the default behavior. See --no-prune\n"
        "\n"
        "        --no-prune           Do not prune uninteresting calls from the trace.\n"
        "                             In this mode the trimmed trace will never omit\n"
        "                             any calls within the range specified in --calls.\n"
        "\n"
        "    -x, --exact              Trim the trace to exactly the calls specified in\n"
        "                             --calls. This option is equivalent to passing\n"
        "                             both --no-deps and --no-prune.\n"
        "\n"
        "        --thread=THREAD_ID   Only retain calls from specified thread\n"
        "\n"
        "    -o, --output=TRACE_FILE  Output trace file\n"
        "\n"
    ;
}

enum {
    CALLS_OPT = CHAR_MAX + 1,
    DEPS_OPT,
    NO_DEPS_OPT,
    PRUNE_OPT,
    NO_PRUNE_OPT,
    THREAD_OPT,
};

const static char *
shortOptions = "ho:x";

const static struct option
longOptions[] = {
    {"help", no_argument, 0, 'h'},
    {"calls", required_argument, 0, CALLS_OPT},
    {"deps", no_argument, 0, DEPS_OPT},
    {"no-deps", no_argument, 0, NO_DEPS_OPT},
    {"prune", no_argument, 0, PRUNE_OPT},
    {"no-prune", no_argument, 0, NO_PRUNE_OPT},
    {"exact", no_argument, 0, 'x'},
    {"thread", required_argument, 0, THREAD_OPT},
    {"output", required_argument, 0, 'o'},
    {0, 0, 0, 0}
};

struct stringCompare {
    bool operator() (const char *a, const char *b) const {
        return strcmp(a, b) < 0;
    }
};

class TraceAnalyzer {
    /* Map for tracking resource dependencies between calls. */
    std::map<const char *, std::set<unsigned>, stringCompare > resources;

    /* The final set of calls required. This consists of calls added
     * explicitly with the require() method as well as all calls
     * implicitly required by those through resource dependencies. */
    std::set<unsigned> required;

    bool transformFeedbackActive;
    bool framebufferObjectActive;
    bool insideBeginEnd;

    /* Rendering often has no side effects, but it can in some cases,
     * (such as when transform feedback is active, or when rendering
     * targets a framebuffer object). */
    bool renderingHasSideEffect() {
        return transformFeedbackActive || framebufferObjectActive;
    }

    /* Provide: Record that the given call affects the given resource
     * as a side effect. */
    void provide(const char *resource, trace::CallNo call_no) {
        resources[resource].insert(call_no);
    }

    /* Consume: Add all calls that provide the given resource to the
     * required list, then clear the list for this resource. */
    void consume(const char *resource) {

        std::set<unsigned> *calls;
        std::set<unsigned>::iterator call;

        /* Insert as required all calls that provide 'resource',
         * then clear these calls. */
        if (resources.count(resource)) {
            calls = &resources[resource];
            for (call = calls->begin(); call != calls->end(); call++) {
                required.insert(*call);
            }
            resources.erase(resource);
        }
    }

    void stateTrackPreCall(trace::Call *call) {

        if (strcmp(call->name(), "glBegin") == 0) {
            insideBeginEnd = true;
            return;
        }

        if (strcmp(call->name(), "glBeginTransformFeedback") == 0) {
            transformFeedbackActive = true;
            return;
        }

        if (strcmp(call->name(), "glBindFramebuffer") == 0) {
            GLenum target;
            GLuint framebuffer;

            target = static_cast<GLenum>(call->arg(0).toSInt());
            framebuffer = call->arg(1).toUInt();

            if (target == GL_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER) {
                if (framebuffer == 0) {
                    framebufferObjectActive = false;
                } else {
                    framebufferObjectActive = true;
                }
            }
            return;
        }
    }

    void stateTrackPostCall(trace::Call *call) {

        if (strcmp(call->name(), "glEnd") == 0) {
            insideBeginEnd = false;
            return;
        }

        if (strcmp(call->name(), "glEndTransformFeedback") == 0) {
            transformFeedbackActive = false;
            return;
        }

        if (call->flags & trace::CALL_FLAG_SWAP_RENDERTARGET &&
            call->flags & trace::CALL_FLAG_END_FRAME) {
            resources.erase("framebuffer");
            return;
        }
    }

    void recordSideEffects(trace::Call *call) {
        /* If call is flagged as no side effects, then we are done here. */
        if (call->flags & trace::CALL_FLAG_NO_SIDE_EFFECTS) {
            return;
        }

        /* Similarly, swap-buffers calls don't have interesting side effects. */
        if (call->flags & trace::CALL_FLAG_SWAP_RENDERTARGET &&
            call->flags & trace::CALL_FLAG_END_FRAME) {
            return;
        }

        /* Handle all rendering operations, (even though only glEnd is
         * flagged as a rendering operation we treat everything from
         * glBegin through glEnd as a rendering operation). */
        if (call->flags & trace::CALL_FLAG_RENDER ||
            insideBeginEnd) {

            provide("framebuffer", call->no);

            /* In some cases, rendering has side effects beyond the
             * framebuffer update. */
            if (renderingHasSideEffect()) {
                provide("state", call->no);
            }

            return;
        }

        /* By default, assume this call affects the state somehow. */
        resources["state"].insert(call->no);
    }

    void requireDependencies(trace::Call *call) {

        /* Swap-buffers calls depend on framebuffer state. */
        if (call->flags & trace::CALL_FLAG_SWAP_RENDERTARGET &&
            call->flags & trace::CALL_FLAG_END_FRAME) {
            consume("framebuffer");
        }

        /* By default, just assume this call depends on generic state. */
        consume("state");
    }


public:
    TraceAnalyzer(): transformFeedbackActive(false),
                     framebufferObjectActive(false),
                     insideBeginEnd(false)
    {}

    ~TraceAnalyzer() {}

    /* Analyze this call by tracking state and recording all the
     * resources provided by this call as side effects.. */
    void analyze(trace::Call *call) {

        stateTrackPreCall(call);

        recordSideEffects(call);

        stateTrackPostCall(call);
    }

    /* Require this call and all of its dependencies to be included in
     * the final trace. */
    void require(trace::Call *call) {

        /* First, find and insert all calls that this call depends on. */
        requireDependencies(call);

        /* Then insert this call itself. */
        required.insert(call->no);
    }

    /* Return a set of all the required calls, (both those calls added
     * explicitly with require() and those implicitly depended
     * upon. */
    std::set<unsigned> *get_required(void) {
        return &required;
    }
};

struct trim_options {
    /* Calls to be included in trace. */
    trace::CallSet calls;

    /* Whether dependency analysis should be performed. */
    bool dependency_analysis;

    /* Whether uninteresting calls should be pruned.. */
    bool prune_uninteresting;

    /* Output filename */
    std::string output;

    /* Emit only calls from this thread (-1 == all threads) */
    int thread;
};

static int
trim_trace(const char *filename, struct trim_options *options)
{
    trace::ParseBookmark beginning;
    trace::Parser p;
    TraceAnalyzer analyzer;
    std::set<unsigned> *required;

    if (!p.open(filename)) {
        std::cerr << "error: failed to open " << filename << "\n";
        return 1;
    }

    /* Mark the beginning so we can return here for pass 2. */
    p.getBookmark(beginning);

    /* In pass 1, analyze which calls are needed. */
    trace::Call *call;
    while ((call = p.parse_call())) {

        /* There's no use doing any work past the last call requested
         * by the user. */
        if (call->no > options->calls.getLast())
            break;

        /* If requested, ignore all calls not belonging to the specified thread. */
        if (options->thread != -1 && call->thread_id != options->thread)
            continue;

        /* Also, prune if uninteresting (unless the user asked for no pruning. */
        if (options->prune_uninteresting && call->flags & trace::CALL_FLAG_UNINTERESTING) {
            continue;
        }

        /* If this call is included in the user-specified call set,
         * then require it (and all dependencies) in the trimmed
         * output. */
        if (options->calls.contains(*call)) {
            analyzer.require(call);
        }

        /* Regardless of whether we include this call or not, we do
         * some dependency tracking (unless disabled by the user). We
         * do this even for calls we have included in the output so
         * that any state updates get performed. */
        if (options->dependency_analysis) {
            analyzer.analyze(call);
        }
    }

    /* Prepare output file and writer for output. */
    if (options->output.empty()) {
        os::String base(filename);
        base.trimExtension();

        options->output = std::string(base.str()) + std::string("-trim.trace");
    }

    trace::Writer writer;
    if (!writer.open(options->output.c_str())) {
        std::cerr << "error: failed to create " << filename << "\n";
        return 1;
    }

    /* Reset bookmark for pass 2. */
    p.setBookmark(beginning);

    /* In pass 2, emit the calls that are required. */
    required = analyzer.get_required();

    while ((call = p.parse_call())) {

        /* There's no use doing any work past the last call requested
         * by the user. */
        if (call->no > options->calls.getLast())
            break;

        if (required->find(call->no) != required->end()) {
            writer.writeCall(call);
        }
        delete call;
    }

    std::cout << "Trimmed trace is available as " << options->output << "\n";

    return 0;
}

static int
command(int argc, char *argv[])
{
    struct trim_options options;

    options.calls = trace::CallSet(trace::FREQUENCY_ALL);
    options.dependency_analysis = true;
    options.prune_uninteresting = true;
    options.output = "";
    options.thread = -1;

    int opt;
    while ((opt = getopt_long(argc, argv, shortOptions, longOptions, NULL)) != -1) {
        switch (opt) {
        case 'h':
            help();
            return 0;
        case CALLS_OPT:
            options.calls = trace::CallSet(optarg);
            break;
        case DEPS_OPT:
            options.dependency_analysis = true;
            break;
        case NO_DEPS_OPT:
            options.dependency_analysis = false;
            break;
        case PRUNE_OPT:
            options.prune_uninteresting = true;
            break;
        case NO_PRUNE_OPT:
            options.prune_uninteresting = false;
            break;
        case 'x':
            options.dependency_analysis = false;
            options.prune_uninteresting = false;
            break;
        case THREAD_OPT:
            options.thread = atoi(optarg);
            break;
        case 'o':
            options.output = optarg;
            break;
        default:
            std::cerr << "error: unexpected option `" << opt << "`\n";
            usage();
            return 1;
        }
    }

    if (optind >= argc) {
        std::cerr << "error: apitrace trim requires a trace file as an argument.\n";
        usage();
        return 1;
    }

    if (argc > optind + 1) {
        std::cerr << "error: extraneous arguments:";
        for (int i = optind + 1; i < argc; i++) {
            std::cerr << " " << argv[i];
        }
        std::cerr << "\n";
        usage();
        return 1;
    }

    return trim_trace(argv[optind], &options);
}

const Command trim_command = {
    "trim",
    synopsis,
    help,
    command
};
