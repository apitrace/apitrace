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

#include <sstream>

#include "cli_trim_auto_analyzer.hpp"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define STRNCMP_LITERAL(var, literal) strncmp((var), (literal), sizeof (literal) -1)

/* Rendering often has no side effects, but it can in some cases,
* (such as when transform feedback is active, or when rendering
* targets a framebuffer object). */
bool
TraceAnalyzer::renderingHasSideEffect(void)
{
    return transformFeedbackActive || framebufferObjectActive;
}

/* Provide: Record that the given call affects the given resource
 * as a side effect. */
void
TraceAnalyzer::provide(std::string resource, trace::CallNo call_no)
{
    resources[resource].insert(call_no);
}

/* Like provide, but with a simply-formatted string, (appending an
 * integer to the given string). */
void
TraceAnalyzer::providef(std::string resource,
                        int resource_no,
                        trace::CallNo call_no)
{
    std::stringstream ss;
    ss << resource << resource_no;
    provide(ss.str(), call_no);
}

/* Link: Establish a dependency between resource 'resource' and
 * resource 'dependency'. This dependency is captured by name so
 * that if the list of calls that provide 'dependency' grows
 * before 'resource' is consumed, those calls will still be
 * captured. */
void
TraceAnalyzer::link(std::string resource, std::string dependency)
{
    dependencies[resource].insert(dependency);
}

/* Like link, but with a simply-formatted string, (appending an
 * integer to the given string). */
void
TraceAnalyzer::linkf(std::string resource, std::string dependency, int dep_no)
{

    std::stringstream ss;
    ss << dependency << dep_no;
    link(resource, ss.str());
}

/* Unlink: Remove dependency from 'resource' on 'dependency'. */
void
TraceAnalyzer::unlink(std::string resource, std::string dependency)
{
    dependencies[resource].erase(dependency);
    if (dependencies[resource].size() == 0) {
        dependencies.erase(resource);
    }
}

/* Like unlink, but with a simply-formated string, (appending an
 * integer to the given string). */
void
TraceAnalyzer::unlinkf(std::string resource, std::string dependency, int dep_no)
{

    std::stringstream ss;
    ss << dependency << dep_no;
    unlink(resource, ss.str());
}

/* Unlink all: Remove dependencies from 'resource' to all other
 * resources. */
void
TraceAnalyzer::unlinkAll(std::string resource)
{
    dependencies.erase(resource);
}

/* Resolve: Recursively compute all calls providing 'resource',
 * (including linked dependencies of 'resource' on other
 * resources). */
