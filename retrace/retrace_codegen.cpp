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

#include "retrace_codegen.hpp"

#include <cmath>

#include <snappy.h>

#include "os_backtrace_cpp.h"
#include "os_backtrace_hpp.h"
#include "os_hpp.h"
#include "os_posix_cpp.h"
#include "os_string_hpp.h"
#include "retrace_library_hpp.h"
#include "retrace_swizzle_cpp.h"
#include "retrace_swizzle_hpp.h"
#include "trace_model_cpp.h"
#include "trace_model_hpp.h"

namespace retrace {

Codegen::Codegen(const std::string &output_dir, const std::string &trace_name,
                 const std::unordered_map<std::string, retrace::FunctionType> &function_types)
    : output_dir(output_dir), trace_name(trace_name), function_types(function_types) {
}

void
Codegen::copy_file(const char *filename, const char *content) {
    FILE *file = fopen((output_dir / filename).c_str(), "w");
    fprintf(file, "%s", content);
    fclose(file);
}

void
Codegen::begin_trace() {
    std::filesystem::create_directory(output_dir);

    copy_file("trace_model.hpp", trace_model_hpp);
    copy_file("trace_model.cpp", trace_model_cpp);
    source_filenames.push_back("trace_model.cpp");
    copy_file("os.hpp", os_hpp);
    copy_file("os_posix.cpp", os_posix_cpp);
    source_filenames.push_back("os_posix.cpp");
    copy_file("os_string.hpp", os_string_hpp);
    copy_file("os_backtrace.hpp", os_backtrace_hpp);
    copy_file("os_backtrace.cpp", os_backtrace_cpp);
    source_filenames.push_back("os_backtrace.cpp");
    copy_file("retrace_library.hpp", retrace_library_hpp);
    copy_file("retrace_swizzle.cpp", retrace_swizzle_cpp);
    source_filenames.push_back("retrace_swizzle.cpp");
    copy_file("retrace_swizzle.hpp", retrace_swizzle_hpp);

    /* Dummy for retrace.hpp to make retrace_swizzle.cpp compile without pulling in the whole project. */
    copy_file("retrace.hpp", R"(
#pragma once

#include <cstdint>
#include <iostream>

#include "trace_model.hpp"

namespace retrace {
    static int debug = 0;
    static int verbosity = 0;

    std::ostream &warning(trace::Call &call) {
        std::cerr << "warning: ";
        return std::cerr;
    }
}
)");

    values_hpp.open(output_dir / "values.hpp");
    values_hpp << R"(
#include "trace_model.hpp"

struct NamedValue {
    const char *name;
    trace::Value *value;
};

static trace::Call *
createCall(trace::Id sig_id, const char *name, trace::CallFlags flags, trace::Value *ret, const std::vector<NamedValue> &args) {
    trace::FunctionSig *sig = new trace::FunctionSig;
    sig->id = sig_id;
    sig->name = name;
    sig->num_args = args.size();
    sig->arg_names = new const char *[sig->num_args];
    for (uint32_t i = 0; i < sig->num_args; i++)
        sig->arg_names[i] = args[i].name;

    /* Replay is always single threaded */
    trace::Call *call = new trace::Call(sig, flags, 0);

    call->ret = ret;
    for (uint32_t i = 0; i < args.size(); i++)
        call->args[i] = {args[i].value};

    return call;
}

static trace::Array *
createArray(const std::vector<trace::Value *> &values) {
    trace::Array *array = new trace::Array(values.size());

    for (uint32_t i = 0; i < values.size(); i++)
      array->values[i] = values[i];

    return array;
}

static trace::Struct *
createStruct(trace::Id sig_id, const char *name, const std::vector<NamedValue> &members) {
    trace::StructSig *sig = new trace::StructSig;
    sig->id = sig_id;
    sig->name = name;
    sig->num_members = members.size();
    sig->member_names = new const char *[sig->num_members];
    for (uint32_t i = 0; i < sig->num_members; i++)
        sig->member_names[i] = members[i].name;

    trace::Struct *s = new trace::Struct(sig);

    for (uint32_t i = 0; i < members.size(); i++)
        s->members[i] = members[i].value;

    return s;
}

static trace::Bitmask *
createBitmask(trace::Id sig_id, const std::vector<trace::BitmaskFlag> &flags,
              unsigned long long value) {
    trace::BitmaskSig *sig = new trace::BitmaskSig;
    sig->id = sig_id;
    sig->num_flags = flags.size();
    sig->flags = new trace::BitmaskFlag[sig->num_flags];
    trace::BitmaskFlag *flags_copy = new trace::BitmaskFlag[sig->num_flags];
    for (uint32_t i = 0; i < sig->num_flags; i++)
        flags_copy[i] = flags[i];
    sig->flags = flags_copy;

    return new trace::Bitmask(sig, value);
}

trace::Null null_value;
trace::Bool true_value(true);
trace::Bool false_value(false);
)";

