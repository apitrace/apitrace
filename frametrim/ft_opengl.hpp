/*********************************************************************
 *
 * Copyright 2023 Collabora Ltd
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

#pragma once

#include "ft_frametrimmer.hpp"
#include "ft_dependecyobject.hpp"
#include "ft_matrixstate.hpp"

namespace frametrim {

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
    PerContextObjects();

    uint32_t m_id;
    VertexArrayMap m_vertex_arrays;
    ProgramPipelineObjectMap m_program_pipelines;
    FramebufferObjectMap m_fbo;

    static uint32_t m_next_id;
};

class OpenGLImpl : public FrameTrimmer {
public:
    using ObjectMap = std::unordered_map<unsigned, UsedObject::Pointer>;

    OpenGLImpl(bool keep_all_states, bool swaps_to_finish);

    void switch_thread(int new_thread) override;

protected:
    void emitState() override;
    void finalize() override;
    ft_callback findCallback(const char *name) override;
    bool skipDeleteObj(const trace::Call& call) override;

private:
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

    void update_context_id(uint32_t context_id);
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
    void oglDispatchCompute(const trace::Call& call, bool indirect);
    void oglBindVertexArray(const trace::Call& call);
    void oglBindBuffer(const trace::Call& call);

    void todo(const trace::Call& call);
    void ignoreHistory(const trace::Call& call);

    bool skipDeleteImpl(unsigned obj_id, DependecyObjectMap& map);
    std::unordered_set<unsigned> finalize(int last_frame_start);

    static unsigned equalChars(const char *l, const char *callname);
    bool checkCommonSuffixes(const char *suffix) const;

    void generate(const trace::Call& call, ePerContext map_type);
    void destroy(const trace::Call& call, ePerContext map_type);
    void callOnBoundObject(const trace::Call& call, ePerContext map_type); 
    void callOnNamedObject(const trace::Call& call, ePerContext map_type); 
    void callOnObjectBoundTo(const trace::Call& call, ePerContext map_type, unsigned bindpoint);
    void fboBindAttachment(const trace::Call& call, DependecyObjectMap& dep_map, unsigned tex_id_param);
    void fboBindAttachmentEXT(const trace::Call& call, DependecyObjectMap& dep_map, unsigned tex_id_param);
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
    FramebufferObjectMap m_fbo_ext;

    std::map<void *, std::shared_ptr<PerContextObjects>> m_contexts;
    std::shared_ptr<PerContextObjects> m_current_context;
    QueryObjectMap m_queries;

    std::map<std::string, PTraceCall> m_state_calls;
    std::map<unsigned, PTraceCall> m_enables;

    std::unordered_map<unsigned, std::shared_ptr<PerContextObjects>> m_thread_active_context;
};

}
