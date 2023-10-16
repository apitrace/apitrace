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

#include "ft_opengl.hpp"

#include <unordered_set>
#include <algorithm>
#include <functional>
#include <sstream>
#include <iostream>
#include <memory>
#include <set>

#include <GL/gl.h>
#include <GL/glext.h>

using std::bind;
using std::placeholders::_1;
using std::make_shared;

namespace frametrim {

PerContextObjects::PerContextObjects():
    m_id(m_next_id++),
    m_fbo(m_id)
{
    m_vertex_arrays.set_current_context_id(m_id);
    m_program_pipelines.set_current_context_id(m_id);
    assert(m_next_id < 0xffffff);
}

uint32_t PerContextObjects::m_next_id = 1;

OpenGLImpl::OpenGLImpl(bool keep_all_states, bool swaps_to_finish):
    FrameTrimmer(keep_all_states, swaps_to_finish),
    m_fbo_ext(1)
{
    registerStateCalls();
    registerLegacyCalls();
    registerRequiredCalls();
    registerFramebufferCalls();
    registerBufferCalls();
    registerProgramCalls();
    registerVaCalls();
    registerTextureCalls();
    registerQueryCalls();
    registerDrawCalls();
    registerIgnoreHistoryCalls();

    global_state.out_list = &m_required_calls;
    global_state.emit_dependencies = m_recording_frame;
}

void OpenGLImpl::emitState()
{
    m_matrix_states.emitStateTo(m_required_calls);

    for (auto&& [name, call] : m_state_calls)
        m_required_calls.insert(call);

    for (auto&& [id, call]: m_enables)
        m_required_calls.insert(call);

    m_programs.emitBoundObjects(m_required_calls);
    m_textures.emitBoundObjects(m_required_calls);
    m_buffers.emitBoundObjects(m_required_calls);
    m_shaders.emitBoundObjects(m_required_calls);
    m_renderbuffers.emitBoundObjects(m_required_calls);
    m_samplers.emitBoundObjects(m_required_calls);
    m_sync_objects.emitBoundObjects(m_required_calls);

    for (auto& [dummy, context] : m_contexts) {
        context->m_vertex_arrays.emitBoundObjects(m_required_calls);
        context->m_program_pipelines.emitBoundObjects(m_required_calls);
        context->m_fbo.emitBoundObjects(m_required_calls);
    }

    m_vertex_attrib_pointers.emitBoundObjects(m_required_calls);
    m_vertex_buffer_pointers.emitBoundObjects(m_required_calls);
    m_legacy_programs.emitBoundObjects(m_required_calls);
    m_queries.emitBoundObjects(m_required_calls);
    m_fbo_ext.emitBoundObjects(m_required_calls);
}

void
OpenGLImpl::finalize()
{
    m_skip_loop_calls.clear();

    m_programs.unbalancedCreateCallsInLastFrame(m_last_frame_start, m_skip_loop_calls);
    m_textures.unbalancedCreateCallsInLastFrame(m_last_frame_start, m_skip_loop_calls);
    m_buffers.unbalancedCreateCallsInLastFrame(m_last_frame_start, m_skip_loop_calls);
    m_shaders.unbalancedCreateCallsInLastFrame(m_last_frame_start, m_skip_loop_calls);
    m_renderbuffers.unbalancedCreateCallsInLastFrame(m_last_frame_start, m_skip_loop_calls);
    m_samplers.unbalancedCreateCallsInLastFrame(m_last_frame_start, m_skip_loop_calls);
    m_sync_objects.unbalancedCreateCallsInLastFrame(m_last_frame_start, m_skip_loop_calls);

    for (auto& [dummy, context] : m_contexts) {
        context->m_vertex_arrays.unbalancedCreateCallsInLastFrame(m_last_frame_start, m_skip_loop_calls);
        context->m_program_pipelines.unbalancedCreateCallsInLastFrame(m_last_frame_start, m_skip_loop_calls);
        context->m_fbo.unbalancedCreateCallsInLastFrame(m_last_frame_start, m_skip_loop_calls);
    }

    m_vertex_attrib_pointers.unbalancedCreateCallsInLastFrame(m_last_frame_start, m_skip_loop_calls);
    m_vertex_buffer_pointers.unbalancedCreateCallsInLastFrame(m_last_frame_start, m_skip_loop_calls);
    m_legacy_programs.unbalancedCreateCallsInLastFrame(m_last_frame_start, m_skip_loop_calls);
    m_queries.unbalancedCreateCallsInLastFrame(m_last_frame_start, m_skip_loop_calls);
}

unsigned
OpenGLImpl::equalChars(const char *prefix, const char *callname)
{
    unsigned retval = 0;
    while (*prefix && *callname && *prefix == *callname) {
        ++retval;
        ++prefix; ++callname;
    }
    if (!*prefix && !*callname)
        ++retval;

    return !*prefix ? retval : 0;
}

bool
OpenGLImpl::checkCommonSuffixes(const char *suffix) const
{
    std::vector<const char *> ext = {
        "ARB", "NV", "EXT", "OES", "SGI",
        "i", "f", "fv", "iv", "v", "uiv",
        "1f", "2f", "3f", "4f",
        "1i", "2i", "3i", "4i",
        "1fv", "2fv", "3fv", "4fv",
        "1iv", "2iv", "3iv", "4iv",
        "2fARB", "3fARB", "4fARB",
        "fvARB", "1fvARB", "2fvARB", "3fvARB", "4fvARB",
        "ivARB", "1ivARB", "2ivARB", "3ivARB", "4ivARB",
        "Nubv"
    };

    for (const auto& i :  ext) {
        if (!strcmp(i, suffix))
            return true;
    }
    return false;
}

ft_callback OpenGLImpl::findCallback(const char *name)
{
    auto cb_range = m_call_table.equal_range(name);
    if (cb_range.first != m_call_table.end() &&
            std::distance(cb_range.first, cb_range.second) > 0) {

        CallTable::const_iterator cb = cb_range.first;
        CallTable::const_iterator i = cb_range.first;
        ++i;

        unsigned max_equal = equalChars(cb->first, name);

        while (i != cb_range.second && i != m_call_table.end()) {
            auto n = equalChars(i->first, name);
            if (n > max_equal) {
                max_equal = n;
                cb = i;
            }
            ++i;
        }

        if (max_equal) {
            if (strcmp(name, cb->first)) {
                if (!checkCommonSuffixes(name + strlen(cb->first)))
                    std::cerr << "Handle " << name << " as " << cb->first << "\n";
            }
            return cb->second;
        }
    }

    return nullptr;
}

bool
OpenGLImpl::skipDeleteObj(const trace::Call& call)
{
    if (!strcmp(call.name(), "glDeleteProgram"))
        return skipDeleteImpl(call.arg(0).toUInt(), m_programs);

    if (!strcmp(call.name(), "glDeleteProgramPipelines"))
        return skipDeleteImpl(call.arg(0).toUInt(), m_current_context->m_program_pipelines);

    if (!strcmp(call.name(), "glDeleteSync"))
        return skipDeleteImpl(call.arg(0).toUInt(), m_sync_objects);

    DependecyObjectMap *map = nullptr;
    if (!strcmp(call.name(), "glDeleteBuffers"))
        map = &m_buffers;

    if (!strcmp(call.name(), "glDeleteTextures"))
        map = &m_textures;

    if (!strcmp(call.name(), "glDeleteFramebuffers"))
        map = &m_current_context->m_fbo;

    if (!strcmp(call.name(), "glDeleteRenderbuffers"))
        map = &m_renderbuffers;

    if (!strcmp(call.name(), "glDeleteQueries"))
        map = &m_queries;

    if (map) {
        const auto ids = (call.arg(1)).toArray();
        for (auto& v : ids->values) {
            if (!skipDeleteImpl(v->toUInt(), *map))
                return false;
        }
        return true;
    }
    return false;
}

bool
OpenGLImpl::skipDeleteImpl(unsigned obj_id, DependecyObjectMap& map)
{
    auto obj = map.getById(obj_id);
    return !obj || (m_recording_frame && !obj->emitted()) || obj->createdBefore(m_last_frame_start);
}

// Map callbacks to call methods of FrameTRimImpl
// Additional data is passed by reference (R) or vy value (V)

#define MAP(name, call) m_call_table.insert(std::make_pair(#name, bind(&OpenGLImpl:: call, this, _1)))

#define MAP_V(name, call, param1) \
    m_call_table.insert(std::make_pair(#name, bind(&OpenGLImpl:: call, this, _1, \
                        param1)))