    main_cpp.open(output_dir / "main.cpp");
    source_filenames.push_back("main.cpp");
    main_cpp << R"(
#include "private.hpp"

replay_args args;

void *
_getPublicProcAddress(const char *procName)
{
    return args.get_public_proc_addr(procName);
}

void *
_getPrivateProcAddress(const char *procName)
{
    return args.get_private_proc_addr(procName);
}

#include "values.hpp"

const struct replay_sequence sequences[] = {
)";

    state_cpp.open(output_dir / "state.cpp");
    source_filenames.push_back("state.cpp");
    state_cpp << R"(
#include "private.hpp"

void
addRegion(unsigned long long address, void *buffer, unsigned long long size) {
    trace::FunctionSig sig{};
    trace::CallFlags flags = 0;
    trace::Call call(&sig, flags, 0);
    retrace::addRegion(call, address, buffer, size);
}

void
delRegionByPointer(void *ptr)
{
    if (ptr)
        retrace::delRegionByPointer(ptr);
}

void *
toPointer(uintptr_t address) {
    trace::Pointer value(address);
    return retrace::toPointer(value);
}

void
resize_window(int width, int height) {
    args.resize_window(width, height);
}
)";

    sequence_h.open(output_dir / "sequence.h");
    sequence_h << R"(
#pragma once

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define RETRACE
#include "glproc.h"

#ifdef __cplusplus
extern "C" {
#endif

void addRegion(unsigned long long address, void *buffer, unsigned long long size);
void delRegionByPointer(void *ptr);
void *toPointer(uintptr_t address);

void resize_window(int width, int height);
)";

    data_bin.open(output_dir / "data.bin", std::ios::binary);
}

void
Codegen::end_trace() {
    end_sequence();

    values_hpp.close();

    main_cpp << R"(
};

extern "C" {

void
get_replay_sequences(const replay_sequence **out_sequences, uint32_t *out_sequence_count,
                     const replay_args *_args)
{
    args = *_args;

    *out_sequences = sequences;
    *out_sequence_count = sizeof(sequences) / sizeof(sequences[0]);
}

}
)";
    main_cpp.close();

    std::ofstream private_hpp(output_dir / "private.hpp");
    private_hpp << R"(
#pragma once

#include <unordered_map>

#undef Bool
#include "retrace_library.hpp"

#include "sequence.h"

#include "retrace_swizzle.hpp"

)";
    private_hpp.close();

    state_cpp.close();

    for (uint32_t i = 0; i < sequence_index; i++)
        sequence_h << "void sequence" << i << "(uintptr_t data);\n";
    sequence_h << R"(
#ifdef __cplusplus
}
#endif
)";
    sequence_h.close();

    std::ofstream meson_build(output_dir / "meson.build");
    meson_build << "project('" << trace_name << "', 'cpp', 'c')\n";
    meson_build << "replay_lib = shared_library(\n";
    meson_build << "  '" << trace_name << "',\n";
    for (const auto &filename : source_filenames)
        meson_build << "  '" << filename << "',\n";
    meson_build << "  name_prefix: ''\n";
    meson_build << ")\n";
    meson_build << "fs = import('fs')\nfs.copyfile('data.bin', '" << trace_name << ".so.data')\n";
    meson_build.close();

    data_bin.close();
}

