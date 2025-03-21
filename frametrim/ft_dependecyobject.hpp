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

#pragma once

#include "ft_tracecall.hpp"
#include <stdint.h>

namespace frametrim {

class UsedObject {
public:
    using Pointer = std::shared_ptr<UsedObject>;

    UsedObject(unsigned id);

    unsigned id() const;

    void addCall(PTraceCall call);
    void setCall(PTraceCall call);

    void addDependency(Pointer dep);
    void setDependency(Pointer dep);

    void emitCallsTo(CallSet& out_list);
    bool emitted() const;

    unsigned extraInfo(const std::string& key) const;
    void setExtraInfo(const std::string& key, unsigned value);
    void eraseExtraInfo(const std::string& key);

    bool createdBefore(unsigned callno) const;

    auto calls() { return m_calls; }
private:

    std::vector<PTraceCall> m_calls;
    std::vector<Pointer> m_dependencies;
    unsigned m_id;
    bool m_emitted;
    std::unordered_map<std::string, unsigned> m_extra_info;
};

class DependecyObjectMap {
public:
    using ObjectMap=std::unordered_map<unsigned, UsedObject::Pointer>;

    void generate(const trace::Call& call);
    void destroy(const trace::Call& call);

    void create(const trace::Call& call);
    void del(const trace::Call& call);

    UsedObject::Pointer bind(const trace::Call& call, unsigned obj_id_param);
    void callOnBoundObject(const trace::Call& call);
    UsedObject::Pointer bindWithCreate(const trace::Call& call, unsigned obj_id_param);
    void callOnObjectBoundTo(const trace::Call& call, unsigned bindpoint);
    void callOnNamedObject(const trace::Call& call);
    UsedObject::Pointer
    callOnBoundObjectWithDep(const trace::Call& call,
                             DependecyObjectMap& other_objects, int dep_obj_param, bool reverse_dep_too);

    void
    callOnBoundObjectWithDepBoundTo(const trace::Call& call, DependecyObjectMap& other_objects,
                                    int bindingpoint);

    void callOnNamedObjectWithDep(const trace::Call& call,
                                  DependecyObjectMap& other_objects, int dep_obj_param, bool reverse_dep_too);
    void callOnNamedObjectWithDepBoundTo(const trace::Call& call,
                                         DependecyObjectMap& other_objects,
                                         int dep_call_param);
    void callOnNamedObjectWithNamedDep(const trace::Call& call, DependecyObjectMap& other_objects,
                                       int dep_call_param);

    UsedObject::Pointer getById(unsigned id) const;
    void addCall(PTraceCall call);

    void emitBoundObjects(CallSet& out_calls);
    UsedObject::Pointer boundTo(unsigned target, unsigned index = 0);

    auto begin() {
        assert(m_current_context_id != 0xffffffff);
        return m_bound_object.begin();
    }

    auto end()
    {
        assert(m_current_context_id != 0xffffffff);
        return m_bound_object.end();
    }

    void addBoundAsDependencyTo(UsedObject& obj);

    UsedObject::Pointer bind(unsigned bindpoint, unsigned id);

    void unbalancedCreateCallsInLastFrame(uint32_t last_frame_start,
                                          std::unordered_set<unsigned>& outSet);

    void set_current_context_id(uint32_t id) {m_current_context_id = id;}
    uint32_t context_id() const { return m_current_context_id;}

    ObjectMap& objects_bound_in_context() {
        return m_bound_object[m_current_context_id];
    }

protected:
    void addObject(unsigned id, UsedObject::Pointer obj);
    UsedObject::Pointer boundAtBinding(unsigned index);
    void generate_internal(const trace::Call& call, int array_id);
private:

    virtual void emitBoundObjectsExt(CallSet& out_calls);
    virtual UsedObject::Pointer bindTarget(unsigned id, unsigned bindpoint);
    virtual unsigned getBindpointFromCall(const trace::Call& call) const = 0;
    virtual unsigned getBindpoint(unsigned target, unsigned index) const;
    virtual bool setTargetType(unsigned id, unsigned target);

    ObjectMap m_objects;
    std::unordered_map<uint32_t, ObjectMap> m_bound_object;

    std::vector<PTraceCall> m_calls;

    uint32_t m_current_context_id {0xffffffff};
};

class DependecyObjectWithSingleBindPointMap: public DependecyObjectMap {
private:
    unsigned getBindpointFromCall(const trace::Call& call) const override;
};

class DependecyObjectWithDefaultBindPointMap: public DependecyObjectMap {
private:
    unsigned getBindpointFromCall(const trace::Call& call) const override;
};

class BufferObjectMap: public DependecyObjectMap {
public:

