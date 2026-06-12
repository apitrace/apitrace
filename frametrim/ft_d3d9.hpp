/*
 * Copyright © 2024 Igalia S.L.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "ft_frametrimmer.hpp"
#include <array>
#include <optional>

namespace frametrim {

template <typename T>
struct MappedObj {
    T object;
    uint32_t level;
    uint32_t face;

    bool operator==(const MappedObj<T>& rhs) const
    {
        return (object == rhs.object) && (level == rhs.level) && (face == rhs.face);
    }
};

/* Mapped addresses are continious chunks of memory and we expect
 * all memcpy's to access existing mapped memory. With that, we could
 * just store the start address of a mapping, and map given address
 * back to object by finding the closest less-equal mapping.
 */
template <typename T>
class MemoryMapper {
  private:
    struct MappingInfo {
        MappedObj<T> obj;
        uint32_t refs;
    };
    std::map<uintptr_t, MappingInfo> mappings;

  public:
    void mapObject(uintptr_t start, const MappedObj<T>& obj)
    {
        auto it = mappings.find(start);
        if (it != mappings.end())
            it->second.refs++;
        else
            mappings[start] = {obj, 1};
    }

    MappedObj<T>* findObject(uintptr_t address)
    {
        auto it = mappings.upper_bound(address);
        if (it == mappings.begin()) {
            return nullptr;
        }
        --it;
        return &it->second.obj;
    }

    bool removeMapping(const MappedObj<T>& obj)
    {
        for (auto it = mappings.rbegin(); it != mappings.rend(); ++it) {
            if (it->second.obj == obj) {
                if (--it->second.refs == 0)
                    mappings.erase(std::next(it).base());
                return true;
            }
        }

        return false;
    }
};

struct ArrayHasher {
    std::size_t operator()(const std::array<uint32_t, 4>& a) const
    {
        std::size_t h = 0;

        for (auto e : a) {
            h ^= std::hash<int>{}(e) + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
    }
};

enum eShaderStage {
    ss_vertex = 0,
    ss_pixel = 1,
};

struct StateCall {
    PTraceCall call;
    uintptr_t resource_used;
};

using StateMap = std::unordered_map<std::array<uint32_t, 4>, StateCall, ArrayHasher>;

class D3D9Object;
struct SubResourceParent {
    std::shared_ptr<D3D9Object> parent;
    uint32_t level;
};

enum class eCallType {
    unknown,
    mem_access,
};

struct D3D9Call {
    eCallType type;
    PTraceCall call;

    D3D9Call(PTraceCall call, eCallType type = eCallType::unknown) : type(type), call(call)
    {
    }
};

class D3D9Object {
  public:
    void reference(const trace::Call& call, bool recording_frame);
    void create(const trace::Call& call, bool recording_frame, bool calls_removable);
    void access(const trace::Call& call, bool recording_frame, eCallType type = eCallType::unknown);
    void release(const trace::Call& call, int references_left, bool recording_frame);

    void addToRequiredCalls(CallSet& required_calls);

    int32_t m_explicit_refs;

    /* While we track most of created resource, we don't track
     * direc3d object or devices, it's much easier to never remove them.
     */
    bool m_calls_removable{false};
    /* All calls that use this object. */
    std::vector<D3D9Call> m_calls;
    /* These calls will replace the corresponding ones in rendering frame. */
    std::vector<trace::Call*> m_overriden_calls;

    bool m_used_in_recording_frame{false};
    bool m_created_before_recording_frame{false};
    uintptr_t m_new_pointer;

    /* Subresources may be created from a parent resource, or
     * one resource could be copied into another. In such cases
     * both objects would have a dependency on each other and
     * one won't be eliminated without the other.
     */
    std::vector<std::shared_ptr<D3D9Object>> m_dependencies;
    SubResourceParent m_subresource_parent;
    /* Break circular dependencies. */
    bool m_emitted;

    /* Captured state of IDirect3DStateBlock9. */
    StateMap m_bundleState;
    bool m_state_block_has_set_constant{false};
};

struct RemapPointerInfo {
    uintptr_t ptr;
    unsigned first_call;
};

class D3D9Impl : public FrameTrimmer {
  public:
    D3D9Impl(bool keep_all_states, bool swaps_to_finish);

    void switch_thread(int new_thread) override;
    void modify_last_frame_call(trace::Call& call) override;

  protected:
    void emitState() override;
    void finalize() override;
    ft_callback findCallback(const char* name) override;
    bool skipDeleteObj(const trace::Call& call) override;

  private:
    void recordStateCall(const trace::Call& call, uint32_t params_to_capture, int32_t resource_idx);

    void registerCalls();

    void recordRequiredCall(const trace::Call& call);

    void updateCallTable(const std::vector<const char*>& names, ft_callback cb);

    void recordShaderSetConstant(const trace::Call& call, eShaderStage stage);

    void recordBeginStateBlock(const trace::Call& call);
    void recordEndStateBlock(const trace::Call& call);
    void recordStateBlockApply(const trace::Call& call);
    void recordStateBlockCapture(const trace::Call& call);

    void recordReset(const trace::Call& call);

    void recordResourceCreation(const trace::Call& call, int32_t resource_idx, bool removable);
    void recordResourceSubresource(const trace::Call& call, int32_t parent_idx, int32_t level_idx,
                                   int32_t subresource_idx);
    void recordResourceInternalCreateOrGet(const trace::Call& call, int32_t resource_idx);
    void recordResourceRef(const trace::Call& call, int32_t resource_idx);
    void recordResourceRelease(const trace::Call& call);
    void recordResourceUsage(const trace::Call& call, int32_t resource_idx,
                             eCallType type = eCallType::unknown);
    void recordResourceLock(const trace::Call& call, int32_t resource_idx, int32_t level_idx,
                            int32_t face_idx, int32_t mem_map_idx);
    void recordResourceUnlock(const trace::Call& call, int32_t resource_idx, int32_t level_idx,
                              int32_t face_idx);

    void recordResourceDep(const trace::Call& call, int32_t resource_src_idx,
                           int32_t resource_dst_idx);

    void recordMemcpy(const trace::Call& call);
    void recordPresent(const trace::Call& call);

    void remapPointer(D3D9Object* obj, uintptr_t resource_ptr, unsigned callno);

    std::unordered_map<std::string, ft_callback> m_call_table;

    MemoryMapper<std::shared_ptr<D3D9Object>> m_mem_mapping;

    StateMap m_state_calls;
    std::array<std::array<PTraceCall, 256>, 2> m_shader_constants;

    std::unordered_map<uintptr_t, std::shared_ptr<D3D9Object>> m_active_objects;
    std::vector<std::shared_ptr<D3D9Object>> m_released_objects;

    std::unique_ptr<D3D9Object> m_currentStateBlock{nullptr};

    uintptr_t m_new_pointers{1};
    std::unordered_map<uintptr_t, RemapPointerInfo> m_pointer_map;
};
} // namespace frametrim
