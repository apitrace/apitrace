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

#include "glproc.h"
#include "glretrace.hpp"
#include "retrace.hpp"
#include "retrace_codegen.hpp"
#include "retrace_swizzle.hpp"

#include "eglimports_h.h"
#include "glimports_h.h"
#include "glproc_cpp.h"
#include "glproc_h.h"

#include "md5.h"

#include <snappy.h>

#include <cmath>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>

static std::unordered_set<std::string> ignore_calls = {
    "wglDescribePixelFormat",
    "glDebugMessageCallback",
};

class GLCodegen : public retrace::Codegen {
public:
    GLCodegen(const std::string &output_dir, const std::string &trace_name)
        : Codegen(output_dir, trace_name, glretrace::gl_func_types)
    {
    }

    virtual uint64_t get_handle_default_value(const char *name) override;

    void emit_gl_call(trace::Call &call);

private:
    GLint active_program = 0;
    GLint active_pipeline = 0;
    GLint active_pack_buffer = 0;
    std::unordered_map<GLint, GLint> pipeline_active_programs;

    std::unordered_map<uint32_t, uintptr_t> current_context;

    std::unordered_map<GLhandleARB, std::vector<std::string>> resource_names;
};

uint64_t
GLCodegen::get_handle_default_value(const char *name) {
    if (strstr(name, "getCurrentContext")) {
        return current_context.at(thread_id);
    } else if (!strcmp("program", name)) {
        uint64_t program = active_program;

        if (active_pipeline && pipeline_active_programs.find(active_pipeline) != pipeline_active_programs.end())
            program = pipeline_active_programs.at(active_pipeline);

        return program;
    } else if (!strcmp("programObj", name)) {
        return active_program;
    }
    return 0;
}

