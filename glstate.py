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


from stdapi import *

from gltypes import *
from glparams import *


texture_targets = [
    ('GL_TEXTURE_1D', 'GL_TEXTURE_BINDING_1D'),
    ('GL_TEXTURE_2D', 'GL_TEXTURE_BINDING_2D'),
    ('GL_TEXTURE_3D', 'GL_TEXTURE_BINDING_3D'),
    #(GL_TEXTURE_CUBE_MAP, 'GL_TEXTURE_BINDING_CUBE_MAP')
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

    def __str__(self):
        return self.radical + self.suffix


class StateGetter(Visitor):
    '''Type visitor that is able to extract the state via one of the glGet*
    functions.

    It will declare any temporary variable
    '''

    def __init__(self, radical, inflections, suffix=''):
        self.inflector = GetInflector(radical, inflections)
        self.suffix = suffix

    def __call__(self, *args):
        pname = args[-1]

        for function, type, count, name in parameters:
            if type is X:
                continue
            if name == pname:
                if count != 1:
                    type = Array(type, str(count))

                return type, self.visit(type, args)

        raise NotImplementedError

    def temp_name(self, args):
        '''Return the name of a temporary variable to hold the state.'''
        pname = args[-1]

        return pname[3:].lower()

    def visit_const(self, const, args):
        return self.visit(const.type, args)

    def visit_scalar(self, type, args):
        temp_name = self.temp_name(args)
        elem_type = self.inflector.reduced_type(type)
        inflection = self.inflector.inflect(type)
        if inflection.endswith('v'):
            print '    %s %s = 0;' % (elem_type, temp_name)
            print '    %s(%s, &%s);' % (inflection + self.suffix, ', '.join(args), temp_name)
        else:
            print '    %s %s = %s(%s);' % (elem_type, temp_name, inflection + self.suffix, ', '.join(args))
        return temp_name

    def visit_string(self, string, args):
        temp_name = self.temp_name(args)
        inflection = self.inflector.inflect(string)
        assert not inflection.endswith('v')
        print '    %s %s = (%s)%s(%s);' % (string, temp_name, string, inflection + self.suffix, ', '.join(args))
        return temp_name

    def visit_alias(self, alias, args):
        return self.visit_scalar(alias, args)

    def visit_enum(self, enum, args):
        return self.visit(GLint, args)

    def visit_bitmask(self, bitmask, args):
        return self.visit(GLint, args)

    def visit_array(self, array, args):
        temp_name = self.temp_name(args)
        if array.length == '1':
            return self.visit(array.type)
        elem_type = self.inflector.reduced_type(array.type)
        inflection = self.inflector.inflect(array.type)
        assert inflection.endswith('v')
        print '    %s %s[%s];' % (elem_type, temp_name, array.length)
        print '    memset(%s, 0, %s * sizeof *%s);' % (temp_name, array.length, temp_name)
        print '    %s(%s, %s);' % (inflection + self.suffix, ', '.join(args), temp_name)
        return temp_name

    def visit_opaque(self, pointer, args):
        temp_name = self.temp_name(args)
        inflection = self.inflector.inflect(pointer)
        assert inflection.endswith('v')
        print '    GLvoid *%s;' % temp_name
        print '    %s(%s, &%s);' % (inflection + self.suffix, ', '.join(args), temp_name)
        return temp_name


glGet = StateGetter('glGet', {
    B: 'Booleanv',
    I: 'Integerv',
    F: 'Floatv',
    D: 'Doublev',
    S: 'String',
    P: 'Pointerv',
})

glGetMaterial = StateGetter('glGetMaterial', {I: 'iv', F: 'fv'})
glGetLight = StateGetter('glGetLight', {I: 'iv', F: 'fv'})
glGetVertexAttrib = StateGetter('glGetVertexAttrib', {I: 'iv', F: 'fv', D: 'dv', P: 'Pointerv'})
glGetTexParameter = StateGetter('glGetTexParameter', {I: 'iv', F: 'fv'})
glGetTexEnv = StateGetter('glGetTexEnv', {I: 'iv', F: 'fv'})
glGetTexLevelParameter = StateGetter('glGetTexLevelParameter', {I: 'iv', F: 'fv'})
glGetProgramARB = StateGetter('glGetProgram', {I: 'iv', F: 'fv', S: 'Stringv'}, 'ARB')


class JsonWriter(Visitor):
    '''Type visitor that will dump a value of the specified type through the
    JSON writer.
    
    It expects a previously declared JSONWriter instance named "json".'''

    def visit_literal(self, literal, instance):
        if literal.format == 'Bool':
            print '    json.writeBool(%s);' % instance
        elif literal.format in ('SInt', 'Uint', 'Float', 'Double'):
            print '    json.writeNumber(%s);' % instance
        else:
            raise NotImplementedError

    def visit_string(self, string, instance):
        assert string.length is None
        print '    json.writeString((const char *)%s);' % instance

    def visit_enum(self, enum, instance):
        if enum.expr == 'GLenum':
            print '    writeEnum(json, %s);' % instance
        else:
            print '    json.writeNumber(%s);' % instance

    def visit_bitmask(self, bitmask, instance):
        raise NotImplementedError

    def visit_alias(self, alias, instance):
        self.visit(alias.type, instance)

    def visit_opaque(self, opaque, instance):
        print '    json.writeNumber((size_t)%s);' % instance

    __index = 0

    def visit_array(self, array, instance):
        index = '__i%u' % JsonWriter.__index
        JsonWriter.__index += 1
        print '    json.beginArray();'
        print '    for (unsigned %s = 0; %s < %s; ++%s) {' % (index, index, array.length, index)
        self.visit(array.type, '%s[%s]' % (instance, index))
        print '    }'
        print '    json.endArray();'



class StateDumper:
    '''Class to generate code to dump all GL state in JSON format via
    stdout.'''

    def __init__(self):
        pass

    def dump(self):
        print '#include <string.h>'
        print '#include <iostream>'
        print '#include <algorithm>'
        print
        print '#include "image.hpp"'
        print '#include "json.hpp"'
        print '#include "glimports.hpp"'
        print '#include "glproc.hpp"'
        print '#include "glsize.hpp"'
        print '#include "glretrace.hpp"'
        print

        print 'static const char *'
        print '_enum_string(GLenum pname)'
        print '{'
        print '    switch(pname) {'
        for name in GLenum.values:
            print '    case %s:' % name
            print '        return "%s";' % name
        print '    default:'
        print '        return NULL;'
        print '    }'
        print '}'
        print

        print 'static const char *'
        print 'enum_string(GLenum pname)'
        print '{'
        print '    const char *s = _enum_string(pname);'
        print '    if (s) {'
        print '        return s;'
        print '    } else {'
        print '        static char buf[16];'
        print '        snprintf(buf, sizeof buf, "0x%04x", pname);'
        print '        return buf;'
        print '    }'
        print '}'
        print

        print 'static inline void'
        print 'writeEnum(JSONWriter &json, GLenum pname)'
        print '{'
        print '    const char *s = _enum_string(pname);'
        print '    if (s) {'
        print '        json.writeString(s);'
        print '    } else {'
        print '        json.writeNumber(pname);'
        print '    }'
        print '}'
        print

        # shaders
        print '''
static void
writeShader(JSONWriter &json, GLuint shader)
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

    json.beginMember(enum_string(shader_type));
    json.writeString(source);
    json.endMember();

    delete [] source;
}

static inline void
writeCurrentProgram(JSONWriter &json)
{
    GLint program = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    if (!program) {
        return;
    }

    GLint attached_shaders = 0;
    glGetProgramiv(program, GL_ATTACHED_SHADERS, &attached_shaders);
    if (!attached_shaders) {
        return;
    }

    GLuint *shaders = new GLuint[attached_shaders];
    GLsizei count = 0;
    glGetAttachedShaders(program, attached_shaders, &count, shaders);
    for (GLsizei i = 0; i < count; ++ i) {
       writeShader(json, shaders[i]);
    }
    delete [] shaders;
}

static inline void
writeArbProgram(JSONWriter &json, GLenum target)
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

    json.beginMember(enum_string(target));
    json.writeString(source);
    json.endMember();

    delete [] source;
}
'''

        # texture image
        print '''
static inline void
writeTextureImage(JSONWriter &json, GLenum target, GLint level)
{
    GLint width, height = 1, depth = 1;

    width = 0;
    glGetTexLevelParameteriv(target, level, GL_TEXTURE_WIDTH, &width);

    if (target != GL_TEXTURE_1D) {
        height = 0;
        glGetTexLevelParameteriv(target, level, GL_TEXTURE_HEIGHT, &height);
        if (target == GL_TEXTURE_3D) {
            depth = 0;
            glGetTexLevelParameteriv(target, level, GL_TEXTURE_DEPTH, &depth);
        }
    }

    if (width <= 0 || height <= 0 || depth <= 0) {
        json.writeNull();
    } else {
        json.beginObject();

        // Tell the GUI this is no ordinary object, but an image
        json.writeStringMember("__class__", "image");

        json.writeNumberMember("__width__", width);
        json.writeNumberMember("__height__", height);
        json.writeNumberMember("__depth__", depth);

        // Hardcoded for now, but we could chose types more adequate to the
        // texture internal format
        json.writeStringMember("__type__", "uint8");
        json.writeBoolMember("__normalized__", true);
        json.writeNumberMember("__channels__", 4);
        
        GLubyte *pixels = new GLubyte[depth*width*height*4];
        
        glGetTexImage(target, level, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        json.beginMember("__data__");
        char *pngBuffer;
        int pngBufferSize;
        Image::writePixelsToBuffer(pixels, width, height, 4, false, &pngBuffer, &pngBufferSize);
        json.writeBase64(pngBuffer, pngBufferSize);
        free(pngBuffer);
        json.endMember(); // __data__

        delete [] pixels;
        json.endObject();
    }
}

static inline void
writeDrawBufferImage(JSONWriter &json, GLenum format)
{
    GLint width  = glretrace::window_width;
    GLint height = glretrace::window_height;

    GLint channels = __gl_format_channels(format);

    if (!width || !height) {
        json.writeNull();
    } else {
        json.beginObject();

        // Tell the GUI this is no ordinary object, but an image
        json.writeStringMember("__class__", "image");

        json.writeNumberMember("__width__", width);
        json.writeNumberMember("__height__", height);
        json.writeNumberMember("__depth__", 1);

        // Hardcoded for now, but we could chose types more adequate to the
        // texture internal format
        json.writeStringMember("__type__", "uint8");
        json.writeBoolMember("__normalized__", true);
        json.writeNumberMember("__channels__", channels);

        GLubyte *pixels = new GLubyte[width*height*channels];
        
        GLint drawbuffer = glretrace::double_buffer ? GL_BACK : GL_FRONT;
        GLint readbuffer = glretrace::double_buffer ? GL_BACK : GL_FRONT;
        glGetIntegerv(GL_DRAW_BUFFER, &drawbuffer);
        glGetIntegerv(GL_READ_BUFFER, &readbuffer);
        glReadBuffer(drawbuffer);

        glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);

        glReadPixels(0, 0, width, height, format, GL_UNSIGNED_BYTE, pixels);

        glPopClientAttrib();
        glReadBuffer(readbuffer);

        json.beginMember("__data__");
        char *pngBuffer;
        int pngBufferSize;
        Image::writePixelsToBuffer(pixels, width, height, channels, false, &pngBuffer, &pngBufferSize);
        //std::cerr <<" Before = "<<(width * height * channels * sizeof *pixels)
        //          <<", after = "<<pngBufferSize << ", ratio = " << double(width * height * channels * sizeof *pixels)/pngBufferSize;
        json.writeBase64(pngBuffer, pngBufferSize);
        free(pngBuffer);
        json.endMember(); // __data__

        delete [] pixels;
        json.endObject();
    }
}

static inline GLuint
downsampledFramebuffer(GLuint oldFbo, GLint drawbuffer,
                       GLint colorRb, GLint depthRb, GLint stencilRb,
                       GLuint *rbs, GLint *numRbs)
{
    GLuint fbo;
    GLint format;
    GLint w, h;

    *numRbs = 0;

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glBindRenderbuffer(GL_RENDERBUFFER, colorRb);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                 GL_RENDERBUFFER_WIDTH, &w);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                 GL_RENDERBUFFER_HEIGHT, &h);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                 GL_RENDERBUFFER_INTERNAL_FORMAT, &format);

    glGenRenderbuffers(1, &rbs[*numRbs]);
    glBindRenderbuffer(GL_RENDERBUFFER, rbs[*numRbs]);
    glRenderbufferStorage(GL_RENDERBUFFER, format, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, drawbuffer,
                              GL_RENDERBUFFER, rbs[*numRbs]);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, oldFbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glDrawBuffer(drawbuffer);
    glReadBuffer(drawbuffer);
    glBlitFramebuffer(0, 0, w, h, 0, 0, w, h,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    ++*numRbs;

    if (stencilRb == depthRb && stencilRb) {
        //combined depth and stencil buffer
        glBindRenderbuffer(GL_RENDERBUFFER, depthRb);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                     GL_RENDERBUFFER_WIDTH, &w);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                     GL_RENDERBUFFER_HEIGHT, &h);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                     GL_RENDERBUFFER_INTERNAL_FORMAT, &format);

        glGenRenderbuffers(1, &rbs[*numRbs]);
        glBindRenderbuffer(GL_RENDERBUFFER, rbs[*numRbs]);
        glRenderbufferStorage(GL_RENDERBUFFER, format, w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER, rbs[*numRbs]);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, oldFbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glBlitFramebuffer(0, 0, w, h, 0, 0, w, h,
                          GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        ++*numRbs;
    } else {
        if (depthRb) {
            glBindRenderbuffer(GL_RENDERBUFFER, depthRb);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                         GL_RENDERBUFFER_WIDTH, &w);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                         GL_RENDERBUFFER_HEIGHT, &h);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                         GL_RENDERBUFFER_INTERNAL_FORMAT, &format);

            glGenRenderbuffers(1, &rbs[*numRbs]);
            glBindRenderbuffer(GL_RENDERBUFFER, rbs[*numRbs]);
            glRenderbufferStorage(GL_RENDERBUFFER, format, w, h);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                      GL_DEPTH_ATTACHMENT,
                                      GL_RENDERBUFFER, rbs[*numRbs]);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, oldFbo);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
            glDrawBuffer(GL_DEPTH_ATTACHMENT);
            glReadBuffer(GL_DEPTH_ATTACHMENT);
            glBlitFramebuffer(0, 0, w, h, 0, 0, w, h,
                              GL_DEPTH_BUFFER_BIT, GL_NEAREST);
            ++*numRbs;
        }
        if (stencilRb) {
            glBindRenderbuffer(GL_RENDERBUFFER, stencilRb);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                         GL_RENDERBUFFER_WIDTH, &w);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                         GL_RENDERBUFFER_HEIGHT, &h);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                     GL_RENDERBUFFER_INTERNAL_FORMAT, &format);

            glGenRenderbuffers(1, &rbs[*numRbs]);
            glBindRenderbuffer(GL_RENDERBUFFER, rbs[*numRbs]);
            glRenderbufferStorage(GL_RENDERBUFFER, format, w, h);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                      GL_STENCIL_ATTACHMENT,
                                      GL_RENDERBUFFER, rbs[*numRbs]);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, oldFbo);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
            glDrawBuffer(GL_STENCIL_ATTACHMENT);
            glReadBuffer(GL_STENCIL_ATTACHMENT);
            glBlitFramebuffer(0, 0, w, h, 0, 0, w, h,
                              GL_STENCIL_BUFFER_BIT, GL_NEAREST);
            ++*numRbs;
        }
    }

    return fbo;
}

static void
writeDrawBuffers(JSONWriter &json, GLboolean writeDepth, GLboolean writeStencil)
{
    json.beginMember("GL_RGBA");
    writeDrawBufferImage(json, GL_RGBA);
    json.endMember();

    if (writeDepth) {
        json.beginMember("GL_DEPTH_COMPONENT");
        writeDrawBufferImage(json, GL_DEPTH_COMPONENT);
        json.endMember();
    }

    if (writeStencil) {
        json.beginMember("GL_STENCIL_INDEX");
        writeDrawBufferImage(json, GL_STENCIL_INDEX);
        json.endMember();
    }
}

'''

        # textures
        print 'static inline void'
        print 'writeTexture(JSONWriter &json, GLenum target, GLenum binding)'
        print '{'
        print '    GLint texture = 0;'
        print '    glGetIntegerv(binding, &texture);'
        print '    if (!glIsEnabled(target) && !texture) {'
        print '        json.writeNull();'
        print '        return;'
        print '    }'
        print
        print '    json.beginObject();'
        print '    json.beginMember("levels");'
        print '    json.beginArray();'
        print '    GLint level = 0;'
        print '    do {'
        print '        GLint width = 0, height = 0;'
        print '        glGetTexLevelParameteriv(target, level, GL_TEXTURE_WIDTH, &width);'
        print '        glGetTexLevelParameteriv(target, level, GL_TEXTURE_HEIGHT, &height);'
        print '        if (!width || !height) {'
        print '            break;'
        print '        }'
        print '        json.beginObject();'
        print
        # FIXME: This is not the original texture name in the trace -- we need
        # to do a reverse lookup of the texture mappings to find the original one
        print '        json.beginMember("binding");'
        print '        json.writeNumber(texture);'
        print '        json.endMember();'
        print
        print '        json.beginMember("image");'
        print '        writeTextureImage(json, target, level);'
        print '        json.endMember();'
        print
        print '        json.endObject();'
        print '        ++level;'
        print '    } while(true);'
        print '    json.endArray();'
        print '    json.endMember(); // levels'
        print
        print '    json.endObject();'
        print '}'
        print

        print 'void glretrace::state_dump(std::ostream &os)'
        print '{'
        print '    JSONWriter json(os);'
        self.dump_parameters()
        self.dump_current_program()
        self.dump_textures()
        self.dump_framebuffer()
        print '}'
        print

    def dump_parameters(self):
        print '    json.beginMember("parameters");'
        print '    json.beginObject();'
        
        self.dump_atoms(glGet)
        
        self.dump_material_params()
        self.dump_light_params()
        self.dump_vertex_attribs()
        self.dump_texenv_params()
        self.dump_program_params()
        self.dump_texture_parameters()

        print '    json.endObject();'
        print '    json.endMember(); // parameters'
        print

    def dump_material_params(self):
        for face in ['GL_FRONT', 'GL_BACK']:
            print '    json.beginMember("%s");' % face
            print '    json.beginObject();'
            self.dump_atoms(glGetMaterial, face)
            print '    json.endObject();'
        print

    def dump_light_params(self):
        print '    GLint max_lights = 0;'
        print '    __glGetIntegerv(GL_MAX_LIGHTS, &max_lights);'
        print '    for (GLint index = 0; index < max_lights; ++index) {'
        print '        GLenum light = GL_LIGHT0 + index;'
        print '        if (glIsEnabled(light)) {'
        print '            char name[32];'
        print '            snprintf(name, sizeof name, "GL_LIGHT%i", index);'
        print '            json.beginMember(name);'
        print '            json.beginObject();'
        self.dump_atoms(glGetLight, '    GL_LIGHT0 + index')
        print '            json.endObject();'
        print '            json.endMember(); // GL_LIGHTi'
        print '        }'
        print '    }'
        print

    def dump_texenv_params(self):
        for target in ['GL_TEXTURE_ENV', 'GL_TEXTURE_FILTER_CONTROL', 'GL_POINT_SPRITE']:
            if target != 'GL_TEXTURE_FILTER_CONTROL':
                print '    if (glIsEnabled(%s)) {' % target
            else:
                print '    {'
            print '        json.beginMember("%s");' % target
            print '        json.beginObject();'
            self.dump_atoms(glGetTexEnv, target)
            print '        json.endObject();'
            print '    }'

    def dump_vertex_attribs(self):
        print '    GLint max_vertex_attribs = 0;'
        print '    __glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attribs);'
        print '    for (GLint index = 0; index < max_vertex_attribs; ++index) {'
        print '        char name[32];'
        print '        snprintf(name, sizeof name, "GL_VERTEX_ATTRIB_ARRAY%i", index);'
        print '        json.beginMember(name);'
        print '        json.beginObject();'
        self.dump_atoms(glGetVertexAttrib, 'index')
        print '        json.endObject();'
        print '        json.endMember(); // GL_VERTEX_ATTRIB_ARRAYi'
        print '    }'
        print

    program_targets = [
        'GL_FRAGMENT_PROGRAM_ARB',
        'GL_VERTEX_PROGRAM_ARB',
    ]

    def dump_program_params(self):
        for target in self.program_targets:
            print '    if (glIsEnabled(%s)) {' % target
            print '        json.beginMember("%s");' % target
            print '        json.beginObject();'
            self.dump_atoms(glGetProgramARB, target)
            print '        json.endObject();'
            print '    }'

    def dump_texture_parameters(self):
        print '    {'
        print '        GLint active_texture = GL_TEXTURE0;'
        print '        glGetIntegerv(GL_ACTIVE_TEXTURE, &active_texture);'
        print '        GLint max_texture_coords = 0;'
        print '        glGetIntegerv(GL_MAX_TEXTURE_COORDS, &max_texture_coords);'
        print '        GLint max_combined_texture_image_units = 0;'
        print '        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_combined_texture_image_units);'
        print '        GLint max_units = std::max(max_combined_texture_image_units, max_texture_coords);'
        print '        for (GLint unit = 0; unit < max_units; ++unit) {'
        print '            char name[32];'
        print '            snprintf(name, sizeof name, "GL_TEXTURE%i", unit);'
        print '            json.beginMember(name);'
        print '            glActiveTexture(GL_TEXTURE0 + unit);'
        print '            json.beginObject();'
        print '            GLint texture;'
        print
        for target, binding in texture_targets:
            print '            // %s' % target
            print '            texture = 0;'
            print '            glGetIntegerv(%s, &texture);' % binding
            print '            if (glIsEnabled(%s) || texture) {' % target
            print '                json.beginMember("%s");' % target
            print '                json.beginObject();'
            self.dump_atoms(glGetTexParameter, target)
            # We only dump the first level parameters
            self.dump_atoms(glGetTexLevelParameter, target, "0")
            print '                json.endObject();'
            print '                json.endMember(); // %s' % target
            print '            }'
            print
        print '            json.endObject();'
        print '            json.endMember(); // GL_TEXTUREi'
        print '        }'
        print '        glActiveTexture(active_texture);'
        print '    }'
        print

    def dump_current_program(self):
        print '    json.beginMember("shaders");'
        print '    json.beginObject();'
        print '    writeCurrentProgram(json);'
        for target in self.program_targets:
            print '    writeArbProgram(json, %s);' % target
        print '    json.endObject();'
        print '    json.endMember(); //shaders'
        print

    def dump_textures(self):
        print '    {'
        print '        json.beginMember("textures");'
        print '        json.beginArray();'
        print '        GLint active_texture = GL_TEXTURE0;'
        print '        glGetIntegerv(GL_ACTIVE_TEXTURE, &active_texture);'
        print '        GLint max_texture_coords = 0;'
        print '        glGetIntegerv(GL_MAX_TEXTURE_COORDS, &max_texture_coords);'
        print '        GLint max_combined_texture_image_units = 0;'
        print '        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_combined_texture_image_units);'
        print '        GLint max_units = std::max(max_combined_texture_image_units, max_texture_coords);'
        print '        for (GLint unit = 0; unit < max_units; ++unit) {'
        print '            glActiveTexture(GL_TEXTURE0 + unit);'
        print '            json.beginObject();'
        for target, binding in texture_targets:
            print '            json.beginMember("%s");' % target
            print '            writeTexture(json, %s, %s);' % (target, binding)
            print '            json.endMember();'
        print '            json.endObject();'
        print '        }'
        print '        glActiveTexture(active_texture);'
        print '        json.endArray();'
        print '        json.endMember(); // texture'
        print '    }'
        print

    def dump_framebuffer(self):
        print '    json.beginMember("framebuffer");'
        print '    json.beginObject();'
        # TODO: Handle real FBOs
        print
        print '    GLint boundDrawFbo = 0, boundReadFbo = 0;'
        print '    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &boundDrawFbo);'
        print '    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &boundReadFbo);'
        print '    if (!boundDrawFbo) {'
        print '        GLint depth_bits = 0;'
        print '        glGetIntegerv(GL_DEPTH_BITS, &depth_bits);'
        print '        GLint stencil_bits = 0;'
        print '        glGetIntegerv(GL_STENCIL_BITS, &stencil_bits);'
        print '        writeDrawBuffers(json, depth_bits, stencil_bits);'
        print '    } else {'
        print '        GLint colorRb, stencilRb, depthRb;'
        print '        GLint boundRb;'
        print '        glGetIntegerv(GL_RENDERBUFFER_BINDING, &boundRb);'
        print '        GLint drawbuffer = glretrace::double_buffer ? GL_BACK : GL_FRONT;'
        print '        glGetIntegerv(GL_DRAW_BUFFER, &drawbuffer);'
        print
        print '        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,'
        print '                                              drawbuffer,'
        print '                                              GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,'
        print '                                              &colorRb);'
        print '        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,'
        print '                                              GL_DEPTH_ATTACHMENT,'
        print '                                              GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,'
        print '                                              &depthRb);'
        print '        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,'
        print '                                              GL_STENCIL_ATTACHMENT,'
        print '                                              GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,'
        print '                                              &stencilRb);'
        print
        print '        GLint colorSamples, depthSamples, stencilSamples;'
        print '        GLuint rbs[3];'
        print '        GLint numRbs = 0;'
        print '        GLuint fboCopy = 0;'
        print '        glBindRenderbuffer(GL_RENDERBUFFER, colorRb);'
        print '        glGetRenderbufferParameteriv(GL_RENDERBUFFER,'
        print '                                     GL_RENDERBUFFER_SAMPLES, &colorSamples);'
        print '        glBindRenderbuffer(GL_RENDERBUFFER, depthRb);'
        print '        glGetRenderbufferParameteriv(GL_RENDERBUFFER,'
        print '                                     GL_RENDERBUFFER_SAMPLES, &depthSamples);'
        print '        glBindRenderbuffer(GL_RENDERBUFFER, stencilRb);'
        print '        glGetRenderbufferParameteriv(GL_RENDERBUFFER,'
        print '                                     GL_RENDERBUFFER_SAMPLES, &stencilSamples);'
        print '        glBindRenderbuffer(GL_RENDERBUFFER, boundRb);'
        print
        print '        if (colorSamples || depthSamples || stencilSamples) {'
        print '            //glReadPixels doesnt support multisampled buffers so we need'
        print '            // to blit the fbo to a temporary one'
        print '            fboCopy = downsampledFramebuffer(boundDrawFbo, drawbuffer,'
        print '                                             colorRb, depthRb, stencilRb,'
        print '                                             rbs, &numRbs);'
        print '        }'
        print '        glDrawBuffer(drawbuffer);'
        print '        glReadBuffer(drawbuffer);'
        print
        print '        writeDrawBuffers(json, depthRb, stencilRb);'
        print
        print '        if (fboCopy) {'
        print '            glBindFramebuffer(GL_FRAMEBUFFER, boundDrawFbo);'
        print '            glBindFramebuffer(GL_READ_FRAMEBUFFER, boundReadFbo);'
        print '            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, boundDrawFbo);'
        print '            glBindRenderbuffer(GL_RENDERBUFFER_BINDING, boundRb);'
        print '            glDeleteRenderbuffers(numRbs, rbs);'
        print '            glDeleteFramebuffers(1, &fboCopy);'
        print '        }'
        print '    }'
        print
        print '    json.endObject();'
        print '    json.endMember(); // framebuffer'
        pass

    def dump_atoms(self, getter, *args):
        for function, type, count, name in parameters:
            inflection = getter.inflector.radical + getter.suffix
            if inflection not in function.split(','):
                continue
            if type is X:
                continue
            print '        // %s' % name
            print '        {'
            type, value = getter(*(args + (name,)))
            print '            if (glGetError() != GL_NO_ERROR) {'
            #print '                std::cerr << "warning: %s(%s) failed\\n";' % (inflection, name)
            print '            } else {'
            print '                json.beginMember("%s");' % name
            JsonWriter().visit(type, value)
            print '                json.endMember();'
            print '            }'
            print '        }'
            print


if __name__ == '__main__':
    StateDumper().dump()