void
Codegen::begin_sequence(uint32_t new_thread_id) {
    if (new_thread_id != thread_id) {
        end_sequence();
        thread_id = new_thread_id;
    }

    if (call_index > 10000) {
        end_sequence();
        call_index = 0;
    }

    if (inside_sequence)
        return;

    std::string sequence_filename = "sequence" + std::to_string(sequence_index) + ".c";
    sequence_c.open(output_dir / sequence_filename);
    source_filenames.push_back(sequence_filename);

    sequence_c << "#include \"sequence.h\"\n\n";
    sequence_c << "void\n";
    sequence_c << "sequence" << sequence_index << "(uintptr_t data) {\n";
    sequence_c << "    void *ptr = NULL; (void)ptr;\n";

    out_param_index = 0;

    inside_sequence = true;
}

void
Codegen::end_sequence() {
    if (!inside_sequence)
        return;

    size_t compressed_size = 0;
    if (data_buffer.size()) {
        compressed_data_buffer.resize(snappy::MaxCompressedLength(data_buffer.size()));

        snappy::RawCompress((const char *)data_buffer.data(), data_buffer.size(), compressed_data_buffer.data(), &compressed_size);
        data_bin.write(compressed_data_buffer.data(), compressed_size);

        data_buffer.clear();
    }

    sequence_c << "}\n";
    sequence_c.close();

    main_cpp << "    {sequence" << sequence_index << ", nullptr, 0, " << thread_id << ", " << compressed_size << "},\n";
    sequence_index++;

    inside_sequence = false;
}

const std::string &
Codegen::deduplicate_value_construction(const std::string &construction)
{
    if (value_variables.find(construction) == value_variables.end()) {
        std::string variable_name = "value" + std::to_string(value_index);
        value_index++;
      
        values_hpp << "static auto " << variable_name << " = " << construction << ";\n";
      
        value_variables[construction] = variable_name;
    }

    return value_variables.at(construction);
}

std::string
Codegen::get_value_construction(trace::Value *value)
{
    if (!value) {
        return "nullptr";
    }

    if (auto v = dynamic_cast<trace::Bool *>(value)) {
        return v->value ? "&true_value" : "&false_value";
    }

    if (dynamic_cast<trace::Null *>(value)) {
        return "&null_value";
    }

    if (auto v = dynamic_cast<trace::SInt *>(value)) {
        return deduplicate_value_construction("new trace::SInt(" + std::to_string(v->value) + ")");
    }

    if (auto v = dynamic_cast<trace::Bitmask *>(value)) {
        const trace::BitmaskSig *sig = v->sig;
        std::string expr = "createBitmask(" + std::to_string(sig->id) + ", {";
        for (uint32_t i = 0; i < sig->num_flags; i++) {
            if (i)
                expr += ", ";
            expr += "{\"";
            expr += sig->flags[i].name;
            expr += "\", ";
            expr += std::to_string(sig->flags[i].value);
            expr += "}";
        }
        expr += "}, ";
        expr += std::to_string(v->value);
        expr += ")";
        return deduplicate_value_construction(expr);
    }

    if (auto v = dynamic_cast<trace::Pointer *>(value)) {
        return deduplicate_value_construction("new trace::Pointer(" + std::to_string(v->value) + ")");
    }

    if (auto v = dynamic_cast<trace::UInt *>(value)) {
        return deduplicate_value_construction("new trace::UInt(" + std::to_string(v->value) + ")");
    }

    if (auto v = dynamic_cast<trace::Float *>(value)) {
        return deduplicate_value_construction("new trace::Float(" + std::to_string(v->value) + ")");
    }

    if (auto v = dynamic_cast<trace::Double *>(value)) {
        return deduplicate_value_construction("new trace::Double(" + std::to_string(v->value) + ")");
    }

    if (auto v = dynamic_cast<trace::String *>(value)) {
        return deduplicate_value_construction("new trace::String(\"" + std::string(v->value) + "\")");
    }

    if (dynamic_cast<trace::WString *>(value)) {
        abort();
        return "nullptr";
    }

    if (dynamic_cast<trace::Enum *>(value)) {
        abort();
        return "nullptr";
    }

    if (auto v = dynamic_cast<trace::Struct *>(value)) {
        const trace::StructSig *sig = v->sig;
        std::string expr = "createStruct(" + std::to_string(sig->id) + ", \"" + sig->name + "\", {";
        for (uint32_t i = 0; i < v->members.size(); i++) {
            if (i)
                expr += ", ";
            expr += "{\"";
            expr += sig->member_names[i];
            expr += "\", ";
            expr += get_value_construction(v->members[i]);
            expr += "}";
        }
        expr += "})";
        return deduplicate_value_construction(expr);
    }

    if (auto v = dynamic_cast<trace::Array *>(value)) {
        std::string expr = "createArray({";
        for (uint32_t i = 0; i < v->values.size(); i++) {
            if (i)
                expr += ", ";
            expr += get_value_construction(v->values[i]);
        }
        expr += "})";
        return deduplicate_value_construction(expr);
    }

    if (dynamic_cast<trace::Blob *>(value)) {
        abort();
        return "nullptr";
    }

    if (dynamic_cast<trace::Repr *>(value)) {
        abort();
        return "nullptr";
    }

    abort();
}

