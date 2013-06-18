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

#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>

#include "json.hpp"
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


static void
getShaderObjSource(ShaderMap &shaderMap, GLhandleARB shaderObj)
{
    if (!shaderObj) {
        return;
    }

    GLint object_type = 0;
    glGetObjectParameterivARB(shaderObj, GL_OBJECT_TYPE_ARB, &object_type);
    if (object_type != GL_SHADER_OBJECT_ARB) {
        return;
    }

    GLint shader_type = 0;
    glGetObjectParameterivARB(shaderObj, GL_OBJECT_SUBTYPE_ARB, &shader_type);
    if (!shader_type) {
        return;
    }

    GLint source_length = 0;
    glGetObjectParameterivARB(shaderObj, GL_OBJECT_SHADER_SOURCE_LENGTH_ARB, &source_length);
    if (!source_length) {
        return;
    }

    GLcharARB *source = new GLcharARB[source_length];
    GLsizei length = 0;
    source[0] = 0;
    glGetShaderSource(shaderObj, source_length, &length, source);

    shaderMap[enumToString(shader_type)] += source;

    delete [] source;
}


static inline void
dumpProgram(JSONWriter &json, GLint program)
{
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


static inline void
dumpProgramObj(JSONWriter &json, GLhandleARB programObj)
{
    GLint attached_shaders = 0;
    glGetObjectParameterivARB(programObj, GL_OBJECT_ATTACHED_OBJECTS_ARB, &attached_shaders);
    if (!attached_shaders) {
        return;
    }

    ShaderMap shaderMap;

    GLhandleARB *shaderObjs = new GLhandleARB[attached_shaders];
    GLsizei count = 0;
    glGetAttachedObjectsARB(programObj, attached_shaders, &count, shaderObjs);
    std::sort(shaderObjs, shaderObjs + count);
    for (GLsizei i = 0; i < count; ++ i) {
       getShaderObjSource(shaderMap, shaderObjs[i]);
    }
    delete [] shaderObjs;

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


static void
dumpUniformValues(JSONWriter &json, GLenum type, const void *values, GLint matrix_stride = 0, GLboolean is_row_major = GL_FALSE) {
    GLenum elemType;
    GLint numCols, numRows;
    _gl_uniform_size(type, elemType, numCols, numRows);
    if (!numCols || !numRows) {
        json.writeNull();
    }

    size_t elemSize = _gl_type_size(elemType);

    GLint row_stride = 0;
    GLint col_stride = 0;
    if (is_row_major) {
        row_stride = elemSize;
        col_stride = matrix_stride ? matrix_stride : numRows * elemSize;
    } else {
        col_stride = elemSize;
        row_stride = matrix_stride ? matrix_stride : numCols * elemSize;
    }

    if (numRows > 1) {
        json.beginArray();
    }

    for (GLint row = 0; row < numRows; ++row) {
        if (numCols > 1) {
            json.beginArray();
        }

        for (GLint col = 0; col < numCols; ++col) {
            union {
                const GLbyte *rawvalue;
                const GLfloat *fvalue;
                const GLdouble *dvalue;
                const GLint *ivalue;
                const GLuint *uivalue;
            } u;

            u.rawvalue = (const GLbyte *)values + row*row_stride + col*col_stride;

            switch (elemType) {
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

        if (numCols > 1) {
            json.endArray();
        }
    }

    if (numRows > 1) {
        json.endArray();
    }
}


/**
 * Dump an uniform that belows to an uniform block.
 */
static void
dumpUniformBlock(JSONWriter &json, GLint program, GLint size, GLenum type, const GLchar *name, GLuint index, GLuint block_index) {

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

    if (0) {
        GLint active_uniform_block_max_name_length = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &active_uniform_block_max_name_length);

        GLchar* block_name = new GLchar[active_uniform_block_max_name_length];

        GLint block_data_size = 0;
        glGetActiveUniformBlockiv(program, index, GL_UNIFORM_BLOCK_DATA_SIZE, &block_data_size);

        GLsizei length = 0;
        glGetActiveUniformBlockName(program, index, active_uniform_block_max_name_length, &length, block_name);

        std::cerr
            << "uniform `" << name << "`, size " << size << ", type " << enumToString(type) << "\n"
            << "  block " << block_index << ", name `" << block_name << "`, size " << block_data_size << "; binding " << slot << "; \n"
            << "  offset " << offset << ", array stride " << array_stride << ", matrix stride " << matrix_stride << ", row_major " << is_row_major << "\n"
        ;

        delete block_name;
    }

    GLint ubo = 0;
    glGetIntegeri_v(GL_UNIFORM_BUFFER_BINDING, slot, &ubo);

    GLint previous_ubo = 0;
    glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &previous_ubo);

    glBindBuffer(GL_UNIFORM_BUFFER, ubo);

    const GLbyte *raw_data = (const GLbyte *)glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_ONLY);
    if (raw_data) {
        std::string qualifiedName = resolveUniformName(name, size);

        for (GLint i = 0; i < size; ++i) {
            std::stringstream ss;
            ss << qualifiedName;

            if (size > 1) {
                ss << '[' << i << ']';
            }

            std::string elemName = ss.str();

            json.beginMember(elemName);

            const GLbyte *row = raw_data + offset + array_stride*i;

            dumpUniformValues(json, type, row, matrix_stride, is_row_major);

            json.endMember();
        }

        glUnmapBuffer(GL_UNIFORM_BUFFER);
    }

    glBindBuffer(GL_UNIFORM_BUFFER, previous_ubo);
}


static void
dumpUniform(JSONWriter &json, GLint program, GLint size, GLenum type, const GLchar *name) {
    GLenum elemType;
    GLint numCols, numRows;
    _gl_uniform_size(type, elemType, numCols, numRows);
    if (elemType == GL_NONE) {
        return;
    }

    union {
        GLfloat fvalues[4*4];
        GLdouble dvalues[4*4];
        GLint ivalues[4*4];
        GLuint uivalues[4*4];
    } u;

    GLint i;

    std::string qualifiedName = resolveUniformName(name, size);

    for (i = 0; i < size; ++i) {
        std::stringstream ss;
        ss << qualifiedName;

        if (size > 1) {
            ss << '[' << i << ']';
        }

        std::string elemName = ss.str();

        GLint location = glGetUniformLocation(program, elemName.c_str());
        assert(location != -1);
        if (location == -1) {
            continue;
        }

        json.beginMember(elemName);

        switch (elemType) {
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

        dumpUniformValues(json, type, &u);

        json.endMember();
    }
}


static void
dumpUniformARB(JSONWriter &json, GLhandleARB programObj, GLint size, GLenum type, const GLchar *name) {
    GLenum elemType;
    GLint numCols, numRows;
    _gl_uniform_size(type, elemType, numCols, numRows);
    GLint numElems = numCols * numRows;
    if (elemType == GL_NONE) {
        return;
    }

    GLfloat fvalues[4*4];
    union {
        GLdouble dvalues[4*4];
        GLfloat fvalues[4*4];
        GLint ivalues[4*4];
    } u;

    GLint i, j;

    std::string qualifiedName = resolveUniformName(name, size);

    for (i = 0; i < size; ++i) {
        std::stringstream ss;
        ss << qualifiedName;

        if (size > 1) {
            ss << '[' << i << ']';
        }

        std::string elemName = ss.str();

        json.beginMember(elemName);

        GLint location = glGetUniformLocationARB(programObj, elemName.c_str());
        if (location == -1) {
            continue;
        }

        switch (elemType) {
        case GL_DOUBLE:
            // glGetUniformdvARB does not exists
            glGetUniformfvARB(programObj, location, fvalues);
            for (j = 0; j < numElems; ++j) {
                u.dvalues[j] = fvalues[j];
            }
            break;
        case GL_FLOAT:
            glGetUniformfvARB(programObj, location, fvalues);
            break;
        case GL_UNSIGNED_INT:
            // glGetUniformuivARB does not exists
        case GL_INT:
            glGetUniformivARB(programObj, location, u.ivalues);
            break;
        case GL_BOOL:
            glGetUniformivARB(programObj, location, u.ivalues);
            break;
        default:
            assert(0);
            break;
        }

        dumpUniformValues(json, type, &u);

        json.endMember();
    }
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
            dumpUniform(json, program, size, type, name);
            continue;
        }

        GLint block_index = -1;
        glGetActiveUniformsiv(program, 1, &index, GL_UNIFORM_BLOCK_INDEX, &block_index);
        if (block_index != -1) {
            dumpUniformBlock(json, program, size, type, name, index, block_index);
            continue;
        }

        assert(0);
    }

    delete [] name;
}


