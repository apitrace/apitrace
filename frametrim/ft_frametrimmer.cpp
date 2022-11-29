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

using DisplayListMap = std::unordered_map<unsigned, UsedObject::Pointer>;
using LegacyProgrambjectMap = DependecyObjectWithDefaultBindPointMap;
using ProgramObjectMap = DependecyObjectWithSingleBindPointMap;
using ProgramPipelineObjectMap = DependecyObjectWithSingleBindPointMap;
using SamplerObjectMap = DependecyObjectWithDefaultBindPointMap;
using SyncObjectMap = DependecyObjectWithSingleBindPointMap;
using ShaderStateMap = DependecyObjectWithDefaultBindPointMap;
using RenderObjectMap = DependecyObjectWithSingleBindPointMap;
using VertexArrayMap = DependecyObjectWithDefaultBindPointMap;

enum ePerContext {
    pc_vertex_array,
    pc_program_pipelines,
    pc_fbo,
};

struct PerContextObjects {
    VertexArrayMap m_vertex_arrays;
    ProgramPipelineObjectMap m_program_pipelines;
    FramebufferObjectMap m_fbo;
};

struct FrameTrimmeImpl {

    using ObjectMap = std::unordered_map<unsigned, UsedObject::Pointer>;

    FrameTrimmeImpl(bool keep_all_states);
    void call(const trace::Call& call, Frametype frametype);
    void startTargetFrame();
    void endTargetFrame();

    std::vector<unsigned> getSortedCallIds();
    std::unordered_set<unsigned> getUniqueCallIds();

    static unsigned equalChars(const char *l, const char *callname);

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
    void oglBindFbo(const trace::Call& call, unsigned bind_param);
    void oglBindTextures(const trace::Call& call);
    void oglBindSamplers(const trace::Call& call);
    void bindWithCreate(const trace::Call& call, DependecyObjectMap& map,
                        unsigned bind_param);
    void oglWaitSync(const trace::Call& call);
    void callOnBoundObjWithDep(const trace::Call& call, DependecyObjectMap& map,
                               unsigned obj_id_param, DependecyObjectMap &dep_map, bool reverse_dep_too);

    void callOnNamedObjectWithNamedDep(const trace::Call& call, ePerContext object_type, DependecyObjectMap &dep_map,
                                       unsigned dep_id_param);

    void oglBindMultitex(const trace::Call& call);
    void oglDispatchCompute(const trace::Call& call);
    void oglBindVertexArray(const trace::Call& call);
    void oglBindBuffer(const trace::Call& call);

    void todo(const trace::Call& call);
    void ignoreHistory(const trace::Call& call);

    bool skipDeleteObj(const trace::Call& call);
    bool skipDeleteImpl(unsigned obj_id, DependecyObjectMap& map);
    std::unordered_set<unsigned> finalize(int last_frame_start);

    bool checkCommonSuffixes(const char *suffix) const;

    void generate(const trace::Call& call, ePerContext map_type);
    void destroy(const trace::Call& call, ePerContext map_type);
    void callOnBoundObject(const trace::Call& call, ePerContext map_type); 
    void callOnNamedObject(const trace::Call& call, ePerContext map_type); 
    void callOnObjectBoundTo(const trace::Call& call, ePerContext map_type, unsigned bindpoint);
    void fboBindAttachment(const trace::Call& call, DependecyObjectMap& dep_map, unsigned tex_id_param);
    void fboNamedBindAttachment(const trace::Call& call, DependecyObjectMap& dep_map, int obj_param_pos); 
    void fboBlit(const trace::Call& call);
    void fboBlitNamed(const trace::Call& call); 
    void fboReadBuffer(const trace::Call& call);
    void bindPerContext(const trace::Call& call, ePerContext map_type, unsigned bind_param);


    DependecyObjectMap& getCurrentMapOfType(ePerContext map_type);

    void createContext(const trace::Call& call, int shared_param);
    void makeCurrent(const trace::Call& call, unsigned param);
    using CallTable = std::multimap<const char *, ft_callback, string_part_less>;
    CallTable m_call_table;

    using CallTableCache =  std::map<const char *, ft_callback>;
    CallTableCache m_call_table_cache;

    CallSet m_required_calls;
    std::set<std::string> m_unhandled_calls;
    DisplayListMap m_display_lists;
    UsedObject::Pointer m_active_display_list;

    AllMatrisStates m_matrix_states;