#define MAP_VV(name, call, param1, param2) \
    m_call_table.insert(std::make_pair(#name, bind(&OpenGLImpl:: call, this, _1, \
                        param1, param2)))

#define MAP_VRV(name, call, param1, data, param2) \
    m_call_table.insert(std::make_pair(#name, bind(&OpenGLImpl:: call, this, _1, \
                        param1, std::ref(data), param2)))

#define MAP_R(name, call, data) \
    m_call_table.insert(std::make_pair(#name, bind(&OpenGLImpl:: call, this, _1, \
                        std::ref(data))))

#define MAP_RV(name, call, data, param1) \
    m_call_table.insert(std::make_pair(#name, bind(&OpenGLImpl:: call, this, _1, \
                        std::ref(data), param1)))

#define MAP_RVRV(name, call, data1, param1, data2, param2) \
    m_call_table.insert(std::make_pair(#name, bind(&OpenGLImpl:: call, this, _1, \
                        std::ref(data1), param1, std::ref(data2), param2)))


// Map callbacks to call methods of object map "obj"

#define MAP_OBJ(name, obj, call) \
    m_call_table.insert(std::make_pair(#name, bind(&call, &obj, _1)))

#define MAP_OBJ_V(name, obj, call, data) \
    m_call_table.insert(std::make_pair(#name, bind(&call, &obj, _1, data)))

#define MAP_OBJ_RVV(name, obj, call, data, param1, param2) \
    m_call_table.insert(std::make_pair(#name, bind(&call, &obj, _1, \
                        std::ref(data), param1, param2)))

#define MAP_OBJ_RV(name, obj, call, data, param) \
    m_call_table.insert(std::make_pair(#name, bind(&call, &obj, _1, \
                        std::ref(data), param)))

#define MAP_OBJ_RVRR(name, obj, call, data1, param, data2, data3) \
    m_call_table.insert(std::make_pair(#name, bind(&call, &obj, _1, \
                        std::ref(data1), param, std::ref(data2), std::ref(data3))))

#define MAP_OBJ_R(name, obj, call, data) \
    m_call_table.insert(std::make_pair(#name, bind(&call, &obj, _1, \
                        std::ref(data))))

#define MAP_OBJ_RR(name, obj, call, data1, data2) \
    m_call_table.insert(std::make_pair(#name, bind(&call, &obj, _1, \
                        std::ref(data1), std::ref(data2))))

#define MAP_OBJ_RRR(name, obj, call, data1, data2, data3) \
    m_call_table.insert(std::make_pair(#name, bind(&call, &obj, _1, \
                        std::ref(data1), std::ref(data2), std::ref(data3))))

void OpenGLImpl::registerLegacyCalls()
{
    // draw calls
    MAP(glBegin, oglBegin);
    MAP(glColor2, oglVertex);
    MAP(glColor3, oglVertex);
    MAP(glColor4, oglVertex);
    MAP(glEnd, oglEnd);
    MAP(glNormal, oglVertex);
    MAP(glRect, oglVertex);
    MAP(glTexCoord2, oglVertex);
    MAP(glTexCoord3, oglVertex);
    MAP(glTexCoord4, oglVertex);
    MAP(glVertex3, oglVertex);
    MAP(glVertex4, oglVertex);
    MAP(glVertex2, oglVertex);
    MAP(glVertex3, oglVertex);
    MAP(glVertex4, oglVertex);

    // display lists
    MAP(glCallList, oglCallList);
    MAP(glDeleteLists, oglDeleteLists);
    MAP(glEndList, oglEndList);
    MAP(glGenLists, oglGenLists);
    MAP(glNewList, oglNewList);

    // shader calls
    MAP_OBJ(glGenPrograms, m_legacy_programs,
               DependecyObjectWithDefaultBindPointMap::generate);
    MAP_RV(glBindProgram, bindWithCreate, m_legacy_programs, 1);

    MAP_OBJ(glDeletePrograms, m_legacy_programs,
               LegacyProgrambjectMap::destroy);

    MAP_OBJ(glProgramString, m_legacy_programs,
               LegacyProgrambjectMap::callOnBoundObject);
    MAP_OBJ(glProgramLocalParameter, m_legacy_programs,
               LegacyProgrambjectMap::callOnBoundObject);
    MAP_OBJ(glProgramEnvParameter, m_legacy_programs,
               LegacyProgrambjectMap::callOnBoundObject);

    MAP_OBJ(glNamedProgramLocalParameter, m_legacy_programs,
               LegacyProgrambjectMap::callOnNamedObject);
    MAP_OBJ(glNamedProgramEnvParameter, m_legacy_programs,
               LegacyProgrambjectMap::callOnNamedObject);

    // Matrix manipulation
    MAP_OBJ(glLoadIdentity, m_matrix_states, AllMatrisStates::loadIdentity);
    MAP_OBJ(glLoadMatrix, m_matrix_states, AllMatrisStates::loadMatrix);
    MAP_OBJ(glMatrixMode, m_matrix_states, AllMatrisStates::matrixMode);
    MAP_OBJ(glMultMatrix, m_matrix_states, AllMatrisStates::matrixOp);
    MAP_OBJ(glOrtho, m_matrix_states, AllMatrisStates::matrixOp);
    MAP_OBJ(glRotate, m_matrix_states, AllMatrisStates::matrixOp);
    MAP_OBJ(glScale, m_matrix_states, AllMatrisStates::matrixOp);
    MAP_OBJ(glTranslate, m_matrix_states, AllMatrisStates::matrixOp);
    MAP_OBJ(glPopMatrix, m_matrix_states, AllMatrisStates::popMatrix);
    MAP_OBJ(glPushMatrix, m_matrix_states, AllMatrisStates::pushMatrix);
    MAP_V(glDispatchCompute, oglDispatchCompute, false);
    MAP_V(glDispatchComputeIndirect, oglDispatchCompute, true);
}

