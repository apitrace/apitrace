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
#include <stdio.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "state_writer.hpp"
#include "glproc.hpp"
#include "glsize.hpp"
#include "glstate.hpp"
#include "glstate_internal.hpp"
#include "halffloat.hpp"


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
dumpProgram(StateWriter &writer, Context &context, GLint program)
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
        writer.beginMember(it->first);
        writer.writeString(it->second);
        writer.endMember();
    }

    // Dump NVIDIA GPU programs via GL_ARB_get_program_binary
    if (context.ARB_get_program_binary) {
        // Undocumented format.
        const GLint GL_PROGRAM_BINARY_NVX = 0x8e21;

        bool hasNvidiaFormat = false;
        GLint numBinaryFormats = 0;
        glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &numBinaryFormats);
        if (numBinaryFormats) {
            std::vector<GLint> binaryFormats(numBinaryFormats);
            glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, &binaryFormats[0]);
            if (std::find(binaryFormats.begin(), binaryFormats.end(),
                          GL_PROGRAM_BINARY_NVX) != binaryFormats.end()) {
                hasNvidiaFormat = true;
            }
        }

        // Avoid calling glGetProgramiv(GL_PROGRAM_BINARY_LENGTH) as it might
        // trigger unnecessary side effects, per
        // https://github.com/apitrace/apitrace/issues/365#issuecomment-275029429
        GLint binaryLength = 0;
        if (hasNvidiaFormat) {
            glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &binaryLength);
        }

        if (binaryLength > 0) {
            std::vector<char> binary(binaryLength);
            GLenum format = GL_NONE;
            glGetProgramBinary(program, binaryLength, NULL, &format, &binary[0]);
            if (format == GL_PROGRAM_BINARY_NVX) {
                if (0) {
                    FILE *fp = fopen("program.bin", "wb");
                    if (fp) {
                        fwrite(&binary[0], 1, binaryLength, fp);
                        fclose(fp);
                    }
                }

                // Extract NVIDIA GPU programs
                std::string str(binary.begin(), binary.end());
                size_t end = 0;
                while (true) {
                    // Each program starts with a !!NV header token
                    size_t start = str.find("!!NV", end);
                    if (start == std::string::npos) {
                        break;
                    }

                    // And is preceeded by a length DWORD
                    assert(start >= end + 4);
                    if (start < end + 4) {
                        break;
                    }
                    uint32_t length;
                    str.copy(reinterpret_cast<char *>(&length), 4, start - 4);
                    assert(start + length <= binaryLength);
                    if (start + length > binaryLength) {
                        break;
                    }

                    std::string nvProg = str.substr(start, length);

                    size_t eol = nvProg.find('\n');
                    std::string nvHeader = nvProg.substr(2, eol - 2);

                    writer.beginMember(nvHeader);
                    writer.writeString(nvProg);
                    writer.endMember();

                    end = start + length;
                }
            }
        }
    }
}


/**
 * Built-in uniforms/attributes need special treatment.
 */
static inline bool
isBuiltinName(const GLchar *name)
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
    GLenum type = GL_NONE;
    GLenum size = 0;

    GLenum elemType = GL_NONE;
    GLint elemStride = 0;

    GLint numCols = 0;
    GLint numRows = 0;

    GLsizei rowStride = 0;
    GLsizei colStride = 0;

    GLsizei arrayStride = 0;

    AttribDesc(void)
    {}

    AttribDesc(GLenum _type,
               GLint _size,
               GLint array_stride = 0,
               GLint matrix_stride = 0,
               GLboolean is_row_major = GL_FALSE) :
        type(_type),
        size(_size),
        arrayStride(array_stride)
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
            if (!array_stride) {
                arrayStride = numCols * colStride;
            }
        } else {
            colStride = elemStride;
            if (!matrix_stride) {
                matrix_stride = numCols * elemStride;
            }
            rowStride = matrix_stride;
            if (!array_stride) {
                arrayStride = numRows * rowStride;
            }
        }
    }

    inline
    operator bool (void) const {
        return elemType != GL_NONE;
    }
};


