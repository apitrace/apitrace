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

#include "ft_dependecyobject.hpp"

#include <cstring>

#include <GL/gl.h>
#include <GL/glext.h>

namespace frametrim {

UsedObject::UsedObject(unsigned id):
    m_id(id),
    m_emitted(true)
{

}

unsigned
UsedObject::id() const
{
    return m_id;
}

bool
UsedObject::emitted() const
{
    return m_emitted;
}

void
UsedObject::addCall(PTraceCall call)
{
    m_calls.push_back(call);
    m_emitted = false;
}

void
UsedObject::setCall(PTraceCall call)
{
    m_calls.clear();
    addCall(call);
}

void
UsedObject::addDependency(Pointer dep)
{
    m_dependencies.push_back(dep);
    m_emitted = false;
}

void UsedObject::setDependency(Pointer dep)
{
    m_dependencies.clear();
    addDependency(dep);
}

void
UsedObject::emitCallsTo(CallSet& out_list)
{
    if (!m_emitted) {
        m_emitted = true;
        for (auto&& n : m_calls)
            out_list.insert(n);

        for (auto&& o : m_dependencies)
            o->emitCallsTo(out_list);
    }
}

unsigned UsedObject::extraInfo(const std::string& key) const
{
    auto v = m_extra_info.find(key);
    return (v != m_extra_info.end()) ? v->second : 0;
}

void UsedObject::setExtraInfo(const std::string& key, unsigned value)
{
    m_extra_info[key] = value;
}

void UsedObject::eraseExtraInfo(const std::string& key)
{
    m_extra_info.erase(key);
}

bool UsedObject::createdBefore(unsigned callno) const
{
    if (m_calls.empty())
        return false;

    return callno > 0 && m_calls[0]->callNo() < callno;
}

void
DependecyObjectMap::generate_internal(const trace::Call& call, int array_id)
{
    auto c = trace2call(call);
    const auto ids = (call.arg(array_id)).toArray();
    std::vector<std::shared_ptr<UsedObject> > created_objs;
    for (auto& v : ids->values) {
        auto old_obj = getById(v->toUInt());
        auto obj = old_obj ? old_obj : std::make_shared<UsedObject>(v->toUInt());
        obj->addCall(c);
        obj->setExtraInfo("valid", 1);
        if (old_obj) {
            obj->setExtraInfo("reused_call", call.no);
        } else {
            obj->setExtraInfo("create_call", call.no);
        }
        addObject(v->toUInt(), obj);

        // make all objects that are created together depend on each other
        for (auto& o : created_objs) {
            o->addDependency(obj);
            obj->addDependency(o);
        }
        created_objs.push_back(obj);
    }
}

void
DependecyObjectMap::generate(const trace::Call& call)
{
    generate_internal(call, 1); 
}

void DependecyObjectMap::destroy(const trace::Call& call)
{
    auto c = trace2call(call);
    std::vector<std::shared_ptr<UsedObject> > destroyed_objs;
    const auto ids = (call.arg(1)).toArray();
    assert(ids);
    for (auto& v : ids->values) {
        unsigned obj_id = v->toUInt();
        auto obj_it = m_objects.find(obj_id);
        if (obj_it != m_objects.end()) {
            assert(obj_it->second->id() == obj_id);
            obj_it->second->addCall(c);
            obj_it->second->eraseExtraInfo("target");
            obj_it->second->setExtraInfo("valid", 0);
            obj_it->second->setExtraInfo("delete_call", call.no);

            // make all objects that are destroyed together depend on each other
            // to avoid the trimmed trace throwing errors if one buffer was
            // not needed otherwise
            for (auto& o : destroyed_objs) {
                o->addDependency(obj_it->second);
                obj_it->second->addDependency(o);
            }
            destroyed_objs.push_back(obj_it->second);
        }
    }
}

void DependecyObjectMap::create(const trace::Call& call)
{
    auto obj = std::make_shared<UsedObject>(call.ret->toUInt());
    addObject(call.ret->toUInt(), obj);
    auto c = trace2call(call);
    obj->addCall(c);
    obj->setExtraInfo("valid", 1);
    obj->setExtraInfo("create_call", call.no);
}

void DependecyObjectMap::del(const trace::Call& call)
{
    auto obj_it = m_objects.find(call.arg(0).toUInt());
    if (obj_it != m_objects.end()) {
        obj_it->second->addCall(trace2call(call));
        obj_it->second->setExtraInfo("valid", 0);
        obj_it->second->setExtraInfo("delete_call", call.no);
    }
}

void DependecyObjectMap::addObject(unsigned id, UsedObject::Pointer obj)
{
    m_objects[id] = obj;
}

UsedObject::Pointer
DependecyObjectMap::bind(const trace::Call& call, unsigned obj_id_param)
{
    unsigned id = call.arg(obj_id_param).toUInt();
    if (!setTargetType(id, call.arg(0).toUInt()))  {
        std::cerr << "Texture target type mismatch in call " << call.no << "\n";
        assert(0);
    }
    int bindpoint = getBindpointFromCall(call);
    if (bindpoint < 0) {
        std::cerr << "Unknown bindpoint in call "
                  << call.no << ":" << call.name() << "\n";
        assert(0);
    }

    if (id) {
        if (m_objects[id] && !m_objects[id]->extraInfo("valid")) {
            std::cerr << "\n" << call.no << ":" << call.name() << "(...) Object " << id
                      << " not valid, was deleted in "
                      << m_objects[id]->extraInfo("delete_call")
                      << " originally created in "
                      << m_objects[id]->extraInfo("create_call");

            unsigned reused_call = m_objects[id]->extraInfo("reused_call");
            if (reused_call > 0)
                std::cerr << " reused in " << reused_call << " ";
            std::cerr << " \n";
        }
    }
    return bindTarget(id, bindpoint);
}

bool DependecyObjectMap::setTargetType(unsigned id, unsigned target)
{
    (void)id;
    (void)target;
    return true;
}

UsedObject::Pointer
DependecyObjectMap::bindTarget(unsigned id, unsigned bindpoint)
{
    assert(m_current_context_id != 0xffffffff);

    auto& current_binding = m_bound_object[m_current_context_id];

    if (id) {
        current_binding[bindpoint] = m_objects[id];
    } else {
        current_binding[bindpoint] = nullptr;
    }
    return current_binding[bindpoint];
}

UsedObject::Pointer
DependecyObjectMap::bind( unsigned bindpoint, unsigned id)
{
    assert(m_current_context_id != 0xffffffff);
    return m_bound_object[m_current_context_id][bindpoint] = m_objects[id];
}

UsedObject::Pointer
DependecyObjectMap::boundTo(unsigned target, unsigned index)
{
    unsigned bindpoint = getBindpoint(target, index);
    return boundAtBinding(bindpoint);
}

UsedObject::Pointer DependecyObjectMap::boundAtBinding(unsigned index)
{
    assert(m_current_context_id != 0xffffffff);
    return m_bound_object[m_current_context_id][index];
}

unsigned DependecyObjectMap::getBindpoint(unsigned target, unsigned index) const
{
    (void)index;
    return target;
}

void
DependecyObjectMap::callOnBoundObject(const trace::Call& call)
{
    unsigned bindpoint = getBindpointFromCall(call);
    if (bindpoint == 0xffffffff) {
        std::cerr << "Unknown bindpoint in call " << call.no
                  << "(" << call.name() << ")\n";
    }

    assert(m_current_context_id != 0xffffffff);
    auto obj = m_bound_object[m_current_context_id][bindpoint];
    if (!obj)  {
        return;
    }

    obj->addCall(trace2call(call));
}

UsedObject::Pointer
DependecyObjectMap::bindWithCreate(const trace::Call& call, unsigned obj_id_param)
{
    unsigned bindpoint = getBindpointFromCall(call);
    unsigned id = call.arg(obj_id_param).toUInt();
    if (!m_objects[id])  {
        m_objects[id] = std::make_shared<UsedObject>(id);
    }
    return bindTarget(id, bindpoint);
}

void
DependecyObjectMap::callOnObjectBoundTo(const trace::Call& call, unsigned bindpoint)
{
    auto obj = boundTo(bindpoint);

    assert(obj);

    obj->addCall(trace2call(call));
}

void
DependecyObjectMap::callOnNamedObject(const trace::Call& call)
{
    auto obj = m_objects[call.arg(0).toUInt()];
    if (!obj) {
        if (call.arg(0).toUInt()) {
            std::cerr << "Named object " << call.arg(0).toUInt()
                      << " doesn't exists in call " << call.no << ": "
                      << call.name() << " ... ignoring\n";
            return;
        }
    } else
        obj->addCall(trace2call(call));
}

UsedObject::Pointer
DependecyObjectMap::callOnBoundObjectWithDep(const trace::Call& call,
                                             DependecyObjectMap& other_objects,
                                             int dep_obj_param,
                                             bool reverse_dep_too)
{
    assert(m_current_context_id != 0xffffffff);
    unsigned obj_id = call.arg(dep_obj_param).toUInt();
    unsigned bindpoint = getBindpointFromCall(call);
    auto bound_obj = m_bound_object[m_current_context_id][bindpoint];

    if (!bound_obj) {
        if (obj_id)
            std::cerr << "No object bound in call " << call.no << ":" << call.name() << "\n";
        return nullptr; 
    }

    UsedObject::Pointer obj = nullptr;
    
    if (obj_id) {
        obj = other_objects.getById(obj_id);
        assert(obj);
        bound_obj->addDependency(obj);
        if (reverse_dep_too)
            obj->addDependency(bound_obj);
    }
    bound_obj->addCall(trace2call(call));
    return obj;
}

void
DependecyObjectMap::callOnBoundObjectWithDepBoundTo(const trace::Call& call,
                                                    DependecyObjectMap& other_objects,
                                                    int bindingpoint)
{
    assert(m_current_context_id != 0xffffffff);
    unsigned bindpoint = getBindpointFromCall(call);
    auto bound_obj = m_bound_object[m_current_context_id][bindpoint];

    if (!bound_obj) {
        return;
    }

    bound_obj->addCall(trace2call(call));

    UsedObject::Pointer obj = nullptr;
    auto dep = other_objects.boundTo(bindingpoint);
    if (dep) {
        bound_obj->addDependency(dep);
        if (global_state.emit_dependencies)
            dep->emitCallsTo(*global_state.out_list);
    }
}

void
DependecyObjectMap::callOnNamedObjectWithDep(const trace::Call& call,
                                             DependecyObjectMap& other_objects,
                                             int dep_obj_param,
                                             bool reverse_dep_too)
{
    auto obj = m_objects[call.arg(0).toUInt()];

    if (!obj) {
        if (!call.arg(0).toUInt())
            return;

        std::cerr << "Call:" << call.no << ":" << call.name()
                  << " Object " << call.arg(0).toUInt() << "not found\n";
        assert(0);
    }



    unsigned dep_obj_id = call.arg(dep_obj_param).toUInt();
    if (dep_obj_id) {
        auto dep_obj = other_objects.getById(dep_obj_id);
        assert(dep_obj);
        obj->addDependency(dep_obj);
        if (reverse_dep_too)
            dep_obj->addDependency(obj);
    }
    obj->addCall(trace2call(call));
}

void
DependecyObjectMap::callOnNamedObjectWithNamedDep(const trace::Call& call,
                                                  DependecyObjectMap& other_objects,
                                                  int dep_call_param)
{
    auto obj = getById(call.arg(0).toUInt());

    if (!obj) {
        std::cerr << "No object bound in call " << call.no << ":" << call.name() << "\n";
        assert(0);
    }
    obj->addCall(trace2call(call));

    auto dep = other_objects.getById(call.arg(dep_call_param).toUInt());
    if (dep) {
        obj->addDependency(dep);
        if (global_state.emit_dependencies)
            dep->emitCallsTo(*global_state.out_list);
    }
}

void
DependecyObjectMap::callOnNamedObjectWithDepBoundTo(const trace::Call& call,
                                                    DependecyObjectMap& other_objects,
                                                    int dep_call_param)
{
    auto obj = getById(call.arg(0).toUInt());
    if (!obj) {
        std::cerr << "No object named " << call.arg(0).toUInt()
                  << " in call " << call.no << ":" << call.name() << "\n";
        assert(0);
    }

    obj->addCall(trace2call(call));

    auto dep = other_objects.boundTo(dep_call_param);
    if (dep) {
        obj->addDependency(dep);
        if (global_state.emit_dependencies)
            dep->emitCallsTo(*global_state.out_list);
    }
}


void DependecyObjectMap::addCall(PTraceCall call)
{
    m_calls.push_back(call);
}

UsedObject::Pointer
DependecyObjectMap::getById(unsigned id) const
{
    auto i = m_objects.find(id);
    return i !=  m_objects.end() ? i->second : nullptr;
}

static bool isNonRepeatCall(const std::string& name)
{
    static const std::unordered_set<std::string> nonRepeateCalls = {
        "glShaderSource",
        "glCompileShader",
        "glAttachShader",
        "glLinkProgram",
    };
    return nonRepeateCalls.find(name) != nonRepeateCalls.end();
}

void
DependecyObjectMap::unbalancedCreateCallsInLastFrame(uint32_t last_frame_start,
                                                     std::unordered_set<unsigned>& outSet)
{
    for (auto&& [key, obj] : m_objects) {
        if (obj && obj->extraInfo("delete_call") == 0) {
            auto create_callno = obj->extraInfo("create_call");
            if (create_callno >= last_frame_start) {
                outSet.insert(create_callno);
            }
            auto reused_callno = obj->extraInfo("reused_call");
            if (reused_callno >= last_frame_start) {
                outSet.insert(reused_callno);
            }
            for (auto&& c : obj->calls()) {
                if (c->callNo() >= last_frame_start && isNonRepeatCall(c->name()))
                    outSet.insert(c->callNo());
            }
        }
    }
}

void
DependecyObjectMap::emitBoundObjects(CallSet& out_calls)
{
    assert(m_current_context_id != 0xffffffff);
    for (auto&& [key, bound_object]: m_bound_object) {
        for (auto&& [key, obj]: bound_object) {
            if (obj)
                obj->emitCallsTo(out_calls);
        }
    }
    for(auto&& c : m_calls)
        out_calls.insert(c);
}

void DependecyObjectMap::addBoundAsDependencyTo(UsedObject& obj)
{
    assert(m_current_context_id != 0xffffffff);
    for (auto&& [key, bound_obj]: m_bound_object[m_current_context_id]) {
        if (bound_obj)
            obj.addDependency(bound_obj);
    }
}

void DependecyObjectMap::emitBoundObjectsExt(CallSet& out_calls)
{
    (void)out_calls;
}

unsigned
DependecyObjectWithSingleBindPointMap::getBindpointFromCall(const trace::Call& call) const
{
    (void)call;
    return 0;
}

unsigned
DependecyObjectWithDefaultBindPointMap::getBindpointFromCall(const trace::Call& call) const
{
    return call.arg(0).toUInt();
}

void BufferObjectMap::bindBuffer(const trace::Call& call)
{
    unsigned target = call.arg(0).toUInt();
    unsigned index = call.arg(1).toUInt();
    unsigned bufid  = call.arg(2).toUInt();

    unsigned bindpoint = getBindpoint(target, 0);
    auto buf = bind(bindpoint, bufid);
    if (index != 0) {
        unsigned bindpoint = getBindpoint(target, index);
        bind(bindpoint, bufid);
    }
    if (buf)
        buf->addCall(trace2call(call));
    else
        addCall(trace2call(call));
}


UsedObject::Pointer
BufferObjectMap::boundToTarget(unsigned target)
{
    return boundTo(target, 0);
}

unsigned
BufferObjectMap::getBindpoint(unsigned target, unsigned index) const
{
    switch (target) {
    case GL_ARRAY_BUFFER:
        return bt_array;
    case GL_ATOMIC_COUNTER_BUFFER:
        return bt_atomic_counter + bt_last * index;
    case GL_COPY_READ_BUFFER:
        return bt_copy_read;
    case GL_COPY_WRITE_BUFFER:
        return bt_copy_write;
    case GL_DISPATCH_INDIRECT_BUFFER:
        return bt_dispatch_indirect;
    case GL_DRAW_INDIRECT_BUFFER:
        return bt_draw_indirect;
    case GL_ELEMENT_ARRAY_BUFFER:
        return bt_element_array;
    case GL_PIXEL_PACK_BUFFER:
        return bt_pixel_pack;
    case GL_PIXEL_UNPACK_BUFFER:
        return bt_pixel_unpack;
    case GL_QUERY_BUFFER:
        return bt_query;
    case GL_SHADER_STORAGE_BUFFER:
        return bt_ssbo + bt_last * index;
    case GL_TEXTURE_BUFFER:
        return bt_texture;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        return bt_tf  + bt_last * index;
    case GL_UNIFORM_BUFFER:
        return bt_uniform + bt_last * index;
    case GL_PARAMETER_BUFFER:
        return bt_parameter;
    }
    std::cerr << "Unknown buffer binding target=" << target << "\n";
    assert(0);
    return 0;
}

unsigned
BufferObjectMap::getBindpointFromCall(const trace::Call& call) const
{
    /* These two calls are handled elsewhere, since they don't touch the  */
    assert(strcmp(call.name(), "glBindBufferRange"));
    assert(strcmp(call.name(), "glBindBufferBase"));

    return getBindpoint(call.arg(0).toUInt(), 0);
}

void
BufferObjectMap::data(const trace::Call& call)
{
    unsigned target = getBindpointFromCall(call);
    auto buf = boundAtBinding(target);
    if (buf) {
        m_buffer_sizes[buf->id()] = call.arg(1).toUInt();
        buf->addCall(trace2call(call));
    }
}

void
BufferObjectMap::namedData(const trace::Call& call)
{
    auto buf = getById(call.arg(0).toUInt());
    if (buf) {
        m_buffer_sizes[buf->id()] = call.arg(1).toUInt();
        buf->addCall(trace2call(call));
    }
}

void
BufferObjectMap::map(const trace::Call& call)
{
    unsigned target = getBindpointFromCall(call);
    auto buf = boundAtBinding(target);
    if (buf) {
        m_mapped_buffers[target][buf->id()] = buf;
        uint64_t begin = call.ret->toUInt();
        uint64_t end = begin + m_buffer_sizes[buf->id()];
        m_buffer_mappings[buf->id()] = std::make_pair(begin, end);
        buf->addCall(trace2call(call));
    }
}

void
BufferObjectMap::namedMap(const trace::Call& call)
{
    auto buf = getById(call.arg(0).toUInt());
    if (buf) {
        m_mapped_buffers[bt_names_access][buf->id()] = buf;
        uint64_t begin = call.ret->toUInt();
        uint64_t end = begin + m_buffer_sizes[buf->id()];
        m_buffer_mappings[buf->id()] = std::make_pair(begin, end);
        buf->addCall(trace2call(call));
    }
}

void
BufferObjectMap::mapRange(const trace::Call& call)
{
    unsigned target = getBindpointFromCall(call);
    auto buf = boundAtBinding(target);
    if (buf) {
        m_mapped_buffers[target][buf->id()] = buf;
        uint64_t begin = call.ret->toUInt();
        uint64_t end = begin + call.arg(2).toUInt();
        m_buffer_mappings[buf->id()] = std::make_pair(begin, end);
        buf->addCall(trace2call(call));
    }
}

void
BufferObjectMap::namedMapRange(const trace::Call& call)
{
    auto buf = getById(call.arg(0).toUInt());
    if (buf) {
        m_mapped_buffers[bt_names_access][buf->id()] = buf;
        uint64_t begin = call.ret->toUInt();
        uint64_t end = begin + call.arg(2).toUInt();
        m_buffer_mappings[buf->id()] = std::make_pair(begin, end);
        buf->addCall(trace2call(call));
    }
}

void
BufferObjectMap::unmap(const trace::Call& call)
{
    unsigned target = getBindpointFromCall(call);
    auto buf = boundAtBinding(target);
    if (buf) {
        m_mapped_buffers[target].erase(buf->id());
        m_old_buffer_mappings[buf->id()] =
                std::make_tuple(m_buffer_mappings[buf->id()].first,
                m_buffer_mappings[buf->id()].second,
                call.no);

        m_buffer_mappings[buf->id()] = std::make_pair(0, 0);
        buf->addCall(trace2call(call));
    }
}

void
BufferObjectMap::namedUnmap(const trace::Call& call)
{
    auto buf = getById(call.arg(0).toUInt());
    if (buf) {
        m_mapped_buffers[bt_names_access].erase(buf->id());
        m_old_buffer_mappings[buf->id()] =
                std::make_tuple(m_buffer_mappings[buf->id()].first,
                m_buffer_mappings[buf->id()].second,
                call.no);
        m_buffer_mappings[buf->id()] = std::make_pair(0, 0);
        buf->addCall(trace2call(call));
    }
}


void
BufferObjectMap::memcopy(const trace::Call& call, CallSet& out_set, bool recording)
{
    uint64_t start = call.arg(0).toUInt();
    uint64_t data_end = start + call.arg(2).toUInt();
    unsigned buf_id = 0;
    for(auto&& [id, range ]: m_buffer_mappings) {
        if (range.first <= start && start < range.second) {
            if (data_end > range.second) {
                std::cerr << "\n:Error "<< call.no << "(memcpy): Mapped target range is ["
                          << range.first << ", " << range.second << "] but data requires ["
                          << start << ", " << data_end << "]\n";
                assert(0);
            }

            buf_id = id;
            break;
        }
    }
    if (!buf_id) {
        std::cerr << "Found no mapping for memcopy to " << start << " in call " << call.no << ": " << call.name() << "\n";
        std::cerr << "Try old mapping ...";
        for(auto&& [id, range ]: m_old_buffer_mappings) {
            auto [begin, end, callid] = range;
            if (begin <= start && start < end) {
                buf_id = id;
                std::cerr << "Range is in buffer " << id << " unmapped in call " << callid << "\n";
                assert(0);
                break;
            }
        }
        std::cerr << "no, address " << start << " was never in a mapped buffer range\n";
        assert(0);
        return;
    }
    auto buf = getById(buf_id);
    buf->addCall(trace2call(call));

    if (recording)
        buf->emitCallsTo(out_set);
}

void BufferObjectMap::addSSBODependencies(UsedObject::Pointer dep)
{
    for(auto && [key, buf]: objects_bound_in_context()) {
        if (buf && ((key % bt_last) == bt_ssbo)) {
            buf->addDependency(dep);
            dep->addDependency(buf);
        }
    }
}

void BufferObjectMap::copyBufferSubData(const trace::Call& call)
{
    auto src = boundTo(call.arg(0).toUInt());
    auto dst = boundTo(call.arg(1).toUInt());
    assert(dst);
    assert(src);
    dst->addDependency(src);
    dst->addCall(trace2call(call));
}

void BufferObjectMap::copyNamedBufferSubData(const trace::Call& call)
{
    auto src = getById(call.arg(0).toUInt());
    auto dst = getById(call.arg(1).toUInt());
    assert(dst);
    assert(src);
    dst->addDependency(src);
    dst->addCall(trace2call(call));
}

VertexAttribObjectMap::VertexAttribObjectMap():next_id(1)
{

}

void
VertexAttribObjectMap::bindAVO(const trace::Call& call, BufferObjectMap& buffers)
{
    unsigned id = call.arg(0).toUInt();
    auto obj = std::make_shared<UsedObject>(next_id);
    addObject(next_id, obj);
    bind(id, next_id);
    auto c = trace2call(call);
    obj->addCall(c);

    auto buf = buffers.boundToTarget(GL_ARRAY_BUFFER);
    if (buf) {
        obj->addDependency(buf);
        if (global_state.emit_dependencies) {
            buf->emitCallsTo(*global_state.out_list);
        }
    }
    if (global_state.current_vao) {
        global_state.current_vao->addDependency(obj);
    }

    ++next_id;
}

void VertexAttribObjectMap::bindVAOBuf(const trace::Call& call, BufferObjectMap& buffers)
{
    unsigned id = call.arg(0).toUInt();
    auto obj = std::make_shared<UsedObject>(next_id);
    addObject(next_id, obj);
    bind(id, next_id);
    obj->addCall(trace2call(call));

    auto buf = buffers.getById(call.arg(1).toUInt());
    assert(buf || (call.arg(1).toUInt() == 0));
    if (buf) {
        obj->addDependency(buf);
        if (global_state.emit_dependencies) {
            buf->emitCallsTo(*global_state.out_list);
        }
    }
    ++next_id;
}

TextureObjectMap::TextureObjectMap():
    m_active_texture(0)
{

}

void TextureObjectMap::generateWithTarget(const trace::Call& call)
{
    generate_internal(call, 2); 
}

void
TextureObjectMap::oglActiveTexture(const trace::Call& call)
{
    unsigned active_texture = call.arg(0).toUInt() - GL_TEXTURE0;

    if (call.arg(0).toUInt() >= GL_TEXTURE0) {
        m_active_texture = active_texture;
        addCall(trace2call(call));
    } else {
        std::cerr << call.no << ": " << call.name()
                  << " Invalid texture unit " << call.arg(0).toUInt()
                  << " specified, ignoring call\n";
    }
}

enum TexTypes {
    gl_texture_buffer,
    gl_texture_1d,
    gl_texture_2d,
    gl_texture_3d,
    gl_texture_cube,
    gl_texture_1d_array,
    gl_texture_2d_array,
    gl_texture_cube_array,
    gl_texture_2dms,
    gl_texture_2dms_array,
    gl_texture_rectangle,
    gl_texture_last
};

UsedObject::Pointer
TextureObjectMap::oglBindMultitex(const trace::Call& call)
{
    unsigned unit = call.arg(0).toUInt() - GL_TEXTURE0;
    unsigned target = call.arg(1).toUInt();
    unsigned id = call.arg(2).toUInt();
    if (!setTargetType(id, target)) {
        std::cerr << "Texture target type mismatch in call " << call.no << "\n";
        assert(0);
    }
    unsigned bindpoint  = getBindpointFromTargetAndUnit(target, unit);
    return bind(bindpoint, id);
}

bool TextureObjectMap::setTargetType(unsigned id, unsigned target)
{
    if (!id)
        return true;
    auto tex = getById(id);
    if (!tex) {
        tex = std::make_shared<UsedObject>(id);
        addObject(id, tex);
    }

    unsigned old_target = tex->extraInfo("target");
    if (!old_target)
        tex->setExtraInfo("target", target);
    else {
        if (old_target != target) {
            std::cerr << "Already have target " << old_target  << " != new target " << target << "\n";
            return false;
        }
    }
    return true;
}

void TextureObjectMap::copy(const trace::Call& call)
{
    unsigned src_id = call.arg(0).toUInt();
    unsigned dst_id = call.arg(6).toUInt();

    auto src = getById(src_id);
    auto dst = getById(dst_id);
    assert(src && dst);
    dst->addCall(trace2call(call));
    dst->addDependency(src);
}

void TextureObjectMap::bindToImageUnit(const trace::Call& call)
{
    unsigned unit = call.arg(0).toUInt();
    unsigned id = call.arg(1).toUInt();

    auto c = trace2call(call);
    if (id) {
        auto tex = getById(id);
        assert(tex);
        tex->addCall(c);
        m_bound_images[context_id()][unit] = tex;
    } else {
        addCall(c);
        m_bound_images[context_id()][unit] = nullptr;
    }
}

void TextureObjectMap::bindFromTextureTarget(unsigned unit, UsedObject::Pointer obj)
{
    unsigned target = obj->extraInfo("target");
    assert(target > 0);
    unsigned bindpoint = getBindpointFromTargetAndUnit(target, unit);
    bind(bindpoint, obj->id());
}

void TextureObjectMap::unbindUnits(unsigned first, unsigned count)
{
    for (unsigned i = 0; i < count; ++i) {
        for (unsigned t = gl_texture_1d; t < gl_texture_last; ++t) {
            unsigned bindpoint = t + (first + i) * gl_texture_last;
            bind(bindpoint, 0);
        }
    }
}

unsigned
TextureObjectMap::getBindpointFromCall(const trace::Call& call) const
{
    unsigned target = call.arg(0).toUInt();
    return getBindpointFromTargetAndUnit(target, m_active_texture);
}

void TextureObjectMap::emitBoundObjectsExt(CallSet& out_calls)
{
    for(auto [unit, tex]: m_bound_images[context_id()])
        if (tex)
            tex->emitCallsTo(out_calls);
}

void TextureObjectMap::addImageDependencies(UsedObject::Pointer dep)
{
    for (auto&& [key, tex]: m_bound_images[context_id()]) {
        if (tex) {
            tex->addDependency(dep);
            dep->addDependency(tex);
        }
    }
}

int
TextureObjectMap::getBindpointFromTargetAndUnit(unsigned target, unsigned unit) const
{
    switch (target) {
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP:
        target = gl_texture_cube;
        break;
    case GL_PROXY_TEXTURE_1D:
    case GL_TEXTURE_1D:
        target = gl_texture_1d;
        break;
    case GL_PROXY_TEXTURE_2D:
    case GL_TEXTURE_2D:
        target = gl_texture_2d;
        break;
    case GL_PROXY_TEXTURE_3D:
    case GL_TEXTURE_3D:
        target = gl_texture_3d;
        break;
    case GL_TEXTURE_CUBE_MAP_ARRAY:
        target = gl_texture_cube_array;
        break;
    case GL_TEXTURE_1D_ARRAY:
        target = gl_texture_1d_array;
        break;
    case GL_TEXTURE_2D_ARRAY:
        target = gl_texture_2d_array;
        break;
    case GL_TEXTURE_2D_MULTISAMPLE:
        target = gl_texture_2dms;
        break;
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        target = gl_texture_2dms_array;
        break;
    case GL_TEXTURE_BUFFER:
        target = gl_texture_buffer;
        break;
    case GL_TEXTURE_RECTANGLE:
        target = gl_texture_rectangle;
        break;
    default:
        std::cerr << "target=" << target << " not supported\n";
        return -1;
        ;
    }
    return target + unit * gl_texture_last;
}

unsigned QueryObjectMap::getBindpointFromCall(const trace::Call& call) const
{
    unsigned target = call.arg(0).toUInt();
    unsigned index = 0;
    if (!strcmp(call.name(), "glBeginQueryIndexed") ||
        !strcmp(call.name(), "glEndQueryIndexed")) {
        index = call.arg(1).toUInt();
    }
    return getBindpoint(target, index);
}

unsigned QueryObjectMap::getBindpoint(unsigned target, unsigned index) const
{
    unsigned bindpoint = index * query_last;
    switch (target) {
    case GL_SAMPLES_PASSED:
        bindpoint += query_samples_passed;
        break;
    case GL_ANY_SAMPLES_PASSED:
        bindpoint += query_any_samples_passed;
        break;
    case GL_ANY_SAMPLES_PASSED_CONSERVATIVE:
        bindpoint += query_primitives_generated;
        break;
    case GL_PRIMITIVES_GENERATED:
        bindpoint += query_transform_feedback;
        break;
    case GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN:
        bindpoint += query_tf_primitives_written;
        break;
    case GL_TIME_ELAPSED:
        bindpoint += query_time_elapses;
        break;
    case GL_TIMESTAMP:
        bindpoint += query_timestamp;
        break;
    default:
        std::cerr << "Unknown query target "<< target << " in " << __func__ << "\n";
        bindpoint = 0xffffffff;
    }
    return bindpoint;
}

void QueryObjectMap::oglEndQuery(const trace::Call& call)
{
    unsigned target = call.arg(0).toUInt();
    endWithTargetIndex(target, 0, call);
}

void QueryObjectMap::oglEndQueryIndexed(const trace::Call& call)
{
    unsigned target = call.arg(0).toUInt();
    unsigned index = call.arg(1).toUInt();
    endWithTargetIndex(target, index, call);
}

void QueryObjectMap::endWithTargetIndex(unsigned target, unsigned index, const trace::Call& call)
{
    auto obj = boundTo(target, index);
    if (obj)
        obj->addCall(trace2call(call));

    bind(getBindpoint(target, index), 0);
}


FramebufferObjectMap::FramebufferObjectMap(uint32_t context_id)
{
    set_current_context_id(context_id);
    auto default_fb = std::make_shared<UsedObject>(0);
    addObject(0, default_fb);
    bind(GL_DRAW_FRAMEBUFFER, 0);
    bind(GL_READ_FRAMEBUFFER, 0);
}

unsigned
FramebufferObjectMap::getBindpointFromCall(const trace::Call& call) const
{
    return call.arg(0).toUInt();
}

UsedObject::Pointer
FramebufferObjectMap::bindTarget(unsigned id, unsigned bindpoint)
{
    UsedObject::Pointer obj = nullptr;
    if (bindpoint == GL_FRAMEBUFFER ||
        bindpoint == GL_DRAW_FRAMEBUFFER) {
        bind(GL_FRAMEBUFFER, id);
        obj = bind(GL_DRAW_FRAMEBUFFER, id);
    }

    if (bindpoint == GL_FRAMEBUFFER ||
        bindpoint == GL_READ_FRAMEBUFFER) {
        obj = bind(GL_READ_FRAMEBUFFER, id);
    }

    return obj;
}

void
FramebufferObjectMap::oglBlit(const trace::Call& call)
{
    auto dest = boundTo(GL_DRAW_FRAMEBUFFER);
    auto src = boundTo(GL_READ_FRAMEBUFFER);
    assert(dest);
    assert(src);
    dest->addDependency(src);
    dest->addCall(trace2call(call));
}

void
FramebufferObjectMap::oglBlitNamed(const trace::Call& call)
{
    auto dest = getById(call.arg(0).toUInt()); 
    auto src = getById(call.arg(1).toUInt()); 
    assert(dest);
    assert(src);
    dest->addDependency(src);
    dest->addCall(trace2call(call));
}


void
FramebufferObjectMap::oglReadBuffer(const trace::Call& call)
{
    auto fbo = boundTo(GL_READ_FRAMEBUFFER);
    if (call.arg(0).toUInt() == GL_BACK && fbo->id() != 0) {
        std::cerr << "\nFBO " << fbo->id()
                  << " bound, but trying to bind GL_BACK as readbuffer in call "
                  << call.no << "\n";
        assert(0);
    }
    callOnObjectBoundTo(call, GL_READ_FRAMEBUFFER);
}

void FramebufferObjectMap::oglDrawFromBuffer(const trace::Call& call, BufferObjectMap& buffers)
{
    auto fbo = boundTo(GL_DRAW_FRAMEBUFFER);
    if (fbo->id()) {
        auto buf = buffers.boundToTarget(GL_ELEMENT_ARRAY_BUFFER);
        if (buf)
            fbo->addDependency(buf);
        fbo->addCall(trace2call(call));
    }
}

GlobalState global_state;

}
