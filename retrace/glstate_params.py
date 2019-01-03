#!/usr/bin/env python
##########################################################################
#
# Copyright 2011 Jose Fonseca
# All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
##########################################################################/


'''Generate code to dump most GL state into JSON.'''


import retrace # to adjust sys.path

from specs.stdapi import *

from specs.gltypes import *
from specs.glparams import *


framebuffer_targets = [
    ('GL_DRAW_FRAMEBUFFER', 'GL_DRAW_FRAMEBUFFER_BINDING'),
    ('GL_READ_FRAMEBUFFER', 'GL_READ_FRAMEBUFFER_BINDING'),
]

class GetInflector:
    '''Objects that describes how to inflect.'''

    reduced_types = {
        B: I,
        E: I,
        I: F,
    }

    def __init__(self, radical, inflections, suffix = ''):
        self.radical = radical
        self.inflections = inflections
        self.suffix = suffix

    def reduced_type(self, type):
        if type in self.inflections:
            return type
        if type in self.reduced_types:
            return self.reduced_type(self.reduced_types[type])
        raise NotImplementedError

    def inflect(self, type):
        return self.radical + self.inflection(type) + self.suffix

    def inflection(self, type):
        type = self.reduced_type(type)
        assert type in self.inflections
        return self.inflections[type]

    def key(self):
        return self.radical + self.suffix

    def __str__(self):
        return self.key()


class StateGetter(Visitor):
    '''Type visitor that is able to extract the state via one of the glGet*
    functions.

    It will declare any temporary variable
    '''

    def __init__(self, radical=None, inflections=None, suffix='', inflector=None, key=None, pnameIdx=-1):
        if inflector is None:
            assert radical is not None
            assert inflections is not None
            self.inflector = GetInflector(radical, inflections, suffix)
        else:
            assert radical is None
            assert inflections is None
            assert suffix == ''
            self.inflector = inflector
        if key is None:
            self.key = self.inflector.key()
        else:
            self.key = key
        self.pnameIdx = pnameIdx

    def iter(self):
        for function, type, count, name in parameters:
            if self.key not in function.split(','):
                continue
            if type is X:
                continue
            yield type, count, name

    def __call__(self, *args):
        pname = args[self.pnameIdx]

        for type, count, name in self.iter():
            if name == pname:
                if count != 1:
                    type = Array(type, count)

                return type, self.visit(type, args)

        raise NotImplementedError

    def temp_name(self, args):
        '''Return the name of a temporary variable to hold the state.'''
        pname = args[self.pnameIdx]

        return pname[3:].lower()

    def visitConst(self, const, args):
        return self.visit(const.type, args)

    def visitScalar(self, type, args):
        temp_name = self.temp_name(args)
        elem_type = self.inflector.reduced_type(type)
        inflection = self.inflector.inflect(type)
        if inflection.endswith('v' + self.inflector.suffix):
            print('    %s %s = 0;' % (elem_type, temp_name))
            print('    %s(%s, &%s);' % (inflection, ', '.join(args), temp_name))
        else:
            print('    %s %s = %s(%s);' % (elem_type, temp_name, inflection, ', '.join(args)))
        return temp_name

    def visitString(self, string, args):
        temp_name = self.temp_name(args)
        inflection = self.inflector.inflect(string)
        assert not inflection.endswith('v' + self.inflector.suffix)
        print('    %s %s = (%s)%s(%s);' % (string, temp_name, string, inflection, ', '.join(args)))
        return temp_name

    def visitAlias(self, alias, args):
        return self.visitScalar(alias, args)

    def visitEnum(self, enum, args):
        return self.visitScalar(enum, args)

    def visitBitmask(self, bitmask, args):
        return self.visit(GLint, args)

    def visitArray(self, array, args):
        temp_name = self.temp_name(args)
        if array.length == '1':
            return self.visit(array.type)
        elem_type = self.inflector.reduced_type(array.type)
        inflection = self.inflector.inflect(array.type)
        assert inflection.endswith('v' + self.inflector.suffix)
        array_length = array.length
        if isinstance(array_length, int):
            # Static integer length
            print('    %s %s[%s + 1];' % (elem_type, temp_name, array_length))
        else:
            # Put the length in a variable to avoid recomputing it every time
            print('    size_t _%s_length = %s;' % (temp_name, array_length))
            array_length = '_%s_length' % temp_name
            # Allocate a dynamic sized array
            print('    %s *%s = _allocator.alloc<%s>(%s + 1);' % (elem_type, temp_name, elem_type, array_length))
        print('    memset(%s, 0, %s * sizeof *%s);' % (temp_name, array_length, temp_name))
        print('    %s[%s] = (%s)0xdeadc0de;' % (temp_name, array_length, elem_type))
        print('    if (%s) {' % array_length)
        print('        %s(%s, %s);' % (inflection, ', '.join(args), temp_name))
        print('    }')
        # Simple buffer overflow detection
        print('    assert(%s[%s] == (%s)0xdeadc0de);' % (temp_name, array_length, elem_type))
        return temp_name

    def visitOpaque(self, pointer, args):
        temp_name = self.temp_name(args)
        inflection = self.inflector.inflect(pointer)
        assert inflection.endswith('v' + self.inflector.suffix)
        print('    GLvoid *%s;' % temp_name)
        print('    %s(%s, &%s);' % (inflection, ', '.join(args), temp_name))
        return temp_name


