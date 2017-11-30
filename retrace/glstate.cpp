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

#include "image.hpp"
#include "state_writer.hpp"
#include "glproc.hpp"
#include "glws.hpp"
#include "glsize.hpp"
#include "glstate.hpp"
#include "glstate_internal.hpp"


namespace glstate {


PixelPackState::PixelPackState(const Context &context)
{
    desktop = !context.ES;
    texture_3d = context.texture_3d;
    pixel_buffer_object = context.pixel_buffer_object;
    color_buffer_float = context.ARB_color_buffer_float;

    // Start with default state
    pack_alignment = 4;
    pack_image_height = 0;
    pack_lsb_first = GL_FALSE;
    pack_row_length = 0;
    pack_skip_images = 0;
    pack_skip_pixels = 0;
    pack_skip_rows = 0;
    pack_swap_bytes = GL_FALSE;
    pixel_pack_buffer_binding = 0;
    clamp_read_color = GL_FIXED_ONLY;

    // Get current state
    glGetIntegerv(GL_PACK_ALIGNMENT, &pack_alignment);
    if (desktop) {
        glGetIntegerv(GL_PACK_LSB_FIRST, &pack_lsb_first);
        glGetIntegerv(GL_PACK_ROW_LENGTH, &pack_row_length);
        glGetIntegerv(GL_PACK_SKIP_PIXELS, &pack_skip_pixels);
        glGetIntegerv(GL_PACK_SKIP_ROWS, &pack_skip_rows);
        glGetIntegerv(GL_PACK_SWAP_BYTES, &pack_swap_bytes);
        if (texture_3d) {
            glGetIntegerv(GL_PACK_IMAGE_HEIGHT, &pack_image_height);
            glGetIntegerv(GL_PACK_SKIP_IMAGES, &pack_skip_images);
        }
    }
    if (pixel_buffer_object) {
        glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &pixel_pack_buffer_binding);
    }
    if (color_buffer_float) {
        glGetIntegerv(GL_CLAMP_READ_COLOR, &clamp_read_color);
    }

    // Reset state for compact images
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    if (desktop) {
        glPixelStorei(GL_PACK_LSB_FIRST, GL_FALSE);
        glPixelStorei(GL_PACK_ROW_LENGTH, 0);
        glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
        glPixelStorei(GL_PACK_SKIP_ROWS, 0);
        glPixelStorei(GL_PACK_SWAP_BYTES, GL_FALSE);
        if (texture_3d) {
            glPixelStorei(GL_PACK_IMAGE_HEIGHT, 0);
            glPixelStorei(GL_PACK_SKIP_IMAGES, 0);
        }
    }
    if (pixel_buffer_object) {
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    }
    if (color_buffer_float) {
        glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
    }
}

PixelPackState::~PixelPackState() {
    glPixelStorei(GL_PACK_ALIGNMENT, pack_alignment);
    if (desktop) {
        glPixelStorei(GL_PACK_LSB_FIRST, pack_lsb_first);
        glPixelStorei(GL_PACK_ROW_LENGTH, pack_row_length);
        glPixelStorei(GL_PACK_SKIP_PIXELS, pack_skip_pixels);
        glPixelStorei(GL_PACK_SKIP_ROWS, pack_skip_rows);
        glPixelStorei(GL_PACK_SWAP_BYTES, pack_swap_bytes);
        if (texture_3d) {
            glPixelStorei(GL_PACK_IMAGE_HEIGHT, pack_image_height);
            glPixelStorei(GL_PACK_SKIP_IMAGES, pack_skip_images);
        }
    }
    if (pixel_buffer_object) {
        glBindBuffer(GL_PIXEL_PACK_BUFFER, pixel_pack_buffer_binding);
    }
    if (color_buffer_float) {
        glClampColor(GL_CLAMP_READ_COLOR, clamp_read_color);
    }
}

TempId::TempId(GLenum _target) :
    target(_target),
    id(0)
{
    switch (target){
    case GL_ARRAY_BUFFER:
        glGenBuffers(1, &id);
        break;
    case GL_FRAMEBUFFER:
        glGenFramebuffers(1, &id);
        break;
    case GL_PROGRAM:
        id = glCreateProgram();
        break;
    case GL_RENDERBUFFER:
        glGenRenderbuffers(1, &id);
        break;
    case GL_TEXTURE:
        glGenTextures(1, &id);
        break;
    case GL_VERTEX_ARRAY:
        glGenVertexArrays(1, &id);
        break;
    default:
        assert(false);
        id = 0;
        return;
    }
}

