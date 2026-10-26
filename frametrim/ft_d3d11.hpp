/*********************************************************************
 *
 * Copyright 2022 Collabora Ltd
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

enum eObjectType {
    ot_dxgi_object,
    ot_device,
    ot_swapchain,
    ot_device_child,
};

enum ePerDevice {
    pd_resources,
    pd_views,
    pd_class_linkages,
    pd_input_layouts,
    pd_shaders,
    pd_states,
    pd_asyncs,
};

enum ePerContextState {
    pc_blend,
    pc_depth_stencil,
    pc_rasterizer,
    pc_viewport,
    pc_scissor,
    pc_primitive_topology,
    STATE_FLAG_COUNT
};

enum eShaderStage {
    ss_vertex,
    ss_hull,
    ss_domain,
    ss_geometry,
    ss_pixel,
    ss_compute,
};

enum ePerContextBinding {
    pcb_shaders,
    pcb_samplers,
    pcb_input_layout,
    pcb_vertex_buffers,
    pcb_index_buffer,
    pcb_constant_buffers,
    pcb_shader_resources,
    pcb_render_targets,
    pcb_depth_stencil_view,
    pcb_unordered_access_views,
    pcb_stream_out_targets,
    BINDING_TYPE_COUNT
};

enum eResourceType {
    rt_buffer,
    rt_texture_1d,
    rt_texture_2d,
    rt_texture_3d,
};

class D3D11Impl;
class D3D11Device;

class Object;
using ImplPtr = std::shared_ptr<D3D11Impl>;
using DepSet = std::unordered_set<std::shared_ptr<Object>>;

struct string_less {
    bool operator () (const char *lhs, const char *rhs) const
    {
        return strcmp(lhs, rhs) < 0;
    }
};

static unsigned ifacelen (const char *start)
{
    const char *end = strstr(start, "::");

    if (!end)
        return strlen(start);

    while (end > start) {
        const char *prev = end - 1;
        if (!isdigit(*prev))
            return (end - start);
        end = prev;
    }

    return 0;
}

struct string_iface_less {
    bool operator () (const char *lhs, const char *rhs) const
    {
        int len = std::max(ifacelen(lhs), ifacelen(rhs));
        return strncmp(lhs, rhs, len) < 0;
    }
};

struct D3D11Box {
    D3D11Box(unsigned left, unsigned top, unsigned front, unsigned right, unsigned bottom, unsigned back);
    bool isEmpty() const;

    unsigned left;
    unsigned top;
    unsigned front;
    unsigned right;
    unsigned bottom;
    unsigned back;
};

struct D3D11Mapping {
    unsigned subres;
    unsigned long long start;
    unsigned long long end;
    unsigned range_min;
    unsigned range_max;
    std::vector<PTraceCall> calls;
    bool discard;

    D3D11Mapping(const trace::Call& call, unsigned subres, unsigned long long begin,
                 unsigned long long end, bool discard);
    void update(const trace::Call& call, unsigned long long begin, unsigned long long end);
    void finish(const trace::Call& call);
};

class Interface {
public:
    using Pointer = std::shared_ptr<Interface>;

    Interface(const char *name);
    Interface(const char *name, Pointer parent);
    void addCall(const char *name, ft_callback callback);

    ft_callback findCall(const char *name);

private:
    const char *m_name;
    Pointer m_parent;
    std::map<const char *, ft_callback, string_less> m_call_table;
};

class Object: public std::enable_shared_from_this<Object> {
public:
    using Pointer = std::shared_ptr<Object>;

    Object(ImplPtr impl, void *id);

    void *id() { return m_id; };
    bool isUnrolled() const { return m_unrolled; };

    PTraceCall getInitCall() const;
    void setInitCall(const trace::Call& call);
    void addCall(const trace::Call& call);
    void addCall(PTraceCall call);
    void clearCalls();
    bool hasDependency(Pointer dep);
    void addDependency(Pointer dep);
    void addDependencies(const trace::Array *deps);
    const DepSet& dependencies() const { return m_dependencies; };
    void emitInitCallsTo(CallSet& out_list);
    void emit(CallSet& out_list, bool parent_is_unrolled);
    bool emitted() const;
    void unroll();

    template <class T>
    std::shared_ptr<T> lookup(void* obj_id);

    template <class T>
    std::shared_ptr<T> get(const trace::Call &call, unsigned obj_id_param_id);

    template<class T, typename... Args>
    std::shared_ptr<T> create(const trace::Call& call, unsigned param_id, Args... args);

    template<class T, typename... Args>
    std::shared_ptr<T> getOrCreate(const trace::Call& call, const char *obj_id_param, Args... args);

    template<class T, typename... Args>
    std::shared_ptr<T> createWithDep(const trace::Call& call, unsigned obj_id_param,
                             unsigned dep_id_param, Args... args);

    template<class T, typename... Args>
    std::shared_ptr<T> fakeCreate(const trace::Call& call, Args... args);

    void AddRef(const trace::Call& call);
    void Release(const trace::Call& call);
    void QueryInterface(const trace::Call& call);

protected:
    void unrollInternal(CallSet& out_list, DepSet& dep_list);
    virtual void emitCalls(CallSet& out_list);
    virtual void emitDependencies(DepSet& dep_list);
    std::weak_ptr<D3D11Impl> m_impl;

private:
    void *m_id;
    unsigned m_refcount;

    PTraceCall m_init_call;
    CallSet m_calls;
    DepSet m_dependencies;
    bool m_emitted;
    bool m_emitting;
    bool m_unrolled;
};

class DXGIObject: public Object {
public:
    using Pointer = std::shared_ptr<DXGIObject>;

    DXGIObject(ImplPtr impl, void *id);

    void GetParent(const trace::Call &call);

protected:
    virtual void getParent(const trace::Call &call) = 0;
};

class DXGIDevice: public DXGIObject {
public:
    DXGIDevice(ImplPtr impl, void *id);

protected:
    virtual void getParent(const trace::Call &call);
};

class DXGIAdapter: public DXGIObject {
public:
    DXGIAdapter(ImplPtr impl, void *id);

protected:
    virtual void getParent(const trace::Call &call);
};

class DXGISwapChain: public DXGIObject {
public:
    using Pointer = std::shared_ptr<DXGISwapChain>;

    DXGISwapChain(ImplPtr impl, void *id, unsigned width, unsigned height);

    void ResizeBuffers(const trace::Call& call);
    void ResizeTarget(const trace::Call& call);
    void GetBuffer(const trace::Call& call);
    void Present(const trace::Call& call);

protected:
    virtual void getParent(const trace::Call &call);

private:
    unsigned m_width;
    unsigned m_height;
};

class DXGIFactory: public DXGIObject {
public:
    using Pointer = std::shared_ptr<DXGISwapChain>;

    DXGIFactory(ImplPtr impl, void *id);

    void CreateSwapChain(const trace::Call& call);
    void CreateSwapChainForHwnd(const trace::Call& call);
    void EnumAdapters(const trace::Call& call);

protected:
    virtual void getParent(const trace::Call &call);
};

class D3D11DeviceChild: public Object {
public:
    using Pointer = std::shared_ptr<D3D11DeviceChild>;

    D3D11DeviceChild(ImplPtr impl, void *id);
};

/* Wrap a call to set some state object */
class D3D11State {
public:
    D3D11State() {};
    D3D11State(const trace::Call& call);
    D3D11State(const trace::Call& call, D3D11DeviceChild::Pointer dep);
    D3D11State(const D3D11State& other);

