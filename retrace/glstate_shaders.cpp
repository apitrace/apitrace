/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/


#include <assert.h>
#include <string.h>

#include "json.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "glproc.hpp"
#include "glsize.hpp"
#include "glstate.hpp"
#include "glstate_internal.hpp"


namespace glstate {


// Mapping from shader type to shader source, used to accumulated the sources
// of different shaders with same type.
typedef std::map<std::string, std::string> ShaderMap;


static void
getShaderSource(ShaderMap &shaderMap, GLuint shader)
{
    if (!shader) {
        return;
    }

    GLint shader_type = 0;
    glGetShaderiv(shader, GL_SHADER_TYPE, &shader_type);
    if (!shader_type) {
        return;
    }

    GLint source_length = 0;
    glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &source_length);
    if (!source_length) {
        return;
    }

    GLchar *source = new GLchar[source_length];
    GLsizei length = 0;
    source[0] = 0;
    glGetShaderSource(shader, source_length, &length, source);

    shaderMap[enumToString(shader_type)] += source;

    delete [] source;
}


static inline void
dumpProgram(JSONWriter &json, GLint program)
{
    if (program <= 0) {
        return;
    }

    GLint attached_shaders = 0;
    glGetProgramiv(program, GL_ATTACHED_SHADERS, &attached_shaders);
    if (!attached_shaders) {
        return;
    }

    ShaderMap shaderMap;

    GLuint *shaders = new GLuint[attached_shaders];
    GLsizei count = 0;
    glGetAttachedShaders(program, attached_shaders, &count, shaders);
    std::sort(shaders, shaders + count);
    for (GLsizei i = 0; i < count; ++ i) {
        getShaderSource(shaderMap, shaders[i]);
    }
    delete [] shaders;

    for (ShaderMap::const_iterator it = shaderMap.begin(); it != shaderMap.end(); ++it) {
        json.beginMember(it->first);
        json.writeString(it->second);
        json.endMember();
    }
}


/**
 * Built-in uniforms can't be queried through glGetUniform*.
 */
static inline bool
isBuiltinUniform(const GLchar *name)
{
    return name[0] == 'g' && name[1] == 'l' && name[2] == '_';
}

/*
 * When fetching the uniform name of an array we usually get name[0]
 * so we need to cut the trailing "[0]" in order to properly construct
 * array names later. Otherwise we endup with stuff like
 * uniformArray[0][0],
 * uniformArray[0][1],
 * instead of
 * uniformArray[0],
 * uniformArray[1].
 */
static std::string
resolveUniformName(const GLchar *name,  GLint size)
{
    std::string qualifiedName(name);
    if (size > 1) {
        std::string::size_type nameLength = qualifiedName.length();
        static const char * const arrayStart = "[0]";
        static const int arrayStartLen = 3;
        if (qualifiedName.rfind(arrayStart) == (nameLength - arrayStartLen)) {
            qualifiedName = qualifiedName.substr(0, nameLength - 3);
        }
    }
    return qualifiedName;
}


struct AttribDesc
{
    GLenum type;
    GLenum size;

    GLenum elemType;
    GLint elemStride;

    GLint numCols;
    GLint numRows;

    GLsizei rowStride;
    GLsizei colStride;

    GLsizei arrayStride;

    AttribDesc() :
        type(GL_NONE),
        size(0),
        elemType(GL_NONE),
        elemStride(0),
        numCols(0),
        numRows(0),
        rowStride(0),
        colStride(0),
        arrayStride(0)
    {}

    AttribDesc(GLenum _type,
               GLint _size,
               GLint array_stride = 0,
               GLint matrix_stride = 0,
               GLboolean is_row_major = GL_FALSE) :
        type(_type),
        size(_size)
    {
        _gl_uniform_size(type, elemType, numCols, numRows);

        elemStride = _gl_type_size(elemType);

        rowStride = 0;
        colStride = 0;
        if (is_row_major) {
            rowStride = elemStride;
            if (!matrix_stride) {
                matrix_stride = numRows * elemStride;
            }
            colStride = matrix_stride;
        } else {
            colStride = elemStride;
            if (!matrix_stride) {
                matrix_stride = numCols * elemStride;
            }
            rowStride = matrix_stride;
        }

        arrayStride = array_stride ? array_stride : size * matrix_stride;
    }