glGet_inflector = GetInflector('glGet', {
    B: 'Booleanv',
    I: 'Integerv',
    F: 'Floatv',
    D: 'Doublev',
    S: 'String',
    P: 'Pointerv',
})

glGet = StateGetter(inflector = glGet_inflector)

# Same as glGet
glGet_texture = StateGetter(inflector = glGet_inflector, key = 'glGet_texture')


glGet_i = StateGetter('glGet', {
    B: 'Booleani_v',
    I: 'Integeri_v',
    F: 'Floati_v',
    D: 'Doublei_v',
}, key = 'glGet_i', pnameIdx=0)


glGetMaterial = StateGetter('glGetMaterial', {I: 'iv', F: 'fv'})
glGetLight = StateGetter('glGetLight', {I: 'iv', F: 'fv'})
glGetVertexAttrib = StateGetter('glGetVertexAttrib', {I: 'iv', F: 'fv', D: 'dv', P: 'Pointerv'})
glGetTexParameter = StateGetter('glGetTexParameter', {I: 'iv', F: 'fv'})
glGetTexEnv = StateGetter('glGetTexEnv', {I: 'iv', F: 'fv'})
glGetTexLevelParameter = StateGetter('glGetTexLevelParameter', {I: 'iv', F: 'fv'})
glGetShader = StateGetter('glGetShaderiv', {I: 'iv'})
glGetProgram = StateGetter('glGetProgram', {I: 'iv'})
glGetProgramARB = StateGetter('glGetProgram', {I: 'iv', F: 'fv', S: 'Stringv'}, 'ARB')
glGetFramebufferAttachmentParameter = StateGetter('glGetFramebufferAttachmentParameter', {I: 'iv'})
glGetSamplerParameter = StateGetter('glGetSamplerParameter', {I: 'iv', F: 'fv'})
glGetBufferParameter = StateGetter('glGetBuffer', {I: 'Parameteriv', P: 'Pointerv'}, key='glGetBufferParameter')