std::string
Codegen::get_call_construction(trace::Call &call)
{
    const trace::FunctionSig *sig = call.sig;
    std::string expr = "createCall(" + std::to_string(sig->id) + ", \"" + sig->name + "\", " +
                       std::to_string(call.flags) + ", ";
    expr += get_value_construction(call.ret);
    expr += ", {";
    for (uint32_t i = 0; i < call.args.size(); i++) {
        if (i)
            expr += ", ";
        expr += "{\"";
        expr += sig->arg_names[i];
        expr += "\", ";
        expr += get_value_construction(&call.arg(i));
        expr += "}";
    }
    expr += "})";
    return deduplicate_value_construction(expr);
}

void
Codegen::emit_constructed_call(trace::Call &call)
{
    assert(!inside_sequence);
    main_cpp << "    {nullptr, " << get_call_construction(call) << ", " << call.no << ", " << call.thread_id << ", 0},\n";
}

static const char *
remap_handle_name(const char *name)
{
    if (strstr(name, "getCurrentContext"))
        return "context";
    if (!strcmp("program", name) || !strcmp("shader", name))
        return "handleARB";
    return name;
}

void
Codegen::emit_get_handle(const trace::Call &call, const retrace::HandleType *handle_type, long long handle) {
    std::unordered_map<long long, std::map<long long, HandleRange>> &key_map = handle_maps[remap_handle_name(handle_type->name)];
    long long key = handle_type->key_name ? get_int_arg(call, handle_type->key_name, get_handle_default_value(handle_type->key_name)) : 0;
    std::map<long long, HandleRange> &range_map = key_map[key];

    std::map<long long, HandleRange>::iterator it = range_map.lower_bound(handle);
    while (it != range_map.begin()) {
        auto pred = it;
        pred--;
        if (handle >= pred->second.base && handle < pred->second.base + pred->second.range) {
            it = pred;
        } else {
            break;
        }
    }

    if (it == range_map.end() || handle < it->second.base || handle >= it->second.base + it->second.range) {
        sequence_c << "(" << handle_type->c_decl << ")";
        sequence_c << handle;
    } else {
        sequence_c << it->second.variable_name;
        uint64_t offset = handle - it->second.base;
        if (offset)
            sequence_c << " + " << offset;
    }
}

