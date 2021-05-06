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
#include "ft_tracecall.hpp"
#include "ft_matrixstate.hpp"
#include "ft_dependecyobject.hpp"


#include "trace_model.hpp"


#include <unordered_set>
#include <algorithm>
#include <functional>
#include <sstream>
#include <iostream>
#include <memory>
#include <set>

#include <GL/gl.h>
#include <GL/glext.h>

namespace frametrim {

using std::bind;
using std::placeholders::_1;
using std::make_shared;


using ft_callback = std::function<void(const trace::Call&)>;

struct string_part_less {
    bool operator () (const char *lhs, const char *rhs) const
    {
        int len = std::min(strlen(lhs), strlen(rhs));
        return strncmp(lhs, rhs, len) < 0;
    }
};

struct FrameTrimmeImpl {

    using ObjectMap = std::unordered_map<unsigned, UsedObject::Pointer>;

    FrameTrimmeImpl(bool keep_all_states);
    void call(const trace::Call& call, Frametype frametype);
    void startTargetFrame();
    void endTargetFrame();

    std::vector<unsigned> getSortedCallIds();
    std::unordered_set<unsigned> getUniqueCallIds();

    static unsigned equalChars(const char *l, const char *r);

    PTraceCall recordStateCall(const trace::Call& call, unsigned no_param_sel);

    void registerStateCalls();
    void registerLegacyCalls();
    void registerRequiredCalls();
    void registerFramebufferCalls();
    void registerBufferCalls();
    void registerProgramCalls();
    void registerVaCalls();
    void registerTextureCalls();
    void registerQueryCalls();
    void registerDrawCalls();
    void registerIgnoreHistoryCalls();


    void recordRequiredCall(const trace::Call& call);

    void updateCallTable(const std::vector<const char*>& names,
                           ft_callback cb);

    void oglBegin(const trace::Call& call);
    void oglEnd(const trace::Call& call);
    void oglVertex(const trace::Call& call);
    void oglEndList(const trace::Call& call);
    void oglGenLists(const trace::Call& call);
    void oglNewList(const trace::Call& call);
    void oglCallList(const trace::Call& call);
    void oglDeleteLists(const trace::Call& call);
    void oglBind(const trace::Call& call, DependecyObjectMap& map, unsigned bind_param);
    void oglDraw(const trace::Call& call);
    void oglBindFbo(const trace::Call& call, DependecyObjectMap& map,
                       unsigned bind_param);
    void oglBindTextures(const trace::Call& call);
    void bindWithCreate(const trace::Call& call, DependecyObjectMap& map,
                        unsigned bind_param);
    void oglWaitSync(const trace::Call& call);
    void callOnBoundObjWithDep(const trace::Call& call, DependecyObjectMap& map,
                               unsigned obj_id_param, DependecyObjectMap &dep_map, bool reverse_dep_too);

    void oglBindMultitex(const trace::Call& call);
    void oglDispatchCompute(const trace::Call& call);
    void oglBindVertexArray(const trace::Call& call);
    void oglBindBuffer(const trace::Call& call);

    void todo(const trace::Call& call);
    void ignoreHistory(const trace::Call& call);

    bool skipDeleteObj(const trace::Call& call);
    bool skipDeleteImpl(unsigned obj_id, DependecyObjectMap& map);
    void finalize();

    using CallTable = std::multimap<const char *, ft_callback, string_part_less>;
    CallTable m_call_table;

    CallSet m_required_calls;
    std::set<std::string> m_unhandled_calls;

    std::unordered_map<unsigned, UsedObject::Pointer> m_display_lists;
    UsedObject::Pointer m_active_display_list;

    AllMatrisStates m_matrix_states;

    using LegacyProgrambjectMap = DependecyObjectWithDefaultBindPointMap;
    using ProgramObjectMap = DependecyObjectWithSingleBindPointMap;
    using ProgramPipelineObjectMap = DependecyObjectWithSingleBindPointMap;
    using SamplerObjectMap = DependecyObjectWithDefaultBindPointMap;
    using SyncObjectMap = DependecyObjectWithSingleBindPointMap;
    using ShaderStateMap = DependecyObjectWithDefaultBindPointMap;
    using RenderObjectMap = DependecyObjectWithSingleBindPointMap;
    using VertexArrayMap = DependecyObjectWithDefaultBindPointMap;