    inline
    operator bool (void) const {
        return elemType != GL_NONE;
    }
};


static void
dumpAttrib(JSONWriter &json,
           const AttribDesc &desc,
           const GLbyte *data)
{
    assert(desc);

    if (desc.numRows > 1) {
        json.beginArray();
    }

    for (GLint row = 0; row < desc.numRows; ++row) {
        if (desc.numCols > 1) {
            json.beginArray();
        }

        for (GLint col = 0; col < desc.numCols; ++col) {
            union {
                const GLbyte *rawvalue;
                const GLfloat *fvalue;
                const GLdouble *dvalue;
                const GLint *ivalue;
                const GLuint *uivalue;
            } u;

            u.rawvalue = data + row*desc.rowStride + col*desc.colStride;

            switch (desc.elemType) {
            case GL_FLOAT:
                json.writeFloat(*u.fvalue);
                break;
            case GL_DOUBLE:
                json.writeFloat(*u.dvalue);
                break;
            case GL_INT:
                json.writeInt(*u.ivalue);
                break;
            case GL_UNSIGNED_INT:
                json.writeInt(*u.uivalue);
                break;
            case GL_BOOL:
                json.writeBool(*u.uivalue);
                break;
            default:
                assert(0);
                json.writeNull();
                break;
            }
        }

        if (desc.numCols > 1) {
            json.endArray();
        }
    }

    if (desc.numRows > 1) {
        json.endArray();
    }
}


static void
dumpAttribArray(JSONWriter &json,
                const std::string & name,
                const AttribDesc &desc,
                const GLbyte *data)
{
    json.beginMember(name);
    if (desc.size > 1) {
        json.beginArray();
    }

    for (GLint i = 0; i < desc.size; ++i) {
        const GLbyte *row = data + desc.arrayStride*i;

        dumpAttrib(json, desc, row);
    }

    if (desc.size > 1) {
        json.endArray();
    }
    json.endMember();
}


/**
 * Dump an uniform that belows to an uniform block.
 */
static void
dumpUniformBlock(JSONWriter &json,
                 GLint program,
                 GLint size,
                 GLenum type,
                 const GLchar *name,
                 GLuint index,
                 GLuint block_index)
{

    GLint offset = 0;
    GLint array_stride = 0;
    GLint matrix_stride = 0;
    GLint is_row_major = GL_FALSE;
    glGetActiveUniformsiv(program, 1, &index, GL_UNIFORM_OFFSET, &offset);
    glGetActiveUniformsiv(program, 1, &index, GL_UNIFORM_ARRAY_STRIDE, &array_stride);
    glGetActiveUniformsiv(program, 1, &index, GL_UNIFORM_MATRIX_STRIDE, &matrix_stride);
    glGetActiveUniformsiv(program, 1, &index, GL_UNIFORM_IS_ROW_MAJOR, &is_row_major);

    GLint slot = -1;
    glGetActiveUniformBlockiv(program, block_index, GL_UNIFORM_BLOCK_BINDING, &slot);
    if (slot == -1) {
        return;
    }

    AttribDesc desc(type, size, array_stride, matrix_stride, is_row_major);
    if (!desc) {
        return;
    }

    if (0) {
        GLint active_uniform_block_max_name_length = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &active_uniform_block_max_name_length);

        GLchar* block_name = new GLchar[active_uniform_block_max_name_length];

        GLint block_data_size = 0;
        glGetActiveUniformBlockiv(program, index, GL_UNIFORM_BLOCK_DATA_SIZE, &block_data_size);

        GLsizei length = 0;
        glGetActiveUniformBlockName(program, index, active_uniform_block_max_name_length, &length, block_name);

        std::cerr
            << "uniform `" << name << "`, size " << size << ", type " << enumToString(desc.type) << "\n"
            << "  block " << block_index << ", name `" << block_name << "`, size " << block_data_size << "; binding " << slot << "; \n"
            << "  offset " << offset << ", array stride " << array_stride << ", matrix stride " << matrix_stride << ", row_major " << is_row_major << "\n"
        ;

        delete block_name;
    }

    GLint ubo = 0;
    glGetIntegeri_v(GL_UNIFORM_BUFFER_BINDING, slot, &ubo);
    GLint start = 0;
    glGetIntegeri_v(GL_UNIFORM_BUFFER_START, slot, &start);

    BufferMapping mapping;
    const GLbyte *raw_data = (const GLbyte *)mapping.map(GL_UNIFORM_BUFFER, ubo);
    if (raw_data) {
        std::string qualifiedName = resolveUniformName(name, size);

        dumpAttribArray(json, qualifiedName, desc, raw_data + start + offset);
    }
}