    void addTo(Object::Pointer dst);
    D3D11DeviceChild::Pointer getDep() const;

private:
    PTraceCall m_call;
    D3D11DeviceChild::Pointer m_dep;
};

class D3D11DepthStencilState: public D3D11DeviceChild {
public:
    using Pointer = std::shared_ptr<D3D11DepthStencilState>;

    D3D11DepthStencilState(ImplPtr impl, void *id, bool write);

    bool isWriteEnabled() const { return m_write; };

private:
    bool m_write;
};

class States {
public:
    States() {};

    D3D11State& operator [](const unsigned& index) { return m_states[index]; };
    std::shared_ptr<D3D11DepthStencilState> getDepthStencilState() const;
    void set(unsigned flag, D3D11State&& state);
    void clear(PTraceCall call);
    void addTo(Object::Pointer dst);

private:
    std::unordered_map<unsigned, D3D11State> m_states;
    PTraceCall m_clearCall;
};

class D3D11Binding: public D3D11DeviceChild {
public:
    using Pointer = std::shared_ptr<D3D11Binding>;

    D3D11Binding(ImplPtr impl, void *id, D3D11DeviceChild::Pointer bound_obj);

    template<class T>
    std::shared_ptr<T> get();

private:
    D3D11DeviceChild::Pointer m_bound_obj;
};

