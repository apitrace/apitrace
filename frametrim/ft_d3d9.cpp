/*
 * Copyright © 2024 Igalia S.L.
 * SPDX-License-Identifier: MIT
 */

#include "ft_d3d9.hpp"

#include <algorithm>
#include <d3d9.h>
#include <functional>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <unordered_set>

using std::bind;
using std::placeholders::_1;

namespace frametrim {

struct SetState {
    const char* method;
    uint32_t params_to_capture;
    int32_t resource_idx;
};

const int32_t NoResource = -1;

const std::vector<SetState> state_calls = {
    {"IDirect3DDevice9::SetClipStatus", 0, NoResource},
    {"IDirect3DDevice9::SetDepthStencilSurface", 0, 1},
    {"IDirect3DDevice9::SetFVF", 0, NoResource},
    {"IDirect3DDevice9::SetIndices", 0, 1},
    {"IDirect3DDevice9::SetMaterial", 0, NoResource},
    {"IDirect3DDevice9::SetNPatchMode", 0, NoResource},
    {"IDirect3DDevice9::SetPixelShader", 0, 1},
    {"IDirect3DDevice9::SetScissorRect", 0, NoResource},
    {"IDirect3DDevice9::SetSoftwareVertexProcessing", 0, NoResource},
    {"IDirect3DDevice9::SetVertexDeclaration", 0, 1},
    {"IDirect3DDevice9::SetVertexShader", 0, 1},
    {"IDirect3DDevice9::SetViewport", 0, NoResource},

    {"IDirect3DDevice9::MultiplyTransform", 1, NoResource},
    {"IDirect3DDevice9::LightEnable", 1, NoResource},
    {"IDirect3DDevice9::SetClipPlane", 1, NoResource},
    {"IDirect3DDevice9::SetCurrentTexturePalette", 1, NoResource},
    {"IDirect3DDevice9::SetGammaRamp", 1, NoResource},
    {"IDirect3DDevice9::SetLight", 1, NoResource},
    {"IDirect3DDevice9::SetPaletteEntries", 1, NoResource},
    {"IDirect3DDevice9::SetRenderState", 1, NoResource},
    {"IDirect3DDevice9::SetRenderTarget", 1, 2},
    {"IDirect3DDevice9::SetStreamSource", 1, 2},
    {"IDirect3DDevice9::SetStreamSourceFreq", 1, NoResource},
    {"IDirect3DDevice9::SetTexture", 1, 2},
    {"IDirect3DDevice9::SetTransform", 1, NoResource},

    {"IDirect3DDevice9::SetSamplerState", 2, NoResource},
    {"IDirect3DDevice9::SetTextureStageState", 2, NoResource},

    // EX

    {"IDirect3DDevice9Ex::SetClipStatus", 0, NoResource},
    {"IDirect3DDevice9Ex::SetDepthStencilSurface", 0, 1},
    {"IDirect3DDevice9Ex::SetFVF", 0, NoResource},
    {"IDirect3DDevice9Ex::SetIndices", 0, 1},
    {"IDirect3DDevice9Ex::SetMaterial", 0, NoResource},
    {"IDirect3DDevice9Ex::SetNPatchMode", 0, NoResource},
    {"IDirect3DDevice9Ex::SetPixelShader", 0, 1},
    {"IDirect3DDevice9Ex::SetScissorRect", 0, NoResource},
    {"IDirect3DDevice9Ex::SetSoftwareVertexProcessing", 0, NoResource},
    {"IDirect3DDevice9Ex::SetVertexDeclaration", 0, 1},
    {"IDirect3DDevice9Ex::SetVertexShader", 0, 1},
    {"IDirect3DDevice9Ex::SetViewport", 0, NoResource},

    {"IDirect3DDevice9Ex::MultiplyTransform", 1, NoResource},
    {"IDirect3DDevice9Ex::LightEnable", 1, NoResource},
    {"IDirect3DDevice9Ex::SetClipPlane", 1, NoResource},
    {"IDirect3DDevice9Ex::SetCurrentTexturePalette", 1, NoResource},
    {"IDirect3DDevice9Ex::SetGammaRamp", 1, NoResource},
    {"IDirect3DDevice9Ex::SetLight", 1, NoResource},
    {"IDirect3DDevice9Ex::SetPaletteEntries", 1, NoResource},
    {"IDirect3DDevice9Ex::SetRenderState", 1, NoResource},
    {"IDirect3DDevice9Ex::SetRenderTarget", 1, 2},
    {"IDirect3DDevice9Ex::SetStreamSource", 1, 2},
    {"IDirect3DDevice9Ex::SetStreamSourceFreq", 1, NoResource},
    {"IDirect3DDevice9Ex::SetTexture", 1, 2},
    {"IDirect3DDevice9Ex::SetTransform", 1, NoResource},

    {"IDirect3DDevice9Ex::SetSamplerState", 2, NoResource},
    {"IDirect3DDevice9Ex::SetTextureStageState", 2, NoResource},
};

D3D9Impl::D3D9Impl(bool keep_all_states, bool swaps_to_finish)
    : FrameTrimmer(keep_all_states, swaps_to_finish)
{
    registerCalls();
}

void D3D9Impl::switch_thread(int new_thread)
{
}

class PointerReplacer : public trace::Visitor {
  public:
    PointerReplacer(std::unordered_map<uintptr_t, RemapPointerInfo>& pointer_map, unsigned callno)
        : m_callno(callno), m_pointer_map(pointer_map) {};