static void
dumpUniform(JSONWriter &json,
            GLint program,
            const AttribDesc & desc,
            const GLchar *name)
{
    if (desc.elemType == GL_NONE) {
        return;
    }

    union {
        GLfloat fvalues[4*4];
        GLdouble dvalues[4*4];
        GLint ivalues[4*4];
        GLuint uivalues[4*4];
        GLbyte data[4*4*4];
    } u;

    GLint i;

    std::string qualifiedName = resolveUniformName(name, desc.size);

    json.beginMember(qualifiedName);
    if (desc.size > 1) {
        json.beginArray();
    }

    for (i = 0; i < desc.size; ++i) {
        std::stringstream ss;
        ss << qualifiedName;
        if (desc.size > 1) {
            ss << '[' << i << ']';
        }
        std::string elemName = ss.str();

        GLint location = glGetUniformLocation(program, elemName.c_str());
        assert(location != -1);
        if (location == -1) {
            continue;
        }

        switch (desc.elemType) {
        case GL_FLOAT:
            glGetUniformfv(program, location, u.fvalues);
            break;
        case GL_DOUBLE:
            glGetUniformdv(program, location, u.dvalues);
            break;
        case GL_INT:
            glGetUniformiv(program, location, u.ivalues);
            break;
        case GL_UNSIGNED_INT:
            glGetUniformuiv(program, location, u.uivalues);
            break;
        case GL_BOOL:
            glGetUniformiv(program, location, u.ivalues);
            break;
        default:
            assert(0);
            break;
        }

        dumpAttrib(json, desc, u.data);
    }

    if (desc.size > 1) {
        json.endArray();
    }

    json.endMember();
}


static inline void
dumpProgramUniforms(JSONWriter &json, GLint program)
{
    GLint active_uniforms = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &active_uniforms);
    if (!active_uniforms) {
        return;
    }

    GLint active_uniform_max_length = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &active_uniform_max_length);
    GLchar *name = new GLchar[active_uniform_max_length];
    if (!name) {
        return;
    }

    for (GLuint index = 0; (GLint)index < active_uniforms; ++index) {
        GLsizei length = 0;
        GLint size = 0;
        GLenum type = GL_NONE;
        glGetActiveUniform(program, index, active_uniform_max_length, &length, &size, &type, name);

        if (isBuiltinUniform(name)) {
            continue;
        }

        GLint location = glGetUniformLocation(program, name);
        if (location != -1) {
            AttribDesc desc(type, size);
            dumpUniform(json, program, desc, name);
            continue;
        }

        GLint block_index = -1;
        glGetActiveUniformsiv(program, 1, &index, GL_UNIFORM_BLOCK_INDEX, &block_index);
        if (block_index != -1) {
            dumpUniformBlock(json, program, size, type, name, index, block_index);
            continue;
        }

        /*
         * We can get here when the driver describes the uniform as active, but
         * on an inactive uniform block.  So just ingnore that.
         */
    }

    delete [] name;
}


// Calculate how many elements (or vertices) can fit in the specified buffer.
static unsigned
calcNumElements(GLsizei bufferSize,
                GLsizei elemOffset,
                GLsizei elemSize,
                GLsizei elemStride)
{
    if (0 >= bufferSize ||
        elemOffset >= bufferSize ||
        elemOffset + elemSize >= bufferSize) {
        return 0;
    }

    assert(elemStride >= 0);
    if (elemStride < 0) {
        return 1;
    }

    if (elemStride == 0) {
        // XXX: It should be infinite, but lets return something more manageable here.
        return 0x10000;
    }

    return (bufferSize - elemOffset - elemSize) / elemStride;
}


