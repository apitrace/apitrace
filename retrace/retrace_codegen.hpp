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
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "trace_model.hpp"
#include "retrace.hpp"

namespace retrace {

struct HandleRange {
    uint64_t base;
    uint64_t range;
    std::string variable_name;
};

class Codegen {
public:
    Codegen(const std::string &output_dir, const std::string &trace_name,
            const std::unordered_map<std::string, retrace::FunctionType> &function_types);
    virtual ~Codegen() {}

    void copy_file(const char *filename, const char *content);

    void begin_trace();
    void end_trace();

    void begin_sequence(uint32_t new_thread_id);
    void end_sequence();

    void emit_constructed_call(trace::Call &call);
    void emit_call(trace::Call &call, const std::unordered_map<std::string, std::string> &arg_overrides);

    virtual uint64_t get_handle_default_value(const char *name) { return 0; }

    std::string get_call_construction(trace::Call &call);
    std::string get_value_construction(trace::Value *value);
    const std::string &deduplicate_value_construction(const std::string &construction);

    void emit_get_handle(const trace::Call &call, const retrace::HandleType *handle_type, long long handle);
    void emit_set_handle(const trace::Call &call, const retrace::HandleType *handle_type, long long handle);
    long long unsigned get_blob_offset(void *data, long long unsigned size);
    bool emit_value_expression(const trace::Call &call, const retrace::ValueType *type, trace::Value *value);

    void emit_malloc(trace::Call &call);
    void emit_memcpy(trace::Call &call);

    std::filesystem::path output_dir;
    std::string trace_name;

    const std::unordered_map<std::string, retrace::FunctionType> &function_types;

    bool inside_sequence = false;
    uint32_t sequence_index = 0;
    uint32_t call_index = 0;
    uint32_t thread_id = 0;

    std::vector<char> data_buffer;
    std::vector<char> compressed_data_buffer;

    std::unordered_map<std::string, std::string> value_variables;
    uint32_t value_index = 0;

    // std::unordered_set<std::string> handle_maps;
    std::unordered_map<std::string, std::unordered_map<long long, std::map<long long, HandleRange>>> handle_maps;

    uint32_t out_param_index = 0;

    std::vector<std::string> source_filenames;

    std::ofstream sequence_h;
    std::ofstream sequence_c;
    std::ofstream main_cpp;
    std::ofstream state_cpp;
    std::ofstream values_hpp;
    std::ofstream data_bin;
};

static inline long long
get_int_arg(const trace::Call &call, const char *name, long long default_value) {
    for (uint32_t i = 0; i < call.args.size(); i++) {
        if (name && !strcmp(name, call.sig->arg_names[i]))
            return call.args[i].value->toSInt();
    }
    return default_value;
}

static bool
value_type_has_handle(const retrace::ValueType *type) {
    if (type->kind == retrace::ValueTypeKind::_const)
        return value_type_has_handle(((const retrace::ConstType *)type)->type);

    if (type->kind == retrace::ValueTypeKind::pointer)
        return value_type_has_handle(((const retrace::PointerType *)type)->type);

    if (type->kind == retrace::ValueTypeKind::obj_pointer)
        return value_type_has_handle(((const retrace::ObjPointerType *)type)->type);

    if (type->kind == retrace::ValueTypeKind::linear_pointer)
        return value_type_has_handle(((const retrace::LinearPointerType *)type)->type);

    if (type->kind == retrace::ValueTypeKind::reference)
        return value_type_has_handle(((const retrace::ReferenceType *)type)->type);

    if (type->kind == retrace::ValueTypeKind::handle)
        return true;

    if (type->kind == retrace::ValueTypeKind::bitmask)
        return value_type_has_handle(((const retrace::BitmaskType *)type)->type);

    if (type->kind == retrace::ValueTypeKind::array)
        return value_type_has_handle(((const retrace::ArrayType *)type)->type);

    if (type->kind == retrace::ValueTypeKind::attrib_array)
        return value_type_has_handle(((const retrace::AttribArrayType *)type)->type);

    if (type->kind == retrace::ValueTypeKind::blob)
        return value_type_has_handle(((const retrace::BlobType *)type)->type);

    if (type->kind == retrace::ValueTypeKind::alias)
        return value_type_has_handle(((const retrace::AliasType *)type)->type);

    if (type->kind == retrace::ValueTypeKind::string)
        return value_type_has_handle(((const retrace::StringType *)type)->type);

    if (type->kind == retrace::ValueTypeKind::polymorphic)
        return value_type_has_handle(((const retrace::PolymorphicType *)type)->switchTypes[0]);

    return false;
}

};