static void
dumpAttrib(StateWriter &writer,
           const AttribDesc &desc,
           const GLbyte *data)
{
    assert(desc);

    if (desc.numRows > 1) {
        writer.beginArray();
    }

    for (GLint row = 0; row < desc.numRows; ++row) {
        if (desc.numCols > 1) {
            writer.beginArray();
        }

        for (GLint col = 0; col < desc.numCols; ++col) {
            union {
                const GLbyte *rawvalue;
                const GLfloat *fvalue;
                const GLhalf *hvalue;
                const GLdouble *dvalue;
                const GLint *ivalue;
                const GLuint *uivalue;
                const GLint64 *i64value;
                const GLuint64 *ui64value;
            } u;

            u.rawvalue = data + row*desc.rowStride + col*desc.colStride;

            switch (desc.elemType) {
            case GL_HALF_FLOAT:
                writer.writeFloat(util_half_to_float(*u.hvalue));
                break;
            case GL_FLOAT:
                writer.writeFloat(*u.fvalue);
                break;
            case GL_DOUBLE:
                writer.writeFloat(*u.dvalue);
                break;
            case GL_INT:
                writer.writeInt(*u.ivalue);
                break;
            case GL_UNSIGNED_INT:
                writer.writeInt(*u.uivalue);
                break;
            case GL_INT64_ARB:
                writer.writeInt(*u.i64value);
                break;
            case GL_UNSIGNED_INT64_ARB:
                writer.writeInt(*u.ui64value);
                break;
            case GL_BOOL:
                writer.writeBool(*u.uivalue);
                break;
            default:
                assert(0);
                writer.writeNull();
                break;
            }
        }

        if (desc.numCols > 1) {
            writer.endArray();
        }
    }

    if (desc.numRows > 1) {
        writer.endArray();
    }
}


static void
dumpAttribArray(StateWriter &writer,
                const std::string & name,
                const AttribDesc &desc,
                const GLbyte *data)
{
    writer.beginMember(name);
    if (desc.size > 1) {
        writer.beginArray();
    }

    for (GLint i = 0; i < desc.size; ++i) {
        const GLbyte *row = data + desc.arrayStride*i;

        dumpAttrib(writer, desc, row);
    }

    if (desc.size > 1) {
        writer.endArray();
    }
    writer.endMember();
}


/**
 * Dump an uniform that belows to an uniform block.
 */
static void
dumpUniformBlock(StateWriter &writer,
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

        delete [] block_name;
    }

    GLint ubo = 0;
    glGetIntegeri_v(GL_UNIFORM_BUFFER_BINDING, slot, &ubo);
    GLint start = 0;
    glGetIntegeri_v(GL_UNIFORM_BUFFER_START, slot, &start);

    BufferMapping mapping;
    const GLbyte *raw_data = (const GLbyte *)mapping.map(GL_UNIFORM_BUFFER, ubo);
    if (raw_data) {
        std::string qualifiedName = resolveUniformName(name, size);

        dumpAttribArray(writer, qualifiedName, desc, raw_data + start + offset);
    }
}


static void
dumpUniform(StateWriter &writer,
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
        GLint64 i64values[4*4];
        GLuint64 ui64values[4*4];
        GLbyte data[4*4*4];
    } u;

    GLint i;

    std::string qualifiedName = resolveUniformName(name, desc.size);

    writer.beginMember(qualifiedName);
    if (desc.size > 1) {
        writer.beginArray();
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
        case GL_INT64_ARB:
            glGetUniformi64vARB(program, location, u.i64values);
            break;
        case GL_UNSIGNED_INT64_ARB:
            glGetUniformui64vARB(program, location, u.ui64values);
            break;
        case GL_BOOL:
            glGetUniformiv(program, location, u.ivalues);
            break;
        default:
            assert(0);
            break;
        }

        dumpAttrib(writer, desc, u.data);
    }

    if (desc.size > 1) {
        writer.endArray();
    }

    writer.endMember();
}