void
GLCodegen::emit_gl_call(trace::Call &call) {
    if (ignore_calls.find(call.name()) != ignore_calls.end())
        return;

    bool is_active_uniform_block_name = !strcmp("glGetActiveUniformBlockName", call.name());

    if ((call.flags & trace::CALL_FLAG_NO_SIDE_EFFECTS) && !is_active_uniform_block_name)
        return;

    const retrace::FunctionType *func_type = nullptr;
    if (function_types.find(call.name()) != function_types.end())
        func_type = &function_types.at(call.name());

    if (!strcmp("CGLSetCurrentContext", call.name())) {
        if (call.ret->toBool())
            current_context[thread_id] = call.arg(0).toUIntPtr();
    } else if (!strcmp("eglMakeCurrent", call.name())) {
        if (call.ret->toBool())
            current_context[thread_id] = call.arg(3).toUIntPtr();
    } else if (!strcmp("glXMakeCurrent", call.name())) {
        if (call.ret->toBool())
            current_context[thread_id] = call.arg(2).toUIntPtr();
    } else if (!strcmp("glXMakeContextCurrent", call.name())) {
        if (call.ret->toBool())
            current_context[thread_id] = call.arg(3).toUIntPtr();
    } else if (!strcmp("wglMakeCurrent", call.name())) {
        current_context[thread_id] = 0;
        if (call.ret->toBool())
            current_context[thread_id] = call.arg(1).toUIntPtr();
    } else if (!strcmp("wglMakeContextCurrentARB", call.name())) {
        current_context[thread_id] = 0;
        if (call.ret->toBool())
            current_context[thread_id] = call.arg(2).toUIntPtr();
    } else if (!strcmp("wglShareLists", call.name())) {
        if (call.ret->toBool())
            current_context[thread_id] = call.arg(0).toUIntPtr();
    } else if (!strcmp("glGetProgramResourceName", call.name())) {
        auto &names = resource_names[call.arg(0).toSInt()];
        uint32_t index = call.arg(2).toUInt();
        if (names.size() <= index)
            names.resize(index + 1);
        names[index] = call.arg(5).toString();
    } else if (!strcmp("glGetProgramResourceiv", call.name())) {
        GLenum program_interface = call.arg(1).toSInt();
        if (program_interface == GL_UNIFORM) {
            GLuint program = call.arg(0).toUInt();
            uint32_t index = call.arg(2).toUInt();
            const trace::Array *props = call.arg(4).toArray();
            const trace::Array *params = call.arg(7).toArray();
            for (uint32_t i = 0; i < props->size(); i++) {
                auto prop = props->values[i];
                if (prop->toSInt() != GL_LOCATION)
                    continue;
                const auto location = params->values[i]->toSInt();
                assert(resource_names[program].size() > index);
                const auto &name = resource_names[program][index];

                const retrace::HandleType *program_type = (const retrace::HandleType *)func_type->parameter_types[1].type;

                retrace::HandleType handle_type(program_type->type, "location", NULL, program_type, "program");
                emit_set_handle(call, &handle_type, location);
                sequence_c << "getLocationForUniform(";
                emit_get_handle(call, program_type, program);
                sequence_c << ", \"" << name << "\", " << location << ");\n";
                break;
            }
        }
    }

    bool new_wsi_sequence =
        !strncmp("glX", call.name(), 3) || !strncmp("wgl", call.name(), 3);
    if (new_wsi_sequence) {
        end_sequence();
        emit_constructed_call(call);
        return;
    }

    begin_sequence(call.thread_id);

    if (is_active_uniform_block_name) {
        emit_set_handle(call, (const retrace::HandleType *)func_type->parameter_types[1].type, call.arg(1).toSInt());
        sequence_c << "mapUniformBlockName(";
        emit_get_handle(call, (const retrace::HandleType *)func_type->parameter_types[0].type, call.arg(0).toSInt());
        sequence_c << ", " << call.arg(1).toUInt() << "llu, \"" << call.arg(4).toString() << "\");\n";
        return;
    }

    if (!strcmp("glUnmapBuffer", call.name())) {
        sequence_c << "    ptr = NULL;\n";
        sequence_c << "    glGetBufferPointerv(" << call.arg(0).toUInt() << ", GL_BUFFER_MAP_POINTER, &ptr);";
        sequence_c << "    delRegionByPointer(ptr);\n";
    } else if (!strcmp("glUnmapBufferARB", call.name())) {
        sequence_c << "    ptr = NULL;\n";
        sequence_c << "    glGetBufferPointervARB(" << call.arg(0).toUInt() << ", GL_BUFFER_MAP_POINTER_ARB, &ptr);",
        sequence_c << "    delRegionByPointer(ptr);\n";
    } else if (!strcmp("glUnmapBufferOES", call.name())) {
        sequence_c << "    ptr = NULL;\n";
        sequence_c << "    glGetBufferPointervOES(" << call.arg(0).toUInt() << ", GL_BUFFER_MAP_POINTER_OES, &ptr);";
        sequence_c << "    delRegionByPointer(ptr);\n";
    } else if (!strcmp("glUnmapNamedBuffer", call.name())) {
        sequence_c << "    ptr = NULL;\n";
        sequence_c << "    glGetNamedBufferPointerv(";
        emit_value_expression(call, func_type->parameter_types[0].type, &call.arg(0));
        sequence_c << ", GL_BUFFER_MAP_POINTER, &ptr);\n";
        sequence_c << "    delRegionByPointer(ptr);\n";
    } else if (!strcmp("glUnmapNamedBufferEXT", call.name())) {
        sequence_c << "    ptr = NULL;\n";
        sequence_c << "    glGetNamedBufferPointervEXT(";
        emit_value_expression(call, func_type->parameter_types[0].type, &call.arg(0));
        sequence_c << ", GL_BUFFER_MAP_POINTER, &ptr);\n";
        sequence_c << "    delRegionByPointer(ptr);\n";
    } else if (!strcmp("glDeleteBuffers", call.name()) ||
               !strcmp("glDeleteBuffersARB", call.name())) {
        const trace::Array *buffers = (const trace::Array *)&call.arg(1);

        assert(func_type->parameter_types[1].type->kind ==
               retrace::ValueTypeKind::array);
        const retrace::ArrayType *buffers_type =
            (const retrace::ArrayType *)func_type->parameter_types[1].type;

        uint32_t n = (uint32_t)call.arg(0).toUInt();
        for (uint32_t i = 0; i < n; i++) {
            sequence_c << "    ptr = NULL;\n";
            sequence_c << "    glGetNamedBufferPointervEXT(";
            emit_value_expression(call, buffers_type->type, buffers->values[i]);
            sequence_c << ", GL_BUFFER_MAP_POINTER, &ptr);\n";
            sequence_c << "    delRegionByPointer(ptr);\n";
        }
    } else if (!strcmp("glClientWaitSync", call.name())) {
        sequence_c << "    clientWaitSync(" << call.ret->toUInt() << ", ";
        emit_value_expression(call, func_type->parameter_types[0].type, &call.arg(0));
        sequence_c << ", " << call.arg(1).toUInt() << ", " << call.arg(2).toUInt() << "llu);\n";
    } else if (!strcmp("glGetSynciv", call.name())) {
        GLint pname = call.arg(1).toSInt();
        GLsizei buf_size = call.arg(2).toSInt();
        trace::Array *values = call.arg(4).toArray();
        if (pname == GL_SYNC_STATUS && buf_size >= 1 && values && values->values[0]->toSInt() == GL_SIGNALED) {
            sequence_c << "    blockOnFence(" << call.ret->toUInt() << ", ";
            emit_value_expression(call, func_type->parameter_types[0].type, &call.arg(0));
            sequence_c << ", GL_SYNC_FLUSH_COMMANDS_BIT);\n";
        }
    }

    bool has_out_pointer = false;
    if (func_type) {
        for (uint32_t i = 0; i < call.args.size(); i++) {
            const retrace::ArgType *arg_type = &func_type->parameter_types[i];
            if (arg_type->output && (arg_type->type->kind == retrace::ValueTypeKind::pointer ||
                                     arg_type->type->kind == retrace::ValueTypeKind::opaque))
                has_out_pointer = true;
        }
    }

    std::unordered_map<std::string, std::string> arg_overrides;
    bool free_ptr = false;
    if (has_out_pointer && !active_pack_buffer) {
        if (!strcmp("glGetTexnImage", call.name())) {
            sequence_c << "    ptr = malloc(" << call.arg(4).toUInt() << "llu);\n";
            arg_overrides["pixels"] = "ptr";
            free_ptr = true;
        } else if (!strcmp("glGetTextureImage", call.name())) {
            sequence_c << "    ptr = malloc(" << call.arg(4).toUInt() << "llu);\n";
            arg_overrides["pixels"] = "ptr";
            free_ptr = true;
        } else if (!strcmp("glReadPixels", call.name())) {
            sequence_c << "    ptr = malloc(" << call.arg(2).toSInt() * call.arg(3).toSInt() * 64 << "llu);\n";
            arg_overrides["pixels"] = "ptr";
            free_ptr = true;
        } else if (!strcmp("glReadnPixels", call.name())) {
            sequence_c << "    ptr = malloc(" << call.arg(6).toSInt() << "llu);\n";
            arg_overrides["data"] = "ptr";
            free_ptr = true;
        } else {
            std::cout << "warning: Skipping call " << call.name() << " because there is no active pack buffer." << std::endl;
            return;
        }
    }

    bool has_out_handle = false;
    if (func_type) {
        for (uint32_t i = 0; i < call.args.size(); i++) {
            const retrace::ArgType *arg_type = &func_type->parameter_types[i];
            if (!arg_type->output || !value_type_has_handle(arg_type->type))
                continue;

            assert(arg_type->type->kind == retrace::ValueTypeKind::array);
            const retrace::ArrayType *array_type =
                (const retrace::ArrayType *)arg_type->type;
            auto array = dynamic_cast<trace::Array *>(call.args[i].value);

            sequence_c << "    " << array_type->type->c_decl << " out" << out_param_index << "_" << i
                       << "[" << array->values.size() << "];\n";
            has_out_handle = true;
        }
    }

    sequence_c << "    ";

    if (func_type->return_type->kind == retrace::ValueTypeKind::handle && call.ret) {
        const retrace::HandleType *handle_type =
            (const retrace::HandleType *)func_type->return_type;
        emit_set_handle(call, handle_type, call.ret->toSInt());
    } else if (func_type->return_type->kind == retrace::ValueTypeKind::linear_pointer && call.ret) {
        sequence_c << "addRegion(" << call.ret->toUIntPtr() << "llu, ";
    }

    emit_call(call, arg_overrides);

    if (func_type->return_type->kind == retrace::ValueTypeKind::linear_pointer && call.ret) {
        const retrace::LinearPointerType *pointer_type =
            (const retrace::LinearPointerType *)func_type->return_type;

        uint32_t size = 1;
        for (uint32_t i = 0; i < call.args.size(); i++) {
            if (pointer_type->size && !strcmp(pointer_type->size, call.sig->arg_names[i])) {
                size = call.args[i].value->toUInt();
                break;
            }
        }

        sequence_c << "), " << size << ");\n";
    } else if (func_type->return_type->kind == retrace::ValueTypeKind::handle &&
               call.ret) {
        sequence_c << ");\n";
    } else {
        sequence_c << ");\n";
    }

    if (has_out_handle) {
        for (uint32_t i = 0; i < call.args.size(); i++) {
            const retrace::ArgType *arg_type = &func_type->parameter_types[i];
            if (!arg_type->output || !value_type_has_handle(arg_type->type))
                continue;
          
            auto array = dynamic_cast<trace::Array *>(call.args[i].value);
            const retrace::ArrayType *array_type =
                (const retrace::ArrayType *)arg_type->type;
            const retrace::HandleType *handle_type =
                (const retrace::HandleType *)array_type->type;

            for (uint32_t j = 0; j < array->values.size(); j++) {
                emit_set_handle(call, handle_type, array->values[j]->toUInt());
                sequence_c << "out" << out_param_index << "_" << i << "[" << j << "];\n";
            }
        }
        out_param_index++;
    }

    if (free_ptr)
        sequence_c << "    free(ptr);\n";

    /* GL specific handling. */

    if (!strcmp("glViewport", call.name())) {
        sequence_c << "    resize_window(" << (call.args[0].value->toSInt() + call.args[2].value->toSInt())
                   << ", " << (call.args[1].value->toSInt() + call.args[3].value->toSInt()) << ");\n";
    } else if (!strcmp("glViewportArrayv", call.name())) {
        int32_t first = call.args[0].value->toSInt();
        int32_t count = call.args[1].value->toSInt();
        const trace::Array *v = dynamic_cast<trace::Array *>(call.args[2].value);
        if (first == 0 && count > 0) {
            sequence_c << "    resize_window(" << (v->values[0]->toSInt() + v->values[2]->toSInt())
                       << ", " << (v->values[1]->toSInt() + v->values[3]->toSInt()) << ");\n";
        }
    } else if (!strcmp("glViewportIndexedf", call.name())) {
        int32_t index = call.args[0].value->toSInt();
        if (index == 0) {
            sequence_c <<  "    resize_window(" << (uint32_t)(call.args[1].value->toFloat() + call.args[3].value->toFloat())
                       << ", " << (uint32_t)(call.args[2].value->toFloat() + call.args[4].value->toFloat()) << ");\n";
        } 
    } else if (!strcmp("glViewportIndexedfv", call.name())) {
        uint32_t index = call.args[0].value->toUInt();
        const trace::Array *v = dynamic_cast<trace::Array *>(call.args[1].value);
        if (index == 0) {
            sequence_c << "    resize_window(" << (uint32_t)(v->values[0]->toFloat() + v->values[2]->toFloat())
                       << ", " << (uint32_t)(v->values[1]->toFloat() + v->values[3]->toFloat()) << ");\n";
        }
    } else if (!strcmp("glBlitFramebuffer", call.name()) || !strcmp("glBlitFramebufferEXT", call.name())) {
        sequence_c << "    resize_window(" << std::max(call.arg(4).toSInt(), call.arg(6).toSInt())
                   << ", " << std::max(call.arg(5).toSInt(), call.arg(7).toSInt()) << ");\n";
    } else if (!strcmp("glActiveShaderProgram", call.name())) {
        pipeline_active_programs[call.arg(0).toSInt()] = call.arg(1).toSInt();
    } else if (!strcmp("glBindProgramPipeline", call.name()) || !strcmp("glBindProgramPipelineEXT", call.name())) {
        active_pipeline = call.arg(0).toSInt();
    } else if (!strcmp("glUseProgram", call.name()) || !strcmp("glUseProgramObjectARB", call.name())) {
        active_program = call.arg(0).toSInt();
    } else if (!strcmp("glBindBuffer", call.name())) {
        if (call.arg(0).toSInt() == GL_PIXEL_PACK_BUFFER)
            active_program = call.arg(1).toSInt();
    }
};