void OpenGLImpl::registerFramebufferCalls()
{
    MAP_RV(glBindRenderbuffer, oglBind, m_renderbuffers, 1);
    MAP_OBJ(glDeleteRenderbuffers, m_renderbuffers, DependecyObjectWithSingleBindPointMap::destroy);
    MAP_OBJ(glGenRenderbuffers, m_renderbuffers, DependecyObjectWithSingleBindPointMap::generate);
    MAP_OBJ(glCreateRenderbuffers, m_renderbuffers, DependecyObjectWithSingleBindPointMap::generate);
    MAP_OBJ(glRenderbufferStorage, m_renderbuffers, DependecyObjectWithSingleBindPointMap::callOnBoundObject);
    MAP_OBJ(glNamedRenderbufferStorage, m_renderbuffers, DependecyObjectWithSingleBindPointMap::callOnNamedObject);
    MAP_OBJ(glRenderbufferStorageMultisample, m_renderbuffers, DependecyObjectWithSingleBindPointMap::callOnBoundObject);

    MAP_V(glGenFramebuffers, generate, pc_fbo);
    MAP_V(glCreateFramebuffers, generate, pc_fbo);
    MAP_V(glDeleteFramebuffers, destroy, pc_fbo);
    MAP_V(glBindFramebuffer, oglBindFbo, 1);
    MAP_VV(glViewport, callOnObjectBoundTo, pc_fbo, GL_DRAW_FRAMEBUFFER);

    MAP(glBlitFramebuffer, fboBlit);
    MAP(glBlitNamedFramebuffer, fboBlitNamed);
    MAP_RV(glFramebufferTexture, fboBindAttachment, m_textures, 2);
    MAP_RV(glNamedFramebufferTexture, fboNamedBindAttachment, m_textures, 2);
    MAP_RV(glNamedFramebufferTextureLayer, fboNamedBindAttachment, m_textures, 2);
    MAP_RV(glFramebufferTextureLayer, fboBindAttachment, m_textures, 2);
    MAP_RV(glFramebufferTexture1D, fboBindAttachment, m_textures,3);
    MAP_RV(glFramebufferTexture2D, fboBindAttachment, m_textures, 3);
    MAP_RV(glFramebufferTexture3D, fboBindAttachment, m_textures, 3);
    MAP_RV(glFramebufferRenderbuffer, fboBindAttachment, m_renderbuffers, 3);
    MAP_RV(glNamedFramebufferRenderbuffer, fboNamedBindAttachment, m_renderbuffers, 3);
    MAP_V(glNamedFramebufferDrawBuffer, callOnNamedObject, pc_fbo); 
    MAP_V(glNamedFramebufferDrawBuffers, callOnNamedObject, pc_fbo); 
    MAP_V(glNamedFramebufferReadBuffer, callOnNamedObject, pc_fbo); 
    MAP(glReadBuffer, fboReadBuffer);

    MAP_VV(glDrawBuffer, callOnObjectBoundTo, pc_fbo, GL_DRAW_FRAMEBUFFER);
    MAP_VV(glDrawBuffers, callOnObjectBoundTo, pc_fbo, GL_DRAW_FRAMEBUFFER);

    MAP_VV(glClearBuffer, callOnObjectBoundTo, pc_fbo, GL_DRAW_FRAMEBUFFER);
    MAP_VV(glClearBufferfi, callOnObjectBoundTo, pc_fbo, GL_DRAW_FRAMEBUFFER);
    MAP_VV(glClearBufferfv, callOnObjectBoundTo, pc_fbo, GL_DRAW_FRAMEBUFFER);
    MAP_VV(glClearBufferiv, callOnObjectBoundTo, pc_fbo, GL_DRAW_FRAMEBUFFER);

    MAP_V(glCheckNamedFramebufferStatus, callOnBoundObject, pc_fbo); 
    
    MAP_V(glInvalidateFramebuffer, callOnBoundObject, pc_fbo); 
    MAP_V(glInvalidateNamedFramebufferData, callOnNamedObject, pc_fbo); 
    MAP_V(glInvalidateNamedFramebufferSubData, callOnNamedObject, pc_fbo); 
    
    MAP_V(glClearNamedFramebuffer, callOnNamedObject, pc_fbo);
    MAP_V(glClearNamedFramebufferfi, callOnNamedObject, pc_fbo);
    
/*    MAP_GENOBJ_DATAREF(glFramebufferTexture3D, m_fbo,
                         FramebufferStateMap::attach_texture3d, m_textures);
      MAP(glReadBuffer, ReadBuffer); */



    MAP_VV(glClear, callOnObjectBoundTo, pc_fbo, GL_DRAW_FRAMEBUFFER);

    MAP_OBJ(glGenFramebuffersEXT, m_fbo_ext, FramebufferObjectMap::generate);
    MAP_OBJ(DeleteFramebuffersEXT, m_fbo_ext, FramebufferObjectMap::destroy);
    MAP_RV(glBindFramebufferEXT, oglBind, m_fbo_ext, 1);

    MAP_RV(glFramebufferTexture1DEXT, fboBindAttachmentEXT, m_textures, 3);
    MAP_RV(glFramebufferTexture2DEXT, fboBindAttachmentEXT, m_textures, 3);
    MAP_RV(glFramebufferTexture3DEXT, fboBindAttachmentEXT, m_textures, 3);
    MAP_RV(glFramebufferRenderbufferEXT, fboBindAttachmentEXT, m_renderbuffers, 3);
}

void
OpenGLImpl::registerBufferCalls()
{
    MAP_OBJ(glGenBuffers, m_buffers, BufferObjectMap::generate);
    MAP_OBJ(glCreateBuffers, m_buffers, BufferObjectMap::generate);
    MAP_OBJ(glDeleteBuffers, m_buffers, BufferObjectMap::destroy);

    MAP(glBindBuffer, oglBindBuffer);
    MAP_OBJ(glBindBufferRange, m_buffers, BufferObjectMap::bindBuffer);

    /* This will need a bit more to be handled correctly */
    MAP_OBJ(glBindBufferBase, m_buffers, BufferObjectMap::bindBuffer);

    MAP_OBJ(glBufferData, m_buffers, BufferObjectMap::data);
    MAP_OBJ(glBufferStorage, m_buffers, BufferObjectMap::data);
    MAP_OBJ(glNamedBufferData, m_buffers, BufferObjectMap::namedData);
    MAP_OBJ(glNamedBufferStorage, m_buffers, BufferObjectMap::namedData);
    MAP_OBJ(glBufferSubData, m_buffers, BufferObjectMap::callOnBoundObject);
    MAP_OBJ(glGetBufferSubData, m_buffers, BufferObjectMap::callOnBoundObject);
    MAP_OBJ(glNamedBufferSubData, m_buffers, BufferObjectMap::callOnNamedObject);
    MAP_OBJ(glGetNamedBufferSubData, m_buffers, BufferObjectMap::callOnNamedObject);
    MAP_OBJ(glCopyBufferSubData, m_buffers, BufferObjectMap::copyBufferSubData);
    MAP_OBJ(glCopyNamedBufferSubData, m_buffers, BufferObjectMap::copyNamedBufferSubData);

    MAP_OBJ(glMapBuffer, m_buffers, BufferObjectMap::map);
    MAP_OBJ(glMapNamedBuffer, m_buffers, BufferObjectMap::namedMap);
    MAP_OBJ(glMapNamedBufferRange, m_buffers, BufferObjectMap::namedMapRange);
    MAP_OBJ(glUnmapNamedBuffer, m_buffers, BufferObjectMap::namedUnmap);
    MAP_OBJ(glMapBufferRange, m_buffers, BufferObjectMap::mapRange);
    MAP_OBJ_RR(memcpy, m_buffers, BufferObjectMap::memcopy, m_required_calls, m_recording_frame);
    MAP_OBJ(glFlushMappedBufferRange, m_buffers, BufferObjectMap::callOnBoundObject);
    MAP_OBJ(glFlushMappedNamedBufferRange, m_buffers, BufferObjectMap::callOnNamedObject);
    MAP_OBJ(glUnmapBuffer, m_buffers, BufferObjectMap::unmap);
    MAP_OBJ(glClearBufferData, m_buffers, BufferObjectMap::callOnBoundObject);
    MAP_OBJ(glClearNamedBufferData, m_buffers, BufferObjectMap::callOnNamedObject);
    MAP_OBJ(glInvalidateBufferData, m_buffers, BufferObjectMap::callOnNamedObject);
}

void OpenGLImpl::registerDrawCalls()
{
    MAP(glDrawArrays, oglDraw);
    MAP(glDrawElements, oglDraw);
    MAP(glDrawElementsBaseVertex, oglDraw);
    MAP(glDrawRangeElements, oglDraw);
    MAP(glDrawRangeElementsBaseVertex, oglDraw);

    MAP(glDrawArraysInstanced, oglDraw);
    MAP(glDrawArraysInstancedBaseInstance, oglDraw);
    MAP(glDrawElementsInstanced, oglDraw);
    MAP(glDrawElementsInstancedBaseVertex, oglDraw);
    MAP(glDrawElementsInstancedBaseVertexBaseInstance, oglDraw);

    MAP(glDrawElementsIndirect, oglDraw);
    MAP(glMultiDrawArraysIndirect, oglDraw);
    MAP(glMultiDrawElementsIndirect, oglDraw);
    MAP(glMultiDrawElements, oglDraw);
}