TempId::~TempId()
{
    switch (target){
    case GL_ARRAY_BUFFER:
        glDeleteBuffers(1, &id);
        break;
    case GL_FRAMEBUFFER:
        glDeleteFramebuffers(1, &id);
        break;
    case GL_PROGRAM:
        glDeleteProgram(id);
        break;
    case GL_RENDERBUFFER:
        glGenRenderbuffers(1, &id);
        break;
    case GL_TEXTURE:
        glDeleteTextures(1, &id);
        break;
    case GL_VERTEX_ARRAY:
        glGenVertexArrays(1, &id);
        break;
    default:
        assert(false);
        id = 0;
        return;
    }
}

TextureBinding::TextureBinding(GLenum _target, GLuint _id) :
    target(_target),
    id(_id),
    prev_id(0)
{
    GLenum binding = getTextureBinding(target);
    glGetIntegerv(binding, (GLint *) &prev_id);
    if (prev_id != id) {
        glBindTexture(target, id);
    }
}

TextureBinding::~TextureBinding() {
    if (prev_id != id) {
        glBindTexture(target, prev_id);
    }
}

static GLenum
getBufferBinding(GLenum target) {
    switch (target) {
    case GL_ARRAY_BUFFER:
        return GL_ARRAY_BUFFER_BINDING;
    case GL_ATOMIC_COUNTER_BUFFER:
        return GL_ATOMIC_COUNTER_BUFFER_BINDING;
    case GL_COPY_READ_BUFFER:
        return GL_COPY_READ_BUFFER_BINDING;
    case GL_COPY_WRITE_BUFFER:
        return GL_COPY_WRITE_BUFFER_BINDING;
    case GL_DRAW_INDIRECT_BUFFER:
        return GL_DRAW_INDIRECT_BUFFER_BINDING;
    case GL_FRAMEBUFFER:
        return GL_FRAMEBUFFER_BINDING;
    case GL_DISPATCH_INDIRECT_BUFFER:
        return GL_DISPATCH_INDIRECT_BUFFER_BINDING;
    case GL_ELEMENT_ARRAY_BUFFER:
        return GL_ELEMENT_ARRAY_BUFFER_BINDING;
    case GL_PIXEL_PACK_BUFFER:
        return GL_PIXEL_PACK_BUFFER_BINDING;
    case GL_PIXEL_UNPACK_BUFFER:
        return GL_PIXEL_UNPACK_BUFFER_BINDING;
    case GL_QUERY_BUFFER:
        return GL_QUERY_BUFFER_BINDING;
    case GL_RENDERBUFFER:
        return GL_RENDERBUFFER_BINDING;
    case GL_SHADER_STORAGE_BUFFER:
        return GL_SHADER_STORAGE_BUFFER_BINDING;
    case GL_TEXTURE_BUFFER:
        return GL_TEXTURE_BUFFER;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        return GL_TRANSFORM_FEEDBACK_BUFFER_BINDING;
    case GL_UNIFORM_BUFFER:
        return GL_UNIFORM_BUFFER_BINDING;
    default:
        assert(false);
        return GL_NONE;
    }
}


BufferBinding::BufferBinding(GLenum _target, GLuint _buffer) :
    target(_target),
    buffer(_buffer),
    prevBuffer(0)
{
    GLenum binding = getBufferBinding(target);
    glGetIntegerv(binding, (GLint *) &prevBuffer);

    if (prevBuffer != buffer) {
        switch(target)
        {
        case GL_FRAMEBUFFER:
            glBindFramebuffer(target, buffer);
            break;
        case GL_RENDERBUFFER:
            glBindRenderbuffer(target, buffer);
            break;
        default:
            glBindBuffer(target, buffer);
            break;
        }
    }
}

BufferBinding::~BufferBinding() {
    if (prevBuffer != buffer) {
        switch(target)
        {
        case GL_FRAMEBUFFER:
            glBindFramebuffer(target, prevBuffer);
            break;
        case GL_RENDERBUFFER:
            glBindRenderbuffer(target, prevBuffer);
            break;
        default:
            glBindBuffer(target, prevBuffer);
            break;
        }
    }
}


BufferMapping::BufferMapping() :
    target(GL_NONE),
    buffer(0),
    map_pointer(NULL),
    unmap(false)
{
}

