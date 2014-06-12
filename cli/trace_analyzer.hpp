/**************************************************************************
 * Copyright 2012 Intel corporation
 *
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

#include <set>

#include <GL/gl.h>
#include <GL/glext.h>

#include "trace_callset.hpp"
#include "trace_fast_callset.hpp"
#include "trace_parser.hpp"

typedef unsigned TrimFlags;

/**
 * Trim flags
 */
enum {

    /* Whether to trim calls that have no side effects. */
    TRIM_FLAG_NO_SIDE_EFFECTS		= (1 << 0),

    /* Whether to trim calls to setup textures that are never used. */
    TRIM_FLAG_TEXTURES		        = (1 << 1),

    /* Whether to trim calls to setup shaders that are never used. */
    TRIM_FLAG_SHADERS			= (1 << 2),

    /* Whether to trim drawing operations outside of the desired call-set. */
    TRIM_FLAG_DRAWING			= (1 << 3),
};

class TraceAnalyzer {
private:
    std::map<std::string, std::set<unsigned> > resources;
    std::map<std::string, std::set<std::string> > dependencies;

    std::map<GLenum, unsigned> texture_map;

    trace::FastCallSet required;

    bool transformFeedbackActive;
    bool framebufferObjectActive;
    bool insideBeginEnd;
    GLuint insideNewEndList;
    GLenum activeTextureUnit;
    GLuint activeProgram;
    unsigned int trimFlags;

    void provide(std::string resource, trace::CallNo call_no);
    void providef(std::string resource, int resource_no, trace::CallNo call_no);

    void link(std::string resource, std::string dependency);
    void linkf(std::string resource, std::string dependency, int dep_no);

    void unlink(std::string resource, std::string dependency);
    void unlinkf(std::string resource, std::string dependency, int dep_no);
    void unlinkAll(std::string resource);

    void stateTrackPreCall(trace::Call *call);

    void recordSideEffects(trace::Call *call);
    bool callHasNoSideEffects(trace::Call *call, const char *name);
    bool recordTextureSideEffects(trace::Call *call, const char *name);
    bool recordShaderSideEffects(trace::Call *call, const char *name);
    bool recordDrawingSideEffects(trace::Call *call, const char *name);

    void stateTrackPostCall(trace::Call *call);

    bool renderingHasSideEffect(void);
    std::set<unsigned> resolve(std::string resource);

    void consume(std::string resource);
    void requireDependencies(trace::Call *call);

public:
    TraceAnalyzer(TrimFlags trimFlags = -1);
    ~TraceAnalyzer();

    /* Analyze this call by tracking state and recording all the
     * resources provided by this call as side effects.. */
    void analyze(trace::Call *call);

    /* Require this call and all of its dependencies to be included in
     * the final trace. */
    void require(trace::Call *call);

    /* Return a set of all the required calls, (both those calls added
     * explicitly with require() and those implicitly depended
     * upon. */
    trace::FastCallSet *get_required(void);
};