void
OpenGLImpl::registerProgramCalls()
{
    MAP_OBJ_RVV(glAttachObject, m_programs,
                   ProgramObjectMap::callOnNamedObjectWithDep, m_shaders, 1, false);
    MAP_OBJ_RVV(glAttachShader, m_programs,
                   ProgramObjectMap::callOnNamedObjectWithDep, m_shaders, 1, false);

    MAP_OBJ(glCompileShader, m_shaders, ShaderStateMap::callOnNamedObject);
    MAP_OBJ(glCreateShader, m_shaders, ShaderStateMap::create);
    MAP_OBJ(glCreateShaderObject, m_shaders, ShaderStateMap::create);
    MAP_OBJ(glDeleteShader, m_shaders, ShaderStateMap::del);
    MAP_OBJ(glShaderSource, m_shaders, ShaderStateMap::callOnNamedObject);

    MAP_OBJ(glBindAttribLocation, m_programs, ProgramObjectMap::callOnNamedObject);
    MAP_OBJ(glGetActiveAttrib, m_programs, ProgramObjectMap::callOnNamedObject);
    MAP_OBJ(glCreateProgram, m_programs, ProgramObjectMap::create);
    MAP_OBJ(glCreateProgramObject, m_programs, ProgramObjectMap::create);
    MAP_OBJ(glDeleteProgram, m_programs, ProgramObjectMap::del);
    MAP_OBJ(glDetachShader, m_programs, ProgramObjectMap::callOnNamedObject);
    MAP_OBJ(glGetAttachedShaders, m_programs, ProgramObjectMap::callOnNamedObject);

    MAP_OBJ(glGetAttribLocation, m_programs, ProgramObjectMap::callOnNamedObject);
    MAP_OBJ(glGetUniformLocation, m_programs, ProgramObjectMap::callOnNamedObject);
    MAP_OBJ(glGetUniformBlockIndex, m_programs, ProgramObjectMap::callOnNamedObject);
    MAP_OBJ(glBindFragDataLocation, m_programs, ProgramObjectMap::callOnNamedObject);
    MAP_OBJ(glLinkProgram, m_programs, ProgramObjectMap::callOnNamedObject);
    MAP_OBJ(glProgramBinary, m_programs, ProgramObjectMap::callOnNamedObject);
    MAP_OBJ(glProgramUniform, m_programs, ProgramObjectMap::callOnNamedObject);

    MAP_OBJ(glUniform, m_programs, ProgramObjectMap::callOnBoundObject);
    MAP_OBJ(glUniformMatrix, m_programs, ProgramObjectMap::callOnBoundObject);
    MAP_OBJ(glUniformBlockBinding, m_programs, ProgramObjectMap::callOnNamedObject);
    MAP_RV(glUseProgram, oglBind, m_programs, 0);
    MAP_RV(glUseProgramObject, oglBind, m_programs, 0);
    MAP_OBJ(glProgramParameter, m_programs, ProgramObjectMap::callOnNamedObject);
    MAP_OBJ(glShaderStorageBlockBinding, m_programs, ProgramObjectMap::callOnNamedObject);

    MAP_V(glGenProgramPipelines, generate, pc_program_pipelines);
    MAP_V(glDeleteProgramPipelines, destroy, pc_program_pipelines);
    MAP_VV(glBindProgramPipelines, bindPerContext, pc_program_pipelines, 0);
    MAP_VRV(glUseProgramStages, callOnNamedObjectWithNamedDep, pc_program_pipelines, m_programs, 2);
    MAP_VRV(glActiveShaderProgram, callOnNamedObjectWithNamedDep, pc_program_pipelines, m_programs, 1);
}