    LegacyProgrambjectMap m_legacy_programs;
    ProgramObjectMap m_programs;
    TextureObjectMap m_textures;
    BufferObjectMap m_buffers;
    ShaderStateMap m_shaders;
    RenderObjectMap m_renderbuffers;
    SamplerObjectMap m_samplers;
    SyncObjectMap m_sync_objects;
    VertexAttribObjectMap m_vertex_attrib_pointers;
    VertexAttribObjectMap m_vertex_buffer_pointers;

    std::map<void *, std::shared_ptr<PerContextObjects>> m_contexts;
    std::shared_ptr<PerContextObjects> m_current_context;
    QueryObjectMap m_queries;

    std::map<std::string, PTraceCall> m_state_calls;
    std::map<unsigned, PTraceCall> m_enables;

    bool m_recording_frame;

    PTraceCall m_last_swap;
    bool m_keep_all_state_calls;
    unsigned m_last_frame_start;
};

FrameTrimmer::FrameTrimmer(bool keep_all_states)
{
    impl = new FrameTrimmeImpl(keep_all_states);
}

FrameTrimmer::~FrameTrimmer()
{
    delete impl;
}

void FrameTrimmer::start_last_frame(uint32_t callno)
{
    std::cerr << "\n---> Start last frame at call no " << callno << "\n";
    impl->m_last_frame_start = callno;
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

std::unordered_set<unsigned> FrameTrimmer::finalize(int last_frame_start)
{
    return impl->finalize(last_frame_start);
}

FrameTrimmeImpl::FrameTrimmeImpl(bool keep_all_states):
    m_recording_frame(false),
    m_keep_all_state_calls(keep_all_states),
    m_last_frame_start(0)
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

    /* Skip delete calls for objects that have never been emitted, or
     * if we are in the last frame and the object was created in an earlier frame.
     * By not deleting such objects looping the last frame will work in more cases */
    if (skipDeleteObj(call)) {
        return;
    }

    auto icb = m_call_table_cache.find(call.name());
    if (icb != m_call_table_cache.end())
        icb->second(call);
    else {
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

            if (max_equal) {
                cb->second(call);
                m_call_table_cache[call.name()] = cb->second;
                if (strcmp(call.name(), cb->first)) {
                    if (!checkCommonSuffixes(call.name() + strlen(cb->first)))
                        std::cerr << "Handle " << call.name() << " as " << cb->first << "\n";
                }
            } else {
                if (m_unhandled_calls.find(call_name) == m_unhandled_calls.end()) {
                    std::cerr << "Call " << call.no
                              << " " << call_name << " not handled\n";
                    m_unhandled_calls.insert(call_name);
                }
            }
        } else  if (!(call.flags & trace::CALL_FLAG_END_FRAME)) {
            /* This should be some debug output only, because we might
             * not handle some calls deliberately */
            if (m_unhandled_calls.find(call_name) == m_unhandled_calls.end()) {
                std::cerr << "Call " << call.no
                          << " " << call_name << " not handled\n";
                m_unhandled_calls.insert(call_name);
            }
        }
    }

    auto c = trace2call(call);

    if (frametype == ft_none) {
        if (call.flags & trace::CALL_FLAG_END_FRAME)
            m_last_swap = c;
    } else {
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

bool
FrameTrimmeImpl::checkCommonSuffixes(const char *suffix) const
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
}

bool
FrameTrimmeImpl::skipDeleteObj(const trace::Call& call)
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
FrameTrimmeImpl::skipDeleteImpl(unsigned obj_id, DependecyObjectMap& map)
{
    auto obj = map.getById(obj_id);
    return !obj || (m_recording_frame && !obj->emitted()) || obj->createdBefore(m_last_frame_start);
}

std::unordered_set<unsigned>
FrameTrimmeImpl::finalize(int last_frame_start)
{
    std::unordered_set<unsigned> result;

    m_programs.unbalancedCreateCallsInLastFrame(last_frame_start, result);
    m_textures.unbalancedCreateCallsInLastFrame(last_frame_start, result);
    m_buffers.unbalancedCreateCallsInLastFrame(last_frame_start, result);
    m_shaders.unbalancedCreateCallsInLastFrame(last_frame_start, result);
    m_renderbuffers.unbalancedCreateCallsInLastFrame(last_frame_start, result);
    m_samplers.unbalancedCreateCallsInLastFrame(last_frame_start, result);
    m_sync_objects.unbalancedCreateCallsInLastFrame(last_frame_start, result);

    for (auto& [dummy, context] : m_contexts) {
        context->m_vertex_arrays.unbalancedCreateCallsInLastFrame(last_frame_start, result);
        context->m_program_pipelines.unbalancedCreateCallsInLastFrame(last_frame_start, result);
        context->m_fbo.unbalancedCreateCallsInLastFrame(last_frame_start, result);
    }

    m_vertex_attrib_pointers.unbalancedCreateCallsInLastFrame(last_frame_start, result);
    m_vertex_buffer_pointers.unbalancedCreateCallsInLastFrame(last_frame_start, result);
    m_legacy_programs.unbalancedCreateCallsInLastFrame(last_frame_start, result);
    m_queries.unbalancedCreateCallsInLastFrame(last_frame_start, result);

    if (m_last_swap)
        m_required_calls.insert(m_last_swap);

    return result;
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
FrameTrimmeImpl::equalChars(const char *prefix, const char *callname)
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

// Map callbacks to call methods of FrameTRimImpl
// Additional data is passed by reference (R) or vy value (V)

#define MAP(name, call) m_call_table.insert(std::make_pair(#name, bind(&FrameTrimmeImpl:: call, this, _1)))

#define MAP_V(name, call, param1) \
    m_call_table.insert(std::make_pair(#name, bind(&FrameTrimmeImpl:: call, this, _1, \
                        param1)))