class JsonWriter(Visitor):
    '''Type visitor that will dump a value of the specified type through the
    JSON writer.
    
    It expects a previously declared StateWriter instance named "writer".'''

    def visitLiteral(self, literal, instance):
        if literal.kind == 'Bool':
            print('    writer.writeBool(%s);' % instance)
        elif literal.kind in ('SInt', 'Uint'):
            print('    writer.writeInt(%s);' % instance)
        elif literal.kind in ('Float', 'Double'):
            print('    writer.writeFloat(%s);' % instance)
        else:
            raise NotImplementedError

    def visitString(self, string, instance):
        assert string.length is None
        print('    writer.writeString((const char *)%s);' % instance)

    def visitEnum(self, enum, instance):
        if enum is GLboolean:
            print('    dumpBoolean(writer, %s);' % instance)
        elif enum is GLenum:
            print('    dumpEnum(writer, %s);' % instance)
        else:
            assert False
            print('    writer.writeInt(%s);' % instance)

    def visitBitmask(self, bitmask, instance):
        raise NotImplementedError

    def visitAlias(self, alias, instance):
        self.visit(alias.type, instance)

    def visitOpaque(self, opaque, instance):
        print('    writer.writeInt((size_t)%s);' % instance)

    __index = 0

    def visitArray(self, array, instance):
        index = '_i%u' % JsonWriter.__index
        JsonWriter.__index += 1
        print('    writer.beginArray();')
        print('    for (unsigned %s = 0; %s < %s; ++%s) {' % (index, index, array.length, index))
        self.visit(array.type, '%s[%s]' % (instance, index))
        print('    }')
        print('    writer.endArray();')