void
Codegen::emit_set_handle(const trace::Call &call, const retrace::HandleType *handle_type, long long handle) {
    std::unordered_map<long long, std::map<long long, HandleRange>> &key_map = handle_maps[remap_handle_name(handle_type->name)];
    long long key = handle_type->key_name ? get_int_arg(call, handle_type->key_name, get_handle_default_value(handle_type->key_name)) : 0;
    std::map<long long, HandleRange> &range_map = key_map[key];

    uint64_t count = get_int_arg(call, handle_type->range, 1);

    HandleRange range = {};
    if (range_map.find(handle) == range_map.end()) {
        range.base = handle;
        range.range = count;
        range.variable_name = std::string(remap_handle_name(handle_type->name)) + "_";
        if (handle >= 0) {
            range.variable_name += std::to_string(handle);
        } else {
            range.variable_name += "m";
            range.variable_name += std::to_string(-handle);
        }
        if (handle_type->key_name) {
            range.variable_name = std::string(remap_handle_name(handle_type->key_name)) +
                                  "_" + std::to_string(key) + "_" + range.variable_name;
        }
        range_map[handle] = range;
        sequence_h << "extern " << handle_type->c_decl << " " << range.variable_name << ";\n";
        state_cpp << handle_type->c_decl << " " << range.variable_name << ";\n";
    } else {
        range_map[handle].range = count;
        range = range_map[handle];
    }

    assert(handle == range.base);
    sequence_c << range.variable_name << " = ";
}

long long unsigned
Codegen::get_blob_offset(void *data, long long unsigned size)
{
    long long unsigned offset = data_buffer.size();
    if (size) {
        data_buffer.resize(offset + size);
        memcpy(&data_buffer[offset], data, size);
    }
    return offset;
}

bool
Codegen::emit_value_expression(const trace::Call &call, const retrace::ValueType *type, trace::Value *value) {
    if (type->kind == retrace::ValueTypeKind::_const) {
        const retrace::ConstType *const_type = (const retrace::ConstType *)type;
        emit_value_expression(call, const_type->type, value);
        return false;
    }

    if (type->kind == retrace::ValueTypeKind::literal) {
        const retrace::LiteralType *literal_type = (const retrace::LiteralType *)type;
        if (!strcmp("Bool", literal_type->encodedKind)) {
            if (value->toBool())
                sequence_c << "true";
            else
                sequence_c << "false";
        } else if (!strcmp("SInt", literal_type->encodedKind)) {
            sequence_c <<  value->toSInt() << "ll";
        } else if (!strcmp("UInt", literal_type->encodedKind)) {
            sequence_c << value->toUInt() << "llu";
        } else if (!strcmp("Float", literal_type->encodedKind) || !strcmp("Double", literal_type->encodedKind)) {
            if (std::isnan(value->toDouble()))
                sequence_c << "NAN";
            else if (value->toDouble() == INFINITY)
                sequence_c << "INFINITY";
            else if (value->toDouble() == -INFINITY)
                sequence_c << "-INFINITY";
            else
                sequence_c << value->toDouble();
        } else {
            abort();
        }
        return false;
    }

    if (type->kind == retrace::ValueTypeKind::pointer ||
        type->kind == retrace::ValueTypeKind::int_pointer ||
        type->kind == retrace::ValueTypeKind::blob ||
        type->kind == retrace::ValueTypeKind::opaque) {
        if (value->toNull()) {
            sequence_c << "NULL";
            return false;
        }

        trace::Blob *blob = dynamic_cast<trace::Blob *>(value);
        if (blob) {
            sequence_c << "(" << type->c_decl << ")(data + " << get_blob_offset(blob->buf, blob->size) << "llu)";
            return false;
        }

        sequence_c << "(" << type->c_decl << ")toPointer(" << value->toUIntPtr() << "llu)";
        return false;
    }

    if (type->kind == retrace::ValueTypeKind::obj_pointer ||
        type->kind == retrace::ValueTypeKind::linear_pointer ||
        type->kind == retrace::ValueTypeKind::reference) {
        abort();
        return false;
    }

    if (type->kind == retrace::ValueTypeKind::handle) {
        const retrace::HandleType *handle_type = (const retrace::HandleType *)type;
        emit_get_handle(call, handle_type, value->toSInt());
        return false;
    }

    if (type->kind == retrace::ValueTypeKind::_enum) {
        sequence_c << value->toSInt() << "ll";
        return false;
    }

    if (type->kind == retrace::ValueTypeKind::bitmask) {
        trace::Bitmask *v = (trace::Bitmask *)value;
        sequence_c << v->value << "llu";
        return false;
    }

    if (type->kind == retrace::ValueTypeKind::array) {
        const retrace::ArrayType *array_type = (const retrace::ArrayType *)type;

        trace::Array *v = value->toArray();
        if (!v) {
            sequence_c << "NULL";
            return false;
        }

        sequence_c << "(" << array_type->type->c_decl << "[]){";
        for (uint32_t i = 0; i < v->values.size(); i++) {
            if (i)
                sequence_c << ", ";
            emit_value_expression(call, array_type->type, v->values[i]);
        }
        sequence_c << "}";
        return false;
    }

    if (type->kind == retrace::ValueTypeKind::attrib_array) {
        abort();
        return false;
    }

    if (type->kind == retrace::ValueTypeKind::_struct) {
        const retrace::StructType *struct_type = (const retrace::StructType *)type;
        trace::Struct *v = (trace::Struct *)value;
        sequence_c << "(" << v->sig->name << "){";
        for (uint32_t i = 0; i < v->members.size(); i++) {
            if (i)
                sequence_c << ", ";
            emit_value_expression(call, struct_type->members[i].type, v->members[i]);
        }
        sequence_c << "}";
        return false;
    }

    if (type->kind == retrace::ValueTypeKind::alias) {
        const retrace::AliasType *alias_type = (const retrace::AliasType *)type;
        emit_value_expression(call, alias_type->type, value);
        return false;
    }

    if (type->kind == retrace::ValueTypeKind::string) {
        trace::String *v = (trace::String *)value;
        sequence_c << "R\"(" << v->value << ")\"";
        return false;
    }

    if (type->kind == retrace::ValueTypeKind::polymorphic) {
        const retrace::PolymorphicType *polymorphic_type = (const retrace::PolymorphicType *)type;
        if (emit_value_expression(call, polymorphic_type->switchTypes[0], value))
            emit_value_expression(call, polymorphic_type->defaultType, value);
        return false;
    }

    abort();
    return true;
}