struct Bindings {
    using ObjectBindings = std::unordered_map<unsigned, std::map<unsigned, D3D11Binding::Pointer>>;
    ObjectBindings& operator [](const int& index) { return m_bindings[index]; };

    void clear(const trace::Call& call, ePerContextBinding binding_type);
    void clearAll(PTraceCall call);
    PTraceCall getClearCall(ePerContextBinding binding_type);

    void merge(Bindings& other) {
        for (int i = 0; i < BINDING_TYPE_COUNT; i++)
            m_bindings[i].merge(other.m_bindings[i]);
    };

private:
    ObjectBindings m_bindings[BINDING_TYPE_COUNT];
    PTraceCall m_clearCalls[BINDING_TYPE_COUNT];
};

class D3D11Resource: public D3D11DeviceChild {
public:
    using Pointer = std::shared_ptr<D3D11Resource>;

    D3D11Resource(ImplPtr impl, void *id);

    void setUpdateFrame();
    void addUpdateCall(const trace::Call& call);
    void addUpdateDependency(Object::Pointer dep);
    virtual void update(const trace::Call& call, unsigned subres, D3D11Box *box, unsigned size);
    virtual void update(D3D11Mapping& mapping);
    virtual void clear();
    virtual unsigned getMappedSize(unsigned width_stride, unsigned depth_stride) = 0;
    virtual bool hasBindFlag(unsigned flag) { return false; };

protected:
    virtual void emitCalls(CallSet& out_list);
    virtual void emitDependencies(DepSet& dep_list);

private:
    uint32_t m_last_update_frame;
    std::vector<PTraceCall> m_update_calls;
    DepSet m_update_dependencies;
};

class D3D11Buffer: public D3D11Resource {
public:
    D3D11Buffer(ImplPtr impl, void *id, unsigned size);

    struct Update {
        unsigned begin;
        unsigned end;
        std::vector<PTraceCall> calls;
    };

    virtual void update(const trace::Call& call, unsigned subres, D3D11Box *box, unsigned size);
    virtual void update(D3D11Mapping& mapping);
    void updateBuffer(std::vector<PTraceCall> &calls, unsigned subres, unsigned begin, unsigned end);
    virtual void clear();
    virtual unsigned getMappedSize(unsigned width_stride, unsigned depth_stride);

protected:
    virtual void emitCalls(CallSet& out_list);

private:
    unsigned m_size;
    std::vector<Update> m_updates;
};

class D3D11Texture1D: public D3D11Resource {
public:
    D3D11Texture1D(ImplPtr impl, void *id);

    virtual unsigned getMappedSize(unsigned width_stride, unsigned depth_stride);
};

class D3D11Texture2D: public D3D11Resource {
public:
    D3D11Texture2D(ImplPtr impl, void *id, unsigned height, unsigned bind_flags);

    virtual unsigned getMappedSize(unsigned width_stride, unsigned depth_stride);
    virtual bool hasBindFlag(unsigned flag) { return m_bind_flags & flag; };

private:
    unsigned m_height;
    unsigned m_bind_flags;
};

class D3D11Texture3D: public D3D11Resource {
public:
    D3D11Texture3D(ImplPtr impl, void *id, unsigned depth);

    virtual unsigned getMappedSize(unsigned width_stride, unsigned depth_stride);

private:
    unsigned m_depth;
};

class D3D11View: public D3D11DeviceChild {
public:
    using Pointer = std::shared_ptr<D3D11View>;

    D3D11View(ImplPtr impl, void *id, D3D11Resource::Pointer res, bool entire);

    void addUpdateCall(const trace::Call& call);
    void addUpdateDependency(Object::Pointer dep);
    void clear(const trace::Call& call);
    bool hasBindFlag(unsigned flag) { return m_resource->hasBindFlag(flag); };

private:
    D3D11Resource::Pointer m_resource;
    bool m_entire;
};