struct TransformFeedbackAttrib {
    std::string name;
    AttribDesc desc;
    GLsizei offset;
    GLsizei stride;
    GLsizei size;
    BufferMapping mapping;
    const GLbyte *map;
};


static inline void
dumpTransformFeedback(JSONWriter &json, GLint program)
{
    GLint transform_feedback_varyings = 0;
    glGetProgramiv(program, GL_TRANSFORM_FEEDBACK_VARYINGS, &transform_feedback_varyings);
    if (!transform_feedback_varyings) {
        return;
    }

    GLint max_name_length = 0;
    glGetProgramiv(program, GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH, &max_name_length);
    std::vector<GLchar> name(max_name_length);

    GLint buffer_mode = GL_INTERLEAVED_ATTRIBS;
    glGetProgramiv(program, GL_TRANSFORM_FEEDBACK_BUFFER_MODE, &buffer_mode);

    std::vector<TransformFeedbackAttrib> attribs(transform_feedback_varyings);

    // Calculate the offsets and strides of each attribute according to
    // the value of GL_TRANSFORM_FEEDBACK_BUFFER_MODE
    GLsizei cum_attrib_offset = 0;
    for (GLint slot = 0; slot < transform_feedback_varyings; ++slot) {
        TransformFeedbackAttrib & attrib = attribs[slot];

        GLsizei length = 0;
        GLsizei size = 0;
        GLenum type = GL_NONE;
        glGetTransformFeedbackVarying(program, slot, max_name_length, &length, &size, &type, &name[0]);

        attrib.name = &name[0];

        const AttribDesc & desc = attrib.desc = AttribDesc(type, size);
        if (!desc) {
            return;
        }

        attrib.size = desc.arrayStride;

        switch (buffer_mode) {
        case GL_INTERLEAVED_ATTRIBS:
            attrib.offset = cum_attrib_offset;
            break;
        case GL_SEPARATE_ATTRIBS:
            attrib.offset = 0;
            attrib.stride = desc.arrayStride;
            break;
        default:
            assert(0);
            attrib.offset = 0;
            attrib.stride = 0;
        }

        cum_attrib_offset += desc.arrayStride;
    }
    if (buffer_mode == GL_INTERLEAVED_ATTRIBS) {
        for (GLint slot = 0; slot < transform_feedback_varyings; ++slot) {
            TransformFeedbackAttrib & attrib = attribs[slot];
            attrib.stride = cum_attrib_offset;
        }
    }

    GLint previous_tbo = 0;
    glGetIntegerv(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, &previous_tbo);

    // Map the buffers and calculate how many vertices can they hold
    // XXX: We currently limit to 1024, or things can get significantly slow.
    unsigned numVertices = 16*1024;
    for (GLint slot = 0; slot < transform_feedback_varyings; ++slot) {
        TransformFeedbackAttrib & attrib = attribs[slot];
        attrib.map = NULL;
        if (slot == 0 || buffer_mode != GL_INTERLEAVED_ATTRIBS) {
            GLint tbo = 0;
            glGetIntegeri_v(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, slot, &tbo);
            if (!tbo) {
                numVertices = 0;
                continue;
            }

            GLint start = 0;
            glGetIntegeri_v(GL_TRANSFORM_FEEDBACK_BUFFER_START, slot, &start);
            GLint size = 0;
            glGetIntegeri_v(GL_TRANSFORM_FEEDBACK_BUFFER_SIZE, slot, &size);

            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tbo);

            if (size == 0) {
                glGetBufferParameteriv(GL_TRANSFORM_FEEDBACK_BUFFER, GL_BUFFER_SIZE, &size);
                assert(size >= start);
                size -= start;
            }

            unsigned numAttribVertices = calcNumElements(size,
                                                         attrib.offset,
                                                         attrib.size,
                                                         attrib.stride);
            numVertices = std::min(numVertices, numAttribVertices);

            attrib.map = (const GLbyte *)attrib.mapping.map(GL_TRANSFORM_FEEDBACK_BUFFER, tbo) + start;
        } else {
            attrib.map = attribs[0].map;
        }
    }
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, previous_tbo);

    // Actually dump the vertices
    json.beginMember("GL_TRANSFORM_FEEDBACK");
    json.beginArray();
    for (unsigned vertex = 0; vertex < numVertices; ++vertex) {
        json.beginObject();
        for (GLint slot = 0; slot < transform_feedback_varyings; ++slot) {
            TransformFeedbackAttrib & attrib = attribs[slot];
            if (!attrib.map) {
                continue;
            }

            const AttribDesc & desc = attrib.desc;
            assert(desc);

            const GLbyte *vertex_data = attrib.map + attrib.stride*vertex + attrib.offset;
            dumpAttribArray(json, attrib.name, desc, vertex_data);
        }
        json.endObject();
    }
    json.endArray();
    json.endMember();
}