    LegacyProgrambjectMap m_legacy_programs;
    ProgramObjectMap m_programs;
    ProgramPipelineObjectMap m_program_pipelines;
    TextureObjectMap m_textures;
    FramebufferObjectMap m_fbo;
    BufferObjectMap m_buffers;
    ShaderStateMap m_shaders;
    RenderObjectMap m_renderbuffers;
    SamplerObjectMap m_samplers;
    SyncObjectMap m_sync_objects;
    VertexArrayMap m_vertex_arrays;
    VertexAttribObjectMap m_vertex_attrib_pointers;
    VertexAttribObjectMap m_vertex_buffer_pointers;
    QueryObjectMap m_queries;

    std::map<std::string, PTraceCall> m_state_calls;
    std::map<unsigned, PTraceCall> m_enables;

    bool m_recording_frame;

    PTraceCall m_last_swap;
    bool m_keep_all_state_calls;
};

FrameTrimmer::FrameTrimmer(bool keep_all_states)
{
    impl = new FrameTrimmeImpl(keep_all_states);
}

FrameTrimmer::~FrameTrimmer()
{
    delete impl;
}

void
FrameTrimmer::call(const trace::Call& call, Frametype target_frame_type)
{
    impl->call(call, target_frame_type);
}

std::vector<unsigned>
FrameTrimmer::getSortedCallIds()
{
    return impl->getSortedCallIds();
}

std::unordered_set<unsigned>
FrameTrimmer::getUniqueCallIds()
{
    return impl->getUniqueCallIds();
}

void FrameTrimmer::finalize()
{
    impl->finalize();
}

FrameTrimmeImpl::FrameTrimmeImpl(bool keep_all_states):
    m_recording_frame(false),
    m_keep_all_state_calls(keep_all_states)
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

void
FrameTrimmeImpl::call(const trace::Call& call, Frametype frametype)
{
    const char *call_name = call.name();

    if (!m_recording_frame && (frametype != ft_none))
        startTargetFrame();
    if (m_recording_frame && (frametype == ft_none))
        endTargetFrame();



    auto cb_range = m_call_table.equal_range(call.name());
    if (cb_range.first != m_call_table.end() &&
            std::distance(cb_range.first, cb_range.second) > 0) {

        CallTable::const_iterator cb = cb_range.first;
        CallTable::const_iterator i = cb_range.first;
        ++i;

        unsigned max_equal = equalChars(cb->first, call_name);

        while (i != cb_range.second && i != m_call_table.end()) {
            auto n = equalChars(i->first, call_name);
            if (n > max_equal) {
                max_equal = n;
                cb = i;
            }
            ++i;
        }

        cb->second(call);
    } else {
        /* This should be some debug output only, because we might
         * not handle some calls deliberately */
        if (m_unhandled_calls.find(call_name) == m_unhandled_calls.end()) {
            std::cerr << "Call " << call.no
                      << " " << call_name << " not handled\n";
            m_unhandled_calls.insert(call_name);
        }
    }

    auto c = trace2call(call);

    if (frametype == ft_none) {
        if (call.flags & trace::CALL_FLAG_END_FRAME)
            m_last_swap = c;
    } else {
        /* Skip delete calls for objects that have never been emitted */
        if (skipDeleteObj(call))
            return;

        if (!(call.flags & trace::CALL_FLAG_END_FRAME)) {
            m_required_calls.insert(c);
        } else {
            if (frametype == ft_retain_frame) {
                m_required_calls.insert(c);
                if (m_last_swap) {
                    m_required_calls.insert(m_last_swap);
                    m_last_swap = nullptr;
                }
            } else
                m_last_swap = c;
        }
    }
}

void FrameTrimmeImpl::startTargetFrame()
{
    std::cerr << "Start recording\n";

    m_recording_frame = true;

    m_matrix_states.emitStateTo(m_required_calls);

    for (auto&& [name, call] : m_state_calls)
        m_required_calls.insert(call);

    for (auto&& [id, call]: m_enables)
        m_required_calls.insert(call);

    m_programs.emitBoundObjects(m_required_calls);
    m_program_pipelines.emitBoundObjects(m_required_calls);
    m_textures.emitBoundObjects(m_required_calls);
    m_fbo.emitBoundObjects(m_required_calls);
    m_buffers.emitBoundObjects(m_required_calls);
    m_shaders.emitBoundObjects(m_required_calls);
    m_renderbuffers.emitBoundObjects(m_required_calls);
    m_samplers.emitBoundObjects(m_required_calls);
    m_sync_objects.emitBoundObjects(m_required_calls);
    m_vertex_arrays.emitBoundObjects(m_required_calls);
    m_vertex_attrib_pointers.emitBoundObjects(m_required_calls);
    m_vertex_buffer_pointers.emitBoundObjects(m_required_calls);
    m_legacy_programs.emitBoundObjects(m_required_calls);
    m_queries.emitBoundObjects(m_required_calls);
}

bool
FrameTrimmeImpl::skipDeleteObj(const trace::Call& call)
{
    if (!strcmp(call.name(), "glDeleteProgram"))
        return skipDeleteImpl(call.arg(0).toUInt(), m_programs);

    if (!strcmp(call.name(), "glDeleteProgramPipelines"))
        return skipDeleteImpl(call.arg(0).toUInt(), m_program_pipelines);

    if (!strcmp(call.name(), "glDeleteSync"))
        return skipDeleteImpl(call.arg(0).toUInt(), m_sync_objects);

    DependecyObjectMap *map = nullptr;
    if (!strcmp(call.name(), "glDeleteBuffers"))
        map = &m_buffers;

    if (!strcmp(call.name(), "glDeleteTextures"))
        map = &m_textures;

    if (!strcmp(call.name(), "glDeleteFramebuffers"))
        map = &m_fbo;

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
FrameTrimmeImpl::skipDeleteImpl(unsigned obj_id, DependecyObjectMap& map)
{
    auto obj = map.getById(obj_id);
    return !obj->emitted();
}

void FrameTrimmeImpl::finalize()
{
    if (m_last_swap)
        m_required_calls.insert(m_last_swap);
}

void FrameTrimmeImpl::endTargetFrame()
{
    std::cerr << "End recording\n";
    m_recording_frame = false;
}

std::unordered_set<unsigned>
FrameTrimmeImpl::getUniqueCallIds()
{
    std::unordered_set<unsigned> retval;
    for(auto&& c: m_required_calls)
        retval.insert(c->callNo());
    return retval;
}

std::vector<unsigned>
FrameTrimmeImpl::getSortedCallIds()
{
    auto make_sure_its_singular = getUniqueCallIds();

    std::vector<unsigned> sorted_calls(
                make_sure_its_singular.begin(),
                make_sure_its_singular.end());
    std::sort(sorted_calls.begin(), sorted_calls.end());
    return sorted_calls;
}

unsigned
FrameTrimmeImpl::equalChars(const char *l, const char *r)
{
    unsigned retval = 0;
    while (*l && *r && *l == *r) {
        ++retval;
        ++l; ++r;
    }
    if (!*l && !*r)
        ++retval;

    return retval;
}

// Map callbacks to call methods of FrameTRimImpl
// Additional data is passed by reference (R) or vy value (V)

#define MAP(name, call) m_call_table.insert(std::make_pair(#name, bind(&FrameTrimmeImpl:: call, this, _1)))

#define MAP_RV(name, call, data, param1) \
    m_call_table.insert(std::make_pair(#name, bind(&FrameTrimmeImpl:: call, this, _1, \
                        std::ref(data), param1)))

#define MAP_RVRV(name, call, data1, param1, data2, param2) \
    m_call_table.insert(std::make_pair(#name, bind(&FrameTrimmeImpl:: call, this, _1, \
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

void FrameTrimmeImpl::registerLegacyCalls()
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

    MAP(glPushClientAttr, todo);
    MAP(glPopClientAttr, todo);


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
    MAP(glDispatchCompute, oglDispatchCompute);
}

void FrameTrimmeImpl::registerFramebufferCalls()
{
    MAP_RV(glBindRenderbuffer, oglBind, m_renderbuffers, 1);
    MAP_OBJ(glDeleteRenderbuffers, m_renderbuffers, DependecyObjectWithSingleBindPointMap::destroy);
    MAP_OBJ(glGenRenderbuffer, m_renderbuffers, DependecyObjectWithSingleBindPointMap::generate);
    MAP_OBJ(glRenderbufferStorage, m_renderbuffers, DependecyObjectWithSingleBindPointMap::callOnBoundObject);

    MAP_OBJ(glGenFramebuffer, m_fbo, FramebufferObjectMap::generate);
    MAP_OBJ(glDeleteFramebuffers, m_fbo, FramebufferObjectMap::destroy);
    MAP_RV(glBindFramebuffer, oglBindFbo, m_fbo, 1);
    MAP_OBJ_V(glViewport, m_fbo, FramebufferObjectMap::callOnObjectBoundTo, GL_DRAW_FRAMEBUFFER);

    MAP_OBJ(glBlitFramebuffer, m_fbo, FramebufferObjectMap::oglBlit);
    MAP_RVRV(glFramebufferTexture, callOnBoundObjWithDep, m_fbo, 2, m_textures, true);
    MAP_RVRV(glFramebufferTexture1D, callOnBoundObjWithDep, m_fbo,
                            3, m_textures, true);
    MAP_RVRV(glFramebufferTexture2D, callOnBoundObjWithDep, m_fbo,
                            3, m_textures, true);

    MAP_OBJ(glReadBuffer, m_fbo, FramebufferObjectMap::oglReadBuffer);
    MAP_OBJ_V(glDrawBuffer, m_fbo, FramebufferObjectMap::callOnObjectBoundTo, GL_DRAW_FRAMEBUFFER);

    MAP_OBJ_V(glClearBuffer, m_fbo, FramebufferObjectMap::callOnObjectBoundTo, GL_DRAW_FRAMEBUFFER);
    MAP_OBJ_V(glClearBufferfi, m_fbo, FramebufferObjectMap::callOnObjectBoundTo, GL_DRAW_FRAMEBUFFER);
    MAP_OBJ_V(glClearBufferfv, m_fbo, FramebufferObjectMap::callOnObjectBoundTo, GL_DRAW_FRAMEBUFFER);
    MAP_OBJ_V(glClearBufferiv, m_fbo, FramebufferObjectMap::callOnObjectBoundTo, GL_DRAW_FRAMEBUFFER);

/*    MAP_GENOBJ_DATAREF(glFramebufferTexture3D, m_fbo,
                         FramebufferStateMap::attach_texture3d, m_textures);
      MAP(glReadBuffer, ReadBuffer); */

    MAP_RVRV(glFramebufferRenderbuffer, callOnBoundObjWithDep,
             m_fbo, 3, m_renderbuffers, true);

    MAP_OBJ_V(glClear, m_fbo, FramebufferObjectMap::callOnObjectBoundTo, GL_DRAW_FRAMEBUFFER);

}

void
FrameTrimmeImpl::registerBufferCalls()
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
    MAP_OBJ(glNamedBufferSubData, m_buffers, BufferObjectMap::callOnNamedObject);
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
    MAP_OBJ(glInvalidateBufferData, m_buffers, BufferObjectMap::callOnNamedObject);
}

void FrameTrimmeImpl::registerDrawCalls()
{
    MAP(glDrawArrays, oglDraw);
    MAP(glDrawElements, oglDraw);
    MAP(glDrawRangeElements, oglDraw);
    MAP(glDrawRangeElementsBaseVertex, oglDraw);

    MAP(glDrawArraysInstanced, oglDraw);

    MAP(glDrawElementsIndirect, oglDraw);
    MAP(glMultiDrawArraysIndirect, oglDraw);
    MAP(glMultiDrawElementsIndirect, oglDraw);
}

void
FrameTrimmeImpl::registerProgramCalls()
{
    MAP_OBJ_RVV(glAttachObject, m_programs,
                   ProgramObjectMap::callOnNamedObjectWithDep, m_shaders, 1, false);
    MAP_OBJ_RVV(glAttachShader, m_programs,
                   ProgramObjectMap::callOnNamedObjectWithDep, m_shaders, 1, false);

    MAP_OBJ(glCompileShader, m_shaders, ShaderStateMap::callOnNamedObject);
    MAP_OBJ(glCreateShader, m_shaders, ShaderStateMap::create);
    MAP_OBJ(glDeleteShader, m_shaders, ShaderStateMap::del);
    MAP_OBJ(glShaderSource, m_shaders, ShaderStateMap::callOnNamedObject);

    MAP_OBJ(glBindAttribLocation, m_programs, ProgramObjectMap::callOnNamedObject);
    MAP_OBJ(glCreateProgram, m_programs, ProgramObjectMap::create);
    MAP_OBJ(glDeleteProgram, m_programs, ProgramObjectMap::del);
    MAP_OBJ(glDetachShader, m_programs, ProgramObjectMap::callOnNamedObject);

    MAP_OBJ(glGetAttribLocation, m_programs, ProgramObjectMap::callOnNamedObject);
    MAP_OBJ(glGetUniformLocation, m_programs, ProgramObjectMap::callOnNamedObject);
    MAP_OBJ(glBindFragDataLocation, m_programs, ProgramObjectMap::callOnNamedObject);
    MAP_OBJ(glLinkProgram, m_programs, ProgramObjectMap::callOnNamedObject);
    MAP_OBJ(glProgramBinary, m_programs, ProgramObjectMap::callOnNamedObject);
    MAP_OBJ(glProgramUniform, m_programs, ProgramObjectMap::callOnNamedObject);

    MAP_OBJ(glUniform, m_programs, ProgramObjectMap::callOnBoundObject);
    MAP_OBJ(glUniformBlockBinding, m_programs, ProgramObjectMap::callOnNamedObject);
    MAP_RV(glUseProgram, oglBind, m_programs, 0);
    MAP_OBJ(glProgramParameter, m_programs, ProgramObjectMap::callOnNamedObject);
    MAP_OBJ(glShaderStorageBlockBinding, m_programs, ProgramObjectMap::callOnNamedObject);

    MAP_OBJ(glGenProgramPipelines, m_program_pipelines, ProgramPipelineObjectMap::generate);
    MAP_OBJ(glDeleteProgramPipelines, m_program_pipelines, ProgramPipelineObjectMap::destroy);
    MAP_RV(glBindProgramPipelines, oglBind, m_program_pipelines, 0);
    MAP_OBJ_RV(glUseProgramStages, m_program_pipelines, ProgramPipelineObjectMap::callOnNamedObjectWithNamedDep,
               m_programs, 2);
    MAP_OBJ_RV(glActiveShaderProgram, m_program_pipelines, ProgramPipelineObjectMap::callOnNamedObjectWithNamedDep,
               m_programs, 1);
}

void FrameTrimmeImpl::registerTextureCalls()
{
    MAP_OBJ(glGenTextures, m_textures, TextureObjectMap::generate);
    MAP_OBJ(glDeleteTextures, m_textures, TextureObjectMap::destroy);

    MAP_OBJ(glActiveTexture, m_textures, TextureObjectMap::oglActiveTexture);
    MAP_OBJ(glClientActiveTexture, m_textures, TextureObjectMap::oglActiveTexture);
    MAP_RV(glBindTexture, oglBind, m_textures, 1);
    MAP(glBindMultiTexture, oglBindMultitex);

    MAP_OBJ(glCompressedTexImage2D, m_textures, TextureObjectMap::callOnBoundObject);
    MAP_OBJ(glGenerateMipmap, m_textures, TextureObjectMap::callOnBoundObject);
    MAP_OBJ_RV(glTexImage1D, m_textures, TextureObjectMap::callOnBoundObjectWithDepBoundTo,
               m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glTexImage2D, m_textures, TextureObjectMap::callOnBoundObjectWithDepBoundTo,
               m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glTexImage3D, m_textures, TextureObjectMap::callOnBoundObjectWithDepBoundTo,
               m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ(glTexStorage1D, m_textures, TextureObjectMap::callOnBoundObject);
    MAP_OBJ(glTexStorage2D, m_textures, TextureObjectMap::callOnBoundObject);
    MAP_OBJ(glTexStorage3D, m_textures, TextureObjectMap::callOnBoundObject);
    MAP_OBJ(glTexImage3D, m_textures, TextureObjectMap::callOnBoundObject);
    MAP_OBJ_RV(glTexSubImage1D, m_textures, TextureObjectMap::callOnBoundObjectWithDepBoundTo,
                    m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glTexSubImage2D, m_textures, TextureObjectMap::callOnBoundObjectWithDepBoundTo,
                    m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glCompressedTexSubImage2D, m_textures, TextureObjectMap::callOnBoundObjectWithDepBoundTo,
                  m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glTexSubImage3D, m_textures, TextureObjectMap::callOnBoundObjectWithDepBoundTo,
                  m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ(glTexParameter, m_textures, TextureObjectMap::callOnBoundObject);
    MAP_OBJ_RVV(glTextureView, m_textures, TextureObjectMap::callOnNamedObjectWithDep,
                   m_textures, 2, true);

    MAP_OBJ_RVV(glTexBuffer, m_textures, TextureObjectMap::callOnBoundObjectWithDep,
                   m_buffers, 2, true);

    /* Should add a dependency on the read fbo */
    MAP_OBJ(glCopyTexSubImage, m_textures, TextureObjectMap::callOnBoundObject);

    MAP_OBJ(glCopyTexImage2D, m_textures, TextureObjectMap::callOnBoundObject);

    MAP_OBJ(glCopyImageSubData, m_textures, TextureObjectMap::copy);
    MAP_OBJ(glBindImageTexture, m_textures, TextureObjectMap::bindToImageUnit);

    /*
    MAP_GENOBJ(glCopyTexSubImage2D, m_textures, TextureStateMap::copy_sub_data);
    */
    MAP_RV(glBindSampler, oglBind, m_samplers, 1);
    MAP_OBJ(glGenSamplers, m_samplers, SamplerObjectMap::generate);
    MAP_OBJ(glDeleteSamplers, m_samplers, SamplerObjectMap::destroy);
    MAP_OBJ(glSamplerParameter, m_samplers, SamplerObjectMap::callOnNamedObject);
    MAP(glBindTextures, oglBindTextures);

}

void FrameTrimmeImpl::registerQueryCalls()
{
    MAP_OBJ(glGenQueries, m_queries, QueryObjectMap::generate);
    MAP_OBJ(glDeleteQueries, m_queries, QueryObjectMap::destroy);
    MAP_RV(glBeginQuery, bindWithCreate, m_queries, 1);
    MAP_OBJ(glEndQuery, m_queries, QueryObjectMap::oglEndQuery);
    MAP_OBJ(glEndQueryIndexed, m_queries, QueryObjectMap::oglEndQueryIndexed);
    MAP_OBJ(glGetQueryObject, m_queries, QueryObjectMap::callOnNamedObject);
    MAP_OBJ(glGetQueryiv, m_queries, QueryObjectMap::callOnBoundObject);
}

void
FrameTrimmeImpl::registerStateCalls()
{
    /* These Functions change the state and we only need to remember the last
     * call before the target frame or an fbo creating a dependency is draw. */
    const std::vector<const char *> state_calls  = {
        "glAlphaFunc",
        "glBlendColor",
        "glBlendEquation",
        "glBlendFunc",
        "glClearColor",
        "glClearDepth",
        "glClearStencil",
        "glClipControl",
        "glColorMask",
        "glColorPointer",
        "glCullFace",
        "glDepthFunc",
        "glDepthMask",
        "glDepthRange",
        "glDepthBounds",
        "glFlush",
        "glFrontFace",
        "glFrustum",
        "glLineStipple",
        "glLineWidth",
        "glPatchParameteri",
        "glPixelZoom",
        "glPointSize",
        "glPolygonMode",
        "glPolygonOffset",
        "glPolygonStipple",
        "glProvokingVertex",
        "glPrimitiveBoundingBox",
        "glSampleCoverage",
        "glShadeModel",
        "glScissor",
        "glStencilMask",
        "glFinish",
    };

    auto state_call_func = bind(&FrameTrimmeImpl::recordStateCall, this, _1, 0);
    auto state_call_1_func = bind(&FrameTrimmeImpl::recordStateCall, this, _1, 1);
    auto keep_state_calls_func = bind(&FrameTrimmeImpl::recordRequiredCall, this, _1);

    /* These are state functions with an extra parameter */

    const std::vector<const char *> state_calls_1  = {
        "glClipPlane",
        "glColorMaskIndexedEXT",
        "glColorMaterial",
        "glFog",
        "glHint",
        "glLight",
        "glPixelTransfer",
        "glStencilOpSeparate",
        "glStencilFuncSeparate",
        "glVertexAttribDivisor",
    };

    /* These are state functions with an extra parameter */
    auto state_call_2_func = bind(&FrameTrimmeImpl::recordStateCall, this, _1, 2);
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
    MAP(glEnable, recordRequiredCall);
    MAP(glEnablei, recordRequiredCall);

    MAP_OBJ(glFenceSync, m_sync_objects, SyncObjectMap::create);
    MAP(glWaitSync, oglWaitSync);
    MAP(glClientWaitSync, oglWaitSync);
    MAP_OBJ(glDeleteSync, m_sync_objects, SyncObjectMap::callOnNamedObject);
}

void FrameTrimmeImpl::registerRequiredCalls()
{
    /* These function set up the context and are, therefore, required
     * TODO: figure out what is really required, and whether the can be
     * tracked like state variables. */
    auto required_func = bind(&FrameTrimmeImpl::recordRequiredCall, this, _1);
    const std::vector<const char *> required_calls = {
        "glXChooseVisual",
        "glXCreateContext",
        "glXDestroyContext",
        "glXMakeCurrent",
        "glXMakeContextCurrent",
        "glXChooseFBConfig",
        "glXQueryExtensionsString",
        "glXSwapIntervalMESA",

        "eglInitialize",
        "eglCreatePlatformWindowSurface",
        "eglBindAPI",
        "eglCreateContext",
        "eglMakeCurrent",

        "glPixelStorei", /* Being lazy here, we could track the dependency
                            in the relevant calls */
    };
    updateCallTable(required_calls, required_func);
}

void FrameTrimmeImpl::registerIgnoreHistoryCalls()
{
    /* These functions are ignored outside required recordings
     * TODO: ignore them also in the copied frames */
    const std::vector<const char *> ignore_history_calls = {
        "glCheckFramebufferStatus",
        "glGetActiveUniform",
        "glGetBoolean",
        "glGetError",
        "glGetFloat",
        "glGetFramebufferAttachmentParameter",
        "glGetInfoLog",
        "glGetInteger",
        "glGetObjectLabelEXT",
        "glGetObjectParameter",
        "glGetProgram",
        "glGetShader",
        "glGetString",
        "glGetTexLevelParameter",
        "glGetTexParameter",
        "glGetTexImage",
        "glGetUniform",
        "glLabelObjectEXT",
        "glIsEnabled",
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
        "glXQueryVersion",
        "eglGetProcAddress",
        "eglQueryString",
        "eglGetError",
        "eglGetPlatformDisplay",
        "eglGetConfigs",
        "eglGetConfigAttrib",
        "eglQuerySurface",
     };
    auto ignore_history_func = bind(&FrameTrimmeImpl::ignoreHistory, this, _1);
    updateCallTable(ignore_history_calls, ignore_history_func);
}


void
FrameTrimmeImpl::registerVaCalls()
{
    MAP_OBJ(glGenVertexArrays, m_vertex_arrays, VertexArrayMap::generate);
    MAP_OBJ(glDeleteVertexArrays, m_vertex_arrays, VertexArrayMap::destroy);
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

    MAP(glVertexPointer, recordRequiredCall);
    MAP(glTexCoordPointer, recordRequiredCall);

    MAP(glVertexAttribBinding, recordRequiredCall);
    MAP(glVertexAttribFormat, recordRequiredCall);
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
FrameTrimmeImpl::updateCallTable(const std::vector<const char*>& names,
                                        ft_callback cb)
{
    for (auto& i : names)
        m_call_table.insert(std::make_pair(i, cb));
}

PTraceCall
FrameTrimmeImpl::recordStateCall(const trace::Call& call,
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
FrameTrimmeImpl::oglBegin(const trace::Call& call)
{
    if (m_active_display_list)
        m_active_display_list->addCall(trace2call(call));
}

void
FrameTrimmeImpl::oglEnd(const trace::Call& call)
{
    if (m_active_display_list)
        m_active_display_list->addCall(trace2call(call));
}

void
FrameTrimmeImpl::oglVertex(const trace::Call& call)
{
    if (m_active_display_list)
        m_active_display_list->addCall(trace2call(call));
}

void
FrameTrimmeImpl::oglEndList(const trace::Call& call)
{
    if (!m_recording_frame)
        m_active_display_list->addCall(trace2call(call));

    m_active_display_list = nullptr;
}

void
FrameTrimmeImpl::oglGenLists(const trace::Call& call)
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
FrameTrimmeImpl::oglNewList(const trace::Call& call)
{
    assert(!m_active_display_list);
    auto list  = m_display_lists.find(call.arg(0).toUInt());
    assert(list != m_display_lists.end());
    m_active_display_list = list->second;

    if (!m_recording_frame)
        m_active_display_list->addCall(trace2call(call));
}

void
FrameTrimmeImpl::oglCallList(const trace::Call& call)
{
    auto list  = m_display_lists.find(call.arg(0).toUInt());
    assert(list != m_display_lists.end());

    if (m_recording_frame)
        list->second->emitCallsTo(m_required_calls);
}

void
FrameTrimmeImpl::oglDeleteLists(const trace::Call& call)
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
FrameTrimmeImpl::oglBind(const trace::Call& call, DependecyObjectMap& map,
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
FrameTrimmeImpl::oglBindBuffer(const trace::Call& call)
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
FrameTrimmeImpl::oglBindTextures(const trace::Call& call)
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

void FrameTrimmeImpl::oglDraw(const trace::Call& call)
{
    auto fb = m_fbo.boundTo(GL_DRAW_FRAMEBUFFER);
    auto cur_prog = m_programs.boundTo(0, 0);
    if (cur_prog) {
        for(auto&& [key, buf]: m_buffers) {
            if (buf && ((key % BufferObjectMap::bt_last) == BufferObjectMap::bt_uniform))
                cur_prog->addDependency(buf);
        }
        m_buffers.addSSBODependencies(cur_prog);
        m_textures.addImageDependencies(cur_prog);

        if (fb->id())
            fb->addDependency(cur_prog);

        if (m_recording_frame)
            cur_prog->emitCallsTo(m_required_calls);
    }

    auto cur_pipeline = m_program_pipelines.boundTo(0, 0);
    if (cur_pipeline) {
        for(auto&& [key, buf]: m_buffers) {
            if (buf && ((key % BufferObjectMap::bt_last) == BufferObjectMap::bt_uniform))
                cur_pipeline->addDependency(buf);
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
        m_vertex_arrays.addBoundAsDependencyTo(*fb);
        m_textures.addBoundAsDependencyTo(*fb);
        m_programs.addBoundAsDependencyTo(*fb);
        m_legacy_programs.addBoundAsDependencyTo(*fb);
        m_program_pipelines.addBoundAsDependencyTo(*fb);
        m_samplers.addBoundAsDependencyTo(*fb);

        for(auto&& [key, vbo]: m_vertex_attrib_pointers) {
            if (vbo)
                fb->addDependency(vbo);

        }
        for(auto&& [key, vbo]: m_vertex_buffer_pointers) {
            if (vbo)
                fb->addDependency(vbo);
        }
    }

    if (m_recording_frame) {

        for(auto&& [key, vao]: m_vertex_arrays) {
            if (vao)
                vao->emitCallsTo(m_required_calls);
        }

        for(auto&& [key, vbo]: m_vertex_attrib_pointers) {
            if (vbo)
                vbo->emitCallsTo(m_required_calls);

        }
        for(auto&& [key, vbo]: m_vertex_buffer_pointers) {
            if (vbo)
                vbo->emitCallsTo(m_required_calls);
        }
        m_buffers.emitBoundObjects(m_required_calls);
    }
}

void
FrameTrimmeImpl::bindWithCreate(const trace::Call& call, DependecyObjectMap& map,
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

void FrameTrimmeImpl::oglBindMultitex(const trace::Call& call)
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
FrameTrimmeImpl::oglWaitSync(const trace::Call& call)
{
    auto obj = m_sync_objects.getById(call.arg(0).toUInt());
    assert(obj);
    obj->addCall(trace2call(call));
    if (m_recording_frame)
        obj->emitCallsTo(m_required_calls);
}

void
FrameTrimmeImpl::oglBindFbo(const trace::Call& call, DependecyObjectMap& map,
                         unsigned bind_param)
{
    auto fb = map.bind(call, bind_param);
    fb->addCall(trace2call(call));
    if (m_recording_frame && fb->id())
        fb->emitCallsTo(m_required_calls);
}

void
FrameTrimmeImpl::callOnBoundObjWithDep(const trace::Call& call, DependecyObjectMap& map,
                                        unsigned obj_id_param,
                                        DependecyObjectMap& dep_map, bool reverse_dep_too)
{
    auto dep_obj = map.callOnBoundObjectWithDep(call, dep_map, obj_id_param,
                                                reverse_dep_too);
    if (m_recording_frame && dep_obj)
        dep_obj->emitCallsTo(m_required_calls);
}

void FrameTrimmeImpl::oglDispatchCompute(const trace::Call& call)
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

    if (m_recording_frame)
        cur_prog->emitCallsTo(m_required_calls);
}

void
FrameTrimmeImpl::todo(const trace::Call& call)
{
    std::cerr << "TODO: " << call.name() << "\n";
}

void
FrameTrimmeImpl::ignoreHistory(const trace::Call& call)
{
    (void)call;
}

void
FrameTrimmeImpl::recordRequiredCall(const trace::Call& call)
{
    auto c = trace2call(call);
    m_required_calls.insert(c);
}

void
FrameTrimmeImpl::oglBindVertexArray(const trace::Call& call)
{
    auto vao = m_vertex_arrays.bind(call, 0);
    global_state.current_vao = vao;
    if (vao)
        vao->addCall(trace2call(call));
    else
        m_vertex_arrays.addCall(trace2call(call));

    if (global_state.emit_dependencies)
        vao->emitCallsTo(*global_state.out_list);
    else {
        auto fb = m_fbo.boundTo(GL_DRAW_FRAMEBUFFER);
        if (fb->id())
            fb->addDependency(vao);
    }
}

}