class D3D11Operation: public D3D11DeviceChild {
public:
    using Pointer = std::shared_ptr<D3D11Operation>;

    D3D11Operation(ImplPtr impl, void *id);

    uint32_t frameNo() const { return m_frame_no; };
    virtual void execute(uint32_t frame_no) = 0;

protected:
    uint32_t m_frame_no;
};

class D3D11ResourceUpdate: public D3D11Operation {
public:
    using Pointer = std::shared_ptr<D3D11Operation>;

    D3D11ResourceUpdate(ImplPtr impl, void *id, D3D11Resource::Pointer res, D3D11Mapping& mapping);
    virtual void execute(uint32_t frame_no);

private:
    D3D11Resource::Pointer m_res;
    D3D11Mapping m_mapping;
};

using SubresourceId = std::tuple<void *, void *, unsigned>;
using MappingMap = std::map<SubresourceId, D3D11Mapping>;
typedef bool(*FilterFn)(unsigned, unsigned, D3D11DeviceChild::Pointer obj);

class D3D11Command: public D3D11Operation {
public:
    using Pointer = std::shared_ptr<D3D11Command>;

    D3D11Command(ImplPtr impl, void *id, bool deferred);

    virtual void link(States& states, Bindings& bindings) = 0;
    virtual void execute(uint32_t frame_no);

protected:
    void addStates(States& states);
    void addBoundAsDependency(Bindings& bindings, enum ePerContextBinding pcb, bool compute);
    void addToBound(Bindings& bindings, enum ePerContextBinding pcb);
    void addToBoundView(Bindings& bindings, enum ePerContextBinding pcb, bool compute, FilterFn filter);

private:
    bool m_deferred;
    std::vector<D3D11View::Pointer> m_delayed_targets;
};

class D3D11Draw: public D3D11Command {
public:
    using Pointer = std::shared_ptr<D3D11Draw>;

    D3D11Draw(ImplPtr impl, void *id, bool deferred);
    void link(States& states, Bindings& bindings) override;
};

class D3D11Dispatch: public D3D11Command {
public:
    using Pointer = std::shared_ptr<D3D11Draw>;

    D3D11Dispatch(ImplPtr impl, void *id, bool deferred);
    void link(States& states, Bindings& bindings) override;
};

using OpList = std::vector<D3D11Operation::Pointer>;

class D3D11CommandList: public D3D11DeviceChild {
public:
    D3D11CommandList(ImplPtr impl, void *id);

    const OpList& operations() const { return m_operations; };
    void addOperation(D3D11Operation::Pointer op);
    void execute();

private:
    OpList m_operations;
};

class D3D11Context: public D3D11DeviceChild {
public:
    using Pointer = std::shared_ptr<D3D11Context>;

    D3D11Context(ImplPtr impl, void *id, bool deferred = false);

    void SetState(const trace::Call& call, ePerContextState state_flag);
    void ClearView(const trace::Call& call);
    void ClearState(const trace::Call& call);

    void BindState(const trace::Call& call, ePerContextState state_flag);
    void SetShader(const trace::Call& call, unsigned bindpoint);
    void BindRenderTargets(const trace::Call& call);
    void BindRenderTargetsAndUAVS(const trace::Call& call);
    void GetBoundRenderTargets(const trace::Call& call);

    void FinishCommandList(const trace::Call& call);
    void ExecuteCommandList(const trace::Call& call);

    void Map(const trace::Call& call);
    void Unmap(const trace::Call& call);
    void UpdateSubresource(const trace::Call& call);
    void CopyResource(const trace::Call& call);
    void CopySubresourceRegion(const trace::Call& call);
    void CopyStructureCount(const trace::Call& call);
    void GenerateMips(const trace::Call& call);

    void Draw(const trace::Call& call);
    void DrawIndirect(const trace::Call& call);
    void Dispatch(const trace::Call& call);
    void DispatchIndirect(const trace::Call& call);

    void Begin(const trace::Call& call);
    void End(const trace::Call& call);

