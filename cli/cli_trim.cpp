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

#include <GL/gl.h>
#include <GL/glext.h>

#include <set>

#include "cli.hpp"

#include "os_string.hpp"

#include "trace_callset.hpp"
#include "trace_parser.hpp"
#include "trace_writer.hpp"

#define STRNCMP_LITERAL(var, literal) strncmp((var), (literal), sizeof (literal) -1)

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
        "        --frames=FRAMESET    Include specified frames in the trimmed output.\n"
        "        --deps               Include additional calls to satisfy dependencies\n"
        "        --no-deps            Do not include calls from dependency analysis\n"
        "        --prune              Omit uninteresting calls from the trace output\n"
        "        --no-prune           Do not prune uninteresting calls from the trace.\n"
        "    -x, --exact              Trim exactly to calls specified in --calls/--frames\n"
        "                             Equivalent to both --no-deps and --no-prune\n"
        "        --print-callset      Print the final set of calls included in output\n"
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
        "        --frames=FRAMESET    Include specified frames in the trimmed output.\n"
        "                             Note that due to dependency analysis and pruning\n"
        "                             of uninteresting calls the resulting trace may\n"
        "                             include more and less calls than specified.\n"
        "                             See --no-deps, --no-prune, and --exact to change\n"
        "                             this behavior.\n"
        "\n"
        "        --deps               Perform dependency analysis and include dependent\n"
        "                             calls as needed, (even if those calls were not\n"
        "                             explicitly requested with --calls or --frames).\n"
        "                             This is the default behavior. See --no-deps and\n"
        "                             --exact to change the behavior.\n"
        "\n"
        "        --no-deps            Do not perform dependency analysis. In this mode\n"
        "                             the trimmed trace will never include calls from\n"
        "                             outside what is specified in --calls or --frames.\n"
        "\n"
        "        --prune              Omit calls with no side effects, even if the call\n"
        "                             is within the range specified by --calls/--frames.\n"
        "                             This is the default behavior. See --no-prune.\n"
        "\n"
        "        --no-prune           Do not prune uninteresting calls from the trace.\n"
        "                             In this mode the trimmed trace will never omit\n"
        "                             any calls within the user-specified range.\n"
        "\n"
        "    -x, --exact              Trim the trace to exactly the calls specified in\n"
        "                             --calls and --frames. This option is equivalent\n"
        "                             to passing both --no-deps and --no-prune.\n"
        "\n"
        "        --print-callset      Print to stdout the final set of calls included\n"
        "                             in the trim output. This can be useful for\n"
        "                             debugging trim operations by using a modified\n"
        "                             callset on the command-line along with --exact.\n"
        "                             Use --calls=@<file> to read callset from a file.\n"
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
    {"exact", no_argument, 0, 'x'},
    {"thread", required_argument, 0, THREAD_OPT},
    {"output", required_argument, 0, 'o'},
    {"print-callset", no_argument, 0, PRINT_CALLSET_OPT},
    {0, 0, 0, 0}
};

struct stringCompare {
    bool operator() (const char *a, const char *b) const {
        return strcmp(a, b) < 0;
    }
};

class TraceAnalyzer {
    /* Maps for tracking resource dependencies between calls. */
    std::map<std::string, std::set<unsigned> > resources;
    std::map<std::string, std::set<std::string> > dependencies;

    /* Maps for tracking OpenGL state. */
    std::map<GLenum, unsigned> texture_map;

    /* The final set of calls required. This consists of calls added
     * explicitly with the require() method as well as all calls
     * implicitly required by those through resource dependencies. */
    std::set<unsigned> required;

    bool transformFeedbackActive;
    bool framebufferObjectActive;
    bool insideBeginEnd;
    GLuint activeProgram;

    /* Rendering often has no side effects, but it can in some cases,
     * (such as when transform feedback is active, or when rendering
     * targets a framebuffer object). */
    bool renderingHasSideEffect() {
        return transformFeedbackActive || framebufferObjectActive;
    }

