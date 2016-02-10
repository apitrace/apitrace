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

#include <sstream>
#include <string.h>
#include <limits.h> // for CHAR_MAX
#include <getopt.h>

#include <set>

#include "cli.hpp"

#include "os_string.hpp"

#include "trace_callset.hpp"
#include "trace_parser.hpp"
#include "trace_writer.hpp"

#include "cli_trim_auto_analyzer.hpp"

static const char *synopsis = "Create a new trace by automatically trimming unecessary calls from an existing trace.";

static void
usage(void)
{
    std::cout
        << "usage: apitrace trim-auto [OPTIONS] TRACE_FILE...\n"
        << synopsis << "\n"
        "\n"
        "    -h, --help               Show this help message and exit\n"
        "\n"
        "        --calls=CALLSET      Include specified calls in the trimmed output.\n"
        "        --frames=FRAMESET    Include specified frames in the trimmed output.\n"
        "\n"
        "        --deps               Perform dependency analysis and include dependent\n"
        "                             calls as needed, (even if those calls were not\n"
        "                             explicitly requested with --calls or --frames).\n"
        "                             (On by default. See --no-deps or --exact)\n"
        "\n"
        "        --no-deps            Do not perform dependency analysis. Output will\n"
        "                             not include any additional calls beyond those\n"
        "                             explicitly requested with --calls or --frames).\n"
        "\n"
        "        --prune              Omit calls with no side effects, even if the call\n"
        "                             is within the range specified by --calls/--frames.\n"
        "                             (On by default. See --no-prune or --exact)\n"
        "\n"
        "        --no-prune           Never omit any calls from the range specified\n"
        "                             --calls/--frames.\n"
        "\n"
        "        --print-callset      Print to stdout the final set of calls included\n"
        "                             in the trim output. This can be useful for\n"
        "                             tweaking the trimmed callset from --auto on the\n"
        "                             command-line.\n"
        "                             Use --calls=@FILE to read callset from a file.\n"
        "\n"
        "        --trim-spec=SPEC     Specifies which classes of calls will be trimmed.\n"
        "                             This option only has an effect if dependency\n"
        "                             analysis is enabled. The argument is a comma-\n"
        "                             separated list of names from the following:\n"
        "\n"
	"                               no-side-effects  Calls with no side effects\n"
	"                               textures         Calls to setup unused textures\n"
	"                               shaders          Calls to setup unused shaders\n"
        "                               drawing          Calls that draw\n"
        "\n"
        "                             The default trim specification includes all of\n"
        "                             the above, (as much as possible will be trimmed).\n"
        "\n"
        "        --thread=THREAD_ID   Only retain calls from specified thread\n"
        "\n"
        "    -o, --output=TRACE_FILE  Output trace file\n"
        "\n"
    ;
}

enum {
    CALLS_OPT = CHAR_MAX + 1,
    FRAMES_OPT,
    DEPS_OPT,
    NO_DEPS_OPT,
    PRUNE_OPT,
    NO_PRUNE_OPT,
    THREAD_OPT,
    PRINT_CALLSET_OPT,
    TRIM_SPEC_OPT
};

const static char *
shortOptions = "ho:x";

const static struct option
longOptions[] = {
    {"help", no_argument, 0, 'h'},
    {"calls", required_argument, 0, CALLS_OPT},
    {"frames", required_argument, 0, FRAMES_OPT},
    {"deps", no_argument, 0, DEPS_OPT},
    {"no-deps", no_argument, 0, NO_DEPS_OPT},
    {"prune", no_argument, 0, PRUNE_OPT},
    {"no-prune", no_argument, 0, NO_PRUNE_OPT},
    {"thread", required_argument, 0, THREAD_OPT},
    {"output", required_argument, 0, 'o'},
    {"print-callset", no_argument, 0, PRINT_CALLSET_OPT},
    {"trim-spec", required_argument, 0, TRIM_SPEC_OPT},
    {0, 0, 0, 0}
};

struct stringCompare {
    bool operator() (const char *a, const char *b) const {
        return strcmp(a, b) < 0;
    }
};

struct trim_auto_options {
    /* Calls to be included in trace. */
    trace::CallSet calls;

    /* Frames to be included in trace. */
    trace::CallSet frames;

    /* Whether dependency analysis should be performed. */
    bool dependency_analysis;

    /* Whether uninteresting calls should be pruned.. */
    bool prune_uninteresting;

    /* Output filename */
    std::string output;

    /* Emit only calls from this thread (-1 == all threads) */
    int thread;

    /* Print resulting callset */
    int print_callset;

    /* What kind of trimming to perform. */
    TrimFlags trim_flags;
};