    void bindObject(const trace::Call& call, ePerContextBinding binding_type,
                    unsigned bindpoint, unsigned slot, void *bound_obj_id,
                    const trace::Array *deps);
    void bindObjects(const trace::Call& call, ePerContextBinding binding_type, unsigned bindpoint,
                     unsigned slot, unsigned ids_param);
    void setObjects(const trace::Call& call, ePerContextBinding binding_type, unsigned bindpoint,
                    unsigned slot, unsigned ids_param);
    void bindSlot(const trace::Call& call, ePerContextBinding binding_type, unsigned id_param);
    void bindSlots(const trace::Call& call, ePerContextBinding binding_type, unsigned bindpoint);

private:
    bool m_deferred;

    States m_states;
    Bindings m_bindings;
    OpList m_operations;

    MappingMap m_discard_mappings; /* for deferred contexts only */

    void addOperation(D3D11Operation::Pointer op);
    void addCommand(D3D11Command::Pointer op);
};

class D3D11Device: public Object  {
public:
    using Pointer = std::shared_ptr<D3D11Device>;
    using ObjectPtr = D3D11DeviceChild::Pointer;

    D3D11Device(ImplPtr impl, void *id);

    D3D11Context::Pointer m_context;
    // TODO deferred contexts

    void GetImmediateContext(const trace::Call& call);
    void CreateDeferredContext(const trace::Call& call);

    void CreateState(const trace::Call& call);
    void CreateDepthStencilState(const trace::Call& call);
    void CreateShader(const trace::Call& call);
    void CreateGeometryShaderWithStreamOutput(const trace::Call& call);
    void CreateBuffer(const trace::Call& call);
    void CreateTexture1D(const trace::Call& call);
    void CreateTexture2D(const trace::Call& call);
    void CreateTexture3D(const trace::Call& call);
    void CreateView(const trace::Call& call);
    void CreateAsync(const trace::Call& call);
    void CreateClassLinkage(const trace::Call& call);
    void CreateInputLayout(const trace::Call& call);
};

class D3D11Impl : public FrameTrimmer, public std::enable_shared_from_this<D3D11Impl> {
public:
    D3D11Impl(bool keep_all_states);

    bool isRecording() { return m_recording_frame; }
    void recordObject(Object::Pointer obj);
    void recordObjectInit(Object::Pointer obj);

    template<class T>
    std::shared_ptr<T> lookup(void* obj_id);

    template<class T>
    std::shared_ptr<T> get(const trace::Call& call, unsigned obj_id_param_id);

    template<class T, typename... Args>
    std::shared_ptr<T> create(const trace::Call& call, Object::Pointer parent,
                              unsigned obj_id_param_id, Args... args);

    template<class T, typename... Args>
    std::shared_ptr<T> createWithDep(const trace::Call& call, Object::Pointer parent,
                                     unsigned obj_id_param, unsigned dep_id_param,
                                     Args... args);

    template<class T, typename... Args>
    std::shared_ptr<T> getOrCreate(const trace::Call& call, Object::Pointer parent,
                                   unsigned obj_id_param_id, Args... args);

    template<class T, typename... Args>
    std::shared_ptr<T> fakeCreate(const trace::Call& call, Object::Pointer parent,
                                   Args... args);

    MappingMap::iterator addMapping(const trace::Call& call, SubresourceId id,
                                    unsigned long long start, unsigned long long end,
                                    bool discard);
    MappingMap::node_type removeMapping(const trace::Call& call, SubresourceId id);

protected:
    ft_callback findCallback(const char *name);
    void emitState();
    bool skipDeleteObj(const trace::Call& call);

private:
    void registerInterfaces();

    void CreateDXGIFactory(const trace::Call& call);
    void D3D11CreateDevice(const trace::Call& call);
    void D3D11CreateDeviceAndSwapChain(const trace::Call& call);
    void memcpy(const trace::Call& call);

    std::map<const char *, ft_callback, string_less> m_call_table;
    std::map<const char *, Interface::Pointer, string_iface_less> m_interfaces;
    std::unordered_map<void *, Object::Pointer> m_objects;

    std::map<SubresourceId, D3D11Mapping> m_buffer_mappings;
    std::map<unsigned, PTraceCall> m_state_calls;
    std::map<unsigned, PTraceCall> m_enables;
};

}
