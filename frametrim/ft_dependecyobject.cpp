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

void
DependecyObjectMap::generate(const trace::Call& call)
{
    auto c = trace2call(call);
    const auto ids = (call.arg(1)).toArray();
    for (auto& v : ids->values) {
        auto obj = std::make_shared<UsedObject>(v->toUInt());
        obj->addCall(c);
        addObject(v->toUInt(), obj);
    }
}

void DependecyObjectMap::destroy(const trace::Call& call)
{
    auto c = trace2call(call);
    const auto ids = (call.arg(1)).toArray();
    for (auto& v : ids->values) {
        assert(m_objects[v->toUInt()]->id() == v->toUInt());
        m_objects[v->toUInt()]->addCall(c);
    }
}

void DependecyObjectMap::create(const trace::Call& call)
{
    auto obj = std::make_shared<UsedObject>(call.ret->toUInt());
    addObject(call.ret->toUInt(), obj);
    auto c = trace2call(call);
    obj->addCall(c);
}

void DependecyObjectMap::del(const trace::Call& call)
{
    auto obj = m_objects[call.arg(0).toUInt()];
    auto c = trace2call(call);
    obj->addCall(c);
}


void DependecyObjectMap::addObject(unsigned id, UsedObject::Pointer obj)
{
    m_objects[id] = obj;
}

UsedObject::Pointer
DependecyObjectMap::bind(const trace::Call& call, unsigned obj_id_param)
{
    unsigned id = call.arg(obj_id_param).toUInt();
    setTargetType(id, call.arg(0).toUInt());
    int bindpoint = getBindpointFromCall(call);
    if (bindpoint < 0) {
        std::cerr << "Unknown bindpoint in call "
                  << call.no << ":" << call.name() << "\n";
        assert(0);
    }
    return bindTarget(id, bindpoint);
}

void
DependecyObjectMap::setTargetType(unsigned id, unsigned target)
{
    (void)id;
    (void)target;
}

UsedObject::Pointer
DependecyObjectMap::bindTarget(unsigned id, unsigned bindpoint)
{
    if (id) {
        m_bound_object[bindpoint] = m_objects[id];
    } else {
        m_bound_object[bindpoint] = nullptr;
    }
    return m_bound_object[bindpoint];
}

UsedObject::Pointer
DependecyObjectMap::bind( unsigned bindpoint, unsigned id)
{
    return m_bound_object[bindpoint] = m_objects[id];
}

UsedObject::Pointer
DependecyObjectMap::boundTo(unsigned target, unsigned index)
{
    unsigned bindpoint = getBindpoint(target, index);
    return boundAtBinding(bindpoint);
}

DependecyObjectMap::ObjectMap::iterator
DependecyObjectMap::begin()
{
    return m_bound_object.begin();
}

DependecyObjectMap::ObjectMap::iterator
DependecyObjectMap::end()
{
    return m_bound_object.end();
}

UsedObject::Pointer DependecyObjectMap::boundAtBinding(unsigned index)
{
    return m_bound_object[index];
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

    if (!m_bound_object[bindpoint])  {
        return;
    }

    m_bound_object[bindpoint]->addCall(trace2call(call));
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
                      << call.name() << "\n";
            assert(0);
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
    unsigned bindpoint = getBindpointFromCall(call);
    if (!m_bound_object[bindpoint]) {
        std::cerr << "No object bound in call " << call.no << ":" << call.name() << "\n";
        assert(0);
    }

    UsedObject::Pointer obj = nullptr;
    unsigned obj_id = call.arg(dep_obj_param).toUInt();
    if (obj_id) {
        obj = other_objects.getById(obj_id);
        assert(obj);
        m_bound_object[bindpoint]->addDependency(obj);
        if (reverse_dep_too)
            obj->addDependency(m_bound_object[bindpoint]);
    }
    m_bound_object[bindpoint]->addCall(trace2call(call));
    return obj;
}