GLCodegen *codegen = nullptr;

void
glretrace::codegen_start() {
    std::string trace_name = std::filesystem::path(retrace::trace_filename).stem();

    codegen = new GLCodegen(retrace::Cpath, trace_name);
    codegen->begin_trace();

    codegen->copy_file("glproc.h", glproc_h);
    codegen->copy_file("glproc.cpp", glproc_cpp);
    codegen->source_filenames.push_back("glproc.cpp");
    codegen->copy_file("eglimports.h", eglimports_h);
    codegen->copy_file("glimports.h", glimports_h);

    codegen->sequence_h << R"(
GLenum blockOnFence(GLsync sync, GLbitfield flags);

GLenum clientWaitSync(GLenum result, GLsync sync, GLbitfield flags, GLuint64 timeout);

GLint mapUniformBlockName(GLuint program, GLint index, const char *name);

GLint getLocationForUniform(GLuint program, const char *name, GLint traced_location);
)";

    codegen->state_cpp << R"(
GLenum
blockOnFence(GLsync sync, GLbitfield flags) {
    GLenum result;

    do {
        result = glClientWaitSync(sync, flags, 1000);
    } while (result == GL_TIMEOUT_EXPIRED);

    return result;
}

GLenum
clientWaitSync(GLenum result, GLsync sync, GLbitfield flags, GLuint64 timeout) {
    switch (result) {
    case GL_ALREADY_SIGNALED:
    case GL_CONDITION_SATISFIED:
        // We must block, as following calls might rely on the fence being
        // signaled
        result = blockOnFence(sync, flags);
        break;
    case GL_TIMEOUT_EXPIRED:
        result = glClientWaitSync(sync, flags, timeout);
        break;
    default:
        break;
    }
    return result;
}

