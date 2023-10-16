/*********************************************************************
 *
 * Copyright 2020 Collabora Ltd
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *********************************************************************/

#include "ft_frametrimmer.hpp"

#include "os_time.hpp"
#include "trace_parser.hpp"
#include "retrace.hpp"
#include "retrace_swizzle.hpp"

#include "trace_callset.hpp"
#include "trace_parser.hpp"
#include "trace_writer.hpp"

#include <limits.h> // for CHAR_MAX
#include <getopt.h>
#include <memory>
#include <queue>

using namespace frametrim;

struct trim_options {
    /* Frames to be included in trace. */
    trace::CallSet setupframes;

    /* Frames to keep replayable */
    trace::CallSet frames;

    unsigned top_frame_call_counts;
    bool keep_all_states;
    bool swap_to_finish;

    /* Output filename */
    std::string output;
};

static const char *synopsis = "Create a new, retracable trace containing only the specified frames.";

static void
usage(void)
{
    std::cout
            << "usage: frametrim [OPTIONS] TRACE_FILE...\n"
            << synopsis << "\n"
                           "\n"
                           "    -h, --help               Show detailed help for trim options and exit\n"
                           "    -f, --frames=FRAME       Frame the trace should be reduced to.\n"
                           "    -s, --setupframes=FRAME  Frame that are kept in the trace but but without the end-of-frame command.\n"
                           "    -t, --top-calls-per-frame=NUMBER Print NUMBER of frames with the top amount of OpenGL calls\n"
                           "    -k, --keep-all-states    Keep all state calls in the trace (This may help with textures that are created by using FBO\n"
                           "    -F, --swap-to-finish     Replace swaps in the setup frame with glFinish\n"
                           "    -o, --output=TRACE_FILE  Output trace file\n"
               ;
}

enum {
    FRAMES_OPT = 'f',
    SETUPFRAMES_OPT = 's'
};

const static char *
shortOptions = "t:hkFo:f:s:x";

bool operator < (std::pair<unsigned, unsigned>& lhs, std::pair<unsigned, unsigned>& rhs)
{
    return lhs.first < rhs.first;
}

const static struct option
longOptions[] = {
    {"help", no_argument, 0, 'h'},
    {"top-calls-per-frame", required_argument, 0, 't'},
    {"frames", required_argument, 0, 'f'},
    {"setupframes", required_argument, 0, 's'},
    {"keep-all-states", no_argument, 0, 'k'},
    {"swap-to-finish", no_argument, 0, 'F'},
    {"output", required_argument, 0, 'o'},
    {0, 0, 0, 0}
};

