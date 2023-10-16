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
#include "trace_parser.hpp"

#include <functional>
#include <set>
#include <unordered_set>
#include <vector>

namespace frametrim {

using ft_callback = std::function<void(const trace::Call&)>;

struct string_part_less {
    bool operator () (const char *lhs, const char *rhs) const
    {
        int len = std::min(strlen(lhs), strlen(rhs));
        return strncmp(lhs, rhs, len) < 0;
    }
};

enum Frametype {
    ft_none = 0,
    ft_key_frame = 1,
    ft_retain_frame = 2
};

class FrameTrimmer
{
public:
    FrameTrimmer(bool keep_all_states, bool swap_to_finish);

    static bool isSupported(trace::API api);
    static std::shared_ptr<FrameTrimmer> create(trace::API api, bool keep_all_states, bool swap_to_finish);

    void call(const trace::Call& call, Frametype target_frame_type);
    void start_last_frame(uint32_t callno);
    void end_last_frame();

    std::unordered_set<unsigned> get_skip_loop_calls();
    std::unordered_set<unsigned> get_swap_to_finish_calls();

    std::vector<unsigned> getSortedCallIds();
    std::unordered_set<unsigned> getUniqueCallIds();

    virtual void switch_thread(int new_thread) {}

protected:
    virtual void emitState() {};
    virtual void finalize() {};
    virtual ft_callback findCallback(const char *name) = 0;
    virtual bool skipDeleteObj(const trace::Call& call) = 0;

    using CallTableCache =  std::map<const char *, ft_callback>;
    CallTableCache m_call_table_cache;

    PTraceCall m_last_swap;
    bool m_recording_frame;
    bool m_keep_all_state_calls;
    bool m_swaps_to_finish;
    unsigned m_last_frame_start;
    int m_current_thread;

    CallSet m_required_calls;
    std::set<std::string> m_unhandled_calls;
    std::unordered_set<unsigned> m_swap_calls;
    std::unordered_set<unsigned> m_skip_loop_calls;
};

}