static int
trim_trace(const char *filename, struct trim_auto_options *options)
{
    trace::ParseBookmark beginning;
    trace::Parser p;
    TraceAnalyzer analyzer(options->trim_flags);
    trace::FastCallSet *required;
    unsigned frame;
    int call_range_first, call_range_last;

    if (!p.open(filename)) {
        std::cerr << "error: failed to open " << filename << "\n";
        return 1;
    }

    /* Mark the beginning so we can return here for pass 2. */
    p.getBookmark(beginning);

    /* In pass 1, analyze which calls are needed. */
    frame = 0;
    trace::Call *call;
    while ((call = p.parse_call())) {

        /* There's no use doing any work past the last call and frame
         * requested by the user. */
        if ((options->calls.empty() || call->no > options->calls.getLast()) &&
            (options->frames.empty() || frame > options->frames.getLast())) {

            delete call;
            break;
        }

        /* If requested, ignore all calls not belonging to the specified thread. */
        if (options->thread != -1 && call->thread_id != options->thread) {
            goto NEXT;
        }

        /* Also, prune if no side effects (unless the user asked for no pruning. */
        if (options->prune_uninteresting && call->flags & trace::CALL_FLAG_NO_SIDE_EFFECTS) {
            goto NEXT;
        }

        /* If this call is included in the user-specified call set,
         * then require it (and all dependencies) in the trimmed
         * output. */
        if (options->calls.contains(*call) ||
            options->frames.contains(frame, call->flags)) {

            analyzer.require(call);
        }

        /* Regardless of whether we include this call or not, we do
         * some dependency tracking (unless disabled by the user). We
         * do this even for calls we have included in the output so
         * that any state updates get performed. */
        if (options->dependency_analysis) {
            analyzer.analyze(call);
        }

    NEXT:
        if (call->flags & trace::CALL_FLAG_END_FRAME)
            frame++;

        delete call;
    }

    /* Prepare output file and writer for output. */
    if (options->output.empty()) {
        os::String base(filename);
        base.trimExtension();

        options->output = std::string(base.str()) + std::string("-trim.trace");
    }

    trace::Writer writer;
    if (!writer.open(options->output.c_str())) {
        std::cerr << "error: failed to create " << options->output << "\n";
        return 1;
    }

    /* Reset bookmark for pass 2. */
    p.setBookmark(beginning);

    /* In pass 2, emit the calls that are required. */
    required = analyzer.get_required();

    frame = 0;
    call_range_first = -1;
    call_range_last = -1;
    while ((call = p.parse_call())) {

        /* There's no use doing any work past the last call and frame
         * requested by the user. */
        if ((options->calls.empty() || call->no > options->calls.getLast()) &&
            (options->frames.empty() || frame > options->frames.getLast())) {

            delete call;
            break;
        }

        if (required->contains(call->no)) {
            writer.writeCall(call);

            if (options->print_callset) {
                if (call_range_first < 0) {
                    call_range_first = call->no;
                    printf ("%d", call_range_first);
                } else if (call->no != call_range_last + 1) {
                    if (call_range_last != call_range_first)
                        printf ("-%d", call_range_last);
                    call_range_first = call->no;
                    printf (",%d", call_range_first);
                }
                call_range_last = call->no;
            }
        }

        if (call->flags & trace::CALL_FLAG_END_FRAME) {
            frame++;
        }

        delete call;
    }

    if (options->print_callset) {
        if (call_range_last != call_range_first)
            printf ("-%d\n", call_range_last);
    }

    std::cerr << "Trimmed trace is available as " << options->output << "\n";

    return 0;
}

static int
parse_trim_spec(const char *trim_spec, TrimFlags *flags)
{
    std::string spec(trim_spec), word;
    size_t start = 0, comma = 0;
    *flags = 0;

    while (start < spec.size()) {
        comma = spec.find(',', start);

        if (comma == std::string::npos)
            word = std::string(spec, start);
        else
            word = std::string(spec, start, comma - start);

        if (strcmp(word.c_str(), "no-side-effects") == 0)
            *flags |= TRIM_FLAG_NO_SIDE_EFFECTS;
        else if (strcmp(word.c_str(), "textures") == 0)
            *flags |= TRIM_FLAG_TEXTURES;
        else if (strcmp(word.c_str(), "shaders") == 0)
            *flags |= TRIM_FLAG_SHADERS;
        else if (strcmp(word.c_str(), "drawing") == 0)
            *flags |= TRIM_FLAG_DRAWING;
        else {
            return 1;
        }

        if (comma == std::string::npos)
            break;

        start = comma + 1;
    }

    return 0;
}

static int
command(int argc, char *argv[])
{
    struct trim_auto_options options;

    options.calls = trace::CallSet(trace::FREQUENCY_NONE);
    options.frames = trace::CallSet(trace::FREQUENCY_NONE);
    options.dependency_analysis = true;
    options.prune_uninteresting = true;
    options.output = "";
    options.thread = -1;
    options.print_callset = 0;
    options.trim_flags = -1;

    int opt;
    while ((opt = getopt_long(argc, argv, shortOptions, longOptions, NULL)) != -1) {
        switch (opt) {
        case 'h':
            usage();
            return 0;
        case CALLS_OPT:
            options.calls.merge(optarg);
            break;
        case FRAMES_OPT:
            options.frames.merge(optarg);
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
        case THREAD_OPT:
            options.thread = atoi(optarg);
            break;
        case 'o':
            options.output = optarg;
            break;
        case PRINT_CALLSET_OPT:
            options.print_callset = 1;
            break;
        case TRIM_SPEC_OPT:
            if (parse_trim_spec(optarg, &options.trim_flags)) {
                std::cerr << "error: illegal value for trim-spec: " << optarg << "\n";
                std::cerr << "See \"apitrace help trim\" for help.\n";
                return 1;
            }
            break;
        default:
            std::cerr << "error: unexpected option `" << (char)opt << "`\n";
            usage();
            return 1;
        }
    }

    /* If neither of --calls nor --frames was set, default to the
     * entire set of calls. */
    if (options.calls.empty() && options.frames.empty()) {
        options.calls = trace::CallSet(trace::FREQUENCY_ALL);
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

    if (options.dependency_analysis) {
        std::cerr <<
            "Note: The dependency analysis in \"apitrace trim-auto\" is still experimental.\n"
            "      We hope that it will be useful, but it may lead to incorrect results.\n"
            "      If you find a trace that misbehaves while trimming, please share that\n"
            "      by sending email to apitrace@lists.freedesktop.org, cworth@cworth.org\n";
    }

    return trim_trace(argv[optind], &options);
}

const Command trim_auto_command = {
    "trim-auto",
    synopsis,
    usage,
    command
};
