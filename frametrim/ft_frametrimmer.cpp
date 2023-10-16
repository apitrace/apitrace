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
#include "ft_opengl.hpp"

#include <algorithm>

namespace frametrim {

FrameTrimmer::FrameTrimmer(bool keep_all_states, bool swap_to_finish):
    m_recording_frame(false),
    m_keep_all_state_calls(keep_all_states),
    m_swaps_to_finish(swap_to_finish),
    m_last_frame_start(0),
    m_current_thread(-1)
{
}

bool
FrameTrimmer::isSupported(trace::API api)
{
    return (api == trace::API_GL || api == trace::API_EGL);
}

std::shared_ptr<FrameTrimmer>
FrameTrimmer::create(trace::API api, bool keep_all_states, bool swap_to_finish)
{
    if (api == trace::API_GL || api == trace::API_EGL) {
        std::cerr << "Creating OpenGL trimmer" << std::endl;
        return std::make_shared<OpenGLImpl>(keep_all_states, swap_to_finish);
    } else {
        assert(0);
    }
}

void
FrameTrimmer::call(const trace::Call& call, Frametype frametype)
{
    const char *call_name = call.name();
    bool end_frame = (call.flags & trace::CALL_FLAG_END_FRAME);

    if (!m_recording_frame && (frametype != ft_none)) {
        std::cerr << "Start recording\n";
        m_recording_frame = true;
        emitState();
    }

    if (m_recording_frame && (frametype == ft_none)) {
        std::cerr << "End recording\n";
        m_recording_frame = false;
    }

    /* Skip delete calls for objects that have never been emitted, or
     * if we are in the last frame and the object was created in an earlier frame.
     * By not deleting such objects looping the last frame will work in more cases */
    if (skipDeleteObj(call)) {
        return;
    }

    if (m_current_thread != call.thread_id) {
        switch_thread(call.thread_id);
        m_current_thread = call.thread_id;
    }

    auto icb = m_call_table_cache.find(call_name);
    if (icb != m_call_table_cache.end())
        icb->second(call);
    else {
        auto func = findCallback(call_name);
        if (func) {
            m_call_table_cache[call_name] = func;
            func(call);
        } else if (!end_frame) {
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
        if (end_frame) {
            if (m_swaps_to_finish && m_last_swap) {
                m_required_calls.insert(c);
                m_swap_calls.insert(m_last_swap->callNo());
            }
            m_last_swap = c;
        }
    } else {
        if (!end_frame) {
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

void
FrameTrimmer::start_last_frame(uint32_t callno)
{
    std::cerr << "\n---> Start last frame at call no " << callno << "\n";
    m_last_frame_start = callno;
}

void
FrameTrimmer::end_last_frame()
{
    finalize();
    if (m_last_swap)
        m_required_calls.insert(m_last_swap);
}

std::unordered_set<unsigned>
FrameTrimmer::get_skip_loop_calls()
{
    return m_skip_loop_calls;
}

std::unordered_set<unsigned>
FrameTrimmer::get_swap_to_finish_calls()
{
    return m_swap_calls;
}

std::vector<unsigned>
FrameTrimmer::getSortedCallIds()
{
    auto make_sure_its_singular = getUniqueCallIds();

    std::vector<unsigned> sorted_calls(
                make_sure_its_singular.begin(),
                make_sure_its_singular.end());
    std::sort(sorted_calls.begin(), sorted_calls.end());
    return sorted_calls;
}

std::unordered_set<unsigned>
FrameTrimmer::getUniqueCallIds()
{
    std::unordered_set<unsigned> retval;
    for (auto&& c: m_required_calls)
        retval.insert(c->callNo());
    return retval;
}

}