void
Codegen::emit_call(trace::Call &call, const std::unordered_map<std::string, std::string> &arg_overrides)
{
    assert(inside_sequence);

    const retrace::FunctionType *func_type = nullptr;
    if (function_types.find(call.name()) != function_types.end())
        func_type = &function_types.at(call.name());
    assert(func_type);

    sequence_c << call.name() << "(";

    for (uint32_t i = 0; i < call.args.size(); i++) {
        if (i)
            sequence_c << ", ";

        const char *arg_name = call.sig->arg_names[i];
        if (arg_overrides.find(arg_name) != arg_overrides.end()) {
            sequence_c << arg_overrides.at(arg_name).c_str();
            continue;
        }

        const retrace::ArgType *arg_type = nullptr;
        if (func_type)
            arg_type = &func_type->parameter_types[i];

        if (arg_type->output && value_type_has_handle(arg_type->type)) {
            sequence_c << "out" << out_param_index << "_" << i;
            continue;
        }

        emit_value_expression(call, arg_type->type, call.args[i].value);
    }

    call_index++;
}

void
Codegen::emit_malloc(trace::Call &call)
{
    unsigned long long size = call.arg(0).toUInt();
    unsigned long long address = call.ret->toUIntPtr();

    if (address) {
        begin_sequence(call.thread_id);
        sequence_c << "    addRegion(" << address << "llu, malloc(" << size << "llu), " << size << "llu);\n";
    }
}

void
Codegen::emit_memcpy(trace::Call &call)
{
    unsigned long long size = call.arg(2).toUInt();
    if (size) {
        begin_sequence(call.thread_id);

        trace::Blob *src_blob = dynamic_cast<trace::Blob *>(&call.arg(1));
        if (src_blob) {
            sequence_c << "    memcpy(toPointer(" << (long long unsigned)call.arg(0).toPointer()
                       << "llu), (void *)(data + " << get_blob_offset(src_blob->buf, src_blob->size)
                       << "llu), " << size << "llu);\n";
        } else {
            sequence_c << "    memcpy(toPointer(" << (long long unsigned)call.arg(0).toPointer()
                       << "llu), toPointer(" << (long long unsigned)call.arg(1).toPointer()
                       << "llu), " << size << "llu);\n";
        }
    }
}

}