void
DependecyObjectMap::callOnBoundObjectWithDepBoundTo(const trace::Call& call,
                                                    DependecyObjectMap& other_objects,
                                                    int bindingpoint, CallSet& out_set, bool recording)
{
    unsigned bindpoint = getBindpointFromCall(call);
    if (!m_bound_object[bindpoint]) {
        std::cerr << "No object bound in call " << call.no << ":" << call.name() << "\n";
        assert(0);
    }
    m_bound_object[bindpoint]->addCall(trace2call(call));

    UsedObject::Pointer obj = nullptr;
    auto dep = other_objects.boundTo(bindingpoint);
    if (dep) {
        m_bound_object[bindpoint]->addDependency(dep);
        if (recording)
            dep->emitCallsTo(out_set);
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
                                                  int dep_call_param, CallSet& out_set, bool recording)
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
        if (recording)
            dep->emitCallsTo(out_set);
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

void
DependecyObjectMap::emitBoundObjects(CallSet& out_calls)
{
    for (auto&& [key, obj]: m_bound_object) {
        if (obj)
            obj->emitCallsTo(out_calls);
    }
    for(auto&& c : m_calls)
        out_calls.insert(c);
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

UsedObject::Pointer
BufferObjectMap::boundToTarget(unsigned target, unsigned index)
{
    return boundTo(target, index);
}

unsigned
BufferObjectMap::getBindpoint(unsigned target, unsigned index) const
{
    switch (target) {
    case GL_ARRAY_BUFFER:
        return bt_array;
    case GL_ATOMIC_COUNTER_BUFFER:
        return bt_atomic_counter + bt_last* index;
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
    }
    std::cerr << "Unknown buffer binding target=" << target << "\n";
    assert(0);
    return 0;
}

unsigned
BufferObjectMap::getBindpointFromCall(const trace::Call& call) const
{
    unsigned target = call.arg(0).toUInt();
    unsigned index = 0;
    if (!strcmp(call.name(), "glBindBufferRange")) {
        index = call.arg(1).toUInt();
    }
    return getBindpoint(target, index);
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
BufferObjectMap::map_range(const trace::Call& call)
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
BufferObjectMap::unmap(const trace::Call& call)
{
    unsigned target = getBindpointFromCall(call);
    auto buf = boundAtBinding(target);
    if (buf) {
        m_mapped_buffers[target].erase(buf->id());
        m_buffer_mappings[buf->id()] = std::make_pair(0, 0);
        buf->addCall(trace2call(call));
    }
}

void
BufferObjectMap::memcopy(const trace::Call& call)
{
    uint64_t start = call.arg(0).toUInt();
    unsigned buf_id = 0;
    for(auto&& [id, range ]: m_buffer_mappings) {
        if (range.first <= start && start < range.second) {
            buf_id = id;
            break;
        }
    }
    if (!buf_id) {
        std::cerr << "Found no mapping for memcopy to " << start << " in call " << call.no << ": " << call.name() << "\n";
        assert(0);
    }
    auto buf = getById(buf_id);
    buf->addCall(trace2call(call));
}

void BufferObjectMap::addSSBODependencies(UsedObject::Pointer dep)
{
    for(auto && [key, buf]: *this) {
        if (buf && ((key % bt_last) == bt_ssbo)) {
            buf->addDependency(dep);
            dep->addDependency(buf);
        }
    }
}

VertexAttribObjectMap::VertexAttribObjectMap():next_id(1)
{

}

void
VertexAttribObjectMap::bindAVO(const trace::Call& call, BufferObjectMap& buffers,
                               CallSet &out_list, bool emit_dependencies)
{
    unsigned id = call.arg(0).toUInt();
    auto obj = std::make_shared<UsedObject>(next_id);
    addObject(next_id, obj);
    bind(id, next_id);
    obj->addCall(trace2call(call));

    auto buf = buffers.boundToTarget(GL_ARRAY_BUFFER);
    if (buf) {
        obj->addDependency(buf);
        if (emit_dependencies) {
            buf->emitCallsTo(out_list);
        }
    }
    ++next_id;
}

void VertexAttribObjectMap::bindVAOBuf(const trace::Call& call, BufferObjectMap& buffers,
                                       CallSet &out_list, bool emit_dependencies)
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
        if (emit_dependencies) {
            buf->emitCallsTo(out_list);
        }
    }
    ++next_id;
}

TextureObjectMap::TextureObjectMap():
    m_active_texture(0)
{

}

void
TextureObjectMap::oglActiveTexture(const trace::Call& call)
{
    m_active_texture = call.arg(0).toUInt() - GL_TEXTURE0;
    addCall(trace2call(call));
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
    setTargetType(id, target);
    unsigned bindpoint  = getBindpointFromTargetAndUnit(target, unit);
    return bind(bindpoint, id);
}

void TextureObjectMap::setTargetType(unsigned id, unsigned target)
{
    if (!id)
        return;
    auto tex = getById(id);
    unsigned old_target = tex->extraInfo("target");
    if (!old_target)
        tex->setExtraInfo("target", target);
    else
        assert(old_target == target);
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
        m_bound_images[unit] = tex;
    } else {
        addCall(c);
        m_bound_images[unit] = nullptr;
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
    for(auto [unit, tex]: m_bound_images)
        if (tex)
            tex->emitCallsTo(out_calls);
}

void TextureObjectMap::addImageDependencies(UsedObject::Pointer dep)
{
    for (auto&& [key, tex]: m_bound_images) {
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


FramebufferObjectMap::FramebufferObjectMap()
{
    auto default_fb = std::make_shared<UsedObject>(0);
    addObject(0, default_fb);
    bind(GL_DRAW_FRAMEBUFFER, 0);
    bind(GL_READ_FRAMEBUFFER, 0);
}

unsigned
FramebufferObjectMap::getBindpointFromCall(const trace::Call& call) const
{
    switch (call.arg(0).toUInt()) {
    case GL_FRAMEBUFFER:
    case GL_DRAW_FRAMEBUFFER:
        return GL_DRAW_FRAMEBUFFER;
    case GL_READ_FRAMEBUFFER:
        return GL_READ_FRAMEBUFFER;
    }
    return 0;
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
        bindpoint == GL_READ_FRAMEBUFFER)
        obj = bind(GL_READ_FRAMEBUFFER, id);

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
FramebufferObjectMap::oglReadBuffer(const trace::Call& call)
{
    auto fbo = boundTo(GL_READ_FRAMEBUFFER);
    assert(call.arg(0).toUInt() != GL_BACK || fbo->id() == 0);
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

}