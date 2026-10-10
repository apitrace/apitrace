/*
 * Copyright © 2025 Valve Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once

#include <cstdint>

#include "trace_model.hpp"

typedef void (*run_api_calls_cb)(uintptr_t data);

struct replay_sequence {
    run_api_calls_cb run_api;
    trace::Call *call;
    /* Used for patching call->no during replay. This avoids
     * creating many similar calls for multi-frame traces.
     */
    uint32_t call_no;
    uint32_t thread_id;
    uint32_t compressed_data_size;
};

typedef void *(*get_proc_addr_cb)(const char *procName);

typedef void (*resize_window_cb)(int width, int height);

struct replay_args {
    /* For GL */
    get_proc_addr_cb get_public_proc_addr;
    get_proc_addr_cb get_private_proc_addr;

    /* For WSI */
    resize_window_cb resize_window;
};

extern "C" {

void get_replay_sequences(const replay_sequence **sequences,
                          uint32_t *sequence_count,
                          const replay_args *_args);

}

typedef void(*get_replay_sequences_cb)(const replay_sequence **sequences,
                                       uint32_t *sequence_count,
                                       const replay_args *_args);

/* Set by get_replay_sequences */
extern replay_args args;