static inline void
dumpProgramUniforms(StateWriter &writer, GLint program)
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

        if (isBuiltinName(name)) {
            continue;
        }

        GLint location = glGetUniformLocation(program, name);
        if (location != -1) {
            AttribDesc desc(type, size);
            dumpUniform(writer, program, desc, name);
            continue;
        }

        GLint block_index = -1;
        glGetActiveUniformsiv(program, 1, &index, GL_UNIFORM_BLOCK_INDEX, &block_index);
        if (block_index != -1) {
            dumpUniformBlock(writer, program, size, type, name, index, block_index);
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
dumpTransformFeedback(StateWriter &writer, GLint program)
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
    writer.beginMember("GL_TRANSFORM_FEEDBACK");
    writer.beginArray();
    for (unsigned vertex = 0; vertex < numVertices; ++vertex) {
        writer.beginObject();
        for (GLint slot = 0; slot < transform_feedback_varyings; ++slot) {
            TransformFeedbackAttrib & attrib = attribs[slot];
            if (!attrib.map) {
                continue;
            }

            const AttribDesc & desc = attrib.desc;
            assert(desc);

            const GLbyte *vertex_data = attrib.map + attrib.stride*vertex + attrib.offset;
            dumpAttribArray(writer, attrib.name, desc, vertex_data);
        }
        writer.endObject();
    }
    writer.endArray();
    writer.endMember();
}


static inline void
dumpArbProgram(StateWriter &writer, Context &context, GLenum target)
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

    writer.beginMember(enumToString(target));
    writer.writeString(source);
    writer.endMember();

    delete [] source;
}


static inline void
dumpArbProgramUniforms(StateWriter &writer, Context &context, GLenum target, const char *prefix)
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

        writer.beginMember(name);
        writer.beginArray();
        writer.writeFloat(params[0]);
        writer.writeFloat(params[1]);
        writer.writeFloat(params[2]);
        writer.writeFloat(params[3]);
        writer.endArray();
        writer.endMember();
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

        writer.beginMember(name);
        writer.beginArray();
        writer.writeFloat(params[0]);
        writer.writeFloat(params[1]);
        writer.writeFloat(params[2]);
        writer.writeFloat(params[3]);
        writer.endArray();
        writer.endMember();
    }
}

static void
dumpProgramUniformsStage(StateWriter &writer, GLint program, const char *stage)
{
    if (program <= 0) {
        return;
    }

    writer.beginMember(stage);
    writer.beginObject();
    dumpProgramUniforms(writer, program);
    writer.endObject();
    writer.endMember();
}

struct VertexAttrib {
    std::string name;
    AttribDesc desc;
    GLsizei offset = 0;
    GLsizei stride = 0;
    GLsizei size = 0;
    const GLbyte *map;
};

