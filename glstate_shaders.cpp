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
dumpUniform(JSONWriter &json, GLint program, GLint size, GLenum type, const GLchar *name) {
    GLenum elemType;
    GLint numElems;
    __gl_uniform_size(type, elemType, numElems);
    if (elemType == GL_NONE) {
        return;
    }

    GLfloat fvalues[4*4];
    GLdouble dvalues[4*4];
    GLint ivalues[4*4];
    GLuint uivalues[4*4];

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

        GLint location = glGetUniformLocation(program, elemName.c_str());

        if (numElems > 1) {
            json.beginArray();
        }

        switch (elemType) {
        case GL_FLOAT:
            glGetUniformfv(program, location, fvalues);
            for (j = 0; j < numElems; ++j) {
                json.writeNumber(fvalues[j]);
            }
            break;
        case GL_DOUBLE:
            glGetUniformdv(program, location, dvalues);
            for (j = 0; j < numElems; ++j) {
                json.writeNumber(dvalues[j]);
            }
            break;
        case GL_INT:
            glGetUniformiv(program, location, ivalues);
            for (j = 0; j < numElems; ++j) {
                json.writeNumber(ivalues[j]);
            }
            break;
        case GL_UNSIGNED_INT:
            glGetUniformuiv(program, location, uivalues);
            for (j = 0; j < numElems; ++j) {
                json.writeNumber(uivalues[j]);
            }
            break;
        case GL_BOOL:
            glGetUniformiv(program, location, ivalues);
            for (j = 0; j < numElems; ++j) {
                json.writeBool(ivalues[j]);
            }
            break;
        default:
            assert(0);
            break;
        }

        if (numElems > 1) {
            json.endArray();
        }

        json.endMember();
    }
}


static void
dumpUniformARB(JSONWriter &json, GLhandleARB programObj, GLint size, GLenum type, const GLchar *name) {

    GLenum elemType;
    GLint numElems;
    __gl_uniform_size(type, elemType, numElems);
    if (elemType == GL_NONE) {
        return;
    }

    GLfloat fvalues[4*4];
    GLint ivalues[4*4];

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

        if (numElems > 1) {
            json.beginArray();
        }

        switch (elemType) {
        case GL_DOUBLE:
            // glGetUniformdvARB does not exists
        case GL_FLOAT:
            glGetUniformfvARB(programObj, location, fvalues);
            for (j = 0; j < numElems; ++j) {
                json.writeNumber(fvalues[j]);
            }
            break;
        case GL_UNSIGNED_INT:
            // glGetUniformuivARB does not exists
        case GL_INT:
            glGetUniformivARB(programObj, location, ivalues);
            for (j = 0; j < numElems; ++j) {
                json.writeNumber(ivalues[j]);
            }
            break;
        case GL_BOOL:
            glGetUniformivARB(programObj, location, ivalues);
            for (j = 0; j < numElems; ++j) {
                json.writeBool(ivalues[j]);
            }
            break;
        default:
            assert(0);
            break;
        }

        if (numElems > 1) {
            json.endArray();
        }

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

    for (GLint index = 0; index < active_uniforms; ++index) {
        GLsizei length = 0;
        GLint size = 0;
        GLenum type = GL_NONE;
        glGetActiveUniform(program, index, active_uniform_max_length, &length, &size, &type, name);

        dumpUniform(json, program, size, type, name);
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
        json.writeNumber(params[0]);
        json.writeNumber(params[1]);
        json.writeNumber(params[2]);
        json.writeNumber(params[3]);
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
        json.writeNumber(params[0]);
        json.writeNumber(params[1]);
        json.writeNumber(params[2]);
        json.writeNumber(params[3]);
        json.endArray();
        json.endMember();
    }
}


void
dumpShadersUniforms(JSONWriter &json)
{
    GLint program = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);

    GLhandleARB programObj = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);

    json.beginMember("shaders");
    json.beginObject();
    if (program) {
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
    if (program) {
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