std::set<unsigned>
TraceAnalyzer::resolve(std::string resource)
{
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
void
TraceAnalyzer::consume(std::string resource)
{

    std::set<unsigned> calls;
    std::set<unsigned>::iterator call;

    calls = resolve(resource);

    dependencies.erase(resource);
    resources.erase(resource);

    for (call = calls.begin(); call != calls.end(); call++) {
        required.add(*call);
    }
}

void
TraceAnalyzer::stateTrackPreCall(trace::Call *call)
{

    const char *name = call->name();

    if (strcmp(name, "glBegin") == 0) {
        insideBeginEnd = true;
        return;
    }

    if (strcmp(name, "glBeginTransformFeedback") == 0) {
        transformFeedbackActive = true;
        return;
    }

    if (strcmp(name, "glActiveTexture") == 0) {
        activeTextureUnit = static_cast<GLenum>(call->arg(0).toSInt());
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

    if (strcmp(name, "glNewList") == 0) {
        GLuint list = call->arg(0).toUInt();

        insideNewEndList = list;
    }
}

void
TraceAnalyzer::stateTrackPostCall(trace::Call *call)
{

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

    if (strcmp(name, "glEndList") == 0) {
        insideNewEndList = 0;
    }
}

bool
TraceAnalyzer::callHasNoSideEffects(trace::Call *call, const char *name)
{
    /* If call is flagged as no side effects, then we are done here. */
    if (call->flags & trace::CALL_FLAG_NO_SIDE_EFFECTS) {
        return true;
    }

    /* Not known as a no-side-effect call. Return false for more analysis. */
    return false;
}

bool
TraceAnalyzer::recordTextureSideEffects(trace::Call *call, const char *name)
{
    if (strcmp(name, "glGenTextures") == 0) {
        const trace::Array *textures = call->arg(1).toArray();
        size_t i;
        GLuint texture;

        if (textures) {
            for (i = 0; i < textures->size(); i++) {
                texture = textures->values[i]->toUInt();
                providef("texture-", texture, call->no);
            }
        }
        return true;
    }

    /* FIXME: When we start tracking framebuffer objects as their own
     * resources, we will want to link the FBO to the given texture
     * resource, (and to this call). For now, just link render state
     * to the texture, and force this call to be required. */
    if (strcmp(name, "glFramebufferTexture2D") == 0) {
        GLuint texture;

        texture = call->arg(3).toUInt();

        linkf("render-state", "texture-", texture);

        provide("state", call->no);
    }

    if (strcmp(name, "glBindTexture") == 0) {
        GLenum target;
        GLuint texture;

        std::stringstream ss_target, ss_texture;

        target = static_cast<GLenum>(call->arg(0).toSInt());
        texture = call->arg(1).toUInt();

        ss_target << "texture-unit-" << activeTextureUnit << "-target-" << target;
        ss_texture << "texture-" << texture;

        resources.erase(ss_target.str());
        provide(ss_target.str(), call->no);

        unlinkAll(ss_target.str());
        link(ss_target.str(), ss_texture.str());

        /* FIXME: This really shouldn't be necessary. The effect
         * this provide() has is that all glBindTexture calls will
         * be preserved in the output trace (never trimmed). Carl
         * has a trace ("btr") where a glBindTexture call should
         * not be necessary at all, (it's immediately followed
         * with a glBindTexture to a different texture and no
         * intervening texture-related calls), yet this 'provide'
         * makes the difference between a trim_stress test failing
         * and passing.
         *
         * More investigation is necessary, but for now, be
         * conservative and don't trim. */
        provide("state", call->no);

        return true;
    }

    /* FIXME: Need to handle glMultiTexImage and friends. */
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

        ss_target << "texture-unit-" << activeTextureUnit << "-target-" << target;
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

        return true;
    }

    if (strcmp(name, "glEnable") == 0) {
        GLenum cap;

        cap = static_cast<GLenum>(call->arg(0).toSInt());

        if (cap == GL_TEXTURE_1D ||
            cap == GL_TEXTURE_2D ||
            cap == GL_TEXTURE_3D ||
            cap == GL_TEXTURE_CUBE_MAP)
        {
            std::stringstream ss;

            ss << "texture-unit-" << activeTextureUnit << "-target-" << cap;

            link("render-state", ss.str());
        }

        provide("state", call->no);
        return true;
    }

    if (strcmp(name, "glDisable") == 0) {
        GLenum cap;

        cap = static_cast<GLenum>(call->arg(0).toSInt());

        if (cap == GL_TEXTURE_1D ||
            cap == GL_TEXTURE_2D ||
            cap == GL_TEXTURE_3D ||
            cap == GL_TEXTURE_CUBE_MAP)
        {
            std::stringstream ss;

            ss << "texture-unit-" << activeTextureUnit << "-target-" << cap;

            unlink("render-state", ss.str());
        }

        provide("state", call->no);
        return true;
    }

    /* No known texture-related side effects. Return false for more analysis. */
    return false;
}

bool
TraceAnalyzer::recordShaderSideEffects(trace::Call *call, const char *name)
{
    if (strcmp(name, "glCreateShader") == 0 ||
        strcmp(name, "glCreateShaderObjectARB") == 0) {

        GLuint shader = call->ret->toUInt();
        providef("shader-", shader, call->no);
        return true;
    }

    if (strcmp(name, "glShaderSource") == 0 ||
        strcmp(name, "glShaderSourceARB") == 0 ||
        strcmp(name, "glCompileShader") == 0 ||
        strcmp(name, "glCompileShaderARB") == 0 ||
        strcmp(name, "glGetShaderiv") == 0 ||
        strcmp(name, "glGetShaderInfoLog") == 0) {

        GLuint shader = call->arg(0).toUInt();
        providef("shader-", shader, call->no);
        return true;
    }

    if (strcmp(name, "glCreateProgram") == 0 ||
        strcmp(name, "glCreateProgramObjectARB") == 0) {

        GLuint program = call->ret->toUInt();
        providef("program-", program, call->no);
        return true;
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

        return true;
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

        return true;
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

        return true;
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

        return true;
    }

    /* For any call that accepts 'location' as its first argument,
     * perform a lookup in our location->program map and add a
     * dependence on the program we find there. */
    if (call->sig->num_args > 0 &&
        strcmp(call->sig->arg_names[0], "location") == 0) {

        providef("program-", activeProgram, call->no);

        /* We can't easily tell if this uniform is being used to
         * associate a sampler in the shader with a texture
         * unit. The conservative option is to assume that it is
         * and create a link from the active program to any bound
         * textures for the given unit number.
         *
         * FIXME: We should be doing the same thing for calls to
         * glUniform1iv. */
        if (strcmp(name, "glUniform1i") == 0 ||
            strcmp(name, "glUniform1iARB") == 0) {

            GLint max_unit = MAX(GL_MAX_TEXTURE_COORDS, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);

            GLint unit = call->arg(1).toSInt();
            std::stringstream ss_program;
            std::stringstream ss_texture;

            if (unit < max_unit) {

                ss_program << "program-" << activeProgram;

                ss_texture << "texture-unit-" << GL_TEXTURE0 + unit << "-target-";

                /* We don't know what target(s) might get bound to
                 * this texture unit, so conservatively link to
                 * all. Only bound textures will actually get inserted
                 * into the output call stream. */
                linkf(ss_program.str(), ss_texture.str(), GL_TEXTURE_1D);
                linkf(ss_program.str(), ss_texture.str(), GL_TEXTURE_2D);
                linkf(ss_program.str(), ss_texture.str(), GL_TEXTURE_3D);
                linkf(ss_program.str(), ss_texture.str(), GL_TEXTURE_CUBE_MAP);
            }
        }

        return true;
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
        return true;
    }

    /* No known shader-related side effects. Return false for more analysis. */
    return false;
}

bool
TraceAnalyzer::recordDrawingSideEffects(trace::Call *call, const char *name)
{
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

        return true;
    }

    /* Though it's not flagged as a "RENDER" operation, we also want
     * to trim swapbuffers calls when trimming drawing operations. */
    if (call->flags & trace::CALL_FLAG_SWAP_RENDERTARGET &&
        call->flags & trace::CALL_FLAG_END_FRAME) {
        return true;
    }

    /* No known drawing-related side effects. Return false for more analysis. */
    return false;
}