    /* Provide: Record that the given call affects the given resource
     * as a side effect. */
    void provide(std::string resource, trace::CallNo call_no) {
        resources[resource].insert(call_no);
    }

    /* Like provide, but with a simply-formatted string, (appending an
     * integer to the given string). */
    void providef(std::string resource, int resource_no, trace::CallNo call_no) {
        std::stringstream ss;
        ss << resource << resource_no;
        provide(ss.str(), call_no);
    }

    /* Link: Establish a dependency between resource 'resource' and
     * resource 'dependency'. This dependency is captured by name so
     * that if the list of calls that provide 'dependency' grows
     * before 'resource' is consumed, those calls will still be
     * captured. */
    void link(std::string resource, std::string dependency) {
        dependencies[resource].insert(dependency);
    }

    /* Like link, but with a simply-formatted string, (appending an
     * integer to the given string). */
    void linkf(std::string resource, std::string dependency, int dep_no) {

        std::stringstream ss;
        ss << dependency << dep_no;
        link(resource, ss.str());
    }

    /* Unlink: Remove dependency from 'resource' on 'dependency'. */
    void unlink(std::string resource, std::string dependency) {
        dependencies[resource].erase(dependency);
        if (dependencies[resource].size() == 0) {
            dependencies.erase(resource);
        }
    }

    /* Like unlink, but with a simply-formated string, (appending an
     * integer to the given string). */
    void unlinkf(std::string resource, std::string dependency, int dep_no) {

        std::stringstream ss;
        ss << dependency << dep_no;
        unlink(resource, ss.str());
    }

    /* Unlink all: Remove dependencies from 'resource' to all other
     * resources. */
    void unlinkAll(std::string resource) {
        dependencies.erase(resource);
    }

    /* Resolve: Recursively compute all calls providing 'resource',
     * (including linked dependencies of 'resource' on other
     * resources). */
    std::set<unsigned> resolve(std::string resource) {
        std::set<std::string> *deps;
        std::set<std::string>::iterator dep;

        std::set<unsigned> *calls;
        std::set<unsigned>::iterator call;

        std::set<unsigned> result, deps_set;

        /* Recursively chase dependencies. */
        if (dependencies.count(resource)) {
            deps = &dependencies[resource];
            for (dep = deps->begin(); dep != deps->end(); dep++) {
                deps_set = resolve(*dep);
                for (call = deps_set.begin(); call != deps_set.end(); call++) {
                    result.insert(*call);
                }
            }
        }

        /* Also look for calls that directly provide 'resource' */
        if (resources.count(resource)) {
            calls = &resources[resource];
            for (call = calls->begin(); call != calls->end(); call++) {
                result.insert(*call);
            }
        }

        return result;
    }

    /* Consume: Resolve all calls that provide the given resource, and
     * add them to the required list. Then clear the call list for
     * 'resource' along with any dependencies. */
    void consume(std::string resource) {

        std::set<unsigned> calls;
        std::set<unsigned>::iterator call;

        calls = resolve(resource);

        dependencies.erase(resource);
        resources.erase(resource);

        for (call = calls.begin(); call != calls.end(); call++) {
            required.insert(*call);
        }
    }