#define MAP_VV(name, call, param1, param2) \
    m_call_table.insert(std::make_pair(#name, bind(&FrameTrimmeImpl:: call, this, _1, \
                        param1, param2)))

#define MAP_VRV(name, call, param1, data, param2) \
    m_call_table.insert(std::make_pair(#name, bind(&FrameTrimmeImpl:: call, this, _1, \
                        param1, std::ref(data), param2)))

#define MAP_R(name, call, data) \
    m_call_table.insert(std::make_pair(#name, bind(&FrameTrimmeImpl:: call, this, _1, \
                        std::ref(data))))

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
    MAP(glDispatchCompute, oglDispatchCompute);
}

void FrameTrimmeImpl::registerFramebufferCalls()
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

void FrameTrimmeImpl::registerDrawCalls()
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
FrameTrimmeImpl::registerProgramCalls()
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

void FrameTrimmeImpl::registerTextureCalls()
{
    MAP_OBJ(glGenTextures, m_textures, TextureObjectMap::generate);
    MAP_OBJ(glCreateTextures, m_textures, TextureObjectMap::generateWithTarget);
    MAP_OBJ(glDeleteTextures, m_textures, TextureObjectMap::destroy);

    MAP_OBJ(glActiveTexture, m_textures, TextureObjectMap::oglActiveTexture);
    MAP_OBJ(glClientActiveTexture, m_textures, TextureObjectMap::oglActiveTexture);
    MAP_RV(glBindTexture, oglBind, m_textures, 1);
    MAP(glBindMultiTexture, oglBindMultitex);

    MAP_OBJ(glCompressedTexImage2D, m_textures, TextureObjectMap::callOnBoundObject);
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
    MAP_OBJ(glTexImage3DMultisample, m_textures, TextureObjectMap::callOnBoundObject);
    MAP_OBJ_RV(glTexSubImage1D, m_textures, TextureObjectMap::callOnBoundObjectWithDepBoundTo,
                    m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glTexSubImage2D, m_textures, TextureObjectMap::callOnBoundObjectWithDepBoundTo,
                    m_buffers, GL_PIXEL_UNPACK_BUFFER);
    MAP_OBJ_RV(glCompressedTexSubImage2D, m_textures, TextureObjectMap::callOnBoundObjectWithDepBoundTo,
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

void FrameTrimmeImpl::registerQueryCalls()
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
FrameTrimmeImpl::registerStateCalls()
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

    auto state_call_func = bind(&FrameTrimmeImpl::recordStateCall, this, _1, 0);
    auto state_call_1_func = bind(&FrameTrimmeImpl::recordStateCall, this, _1, 1);
    auto keep_state_calls_func = bind(&FrameTrimmeImpl::recordRequiredCall, this, _1);

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
        "glStencilOp",
        "glStencilOpSeparate",
        "glStencilFunc",
        "glStencilFuncSeparate",
        "glVertexAttribDivisor",
        "glMultiTexCoord",
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
        "glXCreatePbuffer",
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

void FrameTrimmeImpl::registerIgnoreHistoryCalls()
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
        "glGetNamedFramebufferAttachmentParameteriv", 
        "glGetNamedFramebufferParameteriv",
        "glGetObjectLabelEXT",
        "glGetObjectParameter",
        "glGetProgram",
        "glGetProgramInfoLog",
        "glGetShader",
        "glGetString",
        "glGetTexLevelParameter",
        "glGetTexParameter",
        "glGetTexImage",
        "glGetUniform",
        "glLabelObjectEXT",
        "glIsEnabled",
        "glIsFramebuffer",
        "glIsRenderbuffer", 
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
    auto ignore_history_func = bind(&FrameTrimmeImpl::ignoreHistory, this, _1);
    updateCallTable(ignore_history_calls, ignore_history_func);
}


void
FrameTrimmeImpl::registerVaCalls()
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

void
FrameTrimmeImpl::oglBindSamplers(const trace::Call& call)
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


void FrameTrimmeImpl::oglDraw(const trace::Call& call)
{
    auto fb = m_current_context->m_fbo.boundTo(GL_DRAW_FRAMEBUFFER);
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

    auto cur_pipeline = m_current_context->m_program_pipelines.boundTo(0, 0);
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
        m_current_context->m_vertex_arrays.addBoundAsDependencyTo(*fb);
        m_textures.addBoundAsDependencyTo(*fb);
        m_programs.addBoundAsDependencyTo(*fb);
        m_legacy_programs.addBoundAsDependencyTo(*fb);
        m_current_context->m_program_pipelines.addBoundAsDependencyTo(*fb);
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

        for(auto&& [key, vao]: m_current_context->m_vertex_arrays) {
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
FrameTrimmeImpl::oglBindFbo(const trace::Call& call, unsigned bind_param)
{
    auto fb = m_current_context->m_fbo.bind(call, bind_param);
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


void FrameTrimmeImpl::callOnNamedObjectWithNamedDep(const trace::Call& call, ePerContext object_type,
                                                    DependecyObjectMap &dep_map, unsigned dep_id_param)
{
    auto& map = getCurrentMapOfType(object_type);
    map.callOnNamedObjectWithNamedDep(call, dep_map, dep_id_param);
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

void FrameTrimmeImpl::generate(const trace::Call& call, ePerContext map_type)
{
    getCurrentMapOfType(map_type).generate(call);
}

void FrameTrimmeImpl::destroy(const trace::Call& call, ePerContext map_type)
{
    getCurrentMapOfType(map_type).destroy(call);
}

void FrameTrimmeImpl::bindPerContext(const trace::Call& call, ePerContext map_type, unsigned bind_param)
{
    oglBind(call, getCurrentMapOfType(map_type), bind_param);
}

void FrameTrimmeImpl::callOnObjectBoundTo(const trace::Call& call, ePerContext map_type, unsigned bindpoint)
{
    getCurrentMapOfType(map_type).callOnObjectBoundTo(call, bindpoint);
}

void FrameTrimmeImpl::callOnBoundObject(const trace::Call& call, ePerContext map_type)
{
    getCurrentMapOfType(map_type).callOnBoundObject(call);
}

void FrameTrimmeImpl::callOnNamedObject(const trace::Call& call, ePerContext map_type)
{
    getCurrentMapOfType(map_type).callOnNamedObject(call);
}

void FrameTrimmeImpl::fboBindAttachment(const trace::Call& call, DependecyObjectMap& dep_map, unsigned tex_id_param)
{
    m_current_context->m_fbo.callOnBoundObjectWithDep(call, dep_map, tex_id_param, true);
}

void FrameTrimmeImpl::fboNamedBindAttachment(const trace::Call& call, DependecyObjectMap& dep_map, int obj_param_pos)
{
    m_current_context->m_fbo.callOnNamedObjectWithDep(call, dep_map, obj_param_pos, true); 
}

void FrameTrimmeImpl::fboBlit(const trace::Call& call)
{
    m_current_context->m_fbo.oglBlit(call);
}

void FrameTrimmeImpl::fboBlitNamed(const trace::Call& call)
{
    m_current_context->m_fbo.oglBlitNamed(call);
}

void FrameTrimmeImpl::fboReadBuffer(const trace::Call& call)
{
    m_current_context->m_fbo.oglReadBuffer(call);
}

void FrameTrimmeImpl::createContext(const trace::Call& call, int shared_param)
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
    m_current_context = make_shared<PerContextObjects>();
    m_contexts[call.ret->toPointer()] = m_current_context;
    m_required_calls.insert(trace2call(call));
}

void FrameTrimmeImpl::makeCurrent(const trace::Call& call, unsigned param)
{
    void *ctx_id = call.arg(param).toPointer();
    auto ictx = m_contexts.find(ctx_id);

    if (ictx != m_contexts.end())
        m_current_context = ictx->second;
    else
        m_current_context = nullptr;

    assert(!ctx_id || m_current_context);
    m_required_calls.insert(trace2call(call));
}

DependecyObjectMap& FrameTrimmeImpl::getCurrentMapOfType(ePerContext map_type)
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