static void
dumpVertexAttributes(StateWriter &writer, Context &context, GLint program)
{
    if (program <= 0) {
        return;
    }

    GLint activeAttribs = 0;
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &activeAttribs);
    if (!activeAttribs) {
        return;
    }

    GLint max_name_length = 0;
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_name_length);
    std::vector<GLchar> name(max_name_length);

    std::map<GLuint, BufferMapping> mappings;
    std::vector<VertexAttrib> attribs;
    unsigned count = ~0U;

    for (GLint index = 0; index < activeAttribs; ++index) {
        GLsizei length = 0;
        GLint shaderSize = 0;
        GLenum shaderType = GL_NONE;
        glGetActiveAttrib(program, index, max_name_length, &length, &shaderSize, &shaderType, &name[0]);

        if (isBuiltinName(&name[0])) {
            // Ignore built-in attributes
            continue;
        }

        GLint location = glGetAttribLocation(program, &name[0]);
        if (location < 0) {
            continue;
        }

        GLint buffer = 0;
        glGetVertexAttribiv(location, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &buffer);
        if (!buffer) {
            continue;
        }



        GLint size = 0;
        glGetVertexAttribiv(location, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size);
        GLint type = 0;
        glGetVertexAttribiv(location, GL_VERTEX_ATTRIB_ARRAY_TYPE, &type);
        GLint normalized = 0;
        glGetVertexAttribiv(location, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &normalized);
        GLint stride = 0;
        glGetVertexAttribiv(location, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride);
        GLvoid * pointer = 0;
        glGetVertexAttribPointerv(location, GL_VERTEX_ATTRIB_ARRAY_POINTER, &pointer);

        GLint offset = reinterpret_cast<intptr_t>(pointer);
        assert(offset >= 0);

        GLint divisor = 0;
        glGetVertexAttribiv(location, GL_VERTEX_ATTRIB_ARRAY_DIVISOR, &divisor);
        if (divisor) {
            // TODO: not clear the best way of presenting instanced attibutes on the dump
            std::cerr << "warning: dumping of instanced attributes (" << &name[0] << ") not yet supported\n";
            return;
        }

        if (size == GL_BGRA) {
            std::cerr << "warning: dumping of GL_BGRA attributes (" << &name[0] << ") not yet supported\n";
            size = 4;
        }

        AttribDesc desc(type, size);
        if (!desc) {
            std::cerr << "warning: dumping of packed attribute (" << &name[0] << ") not yet supported\n";
            // TODO: handle
            continue;
        }

        attribs.emplace_back();
        VertexAttrib &attrib = attribs.back();
        attrib.name = &name[0];

        // TODO handle normalized attributes
        if (normalized) {
            std::cerr << "warning: dumping of normalized attribute (" << &name[0] << ") not yet supported\n";
        }

        attrib.desc = desc;
        GLsizei attribSize = attrib.desc.arrayStride;

        if (stride == 0) {
            // tightly packed
            stride = attribSize;
        }

        attrib.offset = offset;
        attrib.stride = stride;

        BufferMapping &mapping = mappings[buffer];
        attrib.map = (const GLbyte *)mapping.map(GL_ARRAY_BUFFER, buffer);
        if (!attrib.map) {
            std::cerr << "warning: could not map buffer for attribute " << &name[0] << ", skipping vertex attribute dump\n";
            return;
        }

        BufferBinding bb(GL_ARRAY_BUFFER, buffer);
        GLint bufferSize = 0;
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);

        if (bufferSize <= offset ||
            bufferSize <= offset + attribSize) {
            return;
        } else {
            unsigned attribCount = (bufferSize - offset - attribSize)/stride + 1;
            count = std::min(count, attribCount);
        }
    }

    if (count == 0 || count == ~0U || attribs.empty()) {
        return;
    }

    writer.beginMember("vertices");
    writer.beginArray();
    for (unsigned vertex = 0; vertex < count; ++vertex) {
        writer.beginObject();
        for (auto attrib : attribs) {
            const AttribDesc & desc = attrib.desc;
            assert(desc);

            const GLbyte *vertex_data = attrib.map + attrib.stride*vertex + attrib.offset;
            dumpAttribArray(writer, attrib.name, desc, vertex_data);
        }
        writer.endObject();
    }
    writer.endArray();
    writer.endMember();
}


static std::vector<GLint>
getProgramResourcei(GLuint program, GLenum programInterface,
                    GLuint index,
                    std::vector<GLenum> const &properties,
                    GLsizei expectedNumberOfResults) {
    std::vector<GLint> result;
    result.resize(expectedNumberOfResults, GL_INVALID_VALUE);
    GLint actuallyWrittenProperties = -1;
    glGetProgramResourceiv(program, programInterface, index, properties.size(),
                           properties.data(), result.size(),
                           &actuallyWrittenProperties, result.data());
    result.resize(std::max(0, actuallyWrittenProperties));
    return result;
}