void
TraceAnalyzer::recordSideEffects(trace::Call *call)
{

    const char *name = call->name();

    /* FIXME: If we encode the list of commands that are executed
     * immediately (as opposed to those that are compiled into a
     * display list) then we could generate a "display-list-X"
     * resource just as we do for "texture-X" resources and only
     * emit it in the trace if a glCallList(X) is emitted. For
     * now, simply punt and include anything within glNewList and
     * glEndList in the trim output. This guarantees that display
     * lists will work, but does not trim out unused display
     * lists. */
    if (insideNewEndList != 0) {
        provide("state", call->no);

        /* Also, any texture bound inside a display list is
         * conservatively considered required. */
        if (strcmp(name, "glBindTexture") == 0) {
            GLuint texture = call->arg(1).toUInt();

            linkf("state", "texture-", texture);
        }

        return;
    }

    if (trimFlags & TRIM_FLAG_NO_SIDE_EFFECTS) {

        if (callHasNoSideEffects(call, name)) {
            return;
        }
    }

    if (trimFlags & TRIM_FLAG_TEXTURES) {
        
        if (recordTextureSideEffects(call, name)) {
            return;
        }
    }

    if (trimFlags & TRIM_FLAG_SHADERS) {

        if (recordShaderSideEffects(call, name)) {
            return;
        }
    }

    if (trimFlags & TRIM_FLAG_DRAWING) {

        if (recordDrawingSideEffects(call, name)) {
            return;
        }
    }

    /* By default, assume this call affects the state somehow. */
    resources["state"].insert(call->no);
}

void
TraceAnalyzer::requireDependencies(trace::Call *call)
{

    /* Swap-buffers calls depend on framebuffer state. */
    if (call->flags & trace::CALL_FLAG_SWAP_RENDERTARGET &&
        call->flags & trace::CALL_FLAG_END_FRAME) {
        consume("framebuffer");
    }

    /* By default, just assume this call depends on generic state. */
    consume("state");
}

TraceAnalyzer::TraceAnalyzer(TrimFlags trimFlagsOpt):
    transformFeedbackActive(false),
    framebufferObjectActive(false),
    insideBeginEnd(false),
    insideNewEndList(0),
    activeTextureUnit(GL_TEXTURE0),
    trimFlags(trimFlagsOpt)
{
    /* Nothing needed. */
}

TraceAnalyzer::~TraceAnalyzer()
{
    /* Nothing needed. */
}

/* Analyze this call by tracking state and recording all the
 * resources provided by this call as side effects.. */
void
TraceAnalyzer::analyze(trace::Call *call)
{

    stateTrackPreCall(call);

    recordSideEffects(call);

    stateTrackPostCall(call);
}

/* Require this call and all of its dependencies to be included in
 * the final trace. */
void
TraceAnalyzer::require(trace::Call *call)
{

    /* First, find and insert all calls that this call depends on. */
    requireDependencies(call);

    /* Then insert this call itself. */
    required.add(call->no);
}

/* Return a set of all the required calls, (both those calls added
 * explicitly with require() and those implicitly depended
 * upon. */
trace::FastCallSet *
TraceAnalyzer::get_required(void)
{
    return &required;
}
