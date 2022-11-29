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

#include "trace_parser.hpp"

#include <vector>
#include <unordered_set>

namespace frametrim {

enum Frametype {
    ft_none = 0,
    ft_key_frame = 1,
    ft_retain_frame = 2
};

class FrameTrimmer
{
public:
    FrameTrimmer(bool keep_all_states);
    ~FrameTrimmer();

    void start_last_frame(uint32_t callno);
    void call(const trace::Call& call, Frametype target_frame_type);

    std::unordered_set<unsigned> finalize(int last_frame_start);

    std::vector<unsigned> getSortedCallIds();
    std::unordered_set<unsigned> getUniqueCallIds();
private:
    struct FrameTrimmeImpl *impl;

};

}
