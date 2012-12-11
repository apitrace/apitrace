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
#include "trace_parser.hpp"

class TraceAnalyzer {
private:
    std::map<std::string, std::set<unsigned> > resources;
    std::map<std::string, std::set<std::string> > dependencies;

    std::map<GLenum, unsigned> texture_map;

    std::set<unsigned> required;

    bool transformFeedbackActive;
    bool framebufferObjectActive;
    bool insideBeginEnd;
    GLuint insideNewEndList;
    GLenum activeTextureUnit;
    GLuint activeProgram;

    void provide(std::string resource, trace::CallNo call_no);
    void providef(std::string resource, int resource_no, trace::CallNo call_no);

    void link(std::string resource, std::string dependency);
    void linkf(std::string resource, std::string dependency, int dep_no);

    void unlink(std::string resource, std::string dependency);
    void unlinkf(std::string resource, std::string dependency, int dep_no);
    void unlinkAll(std::string resource);

    void stateTrackPreCall(trace::Call *call);
    void recordSideEffects(trace::Call *call);
    void stateTrackPostCall(trace::Call *call);

    bool renderingHasSideEffect(void);
    std::set<unsigned> resolve(std::string resource);

    void consume(std::string resource);
    void requireDependencies(trace::Call *call);

public:
    TraceAnalyzer();
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
    std::set<unsigned> *get_required(void);
};