GLint
mapUniformBlockName(GLuint program, GLint index, const char *name) {
    GLint num_blocks = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &num_blocks);
    for (GLint i = 0; i < num_blocks; i++) {
        GLint buf_len;
        glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_NAME_LENGTH, &buf_len);
        std::vector<char> name_buf(buf_len + 1);
        GLint length;
        glGetActiveUniformBlockName(program, i, name_buf.size(), &length, name_buf.data());
        name_buf[length] = '\0';
        if (!strcmp(name, name_buf.data()))
            return i;
    }
    return index;
}

GLint
getLocationForUniform(GLuint program, const char *name, GLint traced_location) {
    GLint active_resources;
    glGetProgramInterfaceiv(program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &active_resources);
    for (GLint i = 0; i < active_resources; i++) {
        static const int kNameBufSize = 512;
        GLchar retraced_name[kNameBufSize] = "";
        GLint retraced_name_len;
        glGetProgramResourceName(program, GL_UNIFORM, i, kNameBufSize, &retraced_name_len, retraced_name);
        retraced_name[retraced_name_len] = '\0';
        if (strcmp(name, retraced_name))
            continue;

        const GLenum prop = GL_LOCATION;
        GLsizei prop_len;
        GLint location;
        glGetProgramResourceiv(program, GL_UNIFORM, i, 1, &prop, 1, &prop_len, &location);
        assert(prop_len == 1);
        return location;
    }
    assert(false);
    return traced_location;
}

)";
}

void
glretrace::call_codegen(trace::Call &call) {
    codegen->emit_gl_call(call);
}

void
glretrace::codegen_end() {
    codegen->end_trace();
    delete codegen;
    codegen = nullptr;
}

static void
malloc_codegen(trace::Call &call) {
    codegen->emit_malloc(call);
}

static void
memcpy_codegen(trace::Call &call) {
    codegen->emit_memcpy(call);
}

const retrace::Entry retrace::stdc_codegen_callbacks[] = {
    {"malloc", &malloc_codegen}, {"memcpy", &memcpy_codegen}, {NULL, NULL}};
