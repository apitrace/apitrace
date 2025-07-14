##########################################################################
#
# Copyright 2008-2010 VMware, Inc.
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


"""GL tracing generator."""


import re
import sys

from trace import Tracer
from dispatch import function_pointer_type, function_pointer_value
import specs.stdapi as stdapi
import specs.glapi as glapi
import specs.glparams as glparams
from specs.glxapi import glxapi


class TypeGetter(stdapi.Visitor):
    '''Determine which glGet*v function that matches the specified type.'''

    def __init__(self, prefix = 'glGet', long_suffix = True, ext_suffix = ''):
        self.prefix = prefix
        self.long_suffix = long_suffix
        self.ext_suffix = ext_suffix

    def visitConst(self, const):
        return self.visit(const.type)

    def visitAlias(self, alias):
        if alias.expr == 'GLboolean':
            if self.long_suffix:
                suffix = 'Booleanv'
                arg_type = alias.expr
            else:
                suffix = 'iv'
                arg_type = 'GLint'
        elif alias.expr == 'GLdouble':
            if self.long_suffix:
                suffix = 'Doublev'
                arg_type = alias.expr
            else:
                suffix = 'dv'
                arg_type = alias.expr
        elif alias.expr == 'GLfloat':
            if self.long_suffix:
                suffix = 'Floatv'
                arg_type = alias.expr
            else:
                suffix = 'fv'
                arg_type = alias.expr
        elif alias.expr in ('GLint', 'GLuint', 'GLsizei'):
            if self.long_suffix:
                suffix = 'Integerv'
                arg_type = 'GLint'
            else:
                suffix = 'iv'
                arg_type = 'GLint'
        else:
            print(alias.expr)
            assert False
        function_name = self.prefix + suffix + self.ext_suffix
        return function_name, arg_type
    
    def visitEnum(self, enum):
        return self.visit(glapi.GLint)

    def visitBitmask(self, bitmask):
        return self.visit(glapi.GLint)

    def visitOpaque(self, pointer):
        return self.prefix + 'Pointerv' + self.ext_suffix, 'GLvoid *'