/**
 * Expects one result per property
 */
static std::vector<GLint>
getProgramResourcei(GLuint program, GLenum programInterface,
                    GLuint index,
                    std::vector<GLenum> const &properties)
{
    return getProgramResourcei(program, programInterface, index, properties,
                               properties.size());
}

static GLint
getProgramResourcei(GLuint program, GLenum programInterface, GLuint index,
                    GLenum property)
{
    auto result = getProgramResourcei(program, programInterface, index,
                                      std::vector<GLenum>{property});
    return result.at(0);
}

static std::string
getProgramResourceName(GLuint program, GLenum programInterface,
                       GLuint index)
{
    auto nameLength = getProgramResourcei(program, programInterface, index,
                                          std::vector<GLenum>{GL_NAME_LENGTH})
                          .at(0);
    std::vector<char> buffer;
    buffer.resize(nameLength);
    glGetProgramResourceName(program, programInterface, index, nameLength, nullptr, buffer.data());
    return {buffer.begin(), buffer.end()};
}

static void
dumpShadersStorageBufferBlocks(StateWriter &writer, Context &context,
                               GLuint program)
{
    if (!(context.ARB_shader_storage_buffer_object &&
          context.ARB_program_interface_query)) {
        return;
    }

    GLint numberOfActiveSSBBs = -1;
    glGetProgramInterfaceiv(program, GL_SHADER_STORAGE_BLOCK,
                            GL_ACTIVE_RESOURCES, &numberOfActiveSSBBs);
    if (numberOfActiveSSBBs > 0) {
        for (GLint ssbbResourceIndex = 0;
             ssbbResourceIndex < numberOfActiveSSBBs; ++ssbbResourceIndex) {
            GLint numberOfActiveVariables =
                getProgramResourcei(program, GL_SHADER_STORAGE_BLOCK,
                                    ssbbResourceIndex, GL_NUM_ACTIVE_VARIABLES);
            auto ssbbName = getProgramResourceName(
                program, GL_SHADER_STORAGE_BLOCK, ssbbResourceIndex);
            auto activeVariables = getProgramResourcei(
                program, GL_SHADER_STORAGE_BLOCK, ssbbResourceIndex,
                {GL_ACTIVE_VARIABLES}, numberOfActiveVariables);
            writer.beginMember(ssbbName);
            writer.beginObject();
            const GLint bufferBinding =
                getProgramResourcei(program, GL_SHADER_STORAGE_BLOCK,
                                    ssbbResourceIndex, GL_BUFFER_BINDING);
            writer.writeIntMember("GL_BUFFER_BINDING", bufferBinding);
            writer.writeIntMember(
                "GL_BUFFER_DATA_SIZE",
                getProgramResourcei(program, GL_SHADER_STORAGE_BLOCK,
                                    ssbbResourceIndex, GL_BUFFER_DATA_SIZE));

            for (auto property : {GL_REFERENCED_BY_VERTEX_SHADER,
                                    GL_REFERENCED_BY_TESS_CONTROL_SHADER,
                                    GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
                                    GL_REFERENCED_BY_GEOMETRY_SHADER,
                                    GL_REFERENCED_BY_FRAGMENT_SHADER,
                                    GL_REFERENCED_BY_COMPUTE_SHADER}) {
                const auto value =
                    getProgramResourcei(program, GL_SHADER_STORAGE_BLOCK,
                                        ssbbResourceIndex, property);
                if (value) {
                    writer.writeBoolMember(enumToString(property), true);
                }
            }

            writer.beginMember("activeVariables");
            writer.beginObject();
            for (GLint variableIndex : activeVariables) {
                auto variableName = getProgramResourceName(
                    program, GL_BUFFER_VARIABLE, variableIndex);
                writer.beginMember(variableName);
                writer.beginObject();

                for (auto property :
                     {GL_TYPE, GL_ARRAY_SIZE, GL_OFFSET, GL_BLOCK_INDEX,
                      GL_ARRAY_STRIDE, GL_MATRIX_STRIDE, GL_IS_ROW_MAJOR,
                      GL_TOP_LEVEL_ARRAY_SIZE, GL_TOP_LEVEL_ARRAY_STRIDE}) {
                    int value = getProgramResourcei(program, GL_BUFFER_VARIABLE,
                                                    variableIndex, property);
                    writer.writeIntMember(enumToString(property), value);
                }

                writer.endObject();
                writer.endMember(); // variableName
            }
            writer.endObject();
            writer.endMember(); // activeVariables
            writer.endObject();
            writer.endMember(); // ssbbName
        }
    }
}