void OpenGLImpl::registerTextureCalls()
{
    MAP_OBJ(glGenTextures, m_textures, TextureObjectMap::generate);
    MAP_OBJ(glCreateTextures, m_textures, TextureObjectMap::generateWithTarget);
    MAP_OBJ(glDeleteTextures, m_textures, TextureObjectMap::destroy);

    MAP_OBJ(glActiveTexture, m_textures, TextureObjectMap::oglActiveTexture);
    MAP_OBJ(glClientActiveTexture, m_textures, TextureObjectMap::oglActiveTexture);
    MAP_RV(glBindTexture, oglBind, m_textures, 1);
    MAP(glBindMultiTexture, oglBindMultitex);

    MAP_OBJ(glGenerateMipmap, m_textures, TextureObjectMap::callOnBoundObject);
    MAP_OBJ(glGenerateTextureMipmap, m_textures, TextureObjectMap::callOnNamedObject);
    MAP_OBJ_RV(glTexImage1D, m_textures, TextureObjectMap::callOnBoundObjectWithDepBoundTo,
               m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glTexImage2D, m_textures, TextureObjectMap::callOnBoundObjectWithDepBoundTo,
               m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glTexImage2DMultisample, m_textures, TextureObjectMap::callOnBoundObjectWithDepBoundTo,
               m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glTexImage3D, m_textures, TextureObjectMap::callOnBoundObjectWithDepBoundTo,
               m_buffers, GL_PIXEL_UNPACK_BUFFER);

    MAP_OBJ_RV(glTextureImage1D, m_textures, TextureObjectMap::callOnNamedObjectWithDepBoundTo,
               m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glTextureSubImage1D, m_textures, TextureObjectMap::callOnNamedObjectWithDepBoundTo,
               m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glTextureImage2D, m_textures, TextureObjectMap::callOnNamedObjectWithDepBoundTo,
               m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glTextureSubImage2D, m_textures, TextureObjectMap::callOnNamedObjectWithDepBoundTo,
               m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glTextureImage3D, m_textures, TextureObjectMap::callOnNamedObjectWithDepBoundTo,
               m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glTextureSubImage3D, m_textures, TextureObjectMap::callOnNamedObjectWithDepBoundTo,
               m_buffers, GL_PIXEL_UNPACK_BUFFER);

    MAP_OBJ(glTexStorage1D, m_textures, TextureObjectMap::callOnBoundObject);
    MAP_OBJ(glTexStorage2D, m_textures, TextureObjectMap::callOnBoundObject);
    MAP_OBJ(glTexStorage2DMultisample, m_textures, TextureObjectMap::callOnBoundObject);
    MAP_OBJ(glTextureStorage2D, m_textures, TextureObjectMap::callOnNamedObject);
    MAP_OBJ(glTexStorage3D, m_textures, TextureObjectMap::callOnBoundObject);
    MAP_OBJ(glTexStorage3DMultisample, m_textures, TextureObjectMap::callOnBoundObject);
    MAP_OBJ(glTexImage3D, m_textures, TextureObjectMap::callOnBoundObject);
    MAP_OBJ(glClearTexImage, m_textures, TextureObjectMap::callOnNamedObject);
    MAP_OBJ(glClearTexSubImage, m_textures, TextureObjectMap::callOnNamedObject);
    MAP_OBJ(glTexImage3DMultisample, m_textures, TextureObjectMap::callOnBoundObject);
    MAP_OBJ_RV(glTexSubImage1D, m_textures, TextureObjectMap::callOnBoundObjectWithDepBoundTo,
                    m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glTexSubImage2D, m_textures, TextureObjectMap::callOnBoundObjectWithDepBoundTo,
                    m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glCompressedTexImage2D, m_textures, TextureObjectMap::callOnBoundObjectWithDepBoundTo,
               m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glCompressedTexSubImage2D, m_textures, TextureObjectMap::callOnBoundObjectWithDepBoundTo,
                  m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glCompressedTextureSubImage2DEXT, m_textures, TextureObjectMap::callOnNamedObjectWithDepBoundTo,
               m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glCompressedTexSubImage3D, m_textures, TextureObjectMap::callOnBoundObjectWithDepBoundTo,
                  m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glTexSubImage3D, m_textures, TextureObjectMap::callOnBoundObjectWithDepBoundTo,
                  m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ(glTexParameter, m_textures, TextureObjectMap::callOnBoundObject);
    MAP_OBJ(glTextureParameter, m_textures, TextureObjectMap::callOnNamedObject);
    MAP_OBJ_RVV(glTextureView, m_textures, TextureObjectMap::callOnNamedObjectWithDep,
                   m_textures, 2, true);

    MAP_OBJ_RVV(glTexBuffer, m_textures, TextureObjectMap::callOnBoundObjectWithDep,
                   m_buffers, 2, true);
    MAP_OBJ_RVV(glTextureBuffer, m_textures, TextureObjectMap::callOnNamedObjectWithDep,
                   m_buffers, 2, true);

    /* Should add a dependency on the read fbo */
    MAP_OBJ(glCopyTexSubImage1D, m_textures, TextureObjectMap::callOnBoundObject);
    MAP_OBJ(glCopyTexSubImage2D, m_textures, TextureObjectMap::callOnBoundObject);

    MAP_OBJ(glCopyTexImage1D, m_textures, TextureObjectMap::callOnBoundObject);
    MAP_OBJ(glCopyTexImage2D, m_textures, TextureObjectMap::callOnBoundObject);

    MAP_OBJ(glCopyImageSubData, m_textures, TextureObjectMap::copy);
    MAP_OBJ(glBindImageTexture, m_textures, TextureObjectMap::bindToImageUnit);

    MAP_RV(glBindSampler, oglBind, m_samplers, 1);
    MAP_OBJ(glGenSamplers, m_samplers, SamplerObjectMap::generate);
    MAP_OBJ(glDeleteSamplers, m_samplers, SamplerObjectMap::destroy);
    MAP_OBJ(glSamplerParameter, m_samplers, SamplerObjectMap::callOnNamedObject);
    MAP(glBindTextures, oglBindTextures);
    MAP(glBindSamplers, oglBindSamplers);

}

void OpenGLImpl::registerQueryCalls()
{
    MAP_OBJ(glGenQueries, m_queries, QueryObjectMap::generate);
    MAP_OBJ(glDeleteQueries, m_queries, QueryObjectMap::destroy);
    MAP_RV(glBeginQuery, bindWithCreate, m_queries, 1);
    MAP_OBJ(glEndQuery, m_queries, QueryObjectMap::oglEndQuery);
    MAP_OBJ(glEndQueryIndexed, m_queries, QueryObjectMap::oglEndQueryIndexed);
    MAP_OBJ(glGetQueryObject, m_queries, QueryObjectMap::callOnNamedObject);
    MAP_OBJ(glGetQueryiv, m_queries, QueryObjectMap::callOnBoundObject);
    MAP_OBJ(glQueryCounter, m_queries, QueryObjectMap::callOnNamedObject); 
}

void
OpenGLImpl::registerStateCalls()
{
    /* These Functions change the state and we only need to remember the last
     * call before the target frame or an fbo creating a dependency is draw. */
    const std::vector<const char *> state_calls  = {
        "glAlphaFunc",
        "glBlendColor",
        "glBlendEquation",
        "glBlendEquationSeparate",
        "glBlendFunc",
        "glBlendFuncSeparate",
        "glClearColor",
        "glClearDepth",
        "glClearStencil",
        "glClipControl",
        "glColorMask",
        "glCullFace",
        "glDebugMessageCallback",
        "glDebugMessageControl",
        "glDepthFunc",
        "glDepthMask",
        "glDepthRange",
        "glDepthBounds",
        "glFlush",
        "glFrontFace",
        "glFrustum",
        "glLineStipple",
        "glLineWidth",
        "glListBase",
        "glLogicOp",
        "glMultiTexCoord",
        "glMemoryBarrier",
        "glPatchParameteri",
        "glPixelZoom",
        "glPointSize",
        "glPolygonMode",
        "glPolygonOffset",
        "glPolygonStipple",
        "glPopAttrib",
        "glPopClientAttrib",
        "glProvokingVertex",
        "glPrimitiveRestartIndex",
        "glPrimitiveBoundingBox",
        "glRasterPos",
        "glSampleCoverage",
        "glShadeModel",
        "glScissor",
        "glStencilMask",
        "glFinish",
    };

    auto state_call_func = bind(&OpenGLImpl::recordStateCall, this, _1, 0);
    auto state_call_1_func = bind(&OpenGLImpl::recordStateCall, this, _1, 1);
    auto keep_state_calls_func = bind(&OpenGLImpl::recordRequiredCall, this, _1);

    /* These are state functions with an extra parameter */

    const std::vector<const char *> state_calls_1  = {
        "glActiveStencilFace",
        "glClampColor",
        "glClipPlane",
        "glColorMaskIndexedEXT",
        "glColorMaterial",
        "glFog",
        "glHint",
        "glLight",
        "glLightModel",
        "glMinSampleShading",
        "glPatchParameteri",
        "glPatchParameterfv",
        "glPixelTransfer",
        "glPointParameter",
        "glPushAttrib",
        "glPushClientAttrib",
        "glRenderMode",
        "glSampleMaski",
        "glScissorIndexed",
        "glStencilOp",
        "glStencilOpSeparate",
        "glStencilFunc",
        "glStencilFuncSeparate",
        "glVertexAttribDivisor",
        "glMultiTexCoord",
    };

    /* These are state functions with an extra parameter */
    auto state_call_2_func = bind(&OpenGLImpl::recordStateCall, this, _1, 2);
    const std::vector<const char *> state_calls_2  = {
        "glMaterial",
        "glTexEnv",
    };

    if (m_keep_all_state_calls) {
        updateCallTable(state_calls, keep_state_calls_func);
        updateCallTable(state_calls_1, keep_state_calls_func);
        updateCallTable(state_calls_2, keep_state_calls_func);
    } else {
        updateCallTable(state_calls, state_call_func);
        updateCallTable(state_calls_1, state_call_1_func);
        updateCallTable(state_calls_2, state_call_2_func);
    }

    MAP(glDisable, recordRequiredCall);
    MAP(glDisablei, recordRequiredCall);
    MAP(glEnable, recordRequiredCall);
    MAP(glEnablei, recordRequiredCall);

    MAP_OBJ(glFenceSync, m_sync_objects, SyncObjectMap::create);
    MAP(glWaitSync, oglWaitSync);
    MAP(glClientWaitSync, oglWaitSync);
    MAP_OBJ(glDeleteSync, m_sync_objects, SyncObjectMap::callOnNamedObject);
}

void OpenGLImpl::registerRequiredCalls()
{
    /* These function set up the context and are, therefore, required
     * TODO: figure out what is really required, and whether the can be
     * tracked like state variables. */
    auto required_func = bind(&OpenGLImpl::recordRequiredCall, this, _1);
    const std::vector<const char *> required_calls = {
        "glXChooseVisual",
        "glXCreatePbuffer",
        "glXCreateWindow",
        "glXDestroyContext",
        "glXGetFBConfigAttrib",
        "glXChooseFBConfig",
        "glXQueryExtension",
        "glXQueryExtensionsString",
        "glXSwapIntervalMESA",
        "glXWaitGL",

        "eglInitialize",
        "eglCreatePlatformWindowSurface",
        "eglDestroyContext",
        "eglDestroySurface",
        "eglBindAPI",
        "eglSwapInterval", 
        "eglTerminate",
        
        "wglChoosePixelFormat", 
        "wglSetPixelFormat", 
        "wglSwapIntervalEXT", 
        
        "glPixelStorei", /* Being lazy here, we could track the dependency
                            in the relevant calls */
    };
    updateCallTable(required_calls, required_func);

    MAP_V(glXCreateContext, createContext, -1);
    MAP_V(glXCreateNewContext, createContext, -1);
    MAP_V(glXCreateContextAttribs, createContext, 2);
    MAP_V(eglCreateContext, createContext, 2);
    MAP_V(wglCreateContext, createContext, -1);
    MAP_V(wglCreateContextAttribs, createContext, 1);

    MAP_V(glXMakeCurrent, makeCurrent, 2);
    MAP_V(glXMakeContextCurrent, makeCurrent, 3);
    MAP_V(eglMakeCurrent, makeCurrent, 3);
    MAP_V(wglMakeCurrent, makeCurrent, 1);
}

void OpenGLImpl::registerIgnoreHistoryCalls()
{
    /* These functions are ignored outside required recordings
     * TODO: ignore them also in the copied frames */
    const std::vector<const char *> ignore_history_calls = {
        "glCheckFramebufferStatus",
        "glGetActiveUniform",
        "glGetActiveUniforms",
        "glGetActiveUniformName",
        "glGetActiveUniformBlockName",
        "glGetActiveUniformBlock",
        "glGetBoolean",
        "glGetError",
        "glGetFloat",
        "glGetFramebufferAttachmentParameter",
        "glGetFramebufferParameteriv",
        "glGetInfoLog",
        "glGetInteger",
        "glGetIntegeri_v",
        "glGetNamedFramebufferAttachmentParameteriv", 
        "glGetNamedFramebufferParameteriv",
        "glGetObjectLabelEXT",
        "glGetObjectParameter",
        "glGetProgram",
        "glGetProgramInfoLog",
        "glGetShader",
        "glGetString",
        "glGetShaderInfoLog",
        "glGetTexLevelParameter",
        "glGetTexParameter",
        "glGetTexImage",
        "glGetUniform",
        "glLabelObjectEXT",
        "glIsEnabled",
        "glIsFramebuffer",
        "glIsRenderbuffer",
        "glIsSync",
        "glIsTexture", 
        "glIsVertexArray",
        "glReadPixels",
        "glXGetClientString",
        "glXGetCurrentContext",
        "glXGetCurrentDisplay",
        "glXGetCurrentDrawable",
        "glXGetFBConfigAttrib",
        "glXGetFBConfigs",
        "glXGetProcAddress",
        "glXGetSwapIntervalMESA",
        "glXGetVisualFromFBConfig",
        "glXQueryDrawable",
        "glXQueryVersion",
        "glXSwapIntervalEXT",
        "eglGetProcAddress",
        "eglQueryString",
        "eglGetError",
        "eglGetPlatformDisplay",
        "eglGetConfigs",
        "eglGetConfigAttrib",
        "eglGetCurrentContext",
        "eglGetCurrentDisplay",
        "eglGetCurrentSurface",
        "eglQuerySurface",
        "wglGetCurrentDC",
        "wglGetCurrentContext",
        "wglGetProcAddress",
        "wglGetExtensionsString",
        "wglGetPixelFormat", 
        "wglDeleteContext", 
        "wglDescribePixelFormat"
     };
    auto ignore_history_func = bind(&OpenGLImpl::ignoreHistory, this, _1);
    updateCallTable(ignore_history_calls, ignore_history_func);
}


void
OpenGLImpl::registerVaCalls()
{
    MAP_V(glGenVertexArrays, generate, pc_vertex_array);
    MAP_V(glDeleteVertexArrays, destroy, pc_vertex_array);
    MAP(glBindVertexArray, oglBindVertexArray);
    //MAP_OBJ(glVertexAttribBinding, m_vertex_buffer_pointers, VertexAttribObjectMap::vaBinding);

    MAP(glDisableVertexAttribArray, recordRequiredCall);
    MAP(glEnableVertexAttribArray, recordRequiredCall);
    MAP_OBJ_R(glVertexAttribPointer, m_vertex_attrib_pointers,
                   VertexAttribObjectMap::bindAVO, m_buffers);
    MAP_OBJ_R(glVertexAttribIPointer, m_vertex_attrib_pointers,
                   VertexAttribObjectMap::bindAVO, m_buffers);
    MAP_OBJ_R(glVertexAttribLPointer, m_vertex_attrib_pointers,
                   VertexAttribObjectMap::bindAVO, m_buffers);

    MAP_OBJ_R(glBindVertexBuffer, m_vertex_buffer_pointers,
                VertexAttribObjectMap::bindVAOBuf, m_buffers);

    MAP(glColorPointer, recordRequiredCall);
    MAP(glVertexPointer, recordRequiredCall);
    MAP(glNormalPointer, recordRequiredCall);
    MAP(glTexCoordPointer, recordRequiredCall);

    MAP(glVertexAttribBinding, recordRequiredCall);
    MAP(glVertexAttribFormat, recordRequiredCall);
    MAP(glVertexAttribIFormat, recordRequiredCall);
    MAP(glVertexAttribLFormat, recordRequiredCall);
    MAP(glVertexBindingDivisor, recordRequiredCall);

    MAP(glVertexAttrib1, recordRequiredCall);
    MAP(glVertexAttrib2, recordRequiredCall);
    MAP(glVertexAttrib3, recordRequiredCall);
    MAP(glVertexAttrib4, recordRequiredCall);
    MAP(glVertexAttribI, recordRequiredCall);
    MAP(glVertexAttribL, recordRequiredCall);
    MAP(glVertexAttribP1, recordRequiredCall);
    MAP(glVertexAttribP2, recordRequiredCall);
    MAP(glVertexAttribP3, recordRequiredCall);
    MAP(glVertexAttribP4, recordRequiredCall);
    MAP(glTexGen, recordRequiredCall);
    MAP(glTextureBarrier, recordRequiredCall);

    MAP(glDisableClientState, recordRequiredCall);
    MAP(glEnableClientState, recordRequiredCall);

}

void
OpenGLImpl::updateCallTable(const std::vector<const char*>& names,
                                        ft_callback cb)
{
    for (auto& i : names)
        m_call_table.insert(std::make_pair(i, cb));
}

PTraceCall
OpenGLImpl::recordStateCall(const trace::Call& call,
                                   unsigned no_param_sel)
{
    std::stringstream s;
    s << call.name();
    for (unsigned i = 0; i < no_param_sel; ++i)
        s << "_" << call.arg(i).toUInt();

    auto c = std::make_shared<TraceCall>(call, s.str());
    m_state_calls[s.str()] = c;

    if (m_active_display_list)
        m_active_display_list->addCall(c);

    return c;
}

void
OpenGLImpl::oglBegin(const trace::Call& call)
{
    if (m_active_display_list)
        m_active_display_list->addCall(trace2call(call));
}

void
OpenGLImpl::oglEnd(const trace::Call& call)
{
    if (m_active_display_list)
        m_active_display_list->addCall(trace2call(call));
}

void
OpenGLImpl::oglVertex(const trace::Call& call)
{
    if (m_active_display_list)
        m_active_display_list->addCall(trace2call(call));
}

void
OpenGLImpl::oglEndList(const trace::Call& call)
{
    if (!m_recording_frame)
        m_active_display_list->addCall(trace2call(call));

    m_active_display_list = nullptr;
}

void
OpenGLImpl::oglGenLists(const trace::Call& call)
{
    unsigned nlists = call.arg(0).toUInt();
    GLuint origResult = (*call.ret).toUInt();
    for (unsigned i = 0; i < nlists; ++i)
        m_display_lists[i + origResult] = std::make_shared<UsedObject>(i + origResult);

    auto c = trace2call(call);
    if (!m_recording_frame)
        m_display_lists[origResult]->addCall(c);
}

void
OpenGLImpl::oglNewList(const trace::Call& call)
{
    assert(!m_active_display_list);
    auto list  = m_display_lists.find(call.arg(0).toUInt());
    assert(list != m_display_lists.end());
    m_active_display_list = list->second;

    if (!m_recording_frame)
        m_active_display_list->addCall(trace2call(call));
}

void
OpenGLImpl::oglCallList(const trace::Call& call)
{
    auto list  = m_display_lists.find(call.arg(0).toUInt());
    assert(list != m_display_lists.end());

    if (m_recording_frame)
        list->second->emitCallsTo(m_required_calls);
}

void
OpenGLImpl::oglDeleteLists(const trace::Call& call)
{
    GLint value = call.arg(0).toUInt();
    GLint value_end = call.arg(1).toUInt() + value;
    for(int i = value; i < value_end; ++i) {
        auto list  = m_display_lists.find(call.arg(0).toUInt());
        assert(list != m_display_lists.end());
        list->second->addCall(trace2call(call));
        m_display_lists.erase(list);
    }
}

void
OpenGLImpl::oglBind(const trace::Call& call, DependecyObjectMap& map,
                      unsigned bind_param)
{
    auto bound_obj = map.bind(call, bind_param);
    if (bound_obj)
        bound_obj->addCall(trace2call(call));
    else
        map.addCall(trace2call(call));

    if (m_recording_frame && bound_obj)
        bound_obj->emitCallsTo(m_required_calls);
}

void
OpenGLImpl::oglBindBuffer(const trace::Call& call)
{
    auto bound_obj = m_buffers.bind(call, 1);
    if (bound_obj) {
        bound_obj->addCall(trace2call(call));
        if (call.arg(0).toUInt() == GL_ELEMENT_ARRAY_BUFFER) {
            if (global_state.current_vao)
                global_state.current_vao->addDependency(bound_obj);
        }
    } else
        m_buffers.addCall(trace2call(call));

    if (m_recording_frame && bound_obj)
        bound_obj->emitCallsTo(m_required_calls);
}


void
OpenGLImpl::oglBindTextures(const trace::Call& call)
{
    unsigned first = call.arg(0).toUInt();
    unsigned count  = call.arg(1).toUInt();

    const auto ids = (call.arg(2)).toArray();
    if (ids) {
        for (unsigned i = 0; i < count; ++i) {
            auto v = ids->values[i];
            unsigned id = v->toUInt();
            if (id) {
                auto obj = m_textures.getById(id);
                obj->addCall(trace2call(call));
                m_textures.bindFromTextureTarget(first + i, obj);
                if (m_recording_frame)
                    obj->emitCallsTo(m_required_calls);
            } else {
                m_textures.unbindUnits(first + i, 1);
            }
        }
        /* Make all bound textures depend on one-another
         * so that if this call is issued, then all used
         * textures are available */
        for (unsigned i = 0; i < count; ++i) {
            unsigned id = ids->values[i]->toUInt();
            if (id) {
                auto obj = m_textures.getById(id);
                for (unsigned j = i + 1; j < count; ++j) {
                    unsigned id2 = ids->values[j]->toUInt();
                    if (id2) {
                        auto dep = m_textures.getById(id2);
                        obj->addDependency(dep);
                        dep->addDependency(obj);
                    }
                }
            }
        }

    } else {
        m_textures.unbindUnits(first, count);
        m_textures.addCall(trace2call(call));
    }
}

void
OpenGLImpl::oglBindSamplers(const trace::Call& call)
{
    unsigned first = call.arg(0).toUInt();
    unsigned count  = call.arg(1).toUInt();

    const auto ids = (call.arg(2)).toArray();
    if (ids) {
        for (unsigned i = 0; i < count; ++i) {
            auto v = ids->values[i];
            unsigned id = v->toUInt();
            if (id) {
                auto obj = m_samplers.getById(id);
                obj->addCall(trace2call(call));
                m_samplers.bind(first + i, id);
                if (m_recording_frame)
                    obj->emitCallsTo(m_required_calls);
            } else {
                m_samplers.bind(first + i, 0);
            }
        }
        /* Make all bound textures depend on one-another
         * so that if this call is issued, then all used
         * textures are available */
        for (unsigned i = 0; i < count; ++i) {
            unsigned id = ids->values[i]->toUInt();
            if (id) {
                auto obj = m_samplers.getById(id);
                for (unsigned j = i + 1; j < count; ++j) {
                    unsigned id2 = ids->values[j]->toUInt();
                    if (id2) {
                        auto dep = m_samplers.getById(id2);
                        obj->addDependency(dep);
                        dep->addDependency(obj);
                    }
                }
            }
        }

    } else {
        for (unsigned i = 0; i < count; ++i)
            m_samplers.bind(first + i, 0);
        m_samplers.addCall(trace2call(call));
    }
}


void OpenGLImpl::oglDraw(const trace::Call& call)
{
    auto fb = m_current_context->m_fbo.boundTo(GL_DRAW_FRAMEBUFFER);
    if (!fb)
        fb = m_fbo_ext.boundTo(GL_DRAW_FRAMEBUFFER);
    auto cur_prog = m_programs.boundTo(0, 0);
    if (cur_prog) {
        for(auto&& [ctx_id, bound_buffers]: m_buffers) {
            for(auto&& [key, buf]: bound_buffers) {
                if (buf && ((key % BufferObjectMap::bt_last) == BufferObjectMap::bt_uniform))
                    cur_prog->addDependency(buf);
            }
        }
        m_buffers.addSSBODependencies(cur_prog);
        m_textures.addImageDependencies(cur_prog);

        if (fb->id())
            fb->addDependency(cur_prog);

        if (m_recording_frame)
            cur_prog->emitCallsTo(m_required_calls);
    }

    auto cur_pipeline = m_current_context->m_program_pipelines.boundTo(0, 0);
    if (cur_pipeline) {
        for(auto&& [ctx_id, bound_buffers]: m_buffers) {
            for(auto&& [key, buf]:  bound_buffers) {
                if (buf && ((key % BufferObjectMap::bt_last) == BufferObjectMap::bt_uniform))
                    cur_pipeline->addDependency(buf);
            }
        }
        m_buffers.addSSBODependencies(cur_pipeline);
        m_textures.addImageDependencies(cur_pipeline);

        if (fb->id())
            fb->addDependency(cur_pipeline);

        if (m_recording_frame)
            cur_pipeline->emitCallsTo(m_required_calls);
    }

    if (fb->id()) {
        fb->addCall(trace2call(call));

        m_buffers.addBoundAsDependencyTo(*fb);
        m_current_context->m_vertex_arrays.addBoundAsDependencyTo(*fb);
        m_textures.addBoundAsDependencyTo(*fb);
        m_programs.addBoundAsDependencyTo(*fb);
        m_legacy_programs.addBoundAsDependencyTo(*fb);
        m_current_context->m_program_pipelines.addBoundAsDependencyTo(*fb);
        m_samplers.addBoundAsDependencyTo(*fb);

        for(auto&& [ctx_id, bound_vao]: m_vertex_attrib_pointers) {
            for(auto&& [key,vao]: bound_vao) {
                if (vao)
                    fb->addDependency(vao);
            }
        }
        for(auto&& [ctx_id, bound_vbo]: m_vertex_buffer_pointers) {
            for(auto&& [key, vbo]: bound_vbo) {
                if (vbo)
                    fb->addDependency(vbo);
            }
        }
    }

    if (m_recording_frame) {
        for(auto&& [key, vao]: m_current_context->m_vertex_arrays.objects_bound_in_context()) {
            if (vao)
                vao->emitCallsTo(m_required_calls);
        }
        for(auto&& [ctx_id, bound_vao]: m_vertex_attrib_pointers) {
            for(auto&& [key, vao]: bound_vao) {
                if (vao)
                    vao->emitCallsTo(m_required_calls);
            }
        }
        for(auto&& [ctx_id, bound_vbo]: m_vertex_buffer_pointers) {
            for(auto&& [key, vbo]: bound_vbo) {
                if (vbo)
                    vbo->emitCallsTo(m_required_calls);
            }
        }
        m_buffers.emitBoundObjects(m_required_calls);
    }
}

void
OpenGLImpl::bindWithCreate(const trace::Call& call, DependecyObjectMap& map,
                                unsigned bind_param)
{
    auto bound_obj = map.bindWithCreate(call, bind_param);
    if (bound_obj)
        bound_obj->addCall(trace2call(call));
    else
        map.addCall(trace2call(call));
    if (m_recording_frame && bound_obj)
        bound_obj->emitCallsTo(m_required_calls);
}

void OpenGLImpl::oglBindMultitex(const trace::Call& call)
{
    auto tex = m_textures.oglBindMultitex(call);
    if (tex)
        tex->addCall(trace2call(call));
    else
        m_textures.addCall(trace2call(call));

    if (m_recording_frame && tex)
        tex->emitCallsTo(m_required_calls);
}

void
OpenGLImpl::oglWaitSync(const trace::Call& call)
{
    auto obj = m_sync_objects.getById(call.arg(0).toUInt());
    assert(obj);
    obj->addCall(trace2call(call));
    if (m_recording_frame)
        obj->emitCallsTo(m_required_calls);
}

void
OpenGLImpl::oglBindFbo(const trace::Call& call, unsigned bind_param)
{
    auto fb = m_current_context->m_fbo.bind(call, bind_param);
    fb->addCall(trace2call(call));
    if (m_recording_frame && fb->id())
        fb->emitCallsTo(m_required_calls);
}

void
OpenGLImpl::callOnBoundObjWithDep(const trace::Call& call, DependecyObjectMap& map,
                                        unsigned obj_id_param,
                                        DependecyObjectMap& dep_map, bool reverse_dep_too)
{
    auto dep_obj = map.callOnBoundObjectWithDep(call, dep_map, obj_id_param,
                                                reverse_dep_too);
    if (m_recording_frame && dep_obj)
        dep_obj->emitCallsTo(m_required_calls);
}


void OpenGLImpl::callOnNamedObjectWithNamedDep(const trace::Call& call, ePerContext object_type,
                                                    DependecyObjectMap &dep_map, unsigned dep_id_param)
{
    auto& map = getCurrentMapOfType(object_type);
    map.callOnNamedObjectWithNamedDep(call, dep_map, dep_id_param);
}

void OpenGLImpl::oglDispatchCompute(const trace::Call& call, bool indirect)
{
    auto cur_prog = m_programs.boundTo(0);
    assert(cur_prog);

    cur_prog->addCall(trace2call(call));
    // Without knowning how the compute program is actually written
    // we have to assume that any bound ssbo and any bound image can be
    // changed by the program, so the program has to depend on all bound
    // ssbos and images, ane they in turn must depend on the program

    m_buffers.addSSBODependencies(cur_prog);
    m_textures.addImageDependencies(cur_prog);

    if (indirect) {
        auto buffer = m_buffers.boundToTarget(GL_DISPATCH_INDIRECT_BUFFER);
        assert(buffer);
        cur_prog->addDependency(buffer);
    }

    if (m_recording_frame)
        cur_prog->emitCallsTo(m_required_calls);
}

void
OpenGLImpl::todo(const trace::Call& call)
{
    std::cerr << "TODO: " << call.name() << "\n";
}

void
OpenGLImpl::ignoreHistory(const trace::Call& call)
{
    (void)call;
}

void
OpenGLImpl::recordRequiredCall(const trace::Call& call)
{
    auto c = trace2call(call);
    m_required_calls.insert(c);
}

void
OpenGLImpl::oglBindVertexArray(const trace::Call& call)
{
    auto vao = m_current_context->m_vertex_arrays.bind(call, 0);
    global_state.current_vao = vao;
    if (vao) {
        vao->addCall(trace2call(call));
        if (global_state.emit_dependencies)
            vao->emitCallsTo(*global_state.out_list);
        auto fb = m_current_context->m_fbo.boundTo(GL_DRAW_FRAMEBUFFER);
        if (fb->id())
            fb->addDependency(vao);
    } else
        m_current_context->m_vertex_arrays.addCall(trace2call(call));
}

void OpenGLImpl::generate(const trace::Call& call, ePerContext map_type)
{
    getCurrentMapOfType(map_type).generate(call);
}

void OpenGLImpl::destroy(const trace::Call& call, ePerContext map_type)
{
    getCurrentMapOfType(map_type).destroy(call);
}

void OpenGLImpl::bindPerContext(const trace::Call& call, ePerContext map_type, unsigned bind_param)
{
    oglBind(call, getCurrentMapOfType(map_type), bind_param);
}

void OpenGLImpl::callOnObjectBoundTo(const trace::Call& call, ePerContext map_type, unsigned bindpoint)
{
    getCurrentMapOfType(map_type).callOnObjectBoundTo(call, bindpoint);
}

void OpenGLImpl::callOnBoundObject(const trace::Call& call, ePerContext map_type)
{
    getCurrentMapOfType(map_type).callOnBoundObject(call);
}

void OpenGLImpl::callOnNamedObject(const trace::Call& call, ePerContext map_type)
{
    getCurrentMapOfType(map_type).callOnNamedObject(call);
}

void OpenGLImpl::fboBindAttachment(const trace::Call& call, DependecyObjectMap& dep_map, unsigned tex_id_param)
{
    m_current_context->m_fbo.callOnBoundObjectWithDep(call, dep_map, tex_id_param, true);
}

void OpenGLImpl::fboBindAttachmentEXT(const trace::Call& call, DependecyObjectMap& dep_map, unsigned tex_id_param)
{
    m_fbo_ext.callOnBoundObjectWithDep(call, dep_map, tex_id_param, true);
}


void OpenGLImpl::fboNamedBindAttachment(const trace::Call& call, DependecyObjectMap& dep_map, int obj_param_pos)
{
    m_current_context->m_fbo.callOnNamedObjectWithDep(call, dep_map, obj_param_pos, true); 
}

void OpenGLImpl::fboBlit(const trace::Call& call)
{
    m_current_context->m_fbo.oglBlit(call);
}

void OpenGLImpl::fboBlitNamed(const trace::Call& call)
{
    m_current_context->m_fbo.oglBlitNamed(call);
}

void OpenGLImpl::fboReadBuffer(const trace::Call& call)
{
    m_current_context->m_fbo.oglReadBuffer(call);
}

void OpenGLImpl::createContext(const trace::Call& call, int shared_param)
{
    if (!m_contexts.empty()) {
        void *shared_context = nullptr;
        if (shared_param >= 0) {
            shared_context = call.arg(shared_param).toPointer();
            if (m_contexts.find(shared_context) == m_contexts.end())
                shared_context = nullptr;
        }
        if (!shared_context)
            std::cerr << "\nWarning: Have more than one non-shared context, this is not well supported\n";
    }
    m_contexts[call.ret->toPointer()] = make_shared<PerContextObjects>();
    m_required_calls.insert(trace2call(call));
    assert(m_contexts[call.ret->toPointer()]->m_fbo.bind(GL_FRAMEBUFFER, 0));
}

void OpenGLImpl::makeCurrent(const trace::Call& call, unsigned param)
{
    void *ctx_id = call.arg(param).toPointer();
    auto ictx = m_contexts.find(ctx_id);
    uint32_t context_id = 0;

    if (ictx != m_contexts.end()) {
        m_current_context = ictx->second;
        m_thread_active_context[call.thread_id] = ictx->second;
        context_id = m_current_context->m_id;
    } else
        m_current_context = nullptr;

    assert(!ctx_id || m_current_context);
    m_required_calls.insert(trace2call(call));
    update_context_id(context_id);
}

void OpenGLImpl::update_context_id(uint32_t context_id)
{
    m_buffers.set_current_context_id(context_id);
    m_textures.set_current_context_id(context_id);
    m_fbo_ext.set_current_context_id(context_id);
    m_renderbuffers.set_current_context_id(context_id);
    m_programs.set_current_context_id(context_id);
    m_samplers.set_current_context_id(context_id);
    m_queries.set_current_context_id(context_id);
    m_vertex_attrib_pointers.set_current_context_id(context_id);
    m_vertex_buffer_pointers.set_current_context_id(context_id);
    m_shaders.set_current_context_id(context_id);
    m_sync_objects.set_current_context_id(context_id);
    m_legacy_programs.set_current_context_id(context_id);
}

void OpenGLImpl::switch_thread(int thread_id)
{
    uint32_t context_id = 0;
    auto ictx = m_thread_active_context.find(thread_id);
    if (ictx != m_thread_active_context.end()) {
        m_current_context = ictx->second;
        context_id = m_current_context->m_id;
    } else {
        m_current_context = nullptr;
    }
    update_context_id(context_id);
}

DependecyObjectMap& OpenGLImpl::getCurrentMapOfType(ePerContext map_type)
{
    switch (map_type) {
    case pc_vertex_array: return m_current_context->m_vertex_arrays;
    case pc_program_pipelines: return m_current_context->m_program_pipelines;
    case pc_fbo: return m_current_context->m_fbo;
    }
    assert(0);
    abort();
}

}