    enum BufTypes {
        bt_array = 1,
        bt_atomic_counter,
        bt_copy_read,
        bt_copy_write,
        bt_dispatch_indirect,
        bt_draw_indirect,
        bt_element_array,
        bt_pixel_pack,
        bt_pixel_unpack,
        bt_query,
        bt_ssbo,
        bt_texture,
        bt_tf,
        bt_uniform,
        bt_parameter,
        bt_names_access,
        bt_last,
    };

    void bindBuffer(const trace::Call& call);
    void data(const trace::Call& call);
    void map(const trace::Call& call);
    void mapRange(const trace::Call& call);
    void unmap(const trace::Call& call);
    void memcopy(const trace::Call& call, CallSet& out_set, bool recording);
    void namedMap(const trace::Call& call);
    void namedMapRange(const trace::Call& call);
    void namedUnmap(const trace::Call& call);

    void namedData(const trace::Call& call);

    UsedObject::Pointer boundToTarget(unsigned target);

    void addSSBODependencies(UsedObject::Pointer dep);

    void copyBufferSubData(const trace::Call& call);
    void copyNamedBufferSubData(const trace::Call& call);

private:
    unsigned getBindpointFromCall(const trace::Call& call) const override;

    unsigned getBindpoint(unsigned target, unsigned index) const override;

    std::unordered_map<unsigned,
        std::unordered_map<unsigned, UsedObject::Pointer>> m_mapped_buffers;

    std::unordered_map<unsigned, unsigned> m_buffer_sizes;
    std::unordered_map<unsigned, std::pair<uint64_t, uint64_t>> m_buffer_mappings;
    std::unordered_map<unsigned, std::tuple<uint64_t, uint64_t, uint64_t>> m_old_buffer_mappings;

};

class VertexAttribObjectMap: public DependecyObjectWithDefaultBindPointMap {
public:
    VertexAttribObjectMap();
    void bindAVO(const trace::Call& call, BufferObjectMap& buffers);
    void bindVAOBuf(const trace::Call& call, BufferObjectMap& buffers);
    void vaBinding(const trace::Call& call, BufferObjectMap& buffers);
private:
    unsigned next_id;
};

class TextureObjectMap: public DependecyObjectMap {
public:
    TextureObjectMap();
    void oglActiveTexture(const trace::Call& call);
    UsedObject::Pointer oglBindMultitex(const trace::Call& call);
    void copy(const trace::Call& call);
    void bindToImageUnit(const trace::Call& call);
    void bindFromTextureTarget(unsigned unit, UsedObject::Pointer obj);
    void addImageDependencies(UsedObject::Pointer dep);
    void unbindUnits(unsigned first, unsigned count);
    void generateWithTarget(const trace::Call& call); 
private:
    void emitBoundObjectsExt(CallSet& out_calls) override;
    unsigned getBindpointFromCall(const trace::Call& call) const override;
    bool setTargetType(unsigned id, unsigned target) override;
    int getBindpointFromTargetAndUnit(unsigned target, unsigned unit) const;
    unsigned m_active_texture;
    std::unordered_map<uint32_t, std::unordered_map<unsigned, UsedObject::Pointer>> m_bound_images;
};

class QueryObjectMap: public DependecyObjectMap {
public:
    enum QueryType {
        query_samples_passed,
        query_any_samples_passed,
        query_primitives_generated,
        query_transform_feedback,
        query_tf_primitives_written,
        query_time_elapses,
        query_timestamp,
        query_last
    };

    void oglBeginQuery(const trace::Call& call);
    void oglBeginQueryIndexed(const trace::Call& call);

    void oglEndQuery(const trace::Call& call);
    void oglEndQueryIndexed(const trace::Call& call);
private:
    unsigned getBindpointFromCall(const trace::Call& call) const override;
    unsigned getBindpoint(unsigned target, unsigned index) const override;

    void endWithTargetIndex(unsigned target, unsigned index, const trace::Call &call);
};

class FramebufferObjectMap: public DependecyObjectMap {
public:
    FramebufferObjectMap(uint32_t context_id);
    void oglBlit(const trace::Call& call);
    void oglBlitNamed(const trace::Call& call);
    void oglReadBuffer(const trace::Call& call);
    void oglDrawFromBuffer(const trace::Call& call, BufferObjectMap &buffers);
private:
    UsedObject::Pointer
    bindTarget(unsigned id, unsigned bindpoint) override;
    unsigned getBindpointFromCall(const trace::Call& call) const override;
};

struct GlobalState {
    CallSet *out_list = nullptr;
    bool emit_dependencies = false;
    UsedObject::Pointer current_vao;
};

extern GlobalState global_state;


}