static inline void
dumpArbProgram(JSONWriter &json, Context &context, GLenum target)
{
    if (context.ES ||
        context.core ||
        !glIsEnabled(target)) {
        return;
    }

    GLint program_length = 0;
    glGetProgramivARB(target, GL_PROGRAM_LENGTH_ARB, &program_length);
    if (!program_length) {
        return;
    }

    GLchar *source = new GLchar[program_length + 1];
    source[0] = 0;
    glGetProgramStringARB(target, GL_PROGRAM_STRING_ARB, source);
    source[program_length] = 0;

    json.beginMember(enumToString(target));
    json.writeString(source);
    json.endMember();

    delete [] source;
}


static inline void
dumpArbProgramUniforms(JSONWriter &json, Context &context, GLenum target, const char *prefix)
{
    if (context.ES ||
        context.core ||
        !glIsEnabled(target)) {
        return;
    }

    GLint program_parameters = 0;
    glGetProgramivARB(target, GL_PROGRAM_PARAMETERS_ARB, &program_parameters);
    if (!program_parameters) {
        return;
    }

    GLint max_program_local_parameters = 0;
    glGetProgramivARB(target, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB, &max_program_local_parameters);
    for (GLint index = 0; index < max_program_local_parameters; ++index) {
        GLdouble params[4] = {0, 0, 0, 0};
        glGetProgramLocalParameterdvARB(target, index, params);

        if (!params[0] && !params[1] && !params[2] && !params[3]) {
            continue;
        }

        char name[256];
        snprintf(name, sizeof name, "%sprogram.local[%i]", prefix, index);

        json.beginMember(name);
        json.beginArray();
        json.writeFloat(params[0]);
        json.writeFloat(params[1]);
        json.writeFloat(params[2]);
        json.writeFloat(params[3]);
        json.endArray();
        json.endMember();
    }

    GLint max_program_env_parameters = 0;
    glGetProgramivARB(target, GL_MAX_PROGRAM_ENV_PARAMETERS_ARB, &max_program_env_parameters);
    for (GLint index = 0; index < max_program_env_parameters; ++index) {
        GLdouble params[4] = {0, 0, 0, 0};
        glGetProgramEnvParameterdvARB(target, index, params);

        if (!params[0] && !params[1] && !params[2] && !params[3]) {
            continue;
        }

        char name[256];
        snprintf(name, sizeof name, "%sprogram.env[%i]", prefix, index);

        json.beginMember(name);
        json.beginArray();
        json.writeFloat(params[0]);
        json.writeFloat(params[1]);
        json.writeFloat(params[2]);
        json.writeFloat(params[3]);
        json.endArray();
        json.endMember();
    }
}

static void
dumpProgramUniformsStage(JSONWriter &json, GLint program, const char *stage)
{
    if (program <= 0) {
        return;
    }

    json.beginMember(stage);
    json.beginObject();
    dumpProgramUniforms(json, program);
    json.endObject();
    json.endMember();
}