class GlTracer(Tracer):

    arrays = [
        ("Vertex", "VERTEX"),
        ("Normal", "NORMAL"),
        ("Color", "COLOR"),
        ("Index", "INDEX"),
        ("TexCoord", "TEXTURE_COORD"),
        ("EdgeFlag", "EDGE_FLAG"),
        ("FogCoord", "FOG_COORD"),
        ("SecondaryColor", "SECONDARY_COLOR"),
    ]
    arrays.reverse()

    # arrays available in ES1
    arrays_es1 = ("Vertex", "Normal", "Color", "TexCoord")

    buffer_targets = [
        "GL_ARRAY_BUFFER",
        "GL_ATOMIC_COUNTER_BUFFER",
        "GL_COPY_READ_BUFFER",
        "GL_COPY_WRITE_BUFFER",
        "GL_DRAW_INDIRECT_BUFFER",
        "GL_DISPATCH_INDIRECT_BUFFER",
        "GL_ELEMENT_ARRAY_BUFFER",
        "GL_PIXEL_PACK_BUFFER",
        "GL_PIXEL_UNPACK_BUFFER",
        "GL_QUERY_BUFFER",
        "GL_SHADER_STORAGE_BUFFER",
        "GL_TEXTURE_BUFFER",
        "GL_TRANSFORM_FEEDBACK_BUFFER",
        "GL_UNIFORM_BUFFER",
    ]

    # Names of the functions that can pack into the current pixel buffer
    # object.  See also the ARB_pixel_buffer_object specification.
    pack_function_regex = re.compile(r'^gl(' + r'|'.join([
        r'Getn?Histogram',
        r'Getn?PolygonStipple',
        r'Getn?PixelMap[a-z]+v',
        r'Getn?Minmax',
        r'Getn?(Convolution|Separable)Filter',
        r'Getn?(Compressed)?(Multi)?Tex(ture)?(Sub)?Image',
        r'Readn?Pixels',
    ]) + r')[0-9A-Z]*$')

    def header(self, api):
        Tracer.header(self, api)

        print('#include <algorithm>')
        print('#include "cxx_compat.hpp"')
        print()
        print('#include "gltrace.hpp"')
        print('#include "gltrace_arrays.hpp"')
        print('#include "glmemshadow.hpp"')
        print('#include "gltrace_unpack_compressed.hpp"')
        print()

        # Whether we need user arrays
        print('static inline bool _need_user_arrays(gltrace::Context *_ctx)')
        print('{')
        print('    if (!_ctx->user_arrays) {')
        print('        return false;')
        print('    }')
        print()
        print('    glfeatures::Profile profile = _ctx->profile;')
        print('    bool es1 = profile.es() && profile.major == 1;')
        print()

        for camelcase_name, uppercase_name in self.arrays:
            # in which profile is the array available?
            profile_check = 'profile.desktop()'
            if camelcase_name in self.arrays_es1:
                profile_check = '(' + profile_check + ' || es1)';

            function_name = 'gl%sPointer' % camelcase_name
            enable_name = 'GL_%s_ARRAY' % uppercase_name
            binding_name = 'GL_%s_ARRAY_BUFFER_BINDING' % uppercase_name
            print('    // %s' % function_name)
            print('  if (%s) {' % profile_check)
            self.array_prolog(api, uppercase_name)
            print('    if (_glIsEnabled(%s) &&' % enable_name)
            print('        _glGetInteger(%s) == 0) {' % binding_name)
            self.array_cleanup(api, uppercase_name)
            print('        return true;')
            print('    }')
            self.array_epilog(api, uppercase_name)
            print('  }')
            print()

        print('    // ES1 does not support generic vertex attributes')
        print('    if (es1)')
        print('        return false;')
        print()
        print('    // glVertexAttribPointer')
        print('    GLint _max_vertex_attribs = _glGetInteger(GL_MAX_VERTEX_ATTRIBS);')
        print('    for (GLint index = 0; index < _max_vertex_attribs; ++index) {')
        print('        if (_glGetVertexAttribi(index, GL_VERTEX_ATTRIB_ARRAY_ENABLED) &&')
        print('            _glGetVertexAttribi(index, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING) == 0) {')
        print('            return true;')
        print('        }')
        print('    }')
        print()

        print('    return false;')
        print('}')
        print()

        print(r'static void _trace_user_arrays(gltrace::Context *_ctx, GLuint count, bool instanced=false, GLuint instancecount=1);')
        print()

        # Declare helper functions to emit fake function calls into the trace
        for function in api.getAllFunctions():
            if function.name in self.fake_function_names:
                print(function.prototype('_fake_' + function.name) + ';')
        print()
        print(r'inline void')
        print(r'_fakeStringMarker(const std::string &s) {')
        print(r'    _fake_glStringMarkerGREMEDY(s.length(), s.data());')
        print(r'}')
        print()

        # Buffer mappings
        print('// whether glMapBufferRange(GL_MAP_WRITE_BIT) has ever been called')
        print('static bool _checkBufferMapRange = false;')
        print()
        print('// whether glBufferParameteriAPPLE(GL_BUFFER_FLUSHING_UNMAP_APPLE, GL_FALSE) has ever been called')
        print('static bool _checkBufferFlushingUnmapAPPLE = false;')
        print()

        # Generate a helper function to determine whether a parameter name
        # refers to a symbolic value or not
        print('static bool')
        print('is_symbolic_pname(GLenum pname) {')
        print('    switch (pname) {')
        for function, type, count, name in glparams.parameters:
            if type is glapi.GLenum:
                print('    case %s:' % name)
        print('        return true;')
        print('    default:')
        print('        return false;')
        print('    }')
        print('}')
        print()
        
        # Generate a helper function to determine whether a parameter value is
        # potentially symbolic or not; i.e., if the value can be represented in
        # an enum or not
        print('template<class T>')
        print('static inline bool')
        print('is_symbolic_param(T param) {')
        print('    return static_cast<T>(static_cast<GLenum>(param)) == param;')
        print('}')
        print()

        # Generate a helper function to know how many elements a parameter has
        print('static size_t')
        print('_gl_param_size(GLenum pname) {')
        print('    switch (pname) {')
        for function, type, count, name in glparams.parameters:
            if name == 'GL_PROGRAM_BINARY_FORMATS':
                count = 0
            if type is not None:
                print('    case %s: return %s;' % (name, count))
        print('    default:')
        print(r'        os::log("apitrace: warning: %s: unknown GLenum 0x%04X\n", __FUNCTION__, pname);')
        print('        return 1;')
        print('    }')
        print('}')
        print()

        # Generate a helper function to get buffer binding
        print('static GLenum')
        print('getBufferBinding(GLenum target) {')
        print('    switch (target) {')
        for target in self.buffer_targets:
            print('    case %s:' % target)
            print('        return %s_BINDING;' % target)
        print('    default:')
        print('        assert(false);')
        print('        return 0;')
        print('    }')
        print('}')
        print()

        print('static GLint')
        print('getBufferName(GLenum target) {')
        print('    GLint bufferName = 0;')
        print('    _glGetIntegerv(getBufferBinding(target), &bufferName);')
        print('    assert(bufferName != 0);')
        print('    return bufferName;')
        print('}')
        print()

        # states such as GL_UNPACK_ROW_LENGTH are not available in GLES
        print('static inline bool')
        print('can_unpack_subimage(void) {')
        print('    gltrace::Context *_ctx = gltrace::getContext();')
        print('    return _ctx->features.unpack_subimage;')
        print('}')
        print()

        # VMWX_map_buffer_debug
        print(r'extern "C" PUBLIC')
        print(r'void APIENTRY')
        print(r'glNotifyMappedBufferRangeVMWX(const void * start, GLsizeiptr length) {')
        self.emit_memcpy('start', 'length')
        print(r'}')
        print()

    getProcAddressFunctionNames = []

    def traceApi(self, api):
        if self.getProcAddressFunctionNames:
            # Generate a function to wrap proc addresses
            getProcAddressFunction = api.getFunctionByName(self.getProcAddressFunctionNames[0])
            argType = getProcAddressFunction.args[0].type
            retType = getProcAddressFunction.type
            
            print('static %s _wrapProcAddress(%s procName, %s procPtr);' % (retType, argType, retType))
            print()
            
            Tracer.traceApi(self, api)
            
            print('static %s _wrapProcAddress(%s procName, %s procPtr) {' % (retType, argType, retType))

            # Provide fallback functions to missing debug functions
            print('    if (!procPtr) {')
            else_ = ''
            for function_name in self.debug_functions:
                if self.api.getFunctionByName(function_name):
                    print('        %sif (strcmp("%s", (const char *)procName) == 0) {' % (else_, function_name))
                    print('            return (%s)&%s;' % (retType, function_name))
                    print('        }')
                else_ = 'else '
            print('        %s{' % else_)
            print('            return NULL;')
            print('        }')
            print('    }')

            for function in api.getAllFunctions():
                ptype = function_pointer_type(function)
                pvalue = function_pointer_value(function)
                print('    if (strcmp("%s", (const char *)procName) == 0) {' % function.name)
                print('        assert(procPtr != (%s)&%s);' % (retType, function.name))
                print('        %s = (%s)procPtr;' % (pvalue, ptype))
                print('        return (%s)&%s;' % (retType, function.name,))
                print('    }')
            print('    os::log("apitrace: warning: unknown function \\"%s\\"\\n", (const char *)procName);')
            print('    return procPtr;')
            print('}')
            print()
        else:
            Tracer.traceApi(self, api)

    array_pointer_function_names = set((
        "glVertexPointer",
        "glNormalPointer",
        "glColorPointer",
        "glIndexPointer",
        "glTexCoordPointer",
        "glEdgeFlagPointer",
        "glFogCoordPointer",
        "glSecondaryColorPointer",
        
        "glInterleavedArrays",

        "glVertexPointerEXT",
        "glNormalPointerEXT",
        "glColorPointerEXT",
        "glIndexPointerEXT",
        "glTexCoordPointerEXT",
        "glEdgeFlagPointerEXT",
        "glFogCoordPointerEXT",
        "glSecondaryColorPointerEXT",
        
        "glMultiTexCoordPointerEXT",

        "glVertexAttribPointer",
        "glVertexAttribPointerARB",
        "glVertexAttribPointerNV",
        "glVertexAttribIPointer",
        "glVertexAttribIPointerEXT",
        "glVertexAttribLPointer",
        "glVertexAttribLPointerEXT",
        
        #"glMatrixIndexPointerARB",
    ))

    # XXX: We currently ignore the gl*Draw*ElementArray* functions
    draw_function_regex = re.compile(r'^(?P<radical>gl([A-Z][a-z]+)*Draw(Range)?(Arrays|Elements))(?P<suffix>[A-Z][a-zA-Z]*)?$' )

    interleaved_formats = [
         'GL_V2F',
         'GL_V3F',
         'GL_C4UB_V2F',
         'GL_C4UB_V3F',
         'GL_C3F_V3F',
         'GL_N3F_V3F',
         'GL_C4F_N3F_V3F',
         'GL_T2F_V3F',
         'GL_T4F_V4F',
         'GL_T2F_C4UB_V3F',
         'GL_T2F_C3F_V3F',
         'GL_T2F_N3F_V3F',
         'GL_T2F_C4F_N3F_V3F',
         'GL_T4F_C4F_N3F_V4F',
    ]

    def traceFunctionImplBody(self, function):
        # Defer tracing of user array pointers...
        if function.name in self.array_pointer_function_names:
            print('    GLint _array_buffer = _glGetInteger(GL_ARRAY_BUFFER_BINDING);')
            print('    if (!_array_buffer) {')
            print('        static bool warned = false;')
            print('        if (!warned) {')
            print('            warned = true;')
            print('            os::log("apitrace: warning: %s: call will be faked due to pointer to user memory (https://git.io/JOMRv)\\n", __FUNCTION__);')
            print('        }')
            print('        gltrace::Context *_ctx = gltrace::getContext();')
            print('        _ctx->user_arrays = true;')
            if function.name == "glVertexAttribPointerNV":
                print(r'        os::log("apitrace: warning: %s: user memory arrays with NV_vertex_program longer supported\n", __FUNCTION__);')
            self.invokeFunction(function)

            # And also break down glInterleavedArrays into the individual calls
            if function.name == 'glInterleavedArrays':
                print()

                # Initialize the enable flags
                for camelcase_name, uppercase_name in self.arrays:
                    flag_name = '_' + uppercase_name.lower()
                    print('        GLboolean %s = GL_FALSE;' % flag_name)
                print()

                # Switch for the interleaved formats
                print('        switch (format) {')
                for format in self.interleaved_formats:
                    print('            case %s:' % format)
                    for camelcase_name, uppercase_name in self.arrays:
                        flag_name = '_' + uppercase_name.lower()
                        if format.find('_' + uppercase_name[0]) >= 0:
                            print('                %s = GL_TRUE;' % flag_name)
                    print('                break;')
                print('            default:')
                print('               return;')
                print('        }')
                print()

                # Emit fake glEnableClientState/glDisableClientState flags
                for camelcase_name, uppercase_name in self.arrays:
                    flag_name = '_' + uppercase_name.lower()
                    enable_name = 'GL_%s_ARRAY' % uppercase_name

                    # Emit a fake function
                    print('        if (%s) {' % flag_name)
                    print('            _fake_glEnableClientState(%s);' % enable_name)
                    print('        } else {')
                    print('            _fake_glDisableClientState(%s);' % enable_name)
                    print('        }')

            # Warn about buggy glGet(GL_*ARRAY_SIZE) not returning GL_BGRA
            buggyFunctions = {
                'glColorPointer':           ('glGetIntegerv',          '',        'GL_COLOR_ARRAY_SIZE'),
                'glSecondaryColorPointer':  ('glGetIntegerv',          '',        'GL_SECONDARY_COLOR_ARRAY_SIZE'),
                'glVertexAttribPointer':    ('glGetVertexAttribiv',    'index, ', 'GL_VERTEX_ATTRIB_ARRAY_SIZE'),
                'glVertexAttribPointerARB': ('glGetVertexAttribivARB', 'index, ', 'GL_VERTEX_ATTRIB_ARRAY_SIZE_ARB'),
            }
            if function.name in buggyFunctions:
                getter, extraArg, pname = buggyFunctions[function.name]
                print(r'        static bool _checked = false;')
                print(r'        if (!_checked && size == GL_BGRA) {')
                print(r'            GLint _size = 0;')
                print(r'            _%s(%s%s, &_size);' % (getter, extraArg, pname))
                print(r'            if (_size != GL_BGRA) {')
                print(r'                os::log("apitrace: warning: %s(%s) does not return GL_BGRA; trace will be incorrect (https://git.io/JOM0n)\n");' % (getter, pname))
                print(r'            }')
                print(r'            _checked = true;')
                print(r'        }')

            print('        return;')
            print('    }')

        # ... to the draw calls
        mo = self.draw_function_regex.match(function.name)
        if mo:
            functionRadical = mo.group('radical')
            print('    gltrace::Context *_ctx = gltrace::getContext();')

            print('    GLMemoryShadow::commitAllWrites(_ctx, trace::fakeMemcpy);')

            print('    if (_need_user_arrays(_ctx)) {')
            if 'Indirect' in function.name:
                print(r'        os::log("apitrace: warning: %s: indirect user arrays not supported\n");' % (function.name,))
            else:
                # Pick the corresponding *Params
                if 'Arrays' in functionRadical:
                    paramsType = 'DrawArraysParams'
                elif 'Elements' in functionRadical:
                    paramsType = 'DrawElementsParams'
                else:
                    assert 0
                if 'Multi' in functionRadical:
                    assert 'drawcount' in function.argNames()
                    paramsType = 'Multi' + paramsType
                print(r'        %s _params;' % paramsType)

                for arg in function.args:
                    paramsMember = arg.name.lower()
                    if paramsMember in ('mode', 'modestride'):
                        continue
                    print(r'        _params.%s = %s;' % (paramsMember, arg.name))

                print('        GLuint _count = _glDraw_count(_ctx, _params);')
                if 'instancecount' in function.argNames():
                    if 'baseinstance' in function.argNames():
                        print('        _trace_user_arrays(_ctx, _count, true, instancecount + baseinstance);')
                    else:
                        print('        _trace_user_arrays(_ctx, _count, true, instancecount);')
                else:
                    print('        _trace_user_arrays(_ctx, _count);')
            print('    }')

        if function.name.startswith("glDispatchCompute"):
            print('    gltrace::Context *_ctx = gltrace::getContext();')
            print('    GLMemoryShadow::commitAllWrites(_ctx, trace::fakeMemcpy);')

        if function.name == 'glLockArraysEXT':
            print('    gltrace::Context *_ctx = gltrace::getContext();')
            print('    if (_ctx) {')
            print('        _ctx->lockedArrayCount = first + count;')
            print('    }')

        if function.name == 'glUnlockArraysEXT':
            print('    gltrace::Context *_ctx = gltrace::getContext();')
            print('    if (_ctx) {')
            print('        _ctx->lockedArrayCount = 0;')
            print('    }')

        # Warn if user arrays are used with glBegin/glArrayElement/glEnd.
        if function.name == 'glBegin':
            print(r'    gltrace::Context *_ctx = gltrace::getContext();')
            print(r'    _ctx->userArraysOnBegin = _need_user_arrays(_ctx);')
        if function.name.startswith('glArrayElement'):
            print(r'    gltrace::Context *_ctx = gltrace::getContext();')
            print(r'    if (_ctx->userArraysOnBegin) {')
            print(r'        os::log("apitrace: warning: user arrays with glArrayElement not supported (https://git.io/JOM0l)\n");')
            print(r'        _ctx->userArraysOnBegin = false;')
            print(r'    }')
        
        # Emit a fake memcpy on buffer uploads
        if function.name == 'glBufferParameteriAPPLE':
            print('    if (pname == GL_BUFFER_FLUSHING_UNMAP_APPLE && param == GL_FALSE) {')
            print('        _checkBufferFlushingUnmapAPPLE = true;')
            print('    }')
        if function.name in ('glUnmapBuffer', 'glUnmapBufferARB'):
            if function.name.endswith('ARB'):
                suffix = 'ARB'
            else:
                suffix = ''
            print('    GLint access_flags = 0;')
            print('    GLint access = 0;')
            print('    bool flush;')
            print('    // GLES3 does not have GL_BUFFER_ACCESS;')
            print('    if (_checkBufferMapRange) {')
            print('        _glGetBufferParameteriv%s(target, GL_BUFFER_ACCESS_FLAGS, &access_flags);' % suffix)
            print('        flush = (access_flags & GL_MAP_WRITE_BIT) && !(access_flags & (GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_PERSISTENT_BIT));')
            print('    } else {')
            print('        _glGetBufferParameteriv%s(target, GL_BUFFER_ACCESS, &access);' % suffix)
            print('        flush = access != GL_READ_ONLY;')
            print('    }')
            print('    if (gltrace::is_coherent_write_map(access_flags)) {')
            print('        gltrace::Context *_ctx = gltrace::getContext();')
            print('        GLint buffer = getBufferName(target);')
            print('        auto it = _ctx->sharedRes->bufferToShadowMemory.find(buffer);')
            print('        if (it != _ctx->sharedRes->bufferToShadowMemory.end()) {')
            print('            it->second->unmap(trace::fakeMemcpy);')
            print('        } else {')
            print(r'            os::log("apitrace: error: %s: cannot find memory shadow\n", __FUNCTION__);')
            print('        }')
            print('        flush = false;')
            print('    }')
            print('    if (flush) {')
            print('        GLvoid *map = NULL;')
            print('        _glGetBufferPointerv%s(target, GL_BUFFER_MAP_POINTER, &map);'  % suffix)
            print('        if (map) {')
            print('            GLint length = -1;')
            print('            if (_checkBufferMapRange) {')
            print('                _glGetBufferParameteriv%s(target, GL_BUFFER_MAP_LENGTH, &length);' % suffix)
            print('                if (length == -1) {')
            print('                    // Mesa drivers refuse GL_BUFFER_MAP_LENGTH without GL 3.0 up-to')
            print('                    // http://cgit.freedesktop.org/mesa/mesa/commit/?id=ffee498fb848b253a7833373fe5430f8c7ca0c5f')
            print('                    static bool warned = false;')
            print('                    if (!warned) {')
            print('                        os::log("apitrace: warning: glGetBufferParameteriv%s(GL_BUFFER_MAP_LENGTH) failed\\n");' % suffix)
            print('                        warned = true;')
            print('                    }')
            print('                }')
            print('            } else {')
            print('                length = 0;')
            print('                _glGetBufferParameteriv%s(target, GL_BUFFER_SIZE, &length);' % suffix)
            print('            }')
            print('            if (_checkBufferFlushingUnmapAPPLE) {')
            print('                GLint flushing_unmap = GL_TRUE;')
            print('                _glGetBufferParameteriv%s(target, GL_BUFFER_FLUSHING_UNMAP_APPLE, &flushing_unmap);' % suffix)
            print('                flush = flush && flushing_unmap;')
            print('            }')
            print('            if (flush && length > 0) {')
            self.emit_memcpy('map', 'length')
            print('            }')
            print('        }')
            print('    }')
        if function.name == 'glUnmapBufferOES':
            print('    GLint access_flags = 0;')
            print('    GLint access = 0;')
            print('    bool flush;')
            print('    // GLES3 does not have GL_BUFFER_ACCESS;')
            print('    if (_checkBufferMapRange) {')
            print('        _glGetBufferParameteriv(target, GL_BUFFER_ACCESS_FLAGS, &access_flags);')
            print('        flush = (access_flags & GL_MAP_WRITE_BIT) && !(access_flags & (GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_PERSISTENT_BIT));')
            print('    } else {')
            print('        _glGetBufferParameteriv(target, GL_BUFFER_ACCESS, &access);')
            print('        flush = access != GL_READ_ONLY;')
            print('    }')
            print('    if (flush) {')
            print('        GLvoid *map = NULL;')
            print('        _glGetBufferPointervOES(target, GL_BUFFER_MAP_POINTER, &map);')
            print('        if (map) {')
            print('            GLint length = 0;')
            print('            GLint offset = 0;')
            print('            if (_checkBufferMapRange) {')
            print('                _glGetBufferParameteriv(target, GL_BUFFER_MAP_LENGTH, &length);')
            print('                _glGetBufferParameteriv(target, GL_BUFFER_MAP_OFFSET, &offset);')
            print('            } else {')
            print('                _glGetBufferParameteriv(target, GL_BUFFER_SIZE, &length);')
            print('            }')
            print('            if (flush && length > 0) {')
            self.emit_memcpy('map', 'length')
            print('            }')
            print('        }')
            print('    }')
        if function.name == 'glUnmapNamedBuffer':
            print('    GLint access_flags = 0;')
            print('    _glGetNamedBufferParameteriv(buffer, GL_BUFFER_ACCESS_FLAGS, &access_flags);')
            print('    if (gltrace::is_coherent_write_map(access_flags)) {')
            print('        gltrace::Context *_ctx = gltrace::getContext();')
            print('        auto it = _ctx->sharedRes->bufferToShadowMemory.find(buffer);')
            print('        if (it != _ctx->sharedRes->bufferToShadowMemory.end()) {')
            print('            it->second->unmap(trace::fakeMemcpy);')
            print('        } else {')
            print(r'            os::log("apitrace: error: %s: cannot find memory shadow\n", __FUNCTION__);')
            print('        }')
            print('    } else if ((access_flags & GL_MAP_WRITE_BIT) &&')
            print('               !(access_flags & (GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_PERSISTENT_BIT))) {')
            print('        GLvoid *map = NULL;')
            print('        _glGetNamedBufferPointerv(buffer, GL_BUFFER_MAP_POINTER, &map);')
            print('        GLint length = 0;')
            print('        _glGetNamedBufferParameteriv(buffer, GL_BUFFER_MAP_LENGTH, &length);')
            print('        if (map && length > 0) {')
            self.emit_memcpy('map', 'length')
            print('        }')
            print('    }')
        if function.name == 'glUnmapNamedBufferEXT':
            print('    GLint access_flags = 0;')
            print('    _glGetNamedBufferParameterivEXT(buffer, GL_BUFFER_ACCESS_FLAGS, &access_flags);')
            print('    if (gltrace::is_coherent_write_map(access_flags)) {')
            print('        gltrace::Context *_ctx = gltrace::getContext();')
            print('        auto it = _ctx->sharedRes->bufferToShadowMemory.find(buffer);')
            print('        if (it != _ctx->sharedRes->bufferToShadowMemory.end()) {')
            print('            it->second->unmap(trace::fakeMemcpy);')
            print('        } else {')
            print(r'            os::log("apitrace: error: %s: cannot find memory shadow\n", __FUNCTION__);')
            print('        }')
            print('    } else if ((access_flags & GL_MAP_WRITE_BIT) &&')
            print('               !(access_flags & (GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_PERSISTENT_BIT))) {')
            print('        GLvoid *map = NULL;')
            print('        _glGetNamedBufferPointervEXT(buffer, GL_BUFFER_MAP_POINTER, &map);')
            print('        GLint length = 0;')
            print('        _glGetNamedBufferParameterivEXT(buffer, GL_BUFFER_MAP_LENGTH, &length);')
            print('        if (map && length > 0) {')
            self.emit_memcpy('map', 'length')
            print('        }')
            print('    }')
        if function.name == 'glFlushMappedBufferRange':
            print('    GLvoid *map = NULL;')
            print('    _glGetBufferPointerv(target, GL_BUFFER_MAP_POINTER, &map);')
            print('    if (map && length > 0) {')
            self.emit_memcpy('(const char *)map + offset', 'length')
            print('    }')
        if function.name == 'glFlushMappedBufferRangeEXT':
            print('    GLvoid *map = NULL;')
            print('    _glGetBufferPointervOES(target, GL_BUFFER_MAP_POINTER_OES, &map);')
            print('    if (map && length > 0) {')
            self.emit_memcpy('(const char *)map + offset', 'length')
            print('    }')
        if function.name == 'glFlushMappedBufferRangeAPPLE':
            print('    GLvoid *map = NULL;')
            print('    _glGetBufferPointerv(target, GL_BUFFER_MAP_POINTER, &map);')
            print('    if (map && size > 0) {')
            self.emit_memcpy('(const char *)map + offset', 'size')
            print('    }')
        if function.name == 'glFlushMappedNamedBufferRange':
            print('    GLvoid *map = NULL;')
            print('    _glGetNamedBufferPointerv(buffer, GL_BUFFER_MAP_POINTER, &map);')
            print('    if (map && length > 0) {')
            self.emit_memcpy('(const char *)map + offset', 'length')
            print('    }')
        if function.name == 'glFlushMappedNamedBufferRangeEXT':
            print('    GLvoid *map = NULL;')
            print('    _glGetNamedBufferPointervEXT(buffer, GL_BUFFER_MAP_POINTER, &map);')
            print('    if (map && length > 0) {')
            self.emit_memcpy('(const char *)map + offset', 'length')
            print('    }')

        # FIXME: We don't support AMD_pinned_memory
        if function.name in ('glBufferStorage', 'glBufferStorageEXT', 'glNamedBufferStorage', 'glNamedBufferStorageEXT'):
            print(r'    if (flags & GL_MAP_NOTIFY_EXPLICIT_BIT_VMWX) {')
            print(r'        if (!(flags & GL_MAP_PERSISTENT_BIT)) {')
            print(r'            os::log("apitrace: warning: %s: MAP_NOTIFY_EXPLICIT_BIT_VMWX set w/o MAP_PERSISTENT_BIT\n", __FUNCTION__);')
            print(r'        }')
            print(r'        if (!(flags & GL_MAP_WRITE_BIT)) {')
            print(r'            os::log("apitrace: warning: %s: MAP_NOTIFY_EXPLICIT_BIT_VMWX set w/o MAP_WRITE_BIT\n", __FUNCTION__);')
            print(r'        }')
            print(r'        flags &= ~GL_MAP_NOTIFY_EXPLICIT_BIT_VMWX;')
            print(r'    }')
            print(r'')
            print(r'    if (gltrace::is_coherent_write_map(flags)) {')
            print(r'        gltrace::Context *_ctx = gltrace::getContext();')
            if function.name in ('glBufferStorage', 'glBufferStorageEXT'):
                print(r'        GLint buffer = getBufferName(target);')
            print(r'        auto memoryShadow = std::make_unique<GLMemoryShadow>();')
            print(r'        const bool success = memoryShadow->init(data, size);')
            print(r'        if (success) {')
            print(r'            _ctx->sharedRes->bufferToShadowMemory.insert_or_assign(buffer, std::move(memoryShadow));')
            print(r'        } else {')
            print(r'            os::log("apitrace: error: %s: cannot create memory shadow\n", __FUNCTION__);')
            print(r'        }')
            print(r'    }')
        if function.name in ('glMapBufferRange', 'glMapBufferRangeEXT', 'glMapNamedBufferRange', 'glMapNamedBufferRangeEXT'):
            print(r'    if (access & GL_MAP_NOTIFY_EXPLICIT_BIT_VMWX) {')
            print(r'        if (!(access & GL_MAP_PERSISTENT_BIT)) {')
            print(r'            os::log("apitrace: warning: %s: MAP_NOTIFY_EXPLICIT_BIT_VMWX set w/o MAP_PERSISTENT_BIT\n", __FUNCTION__);')
            print(r'        }')
            print(r'        if (!(access & GL_MAP_WRITE_BIT)) {')
            print(r'            os::log("apitrace: warning: %s: MAP_NOTIFY_EXPLICIT_BIT_VMWX set w/o MAP_WRITE_BIT\n", __FUNCTION__);')
            print(r'        }')
            print(r'        if (access & GL_MAP_FLUSH_EXPLICIT_BIT) {')
            print(r'            os::log("apitrace: warning: %s: MAP_NOTIFY_EXPLICIT_BIT_VMWX set w/ MAP_FLUSH_EXPLICIT_BIT\n", __FUNCTION__);')
            print(r'        }')
            print(r'        access &= ~GL_MAP_NOTIFY_EXPLICIT_BIT_VMWX;')
            print(r'    }')
        if function.name in ('glBufferData', 'glBufferDataARB'):
            print(r'    if (target == GL_EXTERNAL_VIRTUAL_MEMORY_BUFFER_AMD) {')
            print(r'        os::log("apitrace: warning: GL_AMD_pinned_memory not fully supported\n");')
            print(r'    }')

        # TODO: We don't track GL_INTEL_map_texture mappings
        if function.name == 'glMapTexture2DINTEL':
            print(r'    if (access & GL_MAP_WRITE_BIT) {')
            print(r'        os::log("apitrace: warning: GL_INTEL_map_texture not fully supported\n");')
            print(r'    }')

        # Operations on PBO may use coherent buffers so we must commit them first
        if self.unpack_function_regex.match(function.name) or self.pack_function_regex.match(function.name):
            print('    gltrace::Context *_ctx = gltrace::getContext();')
            print('    GLMemoryShadow::commitAllWrites(_ctx, trace::fakeMemcpy);')
            print('')

        # Don't leave vertex attrib locations to chance.  Instead emit fake
        # glBindAttribLocation calls to ensure that the same locations will be
        # used when retracing.  Trying to remap locations after the fact would
        # be an herculian task given that vertex attrib locations appear in
        # many entry-points, including non-shader related ones.
        if function.name == 'glLinkProgram':
            Tracer.invokeFunction(self, function)
            print('    GLint active_attributes = 0;')
            print('    _glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &active_attributes);')
            print('    for (GLint attrib = 0; attrib < active_attributes; ++attrib) {')
            print('        GLint size = 0;')
            print('        GLenum type = 0;')
            print('        GLchar name[256];')
            # TODO: Use ACTIVE_ATTRIBUTE_MAX_LENGTH instead of 256
            print('        _glGetActiveAttrib(program, attrib, sizeof name, NULL, &size, &type, name);')
            print("        if (name[0] != 'g' || name[1] != 'l' || name[2] != '_') {")
            print('            GLint location = _glGetAttribLocation(program, name);')
            print('            if (location >= 0) {')
            print('                _fake_glBindAttribLocation(program, location, name);')
            print('            }')
            print('        }')
            print('    }')
        if function.name == 'glLinkProgramARB':
            Tracer.invokeFunction(self, function)
            print('    GLint active_attributes = 0;')
            print('    _glGetObjectParameterivARB(programObj, GL_OBJECT_ACTIVE_ATTRIBUTES_ARB, &active_attributes);')
            print('    for (GLint attrib = 0; attrib < active_attributes; ++attrib) {')
            print('        GLint size = 0;')
            print('        GLenum type = 0;')
            print('        GLcharARB name[256];')
            # TODO: Use ACTIVE_ATTRIBUTE_MAX_LENGTH instead of 256
            print('        _glGetActiveAttribARB(programObj, attrib, sizeof name, NULL, &size, &type, name);')
            print("        if (name[0] != 'g' || name[1] != 'l' || name[2] != '_') {")
            print('            GLint location = _glGetAttribLocationARB(programObj, name);')
            print('            if (location >= 0) {')
            print('                _fake_glBindAttribLocationARB(programObj, location, name);')
            print('            }')
            print('        }')
            print('    }')

        Tracer.traceFunctionImplBody(self, function)

    # These entrypoints are only expected to be implemented by tools;
    # drivers will probably not implement them.
    marker_functions = [
        # GL_GREMEDY_string_marker
        'glStringMarkerGREMEDY',
        # GL_GREMEDY_frame_terminator
        'glFrameTerminatorGREMEDY',
    ]

    # These entrypoints may be implemented by drivers, but are also very useful
    # for debugging / analysis tools.
    debug_functions = [
        # GL_KHR_debug
        'glDebugMessageControl',
        'glDebugMessageInsert',
        'glDebugMessageCallback',
        'glGetDebugMessageLog',
        'glPushDebugGroup',
        'glPopDebugGroup',
        'glObjectLabel',
        'glGetObjectLabel',
        'glObjectPtrLabel',
        'glGetObjectPtrLabel',
        # GL_KHR_debug (for OpenGL ES)
        'glDebugMessageControlKHR',
        'glDebugMessageInsertKHR',
        'glDebugMessageCallbackKHR',
        'glGetDebugMessageLogKHR',
        'glPushDebugGroupKHR',
        'glPopDebugGroupKHR',
        'glObjectLabelKHR',
        'glGetObjectLabelKHR',
        'glObjectPtrLabelKHR',
        'glGetObjectPtrLabelKHR',
        # GL_ARB_debug_output
        'glDebugMessageControlARB',
        'glDebugMessageInsertARB',
        'glDebugMessageCallbackARB',
        'glGetDebugMessageLogARB',
        # GL_AMD_debug_output
        'glDebugMessageEnableAMD',
        'glDebugMessageInsertAMD',
        'glDebugMessageCallbackAMD',
        'glGetDebugMessageLogAMD',
        # GL_EXT_debug_label
        'glLabelObjectEXT',
        'glGetObjectLabelEXT',
        # GL_EXT_debug_marker
        'glInsertEventMarkerEXT',
        'glPushGroupMarkerEXT',
        'glPopGroupMarkerEXT',
    ]

    def invokeFunction(self, function):
        if function.name in ('glLinkProgram', 'glLinkProgramARB'):
            # These functions have been dispatched already
            return

        # Force glProgramBinary to fail.  Per ARB_get_program_binary this
        # should signal the app that it needs to recompile.
        if function.name in ('glProgramBinary', 'glProgramBinaryOES'):
            print(r'   binaryFormat = 0xDEADDEAD;')
            print(r'   binary = &binaryFormat;')
            print(r'   length = sizeof binaryFormat;')

        Tracer.invokeFunction(self, function)

    def doInvokeFunction(self, function):
        # Same as invokeFunction() but called both when trace is enabled or disabled.
        #
        # Used to modify the behavior of GL entry-points.

        # Override GL extensions
        if function.name in ('glGetString', 'glGetIntegerv', 'glGetStringi'):
            Tracer.doInvokeFunction(self, function, prefix = 'gltrace::_', suffix = '_override')
            return

        # We implement GL_GREMEDY_*, etc., and not the driver
        if function.name in self.marker_functions:
            return

        # We may be faking KHR_debug, so ensure the pointer queries result is
        # always zeroed to prevent dereference of unitialized pointers
        if function.name == 'glGetPointerv':
            print('    if (params &&')
            print('        (pname == GL_DEBUG_CALLBACK_FUNCTION ||')
            print('         pname == GL_DEBUG_CALLBACK_USER_PARAM)) {')
            print('        *params = NULL;')
            print('    }')

        if function.name in self.getProcAddressFunctionNames:
            nameArg = function.args[0].name
            print('    if (strcmp("glNotifyMappedBufferRangeVMWX", (const char *)%s) == 0) {' % (nameArg,))
            print('        _result = (%s)&glNotifyMappedBufferRangeVMWX;' % (function.type,))
            for marker_function in self.marker_functions:
                if self.api.getFunctionByName(marker_function):
                    print('    } else if (strcmp("%s", (const char *)%s) == 0) {' % (marker_function, nameArg))
                    print('        _result = (%s)&%s;' % (function.type, marker_function))
            print('    } else {')
            Tracer.doInvokeFunction(self, function)

            # Replace function addresses with ours
            # XXX: Doing this here instead of wrapRet means that the trace will
            # contain the addresses of the wrapper functions, and not the real
            # functions, but in practice this should make no difference.
            if function.name in self.getProcAddressFunctionNames:
                print('    _result = _wrapProcAddress(%s, _result);' % (nameArg,))

            print('    }')
            return

        if function.name in ('glGetProgramBinary', 'glGetProgramBinaryOES'):
            print(r'    bufSize = 0;')

        Tracer.doInvokeFunction(self, function)

        if function.name in ('glMapBufferRange', 'glMapBufferRangeEXT', 'glMapNamedBufferRange', 'glMapNamedBufferRangeEXT'):
            print(r'    if (gltrace::is_coherent_write_map(access)) {')
            print(r'        gltrace::Context *_ctx = gltrace::getContext();')
            if function.name in ('glMapBufferRange', 'glMapBufferRangeEXT'):
                print(r'        GLint buffer = getBufferName(target);')
            print(r'        auto it = _ctx->sharedRes->bufferToShadowMemory.find(buffer);')
            print(r'        if (it != _ctx->sharedRes->bufferToShadowMemory.end()) {')
            print(r'            _result = it->second->map(_ctx, _result, access, offset, length);')
            print(r'        } else {')
            print(r'            os::log("apitrace: error: %s: %u: cannot find memory shadow\n", __FUNCTION__, _call);')
            print(r'        }')
            print(r'    }')

        # We should sync back readable coherent buffers only when a fence become signaled
        # because in any other moment application cannot know if changes from operations on
        # GPU are done and buffers are updated.
        if function.name == 'glWaitSync':
            print(r'    gltrace::Context *_ctx = gltrace::getContext();')
            print(r'    GLMemoryShadow::commitAllWrites(_ctx, trace::fakeMemcpy);')
            print(r'    GLMemoryShadow::syncAllForReads(_ctx);')

        if function.name == 'glClientWaitSync':
            print(r'    if (_result == GL_ALREADY_SIGNALED || _result == GL_CONDITION_SATISFIED) {')
            print(r'        gltrace::Context *_ctx = gltrace::getContext();')
            print(r'        GLMemoryShadow::commitAllWrites(_ctx, trace::fakeMemcpy);')
            print(r'        GLMemoryShadow::syncAllForReads(_ctx);')
            print(r'    }')

        if function.name == 'glGetSynciv':
            print(r'    if (pname == GL_SYNC_STATUS && bufSize > 0 && values[0] == GL_SIGNALED) {')
            print(r'        gltrace::Context *_ctx = gltrace::getContext();')
            print(r'        GLMemoryShadow::commitAllWrites(_ctx, trace::fakeMemcpy);')
            print(r'        GLMemoryShadow::syncAllForReads(_ctx);')
            print(r'    }')

        if function.name == 'glGetProgramiv':
            print(r'    if (params && pname == GL_PROGRAM_BINARY_LENGTH) {')
            print(r'        *params = 0;')
            print(r'    }')
        if function.name in ('glGetProgramBinary', 'glGetProgramBinaryOES'):
            print(r'    if (length) {')
            print(r'        *length = 0;')
            print(r'    }')

    def wrapRet(self, function, instance):
        Tracer.wrapRet(self, function, instance)

        # Keep track of buffer mappings
        if function.name in ('glMapBufferRange', 'glMapBufferRangeEXT'):
            print('    if (access & GL_MAP_WRITE_BIT) {')
            print('        _checkBufferMapRange = true;')
            print('    }')

    boolean_names = [
        'GL_FALSE',
        'GL_TRUE',
    ]

    def gl_boolean(self, value):
        return self.boolean_names[int(bool(value))]

    # Regular expression for the names of the functions that unpack from a
    # pixel buffer object.  See the ARB_pixel_buffer_object specification.
    unpack_function_regex = re.compile(r'^gl(' + r'|'.join([
        r'Bitmap',
        r'PolygonStipple',
        r'PixelMap[a-z]+v',
        r'DrawPixels',
        r'Color(Sub)?Table',
        r'(Convolution|Separable)Filter[12]D',
        r'(Compressed)?(Multi)?Tex(ture)?(Sub)?Image[1-4]D',
    ]) + r')[0-9A-Z]*$')

    compressed_image_function_regex = re.compile(r'^glCompressedTex(ture)?(Sub)?Image[1-4]D[0-9A-Z]*$')

    def serializeArgValue(self, function, arg):
        # Recognize offsets instead of blobs when a PBO is bound
        if self.unpack_function_regex.match(function.name) \
           and (isinstance(arg.type, stdapi.Blob) \
                or (isinstance(arg.type, stdapi.Const) \
                    and isinstance(arg.type.type, stdapi.Blob))):
            print('    {')
            print('        gltrace::Context *_ctx = gltrace::getContext();')
            print('        GLint _unpack_buffer = 0;')
            print('        if (_ctx->features.pixel_buffer_object)')
            print('            _glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &_unpack_buffer);')
            print('        if (_unpack_buffer) {')
            print('            trace::localWriter.writePointer((uintptr_t)%s);' % arg.name)
            print('        } else {')
            if self.compressed_image_function_regex.match(function.name):
                print('            %s;' % arg.type.size.format('[](const void* data, GLsizei size){ trace::localWriter.writeBlob(data, size); }'))
            else:
                Tracer.serializeArgValue(self, function, arg)
            print('        }')
            print('    }')
            return

        # Recognize offsets instead of pointers when query buffer is bound
        if function.name.startswith('glGetQueryObject') and arg.output:
            print(r'    gltrace::Context *_ctx = gltrace::getContext();')
            print(r'    GLint _query_buffer = 0;')
            print(r'    if (_ctx->features.query_buffer_object) {')
            print(r'        _query_buffer = _glGetInteger(GL_QUERY_BUFFER_BINDING);')
            print(r'    }')
            print(r'    if (_query_buffer) {')
            print(r'        trace::localWriter.writePointer((uintptr_t)%s);' % arg.name)
            print(r'    } else {')
            Tracer.serializeArgValue(self, function, arg)
            print(r'    }')
            return

        # Several GL state functions take GLenum symbolic names as
        # integer/floats; so dump the symbolic name whenever possible
        if function.name.startswith('gl') \
           and arg.type in (glapi.GLint, glapi.GLfloat, glapi.GLdouble) \
           and arg.name == 'param':
            assert arg.index > 0
            assert function.args[arg.index - 1].name == 'pname'
            assert function.args[arg.index - 1].type == glapi.GLenum
            print('    if (is_symbolic_pname(pname) && is_symbolic_param(%s)) {' % arg.name)
            self.serializeValue(glapi.GLenum, arg.name)
            print('    } else {')
            Tracer.serializeArgValue(self, function, arg)
            print('    }')
            return

        Tracer.serializeArgValue(self, function, arg)

    fake_function_names = [
        'glBindAttribLocation',
        'glBindAttribLocationARB',
        'glBindBuffer',
        'glBitmap',
        'glClientActiveTexture',
        'glDisableClientState',
        'glEnableClientState',
        'glEndList',
        'glNewList',
        'glScissor',
        'glStringMarkerGREMEDY',
        'glTexImage2D',
        'glViewport',
    ]

    def footer(self, api):
        Tracer.footer(self, api)

        # Generate helper functions to emit fake function calls into the trace
        for function in api.getAllFunctions():
            if function.name in self.fake_function_names:
                print(function.prototype('_fake_' + function.name))
                print(r'{')
                self.fake_call(function, function.argNames())
                print(r'}')
                print()

        # A simple state tracker to track the pointer values
        # update the state
        print('static void _trace_user_arrays(gltrace::Context *_ctx, GLuint count, bool instanced, GLuint instancecount)')
        print('{')
        print('    glfeatures::Profile profile = _ctx->profile;')
        print('    bool es1 = profile.es() && profile.major == 1;')
        print()

        # Some apps, in particular Quake3, can tell the driver to lock more
        # vertices than those actually required for the draw call.
        print('    count = std::max(count, _ctx->lockedArrayCount);')
        print()

        # Temporarily unbind the array buffer
        print('    GLint _array_buffer = _glGetInteger(GL_ARRAY_BUFFER_BINDING);')
        print('    if (_array_buffer) {')
        print('        _fake_glBindBuffer(GL_ARRAY_BUFFER, 0);')
        print('    }')
        print()

        for camelcase_name, uppercase_name in self.arrays:
            # in which profile is the array available?
            profile_check = 'profile.desktop()'
            if camelcase_name in self.arrays_es1:
                profile_check = '(' + profile_check + ' || es1)';

            function_name = 'gl%sPointer' % camelcase_name
            enable_name = 'GL_%s_ARRAY' % uppercase_name
            binding_name = 'GL_%s_ARRAY_BUFFER_BINDING' % uppercase_name
            function = api.getFunctionByName(function_name)

            print('    // %s' % function.prototype())
            print('  if (%s) {' % profile_check)
            self.array_trace_prolog(api, uppercase_name)
            self.array_prolog(api, uppercase_name)
            print('    if (_glIsEnabled(%s)) {' % enable_name)
            print('        GLint _binding = _glGetInteger(%s);' % binding_name)
            print('        if (!_binding) {')

            # Get the arguments via glGet*
            for arg in function.args:
                arg_get_enum = 'GL_%s_ARRAY_%s' % (uppercase_name, arg.name.upper())
                arg_get_function, arg_type = TypeGetter().visit(arg.type)
                print('            %s %s = 0;' % (arg_type, arg.name))
                print('            _%s(%s, &%s);' % (arg_get_function, arg_get_enum, arg.name))
            
            arg_names = ', '.join([arg.name for arg in function.args[:-1]])
            print('            size_t _size = _%s_size(%s, count);' % (function.name, arg_names))

            # Emit a fake function
            self.array_trace_intermezzo(api, uppercase_name)
            print('            unsigned _call = trace::localWriter.beginEnter(&_%s_sig, true);' % (function.name,))
            for arg in function.args:
                assert not arg.output
                print('            trace::localWriter.beginArg(%u);' % (arg.index,))
                if arg.name != 'pointer':
                    self.serializeValue(arg.type, arg.name)
                else:
                    print('            trace::localWriter.writeBlob((const void *)%s, _size);' % (arg.name))
                print('            trace::localWriter.endArg();')
            
            print('            trace::localWriter.endEnter();')
            print('            trace::localWriter.beginLeave(_call);')
            print('            trace::localWriter.endLeave();')
            print('        }')
            print('    }')
            self.array_epilog(api, uppercase_name)
            self.array_trace_epilog(api, uppercase_name)
            print('  }')
            print()

        # Samething, but for glVertexAttribPointer*
        #
        # Some variants of glVertexAttribPointer alias conventional and generic attributes:
        # - glVertexAttribPointer: no
        # - glVertexAttribPointerARB: implementation dependent
        # - glVertexAttribPointerNV: yes
        #
        # This means that the implementations of these functions do not always
        # alias, and they need to be considered independently.
        #
        print('    // ES1 does not support generic vertex attributes')
        print('    if (es1)')
        print('        return;')
        print()

        function_name = 'glVertexAttribPointer'
        function = api.getFunctionByName(function_name)

        print('    // %s' % function.prototype())
        print('    GLint _max_vertex_attribs = _glGetInteger(GL_MAX_VERTEX_ATTRIBS);')
        print('    for (GLint index = 0; index < _max_vertex_attribs; ++index) {')
        print('        GLint _enabled = 0;')
        print('        _glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &_enabled);')
        print('        if (_enabled) {')
        print('            GLint _binding = 0;')
        print('            _glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &_binding);')
        print('            if (!_binding) {')

        print('                GLint divisor = 0;')
        print('                if (instanced && _ctx->features.instanced_arrays) {')
        print('                    _glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_DIVISOR, &divisor);')
        print('                }')

        # Get the arguments via glGet*
        for arg in function.args[1:]:
            arg_get_enum = 'GL_VERTEX_ATTRIB_ARRAY_%s' % (arg.name.upper())
            arg_get_function, arg_type = TypeGetter('glGetVertexAttrib', False).visit(arg.type)
            print('                %s %s = 0;' % (arg_type, arg.name))
            print('                _%s(index, %s, &%s);' % (arg_get_function, arg_get_enum, arg.name))

        arg_names = ', '.join([arg.name for arg in function.args[1:-1]])
        print('                size_t _size = _%s_size(%s, divisor > 0 ? instancecount / divisor : count);' % (function.name, arg_names))

        # Emit a fake glVertexAttribPointer function
        print('                unsigned _call = trace::localWriter.beginEnter(&_%s_sig, true);' % (function.name,))
        for arg in function.args:
            assert not arg.output
            print('                trace::localWriter.beginArg(%u);' % (arg.index,))
            if arg.name != 'pointer':
                self.serializeValue(arg.type, arg.name)
            else:
                print('                trace::localWriter.writeBlob((const void *)%s, _size);' % (arg.name))
            print('                trace::localWriter.endArg();')

        print('                trace::localWriter.endEnter();')
        print('                trace::localWriter.beginLeave(_call);')
        print('                trace::localWriter.endLeave();')

        print('            }')
        print('        }')
        print('    }')
        print()

        # Restore the original array_buffer
        print('    if (_array_buffer) {')
        print('        _fake_glBindBuffer(GL_ARRAY_BUFFER, _array_buffer);')
        print('    }')
        print()

        print('}')
        print()

    #
    # Hooks for glTexCoordPointer, which is identical to the other array
    # pointers except the fact that it is indexed by glClientActiveTexture.
    #

    def array_prolog(self, api, uppercase_name):
        if uppercase_name == 'TEXTURE_COORD':
            print('    GLint max_units = 0;')
            print('    if (_ctx->profile.desktop())')
            print('        _glGetIntegerv(GL_MAX_TEXTURE_COORDS, &max_units);')
            print('    else')
            print('        _glGetIntegerv(GL_MAX_TEXTURE_UNITS, &max_units);')
            print('    GLint client_active_texture = GL_TEXTURE0;')
            print('    if (max_units > 0) {')
            print('        _glGetIntegerv(GL_CLIENT_ACTIVE_TEXTURE, &client_active_texture);')
            print('    }')
            print('    GLint unit = 0;')
            print('    do {')
            print('        GLint texture = GL_TEXTURE0 + unit;')
            print('        if (max_units > 0) {')
            print('            _glClientActiveTexture(texture);')
            print('        }')

    def array_trace_prolog(self, api, uppercase_name):
        if uppercase_name == 'TEXTURE_COORD':
            print('    bool client_active_texture_dirty = false;')

    def array_epilog(self, api, uppercase_name):
        if uppercase_name == 'TEXTURE_COORD':
            print('    } while (++unit < max_units);')
        self.array_cleanup(api, uppercase_name)

    def array_cleanup(self, api, uppercase_name):
        if uppercase_name == 'TEXTURE_COORD':
            print('    if (max_units > 0) {')
            print('        _glClientActiveTexture(client_active_texture);')
            print('    }')
        
    def array_trace_intermezzo(self, api, uppercase_name):
        if uppercase_name == 'TEXTURE_COORD':
            print('    if (texture != client_active_texture || client_active_texture_dirty) {')
            print('        client_active_texture_dirty = true;')
            print('        _fake_glClientActiveTexture(texture);')
            print('    }')

    def array_trace_epilog(self, api, uppercase_name):
        if uppercase_name == 'TEXTURE_COORD':
            print('    if (client_active_texture_dirty) {')
            print('        _fake_glClientActiveTexture(client_active_texture);')
            print('    }')

    def emitFakeTexture2D(self):
        print(r'    _fake_glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);')