    void stateTrackPreCall(trace::Call *call) {

        const char *name = call->name();

        if (strcmp(name, "glBegin") == 0) {
            insideBeginEnd = true;
            return;
        }

        if (strcmp(name, "glBeginTransformFeedback") == 0) {
            transformFeedbackActive = true;
            return;
        }

        if (strcmp(name, "glBindTexture") == 0) {
            GLenum target;
            GLuint texture;

            target = static_cast<GLenum>(call->arg(0).toSInt());
            texture = call->arg(1).toUInt();

            if (texture == 0) {
                texture_map.erase(target);
            } else {
                texture_map[target] = texture;
            }

            return;
        }

        if (strcmp(name, "glUseProgram") == 0) {
            activeProgram = call->arg(0).toUInt();
        }

        if (strcmp(name, "glBindFramebuffer") == 0) {
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

        const char *name = call->name();

        if (strcmp(name, "glEnd") == 0) {
            insideBeginEnd = false;
            return;
        }

        if (strcmp(name, "glEndTransformFeedback") == 0) {
            transformFeedbackActive = false;
            return;
        }

        /* If this swapbuffers was included in the trace then it will
         * have already consumed all framebuffer dependencies. If not,
         * then clear them now so that they don't carry over into the
         * next frame. */
        if (call->flags & trace::CALL_FLAG_SWAP_RENDERTARGET &&
            call->flags & trace::CALL_FLAG_END_FRAME) {
            dependencies.erase("framebuffer");
            resources.erase("framebuffer");
            return;
        }
    }

    void recordSideEffects(trace::Call *call) {

        const char *name = call->name();

        /* If call is flagged as no side effects, then we are done here. */
        if (call->flags & trace::CALL_FLAG_NO_SIDE_EFFECTS) {
            return;
        }

        /* Similarly, swap-buffers calls don't have interesting side effects. */
        if (call->flags & trace::CALL_FLAG_SWAP_RENDERTARGET &&
            call->flags & trace::CALL_FLAG_END_FRAME) {
            return;
        }

        if (strcmp(name, "glGenTextures") == 0) {
            const trace::Array *textures = dynamic_cast<const trace::Array *>(&call->arg(1));
            size_t i;
            GLuint texture;

            if (textures) {
                for (i = 0; i < textures->size(); i++) {
                    texture = textures->values[i]->toUInt();
                    providef("texture-", texture, call->no);
                }
            }
            return;
        }

        if (strcmp(name, "glBindTexture") == 0) {
            GLenum target;
            GLuint texture;

            std::stringstream ss_target, ss_texture;

            target = static_cast<GLenum>(call->arg(0).toSInt());
            texture = call->arg(1).toUInt();

            ss_target << "texture-target-" << target;
            ss_texture << "texture-" << texture;

            resources.erase(ss_target.str());
            provide(ss_target.str(), call->no);

            unlinkAll(ss_target.str());
            link(ss_target.str(), ss_texture.str());

            return;
        }

        /* FIXME: Need to handle glMultTexImage and friends. */
        if (STRNCMP_LITERAL(name, "glTexImage") == 0 ||
            STRNCMP_LITERAL(name, "glTexSubImage") == 0 ||
            STRNCMP_LITERAL(name, "glCopyTexImage") == 0 ||
            STRNCMP_LITERAL(name, "glCopyTexSubImage") == 0 ||
            STRNCMP_LITERAL(name, "glCompressedTexImage") == 0 ||
            STRNCMP_LITERAL(name, "glCompressedTexSubImage") == 0 ||
            strcmp(name, "glInvalidateTexImage") == 0 ||
            strcmp(name, "glInvalidateTexSubImage") == 0) {

            std::set<unsigned> *calls;
            std::set<unsigned>::iterator c;
            std::stringstream ss_target, ss_texture;

            GLenum target = static_cast<GLenum>(call->arg(0).toSInt());

            ss_target << "texture-target-" << target;
            ss_texture << "texture-" << texture_map[target];

            /* The texture resource depends on this call and any calls
             * providing the given texture target. */
            provide(ss_texture.str(), call->no);

            if (resources.count(ss_target.str())) {
                calls = &resources[ss_target.str()];
                for (c = calls->begin(); c != calls->end(); c++) {
                    provide(ss_texture.str(), *c);
                }
            }

            return;
        }

        if (strcmp(name, "glEnable") == 0) {
            GLenum cap;

            cap = static_cast<GLenum>(call->arg(0).toSInt());

            if (cap == GL_TEXTURE_1D ||
                cap == GL_TEXTURE_2D ||
                cap == GL_TEXTURE_3D ||
                cap == GL_TEXTURE_CUBE_MAP)
            {
                linkf("render-state", "texture-target-", cap);
            }

            provide("state", call->no);
            return;
        }

        if (strcmp(name, "glDisable") == 0) {
            GLenum cap;

            cap = static_cast<GLenum>(call->arg(0).toSInt());

            if (cap == GL_TEXTURE_1D ||
                cap == GL_TEXTURE_2D ||
                cap == GL_TEXTURE_3D ||
                cap == GL_TEXTURE_CUBE_MAP)
            {
                unlinkf("render-state", "texture-target-", cap);
            }

            provide("state", call->no);
            return;
        }

        if (strcmp(name, "glCreateShader") == 0 ||
            strcmp(name, "glCreateShaderObjectARB") == 0) {

            GLuint shader = call->ret->toUInt();
            providef("shader-", shader, call->no);
            return;
        }

        if (strcmp(name, "glShaderSource") == 0 ||
            strcmp(name, "glShaderSourceARB") == 0 ||
            strcmp(name, "glCompileShader") == 0 ||
            strcmp(name, "glCompileShaderARB") == 0 ||
            strcmp(name, "glGetShaderiv") == 0 ||
            strcmp(name, "glGetShaderInfoLog") == 0) {

            GLuint shader = call->arg(0).toUInt();
            providef("shader-", shader, call->no);
            return;
        }

        if (strcmp(name, "glCreateProgram") == 0 ||
            strcmp(name, "glCreateProgramObjectARB") == 0) {

            GLuint program = call->ret->toUInt();
            providef("program-", program, call->no);
            return;
        }

        if (strcmp(name, "glAttachShader") == 0 ||
            strcmp(name, "glAttachObjectARB") == 0) {

            GLuint program, shader;
            std::stringstream ss_program, ss_shader;

            program = call->arg(0).toUInt();
            shader = call->arg(1).toUInt();

            ss_program << "program-" << program;
            ss_shader << "shader-" << shader;

            link(ss_program.str(), ss_shader.str());
            provide(ss_program.str(), call->no);

            return;
        }

        if (strcmp(name, "glDetachShader") == 0 ||
            strcmp(name, "glDetachObjectARB") == 0) {

            GLuint program, shader;
            std::stringstream ss_program, ss_shader;

            program = call->arg(0).toUInt();
            shader = call->arg(1).toUInt();

            ss_program << "program-" << program;
            ss_shader << "shader-" << shader;

            unlink(ss_program.str(), ss_shader.str());

            return;
        }

        if (strcmp(name, "glUseProgram") == 0 ||
            strcmp(name, "glUseProgramObjectARB") == 0) {

            GLuint program;

            program = call->arg(0).toUInt();

            unlinkAll("render-program-state");

            if (program == 0) {
                unlink("render-state", "render-program-state");
                provide("state", call->no);
            } else {
                std::stringstream ss;

                ss << "program-" << program;

                link("render-state", "render-program-state");
                link("render-program-state", ss.str());

                provide(ss.str(), call->no);
            }

            return;
        }

        if (strcmp(name, "glGetUniformLocation") == 0 ||
            strcmp(name, "glGetUniformLocationARB") == 0 ||
            strcmp(name, "glGetFragDataLocation") == 0 ||
            strcmp(name, "glGetFragDataLocationEXT") == 0 ||
            strcmp(name, "glGetSubroutineUniformLocation") == 0 ||
            strcmp(name, "glGetProgramResourceLocation") == 0 ||
            strcmp(name, "glGetProgramResourceLocationIndex") == 0 ||
            strcmp(name, "glGetVaryingLocationNV") == 0) {

            GLuint program = call->arg(0).toUInt();

            providef("program-", program, call->no);

            return;
        }

        /* For any call that accepts 'location' as its first argument,
         * perform a lookup in our location->program map and add a
         * dependence on the program we find there. */
        if (call->sig->num_args > 0 &&
            strcmp(call->sig->arg_names[0], "location") == 0) {

            providef("program-", activeProgram, call->no);
            return;
        }

        /* FIXME: We cut a huge swath by assuming that any unhandled
         * call that has a first argument named "program" should not
         * be included in the trimmed output unless the program of
         * that number is also included.
         *
         * This heuristic is correct for many cases, but we should
         * actually carefully verify if this includes some calls
         * inappropriately, or if it misses some.
         */
        if (strcmp(name, "glLinkProgram") == 0 ||
            strcmp(name, "glLinkProgramARB") == 0 ||
            (call->sig->num_args > 0 &&
             (strcmp(call->sig->arg_names[0], "program") == 0 ||
              strcmp(call->sig->arg_names[0], "programObj") == 0))) {

            GLuint program = call->arg(0).toUInt();
            providef("program-", program, call->no);
            return;
        }

        /* Handle all rendering operations, (even though only glEnd is
         * flagged as a rendering operation we treat everything from
         * glBegin through glEnd as a rendering operation). */
        if (call->flags & trace::CALL_FLAG_RENDER ||
            insideBeginEnd) {

            std::set<unsigned> calls;
            std::set<unsigned>::iterator c;

            provide("framebuffer", call->no);

            calls = resolve("render-state");

            for (c = calls.begin(); c != calls.end(); c++) {
                provide("framebuffer", *c);
            }

            /* In some cases, rendering has side effects beyond the
             * framebuffer update. */
            if (renderingHasSideEffect()) {
                provide("state", call->no);
                for (c = calls.begin(); c != calls.end(); c++) {
                    provide("state", *c);
                }
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
};

static int
trim_trace(const char *filename, struct trim_options *options)
{
    trace::ParseBookmark beginning;
    trace::Parser p;
    TraceAnalyzer analyzer;
    std::set<unsigned> *required;
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

        /* There's no use doing any work past the last call or frame
         * requested by the user. */
        if (call->no > options->calls.getLast() ||
            frame > options->frames.getLast()) {
            
            delete call;
            break;
        }

        /* If requested, ignore all calls not belonging to the specified thread. */
        if (options->thread != -1 && call->thread_id != options->thread) {
            goto NEXT;
        }

        /* Also, prune if uninteresting (unless the user asked for no pruning. */
        if (options->prune_uninteresting && call->flags & trace::CALL_FLAG_UNINTERESTING) {
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
        std::cerr << "error: failed to create " << filename << "\n";
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

        /* There's no use doing any work past the last call or frame
         * requested by the user. */
        if (call->no > options->calls.getLast() ||
            frame > options->frames.getLast()) {

            break;
        }

        if (required->find(call->no) != required->end()) {
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

    std::cout << "Trimmed trace is available as " << options->output << "\n";

    return 0;
}

static int
command(int argc, char *argv[])
{
    struct trim_options options;

    options.calls = trace::CallSet(trace::FREQUENCY_NONE);
    options.frames = trace::CallSet(trace::FREQUENCY_NONE);
    options.dependency_analysis = true;
    options.prune_uninteresting = true;
    options.output = "";
    options.thread = -1;
    options.print_callset = 0;

    int opt;
    while ((opt = getopt_long(argc, argv, shortOptions, longOptions, NULL)) != -1) {
        switch (opt) {
        case 'h':
            help();
            return 0;
        case CALLS_OPT:
            options.calls = trace::CallSet(optarg);
            break;
        case FRAMES_OPT:
            options.frames = trace::CallSet(optarg);
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
        case PRINT_CALLSET_OPT:
            options.print_callset = 1;
            break;
        default:
            std::cerr << "error: unexpected option `" << opt << "`\n";
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

    return trim_trace(argv[optind], &options);
}

const Command trim_command = {
    "trim",
    synopsis,
    help,
    command
};