void
dumpShadersUniforms(JSONWriter &json, Context &context)
{
    GLint pipeline = 0;
    GLint vertex_program = 0;
    GLint fragment_program = 0;
    GLint geometry_program = 0;
    GLint tess_control_program = 0;
    GLint tess_evaluation_program = 0;
    GLint compute_program = 0;

    if (!context.ES) {
        glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &pipeline);
        if (pipeline) {
            glGetProgramPipelineiv(pipeline, GL_VERTEX_SHADER, &vertex_program);
            glGetProgramPipelineiv(pipeline, GL_FRAGMENT_SHADER, &fragment_program);
            glGetProgramPipelineiv(pipeline, GL_GEOMETRY_SHADER, &geometry_program);
            glGetProgramPipelineiv(pipeline, GL_TESS_CONTROL_SHADER, &tess_control_program);
            glGetProgramPipelineiv(pipeline, GL_TESS_EVALUATION_SHADER, &tess_evaluation_program);
            glGetProgramPipelineiv(pipeline, GL_COMPUTE_SHADER, &compute_program);
        }
    }

    GLint program = 0;
    if (!pipeline) {
        glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    }

    json.beginMember("shaders");
    json.beginObject();
    if (pipeline) {
        dumpProgram(json, vertex_program);
        dumpProgram(json, fragment_program);
        dumpProgram(json, geometry_program);
        dumpProgram(json, tess_control_program);
        dumpProgram(json, tess_evaluation_program);
        dumpProgram(json, compute_program);
    } else if (program) {
        dumpProgram(json, program);
    } else {
        dumpArbProgram(json, context, GL_FRAGMENT_PROGRAM_ARB);
        dumpArbProgram(json, context, GL_VERTEX_PROGRAM_ARB);
    }
    json.endObject();
    json.endMember(); // shaders

    json.beginMember("uniforms");
    json.beginObject();
    if (pipeline) {
        dumpProgramUniformsStage(json, vertex_program, "GL_VERTEX_SHADER");
        dumpProgramUniformsStage(json, fragment_program, "GL_FRAGMENT_SHADER");
        dumpProgramUniformsStage(json, geometry_program, "GL_GEOMETRY_SHADER");
        dumpProgramUniformsStage(json, tess_control_program, "GL_TESS_CONTROL_SHADER");
        dumpProgramUniformsStage(json, tess_evaluation_program, "GL_TESS_EVALUATION_SHADER");
        dumpProgramUniformsStage(json, compute_program, "GL_COMPUTE_SHADER");
    } else if (program) {
        dumpProgramUniforms(json, program);
    } else {
        dumpArbProgramUniforms(json, context, GL_FRAGMENT_PROGRAM_ARB, "fp.");
        dumpArbProgramUniforms(json, context, GL_VERTEX_PROGRAM_ARB, "vp.");
    }
    json.endObject();
    json.endMember(); // uniforms

    json.beginMember("buffers");
    json.beginObject();
    if (program) {
        dumpTransformFeedback(json, program);
    }
    json.endObject();
    json.endMember(); // buffers
}


bool
isGeometryShaderBound(Context &context)
{
    GLint pipeline = 0;
    GLint program = 0;

    if (!context.ES) {
        glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &pipeline);
        if (pipeline) {
            glGetProgramPipelineiv(pipeline, GL_GEOMETRY_SHADER, &program);
        }
    }

    if (!pipeline) {
        glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    }

    if (!program) {
        return false;
    }

    GLint attached_shaders = 0;
    glGetProgramiv(program, GL_ATTACHED_SHADERS, &attached_shaders);
    if (!attached_shaders) {
        return false;
    }

    GLuint *shaders = new GLuint[attached_shaders];
    GLsizei count = 0;
    glGetAttachedShaders(program, attached_shaders, &count, shaders);
    for (GLsizei i = 0; i < count; ++i) {
        GLuint shader = shaders[i];
        if (!shader) {
            continue;
        }

        GLint shader_type = 0;
        glGetShaderiv(shader, GL_SHADER_TYPE, &shader_type);

        if (shader_type == GL_GEOMETRY_SHADER) {
            delete [] shaders;
            return true;
        }
    }

    delete [] shaders;

    return false;
}


} /* namespace glstate */