void
dumpShadersUniforms(StateWriter &writer, Context &context)
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

    writer.beginMember("shaders");
    writer.beginObject();
    if (pipeline) {
        dumpProgram(writer, context, vertex_program);
        dumpProgram(writer, context, fragment_program);
        dumpProgram(writer, context, geometry_program);
        dumpProgram(writer, context, tess_control_program);
        dumpProgram(writer, context, tess_evaluation_program);
        dumpProgram(writer, context, compute_program);
    } else if (program) {
        dumpProgram(writer, context, program);
    } else {
        dumpArbProgram(writer, context, GL_FRAGMENT_PROGRAM_ARB);
        dumpArbProgram(writer, context, GL_VERTEX_PROGRAM_ARB);
    }
    writer.endObject();
    writer.endMember(); // shaders

    writer.beginMember("uniforms");
    writer.beginObject();
    if (pipeline) {
        dumpProgramUniformsStage(writer, vertex_program, "GL_VERTEX_SHADER");
        dumpProgramUniformsStage(writer, fragment_program, "GL_FRAGMENT_SHADER");
        dumpProgramUniformsStage(writer, geometry_program, "GL_GEOMETRY_SHADER");
        dumpProgramUniformsStage(writer, tess_control_program, "GL_TESS_CONTROL_SHADER");
        dumpProgramUniformsStage(writer, tess_evaluation_program, "GL_TESS_EVALUATION_SHADER");
        dumpProgramUniformsStage(writer, compute_program, "GL_COMPUTE_SHADER");
    } else if (program) {
        dumpProgramUniforms(writer, program);
    } else {
        dumpArbProgramUniforms(writer, context, GL_FRAGMENT_PROGRAM_ARB, "fp.");
        dumpArbProgramUniforms(writer, context, GL_VERTEX_PROGRAM_ARB, "vp.");
    }
    writer.endObject();
    writer.endMember(); // uniforms

    writer.beginMember("buffers");
    writer.beginObject();
    if (!context.ES) {
        if (pipeline) {
            dumpVertexAttributes(writer, context, vertex_program);
        } else {
            dumpVertexAttributes(writer, context, program);
        }
        if (program) {
            dumpTransformFeedback(writer, program);
        }
    }
    writer.endObject();
    writer.endMember(); // buffers

    writer.beginMember("shaderstoragebufferblocks");
    writer.beginObject();
    if (pipeline) {
        dumpShadersStorageBufferBlocks(writer, context, vertex_program);
        dumpShadersStorageBufferBlocks(writer, context, fragment_program);
        dumpShadersStorageBufferBlocks(writer, context, geometry_program);
        dumpShadersStorageBufferBlocks(writer, context, tess_control_program);
        dumpShadersStorageBufferBlocks(writer, context, tess_evaluation_program);
        dumpShadersStorageBufferBlocks(writer, context, compute_program);
    } else if (program) {
        dumpShadersStorageBufferBlocks(writer, context, program);
    }
    writer.endObject();
    writer.endMember(); // shaderstoragebufferblocks
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