static inline void
dumpProgramObjUniforms(JSONWriter &json, GLhandleARB programObj)
{
    GLint active_uniforms = 0;
    glGetObjectParameterivARB(programObj, GL_OBJECT_ACTIVE_UNIFORMS_ARB, &active_uniforms);
    if (!active_uniforms) {
        return;
    }

    GLint active_uniform_max_length = 0;
    glGetObjectParameterivARB(programObj, GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB, &active_uniform_max_length);
    GLchar *name = new GLchar[active_uniform_max_length];
    if (!name) {
        return;
    }

    for (GLint index = 0; index < active_uniforms; ++index) {
        GLsizei length = 0;
        GLint size = 0;
        GLenum type = GL_NONE;
        glGetActiveUniformARB(programObj, index, active_uniform_max_length, &length, &size, &type, name);

    if (isBuiltinUniform(name)) {
        continue;
    }

        dumpUniformARB(json, programObj, size, type, name);
    }

    delete [] name;
}


static inline void
dumpArbProgram(JSONWriter &json, GLenum target)
{
    if (!glIsEnabled(target)) {
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
dumpArbProgramUniforms(JSONWriter &json, GLenum target, const char *prefix)
{
    if (!glIsEnabled(target)) {
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
    if (program) {
        json.beginMember(stage);
        json.beginObject();
        dumpProgramUniforms(json, program);
        json.endObject();
        json.endMember();
    }
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
    GLhandleARB programObj = 0;
    if (!pipeline) {
        glGetIntegerv(GL_CURRENT_PROGRAM, &program);
        if (!context.ES && !program) {
            programObj = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
        }
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
    } else if (programObj) {
        dumpProgramObj(json, programObj);
    } else {
        dumpArbProgram(json, GL_FRAGMENT_PROGRAM_ARB);
        dumpArbProgram(json, GL_VERTEX_PROGRAM_ARB);
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
    } else if (programObj) {
        dumpProgramObjUniforms(json, programObj);
    } else {
        dumpArbProgramUniforms(json, GL_FRAGMENT_PROGRAM_ARB, "fp.");
        dumpArbProgramUniforms(json, GL_VERTEX_PROGRAM_ARB, "vp.");
    }
    json.endObject();
    json.endMember(); // uniforms
}


} /* namespace glstate */