GLvoid *
BufferMapping::map(GLenum _target, GLuint _buffer)
{
    if (target == _target && buffer == _buffer) {
        return map_pointer;
    }

    target = _target;
    buffer = _buffer;
    map_pointer = NULL;
    unmap = false;

    BufferBinding bb(target, buffer);

    // Recursive mappings of the same buffer are not allowed.  And with the
    // pursuit of persistent mappings for performance this will become more
    // and more common.
    GLint mapped = GL_FALSE;
    glGetBufferParameteriv(target, GL_BUFFER_MAPPED, &mapped);
    if (mapped) {
        glGetBufferPointerv(target, GL_BUFFER_MAP_POINTER, &map_pointer);
        assert(map_pointer != NULL);

        GLint map_offset = 0;
        glGetBufferParameteriv(target, GL_BUFFER_MAP_OFFSET, &map_offset);
        if (map_offset != 0) {
            std::cerr << "warning: " << enumToString(target) << " buffer " << buffer << " is already mapped with offset " << map_offset << "\n";
            // FIXME: This most likely won't work.  We should remap the
            // buffer with the full range, then re-map when done.  This
            // should never happen in practice with persistent mappings
            // though.
            map_pointer = (GLubyte *)map_pointer - map_offset;
        }
    } else {
        map_pointer = glMapBuffer(target, GL_READ_ONLY);
        if (map_pointer) {
            unmap = true;
        }
    }

    return map_pointer;
}

BufferMapping::~BufferMapping() {
    if (unmap) {
        BufferBinding bb(target, buffer);

        GLenum ret = glUnmapBuffer(target);
        assert(ret == GL_TRUE);
        (void)ret;
    }
}


void
dumpBoolean(StateWriter &writer, GLboolean value)
{
    switch (value) {
    case GL_FALSE:
        writer.writeString("GL_FALSE");
        break;
    case GL_TRUE:
        writer.writeString("GL_TRUE");
        break;
    default:
        writer.writeInt(static_cast<GLint>(value));
        break;
    }
}


void
dumpEnum(StateWriter &writer, GLenum pname)
{
    const char *s = enumToString(pname);
    if (s) {
        writer.writeString(s);
    } else {
        writer.writeInt(pname);
    }
}


/**
 * Get a GL_KHR_debug/GL_EXT_debug_label object label.
 *
 * The returned string should be destroyed with free() when no longer
 * necessary.
 */
char *
getObjectLabel(Context &context, GLenum identifier, GLuint name)
{
    if (!name) {
        return NULL;
    }

    GLsizei length = 0;
    if (context.KHR_debug) {
        /*
         * XXX: According to
         * http://www.khronos.org/registry/gles/extensions/KHR/debug.txt
         * description of glGetObjectLabel:
         *
         *   "If <label> is NULL and <length> is non-NULL then no string will
         *   be returned and the length of the label will be returned in
         *   <length>."
         *
         * However NVIDIA 319.60 drivers return a zero length in such
         * circumstances.  310.14 drivers worked fine though.  So, just rely on
         * GL_MAX_LABEL_LENGTH instead, which might waste a bit of memory, but
         * should work reliably everywhere.
         */
        if (0) {
            glGetObjectLabel(identifier, name, 0, &length, NULL);
        } else {
            glGetIntegerv(GL_MAX_LABEL_LENGTH, &length);
        }
    }
    if (context.EXT_debug_label) {
        glGetObjectLabelEXT(identifier, name, 0, &length, NULL);
    }
    if (!length) {
        return NULL;
    }

    char *label = (char *)calloc(length + 1, 1);
    if (!label) {
        return NULL;
    }

    if (context.KHR_debug) {
        glGetObjectLabel(identifier, name, length + 1, NULL, label);
    }
    if (context.EXT_debug_label) {
        glGetObjectLabelEXT(identifier, name, length + 1, NULL, label);
    }

    if (label[0] == '\0') {
        free(label);
        return NULL;
    }

    return label;
}


/**
 * Dump a GL_KHR_debug/GL_EXT_debug_label object label.
 */
void
dumpObjectLabel(StateWriter &writer, Context &context, GLenum identifier, GLuint name, const char *member) {
    char *label = getObjectLabel(context, identifier, name);
    if (!label) {
        return;
    }

    writer.writeStringMember(member, label);
    free(label);
}


static const GLenum bindings[] = {
    GL_DRAW_BUFFER,
    GL_READ_BUFFER,
    GL_PIXEL_PACK_BUFFER_BINDING,
    GL_PIXEL_UNPACK_BUFFER_BINDING,
    GL_TEXTURE_BINDING_1D,
    GL_TEXTURE_BINDING_1D_ARRAY,
    GL_TEXTURE_BINDING_2D,
    GL_TEXTURE_BINDING_2D_ARRAY,
    GL_TEXTURE_BINDING_2D_MULTISAMPLE,
    GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY,
    GL_TEXTURE_BINDING_3D,
    GL_TEXTURE_BINDING_RECTANGLE,
    GL_TEXTURE_BINDING_CUBE_MAP,
    GL_TEXTURE_BINDING_CUBE_MAP_ARRAY,
    GL_DRAW_FRAMEBUFFER_BINDING,
    GL_READ_FRAMEBUFFER_BINDING,
    GL_RENDERBUFFER_BINDING,
    GL_DRAW_BUFFER0,
    GL_DRAW_BUFFER1,
    GL_DRAW_BUFFER2,
    GL_DRAW_BUFFER3,
    GL_DRAW_BUFFER4,
    GL_DRAW_BUFFER5,
    GL_DRAW_BUFFER6,
    GL_DRAW_BUFFER7,
    GL_TRANSFORM_FEEDBACK_BUFFER_BINDING,
    GL_UNIFORM_BUFFER_BINDING,
};