    void visit(trace::Null*) override
    {
    }
    void visit(trace::Bool*) override
    {
    }
    void visit(trace::SInt*) override
    {
    }
    void visit(trace::UInt*) override
    {
    }
    void visit(trace::Float*) override
    {
    }
    void visit(trace::Double*) override
    {
    }
    void visit(trace::String*) override
    {
    }
    void visit(trace::WString*) override
    {
    }
    void visit(trace::Enum*) override
    {
    }
    void visit(trace::Bitmask*) override
    {
    }
    void visit(trace::Struct* s) override
    {
        for (auto memberValue : s->members) {
            _visit(memberValue);
        }
    }
    void visit(trace::Array* array) override
    {
        for (auto& value : array->values) {
            _visit(value);
        }
    }
    void visit(trace::Blob*) override
    {
    }
    void visit(trace::Pointer* pointer) override
    {
        if (!pointer->value)
            return;

        auto it = m_pointer_map.find(pointer->value);
        if (it == m_pointer_map.end())
            return;

        auto& info = it->second;
        if (info.first_call > m_callno)
            return;

        pointer->value = info.ptr;
    }
    void visit(trace::Repr*) override
    {
    }

  private:
    unsigned m_callno;
    std::unordered_map<uintptr_t, RemapPointerInfo>& m_pointer_map;
};

void D3D9Impl::modify_last_frame_call(trace::Call& call)
{
    if (m_pointer_map.empty())
        return;

    PointerReplacer replacer(m_pointer_map, call.no);
    for (auto& arg : call.args) {
        if (arg.value)
            arg.value->visit(replacer);
    }
    if (call.ret)
        call.ret->visit(replacer);
}

void D3D9Impl::emitState()
{
    assert(m_recording_frame);

    auto addResource = [&](uintptr_t resource) {
        if (resource != 0) {
            auto it = m_active_objects.find(resource);

            if (it != m_active_objects.end()) {
                assert(it != m_active_objects.end());
                it->second->m_used_in_recording_frame = true;
            }
            else {
                std::cerr
                    << "Resource " << std::hex << resource << std::dec
                    << " was set in a state but was deleted without being removed from the state."
                    << std::endl;
            }
        }
    };

    /* Do not drop resources which are used in frame setup */
    for (auto&& [name, call] : m_state_calls) {
        addResource(call.resource_used);
    }
    for (auto&& [addr, d3dobj] : m_active_objects) {
        for (const auto& [key, state] : d3dobj->m_bundleState) {
            addResource(state.resource_used);
        }
    }

    for (auto&& [name, call] : m_state_calls)
        m_required_calls.insert(call.call);

    for (auto& stage : m_shader_constants) {
        for (auto& c : stage) {
            m_required_calls.insert(c);
        }
    }
}

void D3D9Impl::finalize()
{
    /* Only add calls for objects that are actually used */
    for (auto&& [addr, d3dobj] : m_active_objects) {
        if (d3dobj->m_used_in_recording_frame) {
            d3dobj->addToRequiredCalls(m_required_calls);
        }

        for (auto& call : d3dobj->m_overriden_calls) {
            m_overriden_calls.insert(std::make_pair(call->no, call));
        }
    }

    for (auto& d3dobj : m_released_objects) {
        for (auto& call : d3dobj->m_overriden_calls) {
            m_overriden_calls.insert(std::make_pair(call->no, call));
        }
    }
}

int strendswith(const char* str, const char* suffix)
{
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (suffix_len > str_len)
        return 1;
    return strcmp(str + str_len - suffix_len, suffix);
}

ft_callback D3D9Impl::findCallback(const char* name)
{
    auto it = m_call_table.find(name);
    if (it != m_call_table.end()) {
        return it->second;
    }

    if (strstr(name, "::Release") != nullptr) {
        return bind(&D3D9Impl::recordResourceRelease, this, _1);
    }
    if (strstr(name, "::AddRef") != nullptr) {
        return bind(&D3D9Impl::recordResourceRef, this, _1, 0);
    }
    if (strendswith(name, "::GetDevice") == 0) {
        return bind(&D3D9Impl::recordResourceRef, this, _1, 1);
    }
    if (strendswith(name, "::QueryInterface") == 0) {
        return bind(&D3D9Impl::recordResourceRef, this, _1, 2);
    }

    return nullptr;
}

bool D3D9Impl::skipDeleteObj(const trace::Call& call)
{
    /* Doesn't really skip calls in recording frame? */
    return false;
}

#define MAP(name, call) m_call_table.insert(std::make_pair(#name, bind(&D3D9Impl::call, this, _1)))

#define MAP_V(name, call, param1)                                                                  \
    m_call_table.insert(std::make_pair(#name, bind(&D3D9Impl::call, this, _1, param1)))

#define MAP_VV(name, call, param1, param2)                                                         \
    m_call_table.insert(std::make_pair(#name, bind(&D3D9Impl::call, this, _1, param1, param2)))

#define MAP_VVV(name, call, param1, param2, param3)                                                \
    m_call_table.insert(                                                                           \
        std::make_pair(#name, bind(&D3D9Impl::call, this, _1, param1, param2, param3)))

#define MAP_VVVV(name, call, param1, param2, param3, param4)                                       \
    m_call_table.insert(                                                                           \
        std::make_pair(#name, bind(&D3D9Impl::call, this, _1, param1, param2, param3, param4)))

#define MAP_EX(ifc, method, call)                                                                  \
    MAP(ifc::method, call);                                                                        \
    MAP(ifc##Ex::method, call)

#define MAP_EX_V(ifc, method, call, param1)                                                        \
    MAP_V(ifc::method, call, param1);                                                              \
    MAP_V(ifc##Ex::method, call, param1)

#define MAP_EX_VV(ifc, method, call, param1, param2)                                               \
    MAP_VV(ifc::method, call, param1, param2);                                                     \
    MAP_VV(ifc##Ex::method, call, param1, param2)

void D3D9Impl::registerCalls()
{
    for (auto& state : state_calls)
        m_call_table.insert(
            std::make_pair(state.method, bind(&D3D9Impl::recordStateCall, this, _1,
                                              state.params_to_capture, state.resource_idx)));

    MAP_EX(IDirect3DDevice9, BeginStateBlock, recordBeginStateBlock);
    MAP_EX(IDirect3DDevice9, EndStateBlock, recordEndStateBlock);

    MAP(IDirect3DStateBlock9::Apply, recordStateBlockApply);
    MAP(IDirect3DStateBlock9::Capture, recordStateBlockCapture);

    MAP_EX_V(IDirect3DDevice9, SetVertexShaderConstantF, recordShaderSetConstant, ss_vertex);
    MAP_EX_V(IDirect3DDevice9, SetVertexShaderConstantI, recordShaderSetConstant, ss_vertex);
    MAP_EX_V(IDirect3DDevice9, SetVertexShaderConstantB, recordShaderSetConstant, ss_vertex);
    MAP_EX_V(IDirect3DDevice9, SetPixelShaderConstantF, recordShaderSetConstant, ss_pixel);
    MAP_EX_V(IDirect3DDevice9, SetPixelShaderConstantI, recordShaderSetConstant, ss_pixel);
    MAP_EX_V(IDirect3DDevice9, SetPixelShaderConstantB, recordShaderSetConstant, ss_pixel);

    MAP(memcpy, recordMemcpy);
    MAP_EX(IDirect3DDevice9, Reset, recordReset);
    MAP(IDirect3DDevice9Ex::ResetEx, recordReset);
    MAP_EX_VV(IDirect3DDevice9, UpdateSurface, recordResourceDep, 1, 3);
    MAP_EX_VV(IDirect3DDevice9, StretchRect, recordResourceDep, 1, 3);
    MAP_EX_VV(IDirect3DDevice9, UpdateTexture, recordResourceDep, 1, 2);

    MAP_EX_V(IDirect3DDevice9, GetPixelShader, recordResourceRef, 1);
    MAP_EX_V(IDirect3DDevice9, GetVertexShader, recordResourceRef, 1);
    MAP_EX_V(IDirect3DDevice9, GetVertexDeclaration, recordResourceRef, 1);
    MAP_EX_V(IDirect3DDevice9, GetIndices, recordResourceRef, 1);
    MAP_EX_V(IDirect3DDevice9, GetTexture, recordResourceRef, 2);
    MAP_EX_V(IDirect3DDevice9, GetStreamSource, recordResourceRef, 2);

    MAP_VV(Direct3DCreate9, recordResourceCreation, -1, false);
    MAP_VV(Direct3DCreate9Ex, recordResourceCreation, 1, false);
    MAP_EX_VV(IDirect3D9, CreateDevice, recordResourceCreation, 6, false);
    MAP_EX_VV(IDirect3DDevice9, GetDirect3D, recordResourceCreation, 1, false);

    MAP_EX_VV(IDirect3DDevice9, CreateVertexDeclaration, recordResourceCreation, 2, true);
    MAP_EX_VV(IDirect3DDevice9, CreateVertexBuffer, recordResourceCreation, 5, true);
    MAP_EX_VV(IDirect3DDevice9, CreateIndexBuffer, recordResourceCreation, 5, true);
    MAP_EX_VV(IDirect3DDevice9, CreatePixelShader, recordResourceCreation, 2, true);
    MAP_EX_VV(IDirect3DDevice9, CreateVertexShader, recordResourceCreation, 2, true);
    MAP_EX_VV(IDirect3DDevice9, CreateTexture, recordResourceCreation, 7, true);
    MAP_EX_VV(IDirect3DDevice9, CreateCubeTexture, recordResourceCreation, 6, true);
    MAP_EX_VV(IDirect3DDevice9, CreateVolumeTexture, recordResourceCreation, 8, true);
    MAP_EX_VV(IDirect3DDevice9, CreateOffscreenPlainSurface, recordResourceCreation, 5, true);
    MAP_EX_VV(IDirect3DDevice9, CreateRenderTarget, recordResourceCreation, 7, true);
    MAP_EX_VV(IDirect3DDevice9, CreateDepthStencilSurface, recordResourceCreation, 7, true);
    MAP_EX_VV(IDirect3DDevice9, CreateAdditionalSwapChain, recordResourceCreation, 2, true);
    MAP_EX_VV(IDirect3DDevice9, CreateQuery, recordResourceCreation, 2, true);
    MAP_VV(IDirect3DDevice9Ex::CreateRenderTargetEx, recordResourceCreation, 7, true);
    MAP_VV(IDirect3DDevice9Ex::CreateOffscreenPlainSurfaceEx, recordResourceCreation, 7, true);
    MAP_VV(IDirect3DDevice9Ex::CreateDepthStencilSurfaceEx, recordResourceCreation, 7, true);

    MAP_EX_V(IDirect3DDevice9, GetBackBuffer, recordResourceInternalCreateOrGet, 4);
    MAP_EX_V(IDirect3DDevice9, GetRenderTarget, recordResourceInternalCreateOrGet, 2);
    MAP_EX_V(IDirect3DDevice9, GetDepthStencilSurface, recordResourceInternalCreateOrGet, 1);
    MAP_EX_V(IDirect3DDevice9, GetSwapChain, recordResourceInternalCreateOrGet, 2);

    MAP_EX_VV(IDirect3DSwapChain9, GetBackBuffer, recordResourceCreation, 3, true);
    MAP_EX(IDirect3DSwapChain9, Present, recordPresent);

    MAP_VV(IDirect3DQuery9::Issue, recordResourceUsage, 0, eCallType::unknown);

    MAP_VVV(IDirect3DTexture9::GetSurfaceLevel, recordResourceSubresource, 0, 1, 2);
    MAP_VVV(IDirect3DCubeTexture9::GetCubeMapSurface, recordResourceSubresource, 0, 2, 3);
    MAP_VVV(IDirect3DVolumeTexture9::GetVolumeLevel, recordResourceSubresource, 0, 1, 2);

    MAP_VVVV(IDirect3DIndexBuffer9::Lock, recordResourceLock, 0, -1, -1, 3);
    MAP_VVV(IDirect3DIndexBuffer9::Unlock, recordResourceUnlock, 0, -1, -1);
    MAP_VVVV(IDirect3DVertexBuffer9::Lock, recordResourceLock, 0, -1, -1, 3);
    MAP_VVV(IDirect3DVertexBuffer9::Unlock, recordResourceUnlock, 0, -1, -1);

    MAP_VVVV(IDirect3DTexture9::LockRect, recordResourceLock, 0, 1, -1, 2);
    MAP_VVV(IDirect3DTexture9::UnlockRect, recordResourceUnlock, 0, 1, -1);
    MAP_VVVV(IDirect3DCubeTexture9::LockRect, recordResourceLock, 0, 2, 1, 3);
    MAP_VVV(IDirect3DCubeTexture9::UnlockRect, recordResourceUnlock, 0, 2, 1);
    MAP_VVVV(IDirect3DVolume9::LockBox, recordResourceLock, 0, -1, -1, 1);
    MAP_VVV(IDirect3DVolume9::UnlockBox, recordResourceUnlock, 0, -1, -1);
    MAP_VVVV(IDirect3DVolumeTexture9::LockBox, recordResourceLock, 0, 1, -1, 2);
    MAP_VVV(IDirect3DVolumeTexture9::UnlockBox, recordResourceUnlock, 0, 1, -1);
    MAP_VVVV(IDirect3DSurface9::LockRect, recordResourceLock, 0, -1, -1, 1);
    MAP_VVV(IDirect3DSurface9::UnlockRect, recordResourceUnlock, 0, -1, -1);
}

void D3D9Impl::recordBeginStateBlock(const trace::Call& call)
{
    /* E.g. Command and Conquer 3 does this */
    if (m_currentStateBlock)
        return;

    m_currentStateBlock = std::make_unique<D3D9Object>();
    m_currentStateBlock->m_calls.push_back(trace2call(call));
}

void D3D9Impl::recordEndStateBlock(const trace::Call& call)
{
    if (!m_currentStateBlock)
        return;

    /* We get block's pointer only at ::EndStateBlock */

    auto begin_block_call = m_currentStateBlock->m_calls[0];
    m_currentStateBlock->create(call, m_recording_frame, true);
    m_currentStateBlock->m_calls.push_back(begin_block_call);

    auto block_ptr = call.arg(1).toArray()->values[0]->toUIntPtr();

    remapPointer(m_currentStateBlock.get(), block_ptr, call.no);

    m_active_objects.insert_or_assign(block_ptr,
                                      std::shared_ptr<D3D9Object>(m_currentStateBlock.release()));
    m_currentStateBlock.reset();
}

void D3D9Impl::recordStateBlockApply(const trace::Call& call)
{
    /* TODO: Handle ShaderConstant calls */
    auto block_ptr = call.arg(0).toUIntPtr();
    auto& d3dobj = m_active_objects[block_ptr];
    for (const auto& [key, value] : d3dobj->m_bundleState) {
        m_state_calls[key] = value;
    }

    d3dobj->access(call, m_recording_frame);
}

void D3D9Impl::recordStateBlockCapture(const trace::Call& call)
{
    auto block_ptr = call.arg(0).toUIntPtr();
    auto& d3dobj = m_active_objects[block_ptr];

    d3dobj->m_calls.push_back(trace2call(call));

    for (const auto& [key, value] : d3dobj->m_bundleState) {
        const auto&& it = m_state_calls.find(key);
        if (it != m_state_calls.end()) {
            (d3dobj->m_bundleState)[key] = it->second;
            d3dobj->m_calls.push_back(value.call);
        }
    }

    d3dobj->access(call, m_recording_frame);

    if (d3dobj->m_state_block_has_set_constant && !m_recording_frame) {
        std::cerr
            << call.no << " " << call.name()
            << " captures shader constants outside of recording frame, this may lead to issues!"
            << std::endl;
    }
}

void D3D9Impl::recordReset(const trace::Call& call)
{
    std::cerr << call.no << " " << call.name() << " may not be perfectly handled." << std::endl;
    m_currentStateBlock.reset();
    m_state_calls.clear();
    this->recordRequiredCall(call);
}

static const unsigned long long getResourcePtrFromCall(const trace::Call& call, int32_t param_idx)
{
    if (param_idx == -1)
        return call.ret->toUIntPtr();

    if (!call.args[param_idx].value)
        return 0;

    auto& arg = call.arg(param_idx);
    if (arg.toArray() == nullptr)
        return call.arg(param_idx).toUIntPtr();
    else
        return call.arg(param_idx).toArray()->values[0]->toUIntPtr();
}

void D3D9Impl::recordStateCall(const trace::Call& call, uint32_t params_to_capture,
                               int32_t resource_idx)
{
    uintptr_t resource_ptr = 0;
    if (resource_idx != NoResource) {
        resource_ptr = getResourcePtrFromCall(call, resource_idx);
    }

    if (m_recording_frame && resource_ptr > 0) {
        auto it = m_active_objects.find(resource_ptr);
        if (it != m_active_objects.end()) {
            it->second->access(call, true);
        }

        return;
    }

    std::array<uint32_t, 4> key = {0, 0, 0, 0};
    key[0] = call.sig->id;
    for (unsigned i = 0; i < params_to_capture; ++i)
        key[i + 1] = call.arg(i + 1).toUInt();

    auto c = std::make_shared<TraceCall>(call, call.name());

    if (m_currentStateBlock) {
        (m_currentStateBlock->m_bundleState)[key] = {c, resource_ptr};
        return;
    }

    m_state_calls[key] = {c, resource_ptr};
}

void D3D9Impl::recordResourceCreation(const trace::Call& call, int32_t resource_idx, bool removable)
{
    auto resource_ptr = getResourcePtrFromCall(call, resource_idx);
    if (resource_ptr == 0)
        return;

    m_active_objects[resource_ptr] = std::make_shared<D3D9Object>();
    auto& d3dobj = m_active_objects[resource_ptr];
    d3dobj->create(call, m_recording_frame, removable);

    if (!d3dobj->m_calls_removable && !m_recording_frame)
        this->recordRequiredCall(call);

    remapPointer(d3dobj.get(), resource_ptr, call.no);
}

void D3D9Impl::recordResourceSubresource(const trace::Call& call, int32_t parent_idx,
                                         int32_t level_idx, int32_t subresource_idx)
{
    auto parent_ptr = getResourcePtrFromCall(call, parent_idx);
    auto subresource_ptr = getResourcePtrFromCall(call, subresource_idx);
    uint32_t level = call.arg(level_idx).toUInt();

    if (subresource_ptr == 0)
        return;

    auto& parent = m_active_objects[parent_ptr];

    auto subresource_parent = SubResourceParent{parent, level};

    auto sub_it = m_active_objects.find(subresource_ptr);
    bool old_subresource =
        sub_it != m_active_objects.end() &&
        sub_it->second->m_subresource_parent.parent == subresource_parent.parent &&
        sub_it->second->m_subresource_parent.level == subresource_parent.level;
    if (!old_subresource) {
        if (sub_it != m_active_objects.end()) {
            m_released_objects.push_back(sub_it->second);
            m_active_objects.erase(sub_it);
        }
        recordResourceCreation(call, subresource_idx, parent->m_calls_removable);
    }

    auto& subresource = m_active_objects[subresource_ptr];
    subresource->m_subresource_parent = subresource_parent;

    parent->m_dependencies.push_back(subresource);
    subresource->m_dependencies.push_back(parent);

    subresource->reference(call, m_recording_frame);
}

void D3D9Impl::recordResourceInternalCreateOrGet(const trace::Call& call, int32_t resource_idx)
{
    auto resource_ptr = getResourcePtrFromCall(call, resource_idx);
    if (resource_ptr == 0)
        return;

    auto it = m_active_objects.find(resource_ptr);
    if (it != m_active_objects.end()) {
        it->second->reference(call, m_recording_frame);
    }
    else {
        recordResourceCreation(call, resource_idx, false);
    }
}

void D3D9Impl::recordResourceRef(const trace::Call& call, int32_t resource_idx)
{
    auto resource_ptr = getResourcePtrFromCall(call, resource_idx);
    if (resource_ptr == 0)
        return;

    assert(m_active_objects.find(resource_ptr) != m_active_objects.end());

    auto it = m_active_objects.find(resource_ptr);
    if (it == m_active_objects.end()) {
        std::cerr << call.no << " " << call.name() << " references unknown object " << std::endl;
        this->recordRequiredCall(call);
        return;
    }

    auto& d3dobj = it->second;
    d3dobj->reference(call, m_recording_frame);
}

void D3D9Impl::recordResourceRelease(const trace::Call& call)
{
    auto resource_ptr = call.arg(0).toUIntPtr();

    auto it = m_active_objects.find(resource_ptr);
    if (it == m_active_objects.end()) {
        std::cerr << call.no << " " << call.name() << " releases unknown object " << std::endl;
        this->recordRequiredCall(call);
        return;
    }

    uint32_t references_left = call.ret->toUInt();

    auto& d3dobj = it->second;
    d3dobj->release(call, references_left, m_recording_frame);

    if (references_left == 0) {
        m_released_objects.push_back(d3dobj);
        m_active_objects.erase(it);

        if (!d3dobj->m_calls_removable) {
            m_required_calls.insert(trace2call(call));
        }
    }
}

void D3D9Impl::recordResourceUsage(const trace::Call& call, int32_t resource_idx, eCallType type)
{
    auto resource_ptr = getResourcePtrFromCall(call, resource_idx);
    if (resource_ptr == 0)
        return;

    auto it = m_active_objects.find(resource_ptr);
    if (it == m_active_objects.end()) {
        std::cerr << call.no << " " << call.name() << " uses unknown object " << std::endl;
        this->recordRequiredCall(call);
        return;
    }

    auto& d3dobj = it->second;
    d3dobj->access(call, m_recording_frame, type);

    if (!d3dobj->m_calls_removable && !m_recording_frame)
        this->recordRequiredCall(call);
}

void D3D9Impl::recordResourceLock(const trace::Call& call, int32_t resource_idx, int32_t level_idx,
                                  int32_t face_idx, int32_t mem_map_idx)
{
    auto resource_ptr = getResourcePtrFromCall(call, resource_idx);

    /* It's invalid to map different parts of the same level+face,
     * but different level+face are ok to map simultaneously,
     * so we have to track what exactly is locked.
     */
    uint32_t level = 0;
    uint32_t face = 0;
    if (level_idx != -1)
        level = call.arg(level_idx).toUInt();
    if (face_idx != -1)
        face = call.arg(face_idx).toUInt();
    auto& d3dobj = m_active_objects[resource_ptr];

    uint64_t map = 0;

    auto mem_map_arg = call.arg(mem_map_idx).toArray()->values[0];
    auto struct_arg = mem_map_arg->toStruct();
    if (struct_arg) {
        map = struct_arg->members.back()->toUIntPtr();
    }
    else {
        map = mem_map_arg->toUIntPtr();
    }

    if (strncmp("IDirect3DIndex", call.name(), strlen("IDirect3DIndex")) == 0 ||
        strncmp("IDirect3DVertex", call.name(), strlen("IDirect3DVertex")) == 0) {
        uint32_t flags = call.arg(call.args.size() - 1).toUInt();
        /* For vertex and index buffers, the entire buffer will be discarded,
         * so we could just remove all preceding calls writing into the memory.
         * This should eliminate a LOT of dead weight from the trace.
         */
        if (flags & D3DLOCK_DISCARD) {
            d3dobj->m_calls.erase(std::remove_if(d3dobj->m_calls.begin(), d3dobj->m_calls.end(),
                                                 [](const D3D9Call& call) {
                                                     return call.type == eCallType::mem_access;
                                                 }),
                                  d3dobj->m_calls.end());
        }
    }

    m_mem_mapping.mapObject(map, {d3dobj, level, face});

    recordResourceUsage(call, resource_idx, eCallType::mem_access);
}

void D3D9Impl::recordResourceUnlock(const trace::Call& call, int32_t resource_idx,
                                    int32_t level_idx, int32_t face_idx)
{
    auto resource_ptr = getResourcePtrFromCall(call, resource_idx);
    uint32_t level = 0;
    uint32_t face = 0;
    if (level_idx != -1)
        level = call.arg(level_idx).toUInt();
    if (face_idx != -1)
        face = call.arg(face_idx).toUInt();
    auto& d3dobj = m_active_objects[resource_ptr];
    m_mem_mapping.removeMapping({d3dobj, level, face});

    recordResourceUsage(call, resource_idx, eCallType::mem_access);
}

void D3D9Impl::recordMemcpy(const trace::Call& call)
{
    auto map = call.arg(0).toUIntPtr();
    auto obj = m_mem_mapping.findObject(map);
    assert(obj);
    obj->object->access(call, m_recording_frame, eCallType::mem_access);
}

void D3D9Impl::recordResourceDep(const trace::Call& call, int32_t resource_src_idx,
                                 int32_t resource_dst_idx)
{
    auto src_ptr = getResourcePtrFromCall(call, resource_src_idx);
    auto dst_ptr = getResourcePtrFromCall(call, resource_dst_idx);

    auto& src = m_active_objects[src_ptr];
    auto& dst = m_active_objects[dst_ptr];

    dst->m_dependencies.push_back(src);

    dst->access(call, m_recording_frame);
}

void D3D9Impl::recordPresent(const trace::Call& call)
{
    if (!m_recording_frame)
        return;
    recordResourceUsage(call, 0);
}

void D3D9Impl::remapPointer(D3D9Object* obj, uintptr_t resource_ptr, unsigned callno)
{
    /* In recording frame we remove ::Release of old objects for them to be present
     * during frame looping. However new objects may use the same address as old object,
     * so we have to change addresses for new objects.
     */
    if (m_recording_frame && !obj->m_created_before_recording_frame) {
        auto it = m_pointer_map.find(resource_ptr);
        if (it == m_pointer_map.end()) {
            auto remapped_ptr = m_new_pointers++;
            m_pointer_map[resource_ptr] = RemapPointerInfo{remapped_ptr, callno};
        }
    }
}

void D3D9Impl::updateCallTable(const std::vector<const char*>& names, ft_callback cb)
{
    for (auto& i : names)
        m_call_table.insert(std::make_pair(i, cb));
}

void D3D9Impl::recordShaderSetConstant(const trace::Call& call, eShaderStage stage)
{
    if (m_currentStateBlock) {
        m_currentStateBlock->m_calls.push_back(trace2call(call));
        m_currentStateBlock->m_state_block_has_set_constant = true;
    }
    else {
        /* Constants are unlikely to be preserved across frames,
         * but could be the majority of calls when targetting
         * far enough frame.
         */
        // m_required_calls.insert(trace2call(call));
        auto pcall = trace2call(call);
        auto start_reg = call.arg(1).toUInt();
        auto count = call.arg(3).toUInt();
        for (unsigned i = start_reg; i < (start_reg + count); i++) {
            m_shader_constants[stage][i] = pcall;
        }
    }
}

void D3D9Impl::recordRequiredCall(const trace::Call& call)
{
    m_required_calls.insert(trace2call(call));
}

void D3D9Object::reference(const trace::Call& call, bool recording_frame)
{
    m_used_in_recording_frame |= recording_frame;
    m_explicit_refs++;
}

void D3D9Object::create(const trace::Call& call, bool recording_frame, bool calls_removable)
{
    if (m_created_before_recording_frame && m_used_in_recording_frame) {
        std::cerr << call.no << " " << call.name()
                  << " creates object with the same pointer as another object which is not freed "
                     "in recording frame! "
                  << std::endl;
    }

    m_calls_removable = calls_removable;
    m_created_before_recording_frame = !recording_frame;

    access(call, recording_frame);
}

void D3D9Object::access(const trace::Call& call, bool recording_frame, eCallType type)
{
    if (!recording_frame)
        m_calls.push_back(D3D9Call(trace2call(call), type));
    m_used_in_recording_frame |= recording_frame;
}

void D3D9Object::release(const trace::Call& call, int references_left, bool recording_frame)
{
    m_used_in_recording_frame |= recording_frame;
    m_explicit_refs--;

    if (!recording_frame) {
        /* We track explicit references to drop matching releases,
         * the last ::Release isn't counted and "hidden" references are ignored.
         */
        if (m_explicit_refs < 0) {
            m_calls.push_back(trace2call(call));
        }
        return;
    }

    if (m_created_before_recording_frame) {
        /* Recording frame should not release resources created in the setup. */
        static const char* args[] = {"col", "wszName"};
        static const trace::FunctionSig sig = {10, "D3DPERF_SetMarker", 2, args};
        auto new_call = new trace::Call(&sig, 0, call.thread_id);
        new_call->args[0].value = new trace::UInt(0);
        char* str = new char[strlen(call.sig->name) + 1];
        strcpy(str, call.sig->name);
        new_call->args[1].value = new trace::String(str);
        new_call->no = call.no;

        m_overriden_calls.emplace_back(new_call);
    }
}

void D3D9Object::addToRequiredCalls(CallSet& required_calls)
{
    if (m_emitted)
        return;
    m_emitted = true;

    for (auto& d3d9call : m_calls)
        required_calls.insert(d3d9call.call);
    for (auto& dep : m_dependencies)
        dep->addToRequiredCalls(required_calls);
}

} // namespace frametrim