class StateDumper:
    '''Class to generate code to dump all GL state in JSON format via
    stdout.'''

    def __init__(self):
        pass

    def dump(self):
        print('#include <assert.h>')
        print('#include <string.h>')
        print()
        print('#include "state_writer.hpp"')
        print('#include "scoped_allocator.hpp"')
        print('#include "glproc.hpp"')
        print('#include "glsize.hpp"')
        print('#include "glstate.hpp"')
        print('#include "glstate_internal.hpp"')
        print()
        print('namespace glstate {')
        print()

        print('const char *')
        print('enumToString(GLenum pname)')
        print('{')
        print('    switch (pname) {')
        for name in GLenum.values:
            print('    case %s:' % name)
            print('        return "%s";' % name)
        print('    default:')
        print('        return NULL;')
        print('    }')
        print('}')
        print()

        print('static void')
        print('dumpTextureTargetParameters(StateWriter &writer, Context &context, GLenum target, GLuint texture)')
        print('{')
        print('    writer.beginMember(enumToString(target));')
        print('    writer.beginObject();')
        print('    dumpObjectLabel(writer, context, GL_TEXTURE, texture, "GL_TEXTURE_LABEL");')
        # ARB_texture_buffer forbids glGetTexParameter and
        # glGetTexLevelParameter for TEXTURE_BUFFER, but
        # ARB_texture_buffer_range introduced parameters which can be queries
        # with glGetTexLevelParameter...
        print('    if (target != GL_TEXTURE_BUFFER) {')
        self.dump_atoms(glGetTexParameter, 'target')
        print('    }')
        print('    if (!context.ES) {')
        print('        GLenum levelTarget;')
        print('        if (target == GL_TEXTURE_CUBE_MAP ||')
        print('            target == GL_TEXTURE_CUBE_MAP_ARRAY) {')
        print('            // Must pick a face')
        print('            levelTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X;')
        print('        } else {')
        print('            levelTarget = target;')
        print('        }')
        self.dump_atoms(glGetTexLevelParameter, 'levelTarget', '0')
        print('    }')
        print('    writer.endObject();')
        print('    writer.endMember(); // target')
        print('}')
        print()

        print('static void')
        print('dumpFramebufferAttachementParameters(StateWriter &writer, Context &context, GLenum target, GLenum attachment)')
        print('{')
        self.dump_attachment_parameters('target', 'attachment')
        print('}')
        print()

        print('void dumpParameters(StateWriter &writer, Context &context)')
        print('{')
        print('    ScopedAllocator _allocator;')
        print('    (void)_allocator;')
        print()
        print('    writer.beginMember("parameters");')
        print('    writer.beginObject();')
        
        self.dump_atoms(glGet)
        
        self.dump_material_params()
        self.dump_light_params()
        self.dump_vertex_attribs()
        self.dump_program_params()
        self.dump_buffer_parameters()
        self.dump_texture_parameters()
        self.dump_framebuffer_parameters()
        self.dump_labels()

        print('    writer.endObject();')
        print('    writer.endMember(); // parameters')
        print('}')
        print()
        
        print('} /*namespace glstate */')

    def dump_material_params(self):
        print('    if (!context.ES) {')
        for face in ['GL_FRONT', 'GL_BACK']:
            print('    writer.beginMember("%s");' % face)
            print('    writer.beginObject();')
            self.dump_atoms(glGetMaterial, face)
            print('    writer.endObject();')
        print('    }')
        print()

    def dump_light_params(self):
        print('    GLint max_lights = 0;')
        print('    _glGetIntegerv(GL_MAX_LIGHTS, &max_lights);')
        print('    for (GLint index = 0; index < max_lights; ++index) {')
        print('        GLenum light = GL_LIGHT0 + index;')
        print('        if (glIsEnabled(light)) {')
        print('            std::string name = std::string("GL_LIGHT") + std::to_string(index);')
        print('            writer.beginMember(name);')
        print('            writer.beginObject();')
        self.dump_atoms(glGetLight, '    GL_LIGHT0 + index')
        print('            writer.endObject();')
        print('            writer.endMember(); // GL_LIGHTi')
        print('        }')
        print('    }')
        print()

    def dump_sampler_params(self):
        print('    // GL_SAMPLER_BINDING')
        print('    if (context.ARB_sampler_objects) {')
        print('        GLint sampler_binding = 0;')
        print('        glGetIntegerv(GL_SAMPLER_BINDING, &sampler_binding);')
        print('        writer.beginMember("GL_SAMPLER_BINDING");')
        print('        writer.writeInt(sampler_binding);')
        print('        writer.endMember();')
        print('        if (sampler_binding) {')
        print('            writer.beginMember("GL_SAMPLER");')
        print('            writer.beginObject();')
        print('            dumpObjectLabel(writer, context, GL_SAMPLER, sampler_binding, "GL_SAMPLER_LABEL");')
        for _, _, name in glGetSamplerParameter.iter():
            self.dump_atom(glGetSamplerParameter, 'sampler_binding', name)
        print('           writer.endObject();')
        print('           writer.endMember(); // GL_SAMPLER')
        print('       }')
        print('    }')

    def texenv_param_target(self, name):
        if name == 'GL_TEXTURE_LOD_BIAS':
           return 'GL_TEXTURE_FILTER_CONTROL'
        elif name == 'GL_COORD_REPLACE':
           return 'GL_POINT_SPRITE'
        else:
           return 'GL_TEXTURE_ENV'

    def dump_texenv_params(self):
        for target in ['GL_TEXTURE_ENV', 'GL_TEXTURE_FILTER_CONTROL', 'GL_POINT_SPRITE']:
            print('    if (!context.ES) {')
            print('        writer.beginMember("%s");' % target)
            print('        writer.beginObject();')
            for _, _, name in glGetTexEnv.iter():
                if self.texenv_param_target(name) == target:
                    self.dump_atom(glGetTexEnv, target, name) 
            print('        writer.endObject();')
            print('    }')

    def dump_vertex_attribs(self):
        print('    GLint max_vertex_attribs = 0;')
        print('    _glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attribs);')
        print('    for (GLint index = 0; index < max_vertex_attribs; ++index) {')
        print('        std::string name = std::string("GL_VERTEX_ATTRIB_ARRAY") + std::to_string(index);')
        print('        writer.beginMember(name);')
        print('        writer.beginObject();')
        self.dump_atoms(glGetVertexAttrib, 'index')
        
        # Dump vertex attrib buffer label
        print('        GLint buffer_binding = 0;')
        print('        glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &buffer_binding);')
        print('        dumpObjectLabel(writer, context, GL_BUFFER, buffer_binding, "GL_VERTEX_ATTRIB_ARRAY_BUFFER_LABEL");')

        print('        writer.endObject();')
        print('        writer.endMember(); // GL_VERTEX_ATTRIB_ARRAYi')
        print('    }')
        print()

    object_bindings = [
        ('GL_BUFFER', 'GL_ARRAY_BUFFER_BINDING'),
        ('GL_BUFFER', 'GL_COLOR_ARRAY_BUFFER_BINDING'),
        ('GL_BUFFER', 'GL_EDGE_FLAG_ARRAY_BUFFER_BINDING'),
        ('GL_BUFFER', 'GL_ELEMENT_ARRAY_BUFFER_BINDING'),
        ('GL_BUFFER', 'GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING'),
        ('GL_BUFFER', 'GL_INDEX_ARRAY_BUFFER_BINDING'),
        ('GL_BUFFER', 'GL_NORMAL_ARRAY_BUFFER_BINDING'),
        ('GL_BUFFER', 'GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING'),
        ('GL_BUFFER', 'GL_TEXTURE_BUFFER'),
        ('GL_BUFFER', 'GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING'),
        ('GL_BUFFER', 'GL_TRANSFORM_FEEDBACK_BUFFER_BINDING'),
        ('GL_BUFFER', 'GL_VERTEX_ARRAY_BUFFER_BINDING'),
        ('GL_BUFFER', 'GL_WEIGHT_ARRAY_BUFFER_BINDING'),
        ('GL_FRAMEBUFFER', 'GL_DRAW_FRAMEBUFFER_BINDING'),
        ('GL_FRAMEBUFFER', 'GL_READ_FRAMEBUFFER_BINDING'),
        ('GL_PROGRAM', 'GL_CURRENT_PROGRAM'),
        ('GL_PROGRAM_PIPELINE', 'GL_PROGRAM_PIPELINE_BINDING'),
        ('GL_RENDERBUFFER', 'GL_RENDERBUFFER_BINDING'),
        ('GL_TRANSFORM_FEEDBACK', 'GL_TRANSFORM_FEEDBACK_BINDING'),
        ('GL_VERTEX_ARRAY', 'GL_VERTEX_ARRAY_BINDING'),
    ]

    def dump_labels(self):
        for object_type, object_binding in self.object_bindings:
            member_name = object_binding.replace('BINDING', 'LABEL')
            if member_name == object_binding:
                member_name += '_LABEL'
            print('    {')
            print('        GLint binding = 0;')
            print('        glGetIntegerv(%s, &binding);' % object_binding)
            print('        dumpObjectLabel(writer, context, %s, binding, "%s");' % (object_type, member_name))
            print('    }')

    program_targets = [
        'GL_FRAGMENT_PROGRAM_ARB',
        'GL_VERTEX_PROGRAM_ARB',
    ]

    def dump_program_params(self):
        for target in self.program_targets:
            print('    if (glIsEnabled(%s)) {' % target)
            print('        writer.beginMember("%s");' % target)
            print('        writer.beginObject();')
            self.dump_atoms(glGetProgramARB, target)
            print('        writer.endObject();')
            print('    }')

    buffer_targets = [
        ('GL_ARRAY_BUFFER', 'GL_ARRAY_BUFFER_BINDING', None),
        # FIXME: Causes Mesa to segfault (issue #332).  Disable for now.
        #('GL_ATOMIC_COUNTER_BUFFER', 'GL_ATOMIC_COUNTER_BUFFER_BINDING', 'GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS'),
        ('GL_COPY_READ_BUFFER', 'GL_COPY_READ_BUFFER_BINDING', None),
        ('GL_COPY_WRITE_BUFFER', 'GL_COPY_WRITE_BUFFER_BINDING', None),
        ('GL_DRAW_INDIRECT_BUFFER', 'GL_DRAW_INDIRECT_BUFFER_BINDING', None),
        ('GL_DISPATCH_INDIRECT_BUFFER', 'GL_DISPATCH_INDIRECT_BUFFER_BINDING', None),
        ('GL_ELEMENT_ARRAY_BUFFER', 'GL_ELEMENT_ARRAY_BUFFER_BINDING', None),
        ('GL_PIXEL_PACK_BUFFER', 'GL_PIXEL_PACK_BUFFER_BINDING', None),
        ('GL_PIXEL_UNPACK_BUFFER', 'GL_PIXEL_UNPACK_BUFFER_BINDING', None),
        ('GL_QUERY_BUFFER', 'GL_QUERY_BUFFER_BINDING', None),
        ('GL_SHADER_STORAGE_BUFFER', 'GL_SHADER_STORAGE_BUFFER_BINDING', 'GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS'),
        ('GL_TEXTURE_BUFFER', 'GL_TEXTURE_BUFFER', None),
        ('GL_TRANSFORM_FEEDBACK_BUFFER', 'GL_TRANSFORM_FEEDBACK_BUFFER_BINDING', 'GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS'),
        ('GL_UNIFORM_BUFFER', 'GL_UNIFORM_BUFFER_BINDING', 'GL_MAX_UNIFORM_BUFFER_BINDINGS'),
    ]

    def dump_buffer_parameters(self):
        # general binding points
        for target, binding, max_bindings in self.buffer_targets:
            print('    // %s' % target)
            print('    {')
            print('        writer.beginMember("%s");' % target)
            print('        writer.beginObject();')
            print('        GLint buffer = 0;')
            print('        glGetIntegerv(%s, &buffer);' % binding)
            print('        if (buffer) {')
            self.dump_atoms(glGetBufferParameter, target)
            print('        }')
            if max_bindings is not None:
                # indexed binding points
                start = target + '_START'
                size = target + '_SIZE'
                print('        GLint max_bindings = 0;')
                print('        glGetIntegerv(%s, &max_bindings);' % max_bindings)
                print('        if (max_bindings) {')
                print('            writer.beginMember("i");')
                print('            writer.beginArray();')
                print('            for (GLint i = 0; i < max_bindings; ++i) {')
                print('                writer.beginObject();')
                for pname in [binding, start, size]:
                    self.dump_atom(glGet_i, pname, 'i')
                print('                writer.endObject();')
                print('            }')
                print('            writer.endArray();')
                print('            writer.endMember();')
                print('        }')

            print('        writer.endObject();')
            print('        writer.endMember();')
            print('    }')
            print()

    def dump_texture_parameters(self):
        print('    {')
        print('        GLint active_texture = GL_TEXTURE0;')
        print('        glGetIntegerv(GL_ACTIVE_TEXTURE, &active_texture);')
        print('        GLint max_texture_coords = 0;')
        print('        if (!context.core) {')
        print('            glGetIntegerv(GL_MAX_TEXTURE_COORDS, &max_texture_coords);')
        print('        }')
        print('        GLint max_combined_texture_image_units = 0;')
        print('        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_combined_texture_image_units);')
        print('        max_combined_texture_image_units = std::max(max_combined_texture_image_units, 2);')
        print('        GLint max_units = std::max(max_combined_texture_image_units, max_texture_coords);')
        print('        for (GLint unit = 0; unit < max_units; ++unit) {')
        print('            std::string name = std::string("GL_TEXTURE") + std::to_string(unit);')
        print('            writer.beginMember(name);')
        print('            glActiveTexture(GL_TEXTURE0 + unit);')
        print('            writer.beginObject();')
        print()
        self.dump_atoms(glGet_texture)
        print()
        print('            for (unsigned i = 0; i < numTextureTargets; ++i) {')
        print('                GLenum target = textureTargets[i];')
        print('                GLboolean enabled = GL_FALSE;')
        print('                if (unit < max_texture_coords &&')
        print('                    (target == GL_TEXTURE_1D ||')
        print('                     target == GL_TEXTURE_2D ||')
        print('                     target == GL_TEXTURE_3D ||')
        print('                     target == GL_TEXTURE_CUBE_MAP ||')
        print('                     target == GL_TEXTURE_RECTANGLE)) {')
        print('                    glGetBooleanv(target, &enabled);')
        print('                    writer.beginMember(enumToString(target));')
        print('                    dumpBoolean(writer, enabled);')
        print('                    writer.endMember();')
        print('                }')
        print('                GLint texture = 0;')
        print('                GLenum binding = getTextureBinding(target);')
        print('                glGetIntegerv(binding, &texture);')
        print('                if (enabled || texture) {')
        print('                    dumpTextureTargetParameters(writer, context, target, texture);')
        print('                }')
        print('            }')
        print()
        self.dump_sampler_params()
        print()
        print('            if (unit < max_texture_coords) {')
        self.dump_texenv_params()
        print('            }')
        print('            writer.endObject();')
        print('            writer.endMember(); // GL_TEXTUREi')
        print('        }')
        print('        glActiveTexture(active_texture);')
        print('    }')
        print()

    def dump_framebuffer_parameters(self):
        print('    {')
        print('        GLint max_color_attachments = 0;')
        print('        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachments);')
        print('        GLint framebuffer;')
        for target, binding in framebuffer_targets:
            print('            // %s' % target)
            print('            framebuffer = 0;')
            print('            glGetIntegerv(%s, &framebuffer);' % binding)
            print('            if (framebuffer) {')
            print('                writer.beginMember("%s");' % target)
            print('                writer.beginObject();')
            print('                dumpObjectLabel(writer, context, GL_FRAMEBUFFER, framebuffer, "GL_FRAMEBUFFER_LABEL");')
            print('                for (GLint i = 0; i < max_color_attachments; ++i) {')
            print('                    GLint color_attachment = GL_COLOR_ATTACHMENT0 + i;')
            print('                    dumpFramebufferAttachementParameters(writer, context, %s, color_attachment);' % target)
            print('                }')
            print('                dumpFramebufferAttachementParameters(writer, context, %s, GL_DEPTH_ATTACHMENT);' % target)
            print('                dumpFramebufferAttachementParameters(writer, context, %s, GL_STENCIL_ATTACHMENT);' % target)
            print('                writer.endObject();')
            print('                writer.endMember(); // %s' % target)
            print('            }')
            print()
        print('    }')
        print()

    def dump_attachment_parameters(self, target, attachment):
        print('            {')
        print('                GLint object_type = GL_NONE;')
        print('                glGetFramebufferAttachmentParameteriv(%s, %s, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &object_type);' % (target, attachment))
        print('                if (object_type != GL_NONE) {')
        print('                    writer.beginMember(enumToString(%s));' % attachment)
        print('                    writer.beginObject();')
        self.dump_atoms(glGetFramebufferAttachmentParameter, target, attachment)
        print('                    GLint object_name = 0;')
        print('                    glGetFramebufferAttachmentParameteriv(%s, %s, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &object_name);' % (target, attachment))
        print('                    dumpObjectLabel(writer, context, object_type, object_name, "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_LABEL");')
        print('                    writer.endObject();')
        print('                    writer.endMember(); // GL_x_ATTACHMENT')
        print('                }')
        print('            }')

    def dump_atoms(self, getter, *args):
        for _, _, name in getter.iter():
            self.dump_atom(getter, *(args + (name,)))

    def dump_atom(self, getter, *args):
        name = args[getter.pnameIdx]

        print('        // %s' % name)
        print('        {')
        print('            flushErrors();')
        type, value = getter(*args)
        print('            if (glGetError() != GL_NO_ERROR) {')
        #print '                std::cerr << "warning: %s(%s) failed\\n";' % (inflection, name)
        print('                flushErrors();')
        print('            } else {')
        print('                writer.beginMember("%s");' % name)
        JsonWriter().visit(type, value)
        print('                writer.endMember();')
        print('            }')
        print('        }')
        print()


if __name__ == '__main__':
    StateDumper().dump()