#define NUM_BINDINGS sizeof(bindings)/sizeof(bindings[0])


static void APIENTRY
debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                     GLsizei length, const GLchar* message, const void *userParam)
{
    // Ignore NVIDIA buffer usage warnings: when dumping we inevitably use
    // buffers differently than declared
    if (source == GL_DEBUG_SOURCE_API &&
        type == GL_DEBUG_TYPE_OTHER &&
        id == 131188 &&
        severity == GL_DEBUG_SEVERITY_LOW) {
        return;
    };

    const char *severityStr = "";
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        severityStr = " high";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        break;
    case GL_DEBUG_SEVERITY_LOW:
        severityStr = " low";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        /* ignore */
        return;
    default:
        assert(0);
    }

    const char *sourceStr = "";
    switch (source) {
    case GL_DEBUG_SOURCE_API:
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        sourceStr = " window system";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        sourceStr = " shader compiler";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        sourceStr = " third party";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        sourceStr = " application";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        break;
    default:
        assert(0);
    }

    const char *typeStr = "";
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        typeStr = " error";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        typeStr = " deprecated behaviour";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        typeStr = " undefined behaviour";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        typeStr = " portability issue";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        return;
    default:
        assert(0);
        /* fall-through */
    case GL_DEBUG_TYPE_OTHER:
        typeStr = " issue";
        break;
    case GL_DEBUG_TYPE_MARKER:
    case GL_DEBUG_TYPE_PUSH_GROUP:
    case GL_DEBUG_TYPE_POP_GROUP:
        return;
    }

    std::cerr << "warning: message:" << severityStr << sourceStr << typeStr;

    if (id) {
        std::cerr << " " << id;
    }

    std::cerr << ": ";

    std::cerr << message;

    // Write new line if not included in the message already.
    size_t messageLen = strlen(message);
    if (!messageLen ||
        (message[messageLen - 1] != '\n' &&
         message[messageLen - 1] != '\r')) {
       std::cerr << std::endl;
    }

    /* To help debug bugs in glstate. */
    if (0) {
        os::breakpoint();
    }
}


void dumpCurrentContext(StateWriter &writer)
{

#ifndef NDEBUG
    GLint old_bindings[NUM_BINDINGS];
    for (unsigned i = 0; i < NUM_BINDINGS; ++i) {
        old_bindings[i] = 0;
        glGetIntegerv(bindings[i], &old_bindings[i]);
    }
#endif

    Context context;

    /* Temporarily disable messages, as dumpParameters blindlessly tries to
     * get state, regardless the respective extension is supported or not.
     */
    GLDEBUGPROC prevDebugCallbackFunction = 0;
    void *prevDebugCallbackUserParam = 0;
    if (context.KHR_debug) {
        glGetPointerv(GL_DEBUG_CALLBACK_FUNCTION, (GLvoid **) &prevDebugCallbackFunction);
        glGetPointerv(GL_DEBUG_CALLBACK_USER_PARAM, &prevDebugCallbackUserParam);
        glDebugMessageCallback(NULL, NULL);
    }

    dumpParameters(writer, context);

    // Use our own debug-message callback.
    if (context.KHR_debug) {
        glDebugMessageCallback(debugMessageCallback, NULL);
    }

    dumpShadersUniforms(writer, context);
    dumpTextures(writer, context);
    dumpFramebuffer(writer, context);

#ifndef NDEBUG
    for (unsigned i = 0; i < NUM_BINDINGS; ++i) {
        GLint new_binding = 0;
        glGetIntegerv(bindings[i], &new_binding);
        if (new_binding != old_bindings[i]) {
            std::cerr << "warning: " << enumToString(bindings[i]) << " was clobbered\n";
        }
    }
#endif

    // Restore debug message callback
    if (context.KHR_debug) {
        glDebugMessageCallback(prevDebugCallbackFunction, prevDebugCallbackUserParam);
    }
}


} /* namespace glstate */