static int trim_to_frame(const char *filename,
                         const struct trim_options& options)
{

    trace::Parser p;
    unsigned frame;

    if (!p.open(filename)) {
        std::cerr << "error: failed to open " << filename << "\n";
        return 1;
    }

    if (options.frames.getLast() < options.setupframes.getLast() &&
        !options.setupframes.empty()) {
        std::cerr << "error: last frame to keep ("
                  << options.frames.getLast()
                  << ") must be larger than last key frame"
                  << options.setupframes.getLast() << "\n";
        return 1;
    }

    auto out_filename = options.output;

    /* Prepare output file and writer for output. */
    if (options.output.empty()) {
        os::String base(filename);
        base.trimExtension();

        out_filename = std::string(base.str()) + std::string("-trim.trace");
    }

    frame = 0;
    uint64_t callid = 0;
    std::unique_ptr<trace::Call> call(p.parse_call());
    std::priority_queue<std::pair<unsigned, unsigned>> calls_in_frame;

    /* Determine whether the trace API is supported */
    while (call && p.api == trace::API_UNKNOWN)
        call.reset(p.parse_call());
    if (!FrameTrimmer::isSupported(p.api)) {
        std::cerr << "error: unsupported API" << std::endl;
        return 1;
    }
    p.close();
    p.open(filename);
    call.reset(p.parse_call());

    auto trimmer = FrameTrimmer::create(p.api, options.keep_all_states, options.swap_to_finish);

    unsigned calls_in_this_frame = 0;
    uint32_t last_frame_start = 0;

    while (call) {
        /* There's no use doing any work past the last call and frame
        * requested by the user. */
        if (frame > options.frames.getLast()) {
            break;
        }

        Frametype ft = ft_none;
        if (options.setupframes.contains(frame, call->flags))
            ft = ft_key_frame;
        if (options.frames.contains(frame, call->flags)) {
            ft = ft_retain_frame;
            if ((last_frame_start == 0) && frame == options.frames.getLast()) {
                last_frame_start = call->no - 1;
                trimmer->start_last_frame(last_frame_start);
            }
        }

        trimmer->call(*call, ft);

        if (call->flags & trace::CALL_FLAG_END_FRAME) {
            if (options.top_frame_call_counts > 0) {
                calls_in_frame.push(std::make_pair(calls_in_this_frame, frame));
            }
            calls_in_this_frame = 0;
            frame++;
        }

        callid++;
        if (!(callid & 0xff))
            std::cerr << "\rScanning frame:" << frame
                      << " type:" << ft
                      << " call:" << call->no;

        call.reset(p.parse_call());
        ++calls_in_this_frame;
    }

    trimmer->end_last_frame();
    auto skip_loop_calls = trimmer->get_skip_loop_calls();
    auto swap_calls = trimmer->get_swap_to_finish_calls();

    std::cerr << "\nDone scanning frames\n";

    trace::Writer writer;
    if (!writer.open(out_filename.c_str(), p.getVersion(), p.getProperties())) {
        std::cerr << "error: failed to create " << out_filename << "\n";
        return 2;
    }

    auto call_ids = trimmer->getUniqueCallIds();
    std::cerr << "Write output file\n";

    p.close();
    p.open(filename);
    call.reset(p.parse_call());

    std::cerr << "Copying " << call_ids.size() << " calls\n";
    std::cerr << "Write calls before " << last_frame_start << " and setup calls from last frame\n";

    int call_id = 0;
    const trace::FunctionSig glFinishSig = {0, "glFinish", 0, NULL};

    while (1) {
        while (call && call_ids.find(call->no) == call_ids.end())
            call.reset(p.parse_call());

        if (!call)
            break;

        // Write setup calls in last frame  before last frame starts
        if (call->no < last_frame_start ||
                skip_loop_calls.find(call->no) != skip_loop_calls.end()) {

            if (options.swap_to_finish &&
                    swap_calls.find(call->no) != swap_calls.end()) {
                call.reset(new trace::Call(&glFinishSig, 0, call->thread_id));
            }

            call->no = call_id++;
            writer.writeCall(call.get());
        }
        call.reset(p.parse_call());
    }

    // Now write the last frame without the setup calls
    p.close();
    p.open(filename);
    call.reset(p.parse_call());

    std::cerr << "Write calls after " << last_frame_start << " without setup calls\n";
    while (1) {
        while (call &&
               (call->no < last_frame_start ||
                call_ids.find(call->no) == call_ids.end()))
            call.reset(p.parse_call());

        if (!call)
            break;

        if (skip_loop_calls.find(call->no) == skip_loop_calls.end()) {
            call->no = call_id++;
            writer.writeCall(call.get());
        }
        call.reset(p.parse_call());
    }

    if (options.top_frame_call_counts) {
        unsigned count = options.top_frame_call_counts;
        std::cerr << "Calls per frame:\n";
        while (count-- && !calls_in_frame.empty()) {
            auto fc = calls_in_frame.top();
            calls_in_frame.pop();
            std::cerr << "  Frame[" << fc.second << "] = " << fc.first << "\n";
        }
    }

    return 0;
}


int main(int argc, char **argv)
{
    struct trim_options options;
    options.frames = trace::CallSet(trace::FREQUENCY_NONE);
    options.top_frame_call_counts = false;
    options.keep_all_states = false;
    options.swap_to_finish = false;

    int opt;
    while ((opt = getopt_long(argc, argv, shortOptions, longOptions, nullptr)) != -1) {
        switch (opt) {
        case 'h':
            usage();
            return 0;
        case FRAMES_OPT:
            options.frames.merge(optarg);
            break;
        case SETUPFRAMES_OPT:
            options.setupframes.merge(optarg);
            break;
        case 'o':
            options.output = optarg;
            break;
        case 't':
            options.top_frame_call_counts = atoi(optarg);
            break;
        case 'k':
            options.keep_all_states = true;
            break;
        case 'F':
            options.swap_to_finish = true;
            break;
        default:
            std::cerr << "error: unexpected option `" << (char)opt << "`\n";
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

    std::cerr << "gltrim may not retain all information required to recreate\n"
              << "the target frame(s) as expected, it is advised to check the\n"
              << "results and add setup frames and additional states if needed.\n"
                 "For further details see frametrim/frametrim.markdown in the "
                 "source code.\n";

    return trim_to_frame(argv[optind], options);
}
