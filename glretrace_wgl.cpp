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


#include "glproc.hpp"
#include "retrace.hpp"
#include "glretrace.hpp"


using namespace glretrace;


typedef std::map<unsigned long long, glws::Drawable *> DrawableMap;
typedef std::map<unsigned long long, glws::Context *> ContextMap;
static DrawableMap drawable_map;
static DrawableMap pbuffer_map;
static ContextMap context_map;


static glws::Drawable *
getDrawable(unsigned long long hdc) {
    if (hdc == 0) {
        return NULL;
    }

    DrawableMap::const_iterator it;
    it = drawable_map.find(hdc);
    if (it == drawable_map.end()) {
        return (drawable_map[hdc] = ws->createDrawable(visual));
    }

    return it->second;
}

static void retrace_wglCreateContext(Trace::Call &call) {
    unsigned long long orig_context = call.ret->toUIntPtr();
    glws::Context *context = ws->createContext(glretrace::visual);
    context_map[orig_context] = context;
}

static void retrace_wglDeleteContext(Trace::Call &call) {
}

static void retrace_wglMakeCurrent(Trace::Call &call) {
    if (drawable && context) {
        glFlush();
        if (!double_buffer) {
            frame_complete(call.no);
        }
    }
    
    glws::Drawable *new_drawable = getDrawable(call.arg(0).toUIntPtr());
    glws::Context *new_context = context_map[call.arg(1).toUIntPtr()];

    bool result = ws->makeCurrent(new_drawable, new_context);

    if (new_drawable && new_context && result) {
        drawable = new_drawable;
        context = new_context;
    } else {
        drawable = NULL;
        context = NULL;
    }
}

static void retrace_wglCopyContext(Trace::Call &call) {
}

static void retrace_wglChoosePixelFormat(Trace::Call &call) {
}

static void retrace_wglDescribePixelFormat(Trace::Call &call) {
}

static void retrace_wglSetPixelFormat(Trace::Call &call) {
}

static void retrace_wglSwapBuffers(Trace::Call &call) {
    frame_complete(call.no);
    if (double_buffer) {
        drawable->swapBuffers();
    } else {
        glFlush();
    }
}

static void retrace_wglShareLists(Trace::Call &call) {
    unsigned long long hglrc1 = call.arg(0).toUIntPtr();
    unsigned long long hglrc2 = call.arg(1).toUIntPtr();

    glws::Context *share_context = context_map[hglrc1];
    glws::Context *old_context = context_map[hglrc2];

    glws::Context *new_context =
        ws->createContext(old_context->visual, share_context);
    if (new_context) {
        delete old_context;
        context_map[hglrc2] = new_context;
    }
}

static void retrace_wglCreateLayerContext(Trace::Call &call) {
    retrace_wglCreateContext(call);
}

static void retrace_wglDescribeLayerPlane(Trace::Call &call) {
}

static void retrace_wglSetLayerPaletteEntries(Trace::Call &call) {
}

static void retrace_wglRealizeLayerPalette(Trace::Call &call) {
}

static void retrace_wglSwapLayerBuffers(Trace::Call &call) {
    retrace_wglSwapBuffers(call);
}

static void retrace_wglUseFontBitmapsA(Trace::Call &call) {
}

static void retrace_wglUseFontBitmapsW(Trace::Call &call) {
}

static void retrace_wglSwapMultipleBuffers(Trace::Call &call) {
}

static void retrace_wglUseFontOutlinesA(Trace::Call &call) {
}

static void retrace_wglUseFontOutlinesW(Trace::Call &call) {
}

static void retrace_wglCreateBufferRegionARB(Trace::Call &call) {
}

static void retrace_wglDeleteBufferRegionARB(Trace::Call &call) {
}

static void retrace_wglSaveBufferRegionARB(Trace::Call &call) {
}

static void retrace_wglRestoreBufferRegionARB(Trace::Call &call) {
}

static void retrace_wglChoosePixelFormatARB(Trace::Call &call) {
}

static void retrace_wglMakeContextCurrentARB(Trace::Call &call) {
}

static void retrace_wglCreatePbufferARB(Trace::Call &call) {
    int iWidth = call.arg(2).toUInt();
    int iHeight = call.arg(3).toUInt();

    unsigned long long orig_pbuffer = call.ret->toUIntPtr();
    glws::Drawable *drawable = ws->createDrawable(glretrace::visual);

    drawable->resize(iWidth, iHeight);

    pbuffer_map[orig_pbuffer] = drawable;
}

static void retrace_wglGetPbufferDCARB(Trace::Call &call) {
    glws::Drawable *pbuffer = pbuffer_map[call.arg(0).toUIntPtr()];

    unsigned long long orig_hdc = call.ret->toUIntPtr();

    drawable_map[orig_hdc] = pbuffer;
}

static void retrace_wglReleasePbufferDCARB(Trace::Call &call) {
}

static void retrace_wglDestroyPbufferARB(Trace::Call &call) {
}

static void retrace_wglQueryPbufferARB(Trace::Call &call) {
}

static void retrace_wglBindTexImageARB(Trace::Call &call) {
}

static void retrace_wglReleaseTexImageARB(Trace::Call &call) {
}

static void retrace_wglSetPbufferAttribARB(Trace::Call &call) {
}

static void retrace_wglCreateContextAttribsARB(Trace::Call &call) {
    unsigned long long orig_context = call.ret->toUIntPtr();
    glws::Context *share_context = NULL;

    if (call.arg(1).toPointer()) {
        share_context = context_map[call.arg(1).toUIntPtr()];
    }

    glws::Context *context = ws->createContext(glretrace::visual, share_context);
    context_map[orig_context] = context;
}

static void retrace_wglMakeContextCurrentEXT(Trace::Call &call) {
}

static void retrace_wglChoosePixelFormatEXT(Trace::Call &call) {
}

static void retrace_wglSwapIntervalEXT(Trace::Call &call) {
}

static void retrace_wglAllocateMemoryNV(Trace::Call &call) {
}

static void retrace_wglFreeMemoryNV(Trace::Call &call) {
}

static void retrace_glAddSwapHintRectWIN(Trace::Call &call) {
}

static void retrace_wglGetProcAddress(Trace::Call &call) {
}

void glretrace::retrace_call_wgl(Trace::Call &call) {
    const char *name = call.name();

    switch (name[0]) {
    case 'g':
        if (name[1] == 'l' && name[2] == 'A' && name[3] == 'd' && name[4] == 'd' && name[5] == 'S' && name[6] == 'w' && name[7] == 'a' && name[8] == 'p' && name[9] == 'H' && name[10] == 'i' && name[11] == 'n' && name[12] == 't' && name[13] == 'R' && name[14] == 'e' && name[15] == 'c' && name[16] == 't' && name[17] == 'W' && name[18] == 'I' && name[19] == 'N' && name[20] == '\0') {
            // glAddSwapHintRectWIN
            retrace_glAddSwapHintRectWIN(call);
            return;
        }
        break;
    case 'w':
        switch (name[1]) {
        case 'g':
            switch (name[2]) {
            case 'l':
                switch (name[3]) {
                case 'A':
                    if (name[4] == 'l' && name[5] == 'l' && name[6] == 'o' && name[7] == 'c' && name[8] == 'a' && name[9] == 't' && name[10] == 'e' && name[11] == 'M' && name[12] == 'e' && name[13] == 'm' && name[14] == 'o' && name[15] == 'r' && name[16] == 'y' && name[17] == 'N' && name[18] == 'V' && name[19] == '\0') {
                        // wglAllocateMemoryNV
                        retrace_wglAllocateMemoryNV(call);
                        return;
                    }
                    break;
                case 'B':
                    if (name[4] == 'i' && name[5] == 'n' && name[6] == 'd' && name[7] == 'T' && name[8] == 'e' && name[9] == 'x' && name[10] == 'I' && name[11] == 'm' && name[12] == 'a' && name[13] == 'g' && name[14] == 'e' && name[15] == 'A' && name[16] == 'R' && name[17] == 'B' && name[18] == '\0') {
                        // wglBindTexImageARB
                        retrace_wglBindTexImageARB(call);
                        return;
                    }
                    break;
                case 'C':
                    switch (name[4]) {
                    case 'h':
                        switch (name[5]) {
                        case 'o':
                            switch (name[6]) {
                            case 'o':
                                switch (name[7]) {
                                case 's':
                                    switch (name[8]) {
                                    case 'e':
                                        switch (name[9]) {
                                        case 'P':
                                            switch (name[10]) {
                                            case 'i':
                                                switch (name[11]) {
                                                case 'x':
                                                    switch (name[12]) {
                                                    case 'e':
                                                        switch (name[13]) {
                                                        case 'l':
                                                            switch (name[14]) {
                                                            case 'F':
                                                                switch (name[15]) {
                                                                case 'o':
                                                                    switch (name[16]) {
                                                                    case 'r':
                                                                        switch (name[17]) {
                                                                        case 'm':
                                                                            switch (name[18]) {
                                                                            case 'a':
                                                                                switch (name[19]) {
                                                                                case 't':
                                                                                    switch (name[20]) {
                                                                                    case '\0':
                                                                                        // wglChoosePixelFormat
                                                                                        retrace_wglChoosePixelFormat(call);
                                                                                        return;
                                                                                        break;
                                                                                    case 'A':
                                                                                        if (name[21] == 'R' && name[22] == 'B' && name[23] == '\0') {
                                                                                            // wglChoosePixelFormatARB
                                                                                            retrace_wglChoosePixelFormatARB(call);
                                                                                            return;
                                                                                        }
                                                                                        break;
                                                                                    case 'E':
                                                                                        if (name[21] == 'X' && name[22] == 'T' && name[23] == '\0') {
                                                                                            // wglChoosePixelFormatEXT
                                                                                            retrace_wglChoosePixelFormatEXT(call);
                                                                                            return;
                                                                                        }
                                                                                        break;
                                                                                    }
                                                                                    break;
                                                                                }
                                                                                break;
                                                                            }
                                                                            break;
                                                                        }
                                                                        break;
                                                                    }
                                                                    break;
                                                                }
                                                                break;
                                                            }
                                                            break;
                                                        }
                                                        break;
                                                    }
                                                    break;
                                                }
                                                break;
                                            }
                                            break;
                                        }
                                        break;
                                    }
                                    break;
                                }
                                break;
                            }
                            break;
                        }
                        break;
                    case 'o':
                        if (name[5] == 'p' && name[6] == 'y' && name[7] == 'C' && name[8] == 'o' && name[9] == 'n' && name[10] == 't' && name[11] == 'e' && name[12] == 'x' && name[13] == 't' && name[14] == '\0') {
                            // wglCopyContext
                            retrace_wglCopyContext(call);
                            return;
                        }
                        break;
                    case 'r':
                        switch (name[5]) {
                        case 'e':
                            switch (name[6]) {
                            case 'a':
                                switch (name[7]) {
                                case 't':
                                    switch (name[8]) {
                                    case 'e':
                                        switch (name[9]) {
                                        case 'B':
                                            if (name[10] == 'u' && name[11] == 'f' && name[12] == 'f' && name[13] == 'e' && name[14] == 'r' && name[15] == 'R' && name[16] == 'e' && name[17] == 'g' && name[18] == 'i' && name[19] == 'o' && name[20] == 'n' && name[21] == 'A' && name[22] == 'R' && name[23] == 'B' && name[24] == '\0') {
                                                // wglCreateBufferRegionARB
                                                retrace_wglCreateBufferRegionARB(call);
                                                return;
                                            }
                                            break;
                                        case 'C':
                                            switch (name[10]) {
                                            case 'o':
                                                switch (name[11]) {
                                                case 'n':
                                                    switch (name[12]) {
                                                    case 't':
                                                        switch (name[13]) {
                                                        case 'e':
                                                            switch (name[14]) {
                                                            case 'x':
                                                                switch (name[15]) {
                                                                case 't':
                                                                    switch (name[16]) {
                                                                    case '\0':
                                                                        // wglCreateContext
                                                                        retrace_wglCreateContext(call);
                                                                        return;
                                                                        break;
                                                                    case 'A':
                                                                        if (name[17] == 't' && name[18] == 't' && name[19] == 'r' && name[20] == 'i' && name[21] == 'b' && name[22] == 's' && name[23] == 'A' && name[24] == 'R' && name[25] == 'B' && name[26] == '\0') {
                                                                            // wglCreateContextAttribsARB
                                                                            retrace_wglCreateContextAttribsARB(call);
                                                                            return;
                                                                        }
                                                                        break;
                                                                    }
                                                                    break;
                                                                }
                                                                break;
                                                            }
                                                            break;
                                                        }
                                                        break;
                                                    }
                                                    break;
                                                }
                                                break;
                                            }
                                            break;
                                        case 'L':
                                            if (name[10] == 'a' && name[11] == 'y' && name[12] == 'e' && name[13] == 'r' && name[14] == 'C' && name[15] == 'o' && name[16] == 'n' && name[17] == 't' && name[18] == 'e' && name[19] == 'x' && name[20] == 't' && name[21] == '\0') {
                                                // wglCreateLayerContext
                                                retrace_wglCreateLayerContext(call);
                                                return;
                                            }
                                            break;
                                        case 'P':
                                            if (name[10] == 'b' && name[11] == 'u' && name[12] == 'f' && name[13] == 'f' && name[14] == 'e' && name[15] == 'r' && name[16] == 'A' && name[17] == 'R' && name[18] == 'B' && name[19] == '\0') {
                                                // wglCreatePbufferARB
                                                retrace_wglCreatePbufferARB(call);
                                                return;
                                            }
                                            break;
                                        }
                                        break;
                                    }
                                    break;
                                }
                                break;
                            }
                            break;
                        }
                        break;
                    }
                    break;
                case 'D':
                    switch (name[4]) {
                    case 'e':
                        switch (name[5]) {
                        case 'l':
                            switch (name[6]) {
                            case 'e':
                                switch (name[7]) {
                                case 't':
                                    switch (name[8]) {
                                    case 'e':
                                        switch (name[9]) {
                                        case 'B':
                                            if (name[10] == 'u' && name[11] == 'f' && name[12] == 'f' && name[13] == 'e' && name[14] == 'r' && name[15] == 'R' && name[16] == 'e' && name[17] == 'g' && name[18] == 'i' && name[19] == 'o' && name[20] == 'n' && name[21] == 'A' && name[22] == 'R' && name[23] == 'B' && name[24] == '\0') {
                                                // wglDeleteBufferRegionARB
                                                retrace_wglDeleteBufferRegionARB(call);
                                                return;
                                            }
                                            break;
                                        case 'C':
                                            if (name[10] == 'o' && name[11] == 'n' && name[12] == 't' && name[13] == 'e' && name[14] == 'x' && name[15] == 't' && name[16] == '\0') {
                                                // wglDeleteContext
                                                retrace_wglDeleteContext(call);
                                                return;
                                            }
                                            break;
                                        }
                                        break;
                                    }
                                    break;
                                }
                                break;
                            }
                            break;
                        case 's':
                            switch (name[6]) {
                            case 'c':
                                switch (name[7]) {
                                case 'r':
                                    switch (name[8]) {
                                    case 'i':
                                        switch (name[9]) {
                                        case 'b':
                                            switch (name[10]) {
                                            case 'e':
                                                switch (name[11]) {
                                                case 'L':
                                                    if (name[12] == 'a' && name[13] == 'y' && name[14] == 'e' && name[15] == 'r' && name[16] == 'P' && name[17] == 'l' && name[18] == 'a' && name[19] == 'n' && name[20] == 'e' && name[21] == '\0') {
                                                        // wglDescribeLayerPlane
                                                        retrace_wglDescribeLayerPlane(call);
                                                        return;
                                                    }
                                                    break;
                                                case 'P':
                                                    if (name[12] == 'i' && name[13] == 'x' && name[14] == 'e' && name[15] == 'l' && name[16] == 'F' && name[17] == 'o' && name[18] == 'r' && name[19] == 'm' && name[20] == 'a' && name[21] == 't' && name[22] == '\0') {
                                                        // wglDescribePixelFormat
                                                        retrace_wglDescribePixelFormat(call);
                                                        return;
                                                    }
                                                    break;
                                                }
                                                break;
                                            }
                                            break;
                                        }
                                        break;
                                    }
                                    break;
                                }
                                break;
                            case 't':
                                if (name[7] == 'r' && name[8] == 'o' && name[9] == 'y' && name[10] == 'P' && name[11] == 'b' && name[12] == 'u' && name[13] == 'f' && name[14] == 'f' && name[15] == 'e' && name[16] == 'r' && name[17] == 'A' && name[18] == 'R' && name[19] == 'B' && name[20] == '\0') {
                                    // wglDestroyPbufferARB
                                    retrace_wglDestroyPbufferARB(call);
                                    return;
                                }
                                break;
                            }
                            break;
                        }
                        break;
                    }
                    break;
                case 'F':
                    if (name[4] == 'r' && name[5] == 'e' && name[6] == 'e' && name[7] == 'M' && name[8] == 'e' && name[9] == 'm' && name[10] == 'o' && name[11] == 'r' && name[12] == 'y' && name[13] == 'N' && name[14] == 'V' && name[15] == '\0') {
                        // wglFreeMemoryNV
                        retrace_wglFreeMemoryNV(call);
                        return;
                    }
                    break;
                case 'G':
                    switch (name[4]) {
                    case 'e':
                        switch (name[5]) {
                        case 't':
                            switch (name[6]) {
                            case 'C':
                                switch (name[7]) {
                                case 'u':
                                    switch (name[8]) {
                                    case 'r':
                                        switch (name[9]) {
                                        case 'r':
                                            switch (name[10]) {
                                            case 'e':
                                                switch (name[11]) {
                                                case 'n':
                                                    switch (name[12]) {
                                                    case 't':
                                                        switch (name[13]) {
                                                        case 'C':
                                                            if (name[14] == 'o' && name[15] == 'n' && name[16] == 't' && name[17] == 'e' && name[18] == 'x' && name[19] == 't' && name[20] == '\0') {
                                                                // wglGetCurrentContext
                                                                return;
                                                            }
                                                            break;
                                                        case 'D':
                                                            if (name[14] == 'C' && name[15] == '\0') {
                                                                // wglGetCurrentDC
                                                                return;
                                                            }
                                                            break;
                                                        case 'R':
                                                            switch (name[14]) {
                                                            case 'e':
                                                                switch (name[15]) {
                                                                case 'a':
                                                                    switch (name[16]) {
                                                                    case 'd':
                                                                        switch (name[17]) {
                                                                        case 'D':
                                                                            switch (name[18]) {
                                                                            case 'C':
                                                                                switch (name[19]) {
                                                                                case 'A':
                                                                                    if (name[20] == 'R' && name[21] == 'B' && name[22] == '\0') {
                                                                                        // wglGetCurrentReadDCARB
                                                                                        return;
                                                                                    }
                                                                                    break;
                                                                                case 'E':
                                                                                    if (name[20] == 'X' && name[21] == 'T' && name[22] == '\0') {
                                                                                        // wglGetCurrentReadDCEXT
                                                                                        return;
                                                                                    }
                                                                                    break;
                                                                                }
                                                                                break;
                                                                            }
                                                                            break;
                                                                        }
                                                                        break;
                                                                    }
                                                                    break;
                                                                }
                                                                break;
                                                            }
                                                            break;
                                                        }
                                                        break;
                                                    }
                                                    break;
                                                }
                                                break;
                                            }
                                            break;
                                        }
                                        break;
                                    }
                                    break;
                                }
                                break;
                            case 'D':
                                if (name[7] == 'e' && name[8] == 'f' && name[9] == 'a' && name[10] == 'u' && name[11] == 'l' && name[12] == 't' && name[13] == 'P' && name[14] == 'r' && name[15] == 'o' && name[16] == 'c' && name[17] == 'A' && name[18] == 'd' && name[19] == 'd' && name[20] == 'r' && name[21] == 'e' && name[22] == 's' && name[23] == 's' && name[24] == '\0') {
                                    // wglGetDefaultProcAddress
                                    return;
                                }
                                break;
                            case 'E':
                                switch (name[7]) {
                                case 'x':
                                    switch (name[8]) {
                                    case 't':
                                        switch (name[9]) {
                                        case 'e':
                                            switch (name[10]) {
                                            case 'n':
                                                switch (name[11]) {
                                                case 's':
                                                    switch (name[12]) {
                                                    case 'i':
                                                        switch (name[13]) {
                                                        case 'o':
                                                            switch (name[14]) {
                                                            case 'n':
                                                                switch (name[15]) {
                                                                case 's':
                                                                    switch (name[16]) {
                                                                    case 'S':
                                                                        switch (name[17]) {
                                                                        case 't':
                                                                            switch (name[18]) {
                                                                            case 'r':
                                                                                switch (name[19]) {
                                                                                case 'i':
                                                                                    switch (name[20]) {
                                                                                    case 'n':
                                                                                        switch (name[21]) {
                                                                                        case 'g':
                                                                                            switch (name[22]) {
                                                                                            case 'A':
                                                                                                if (name[23] == 'R' && name[24] == 'B' && name[25] == '\0') {
                                                                                                    // wglGetExtensionsStringARB
                                                                                                    return;
                                                                                                }
                                                                                                break;
                                                                                            case 'E':
                                                                                                if (name[23] == 'X' && name[24] == 'T' && name[25] == '\0') {
                                                                                                    // wglGetExtensionsStringEXT
                                                                                                    return;
                                                                                                }
                                                                                                break;
                                                                                            }
                                                                                            break;
                                                                                        }
                                                                                        break;
                                                                                    }
                                                                                    break;
                                                                                }
                                                                                break;
                                                                            }
                                                                            break;
                                                                        }
                                                                        break;
                                                                    }
                                                                    break;
                                                                }
                                                                break;
                                                            }
                                                            break;
                                                        }
                                                        break;
                                                    }
                                                    break;
                                                }
                                                break;
                                            }
                                            break;
                                        }
                                        break;
                                    }
                                    break;
                                }
                                break;
                            case 'L':
                                if (name[7] == 'a' && name[8] == 'y' && name[9] == 'e' && name[10] == 'r' && name[11] == 'P' && name[12] == 'a' && name[13] == 'l' && name[14] == 'e' && name[15] == 't' && name[16] == 't' && name[17] == 'e' && name[18] == 'E' && name[19] == 'n' && name[20] == 't' && name[21] == 'r' && name[22] == 'i' && name[23] == 'e' && name[24] == 's' && name[25] == '\0') {
                                    // wglGetLayerPaletteEntries
                                    return;
                                }
                                break;
                            case 'P':
                                switch (name[7]) {
                                case 'b':
                                    if (name[8] == 'u' && name[9] == 'f' && name[10] == 'f' && name[11] == 'e' && name[12] == 'r' && name[13] == 'D' && name[14] == 'C' && name[15] == 'A' && name[16] == 'R' && name[17] == 'B' && name[18] == '\0') {
                                        // wglGetPbufferDCARB
                                        retrace_wglGetPbufferDCARB(call);
                                        return;
                                    }
                                    break;
                                case 'i':
                                    switch (name[8]) {
                                    case 'x':
                                        switch (name[9]) {
                                        case 'e':
                                            switch (name[10]) {
                                            case 'l':
                                                switch (name[11]) {
                                                case 'F':
                                                    switch (name[12]) {
                                                    case 'o':
                                                        switch (name[13]) {
                                                        case 'r':
                                                            switch (name[14]) {
                                                            case 'm':
                                                                switch (name[15]) {
                                                                case 'a':
                                                                    switch (name[16]) {
                                                                    case 't':
                                                                        switch (name[17]) {
                                                                        case '\0':
                                                                            // wglGetPixelFormat
                                                                            return;
                                                                            break;
                                                                        case 'A':
                                                                            switch (name[18]) {
                                                                            case 't':
                                                                                switch (name[19]) {
                                                                                case 't':
                                                                                    switch (name[20]) {
                                                                                    case 'r':
                                                                                        switch (name[21]) {
                                                                                        case 'i':
                                                                                            switch (name[22]) {
                                                                                            case 'b':
                                                                                                switch (name[23]) {
                                                                                                case 'f':
                                                                                                    switch (name[24]) {
                                                                                                    case 'v':
                                                                                                        switch (name[25]) {
                                                                                                        case 'A':
                                                                                                            if (name[26] == 'R' && name[27] == 'B' && name[28] == '\0') {
                                                                                                                // wglGetPixelFormatAttribfvARB
                                                                                                                return;
                                                                                                            }
                                                                                                            break;
                                                                                                        case 'E':
                                                                                                            if (name[26] == 'X' && name[27] == 'T' && name[28] == '\0') {
                                                                                                                // wglGetPixelFormatAttribfvEXT
                                                                                                                return;
                                                                                                            }
                                                                                                            break;
                                                                                                        }
                                                                                                        break;
                                                                                                    }
                                                                                                    break;
                                                                                                case 'i':
                                                                                                    switch (name[24]) {
                                                                                                    case 'v':
                                                                                                        switch (name[25]) {
                                                                                                        case 'A':
                                                                                                            if (name[26] == 'R' && name[27] == 'B' && name[28] == '\0') {
                                                                                                                // wglGetPixelFormatAttribivARB
                                                                                                                return;
                                                                                                            }
                                                                                                            break;
                                                                                                        case 'E':
                                                                                                            if (name[26] == 'X' && name[27] == 'T' && name[28] == '\0') {
                                                                                                                // wglGetPixelFormatAttribivEXT
                                                                                                                return;
                                                                                                            }
                                                                                                            break;
                                                                                                        }
                                                                                                        break;
                                                                                                    }
                                                                                                    break;
                                                                                                }
                                                                                                break;
                                                                                            }
                                                                                            break;
                                                                                        }
                                                                                        break;
                                                                                    }
                                                                                    break;
                                                                                }
                                                                                break;
                                                                            }
                                                                            break;
                                                                        }
                                                                        break;
                                                                    }
                                                                    break;
                                                                }
                                                                break;
                                                            }
                                                            break;
                                                        }
                                                        break;
                                                    }
                                                    break;
                                                }
                                                break;
                                            }
                                            break;
                                        }
                                        break;
                                    }
                                    break;
                                case 'r':
                                    if (name[8] == 'o' && name[9] == 'c' && name[10] == 'A' && name[11] == 'd' && name[12] == 'd' && name[13] == 'r' && name[14] == 'e' && name[15] == 's' && name[16] == 's' && name[17] == '\0') {
                                        // wglGetProcAddress
                                        retrace_wglGetProcAddress(call);
                                        return;
                                    }
                                    break;
                                }
                                break;
                            case 'S':
                                if (name[7] == 'w' && name[8] == 'a' && name[9] == 'p' && name[10] == 'I' && name[11] == 'n' && name[12] == 't' && name[13] == 'e' && name[14] == 'r' && name[15] == 'v' && name[16] == 'a' && name[17] == 'l' && name[18] == 'E' && name[19] == 'X' && name[20] == 'T' && name[21] == '\0') {
                                    // wglGetSwapIntervalEXT
                                    return;
                                }
                                break;
                            }
                            break;
                        }
                        break;
                    }
                    break;
                case 'M':
                    switch (name[4]) {
                    case 'a':
                        switch (name[5]) {
                        case 'k':
                            switch (name[6]) {
                            case 'e':
                                switch (name[7]) {
                                case 'C':
                                    switch (name[8]) {
                                    case 'o':
                                        switch (name[9]) {
                                        case 'n':
                                            switch (name[10]) {
                                            case 't':
                                                switch (name[11]) {
                                                case 'e':
                                                    switch (name[12]) {
                                                    case 'x':
                                                        switch (name[13]) {
                                                        case 't':
                                                            switch (name[14]) {
                                                            case 'C':
                                                                switch (name[15]) {
                                                                case 'u':
                                                                    switch (name[16]) {
                                                                    case 'r':
                                                                        switch (name[17]) {
                                                                        case 'r':
                                                                            switch (name[18]) {
                                                                            case 'e':
                                                                                switch (name[19]) {
                                                                                case 'n':
                                                                                    switch (name[20]) {
                                                                                    case 't':
                                                                                        switch (name[21]) {
                                                                                        case 'A':
                                                                                            if (name[22] == 'R' && name[23] == 'B' && name[24] == '\0') {
                                                                                                // wglMakeContextCurrentARB
                                                                                                retrace_wglMakeContextCurrentARB(call);
                                                                                                return;
                                                                                            }
                                                                                            break;
                                                                                        case 'E':
                                                                                            if (name[22] == 'X' && name[23] == 'T' && name[24] == '\0') {
                                                                                                // wglMakeContextCurrentEXT
                                                                                                retrace_wglMakeContextCurrentEXT(call);
                                                                                                return;
                                                                                            }
                                                                                            break;
                                                                                        }
                                                                                        break;
                                                                                    }
                                                                                    break;
                                                                                }
                                                                                break;
                                                                            }
                                                                            break;
                                                                        }
                                                                        break;
                                                                    }
                                                                    break;
                                                                }
                                                                break;
                                                            }
                                                            break;
                                                        }
                                                        break;
                                                    }
                                                    break;
                                                }
                                                break;
                                            }
                                            break;
                                        }
                                        break;
                                    case 'u':
                                        if (name[9] == 'r' && name[10] == 'r' && name[11] == 'e' && name[12] == 'n' && name[13] == 't' && name[14] == '\0') {
                                            // wglMakeCurrent
                                            retrace_wglMakeCurrent(call);
                                            return;
                                        }
                                        break;
                                    }
                                    break;
                                }
                                break;
                            }
                            break;
                        }
                        break;
                    }
                    break;
                case 'Q':
                    if (name[4] == 'u' && name[5] == 'e' && name[6] == 'r' && name[7] == 'y' && name[8] == 'P' && name[9] == 'b' && name[10] == 'u' && name[11] == 'f' && name[12] == 'f' && name[13] == 'e' && name[14] == 'r' && name[15] == 'A' && name[16] == 'R' && name[17] == 'B' && name[18] == '\0') {
                        // wglQueryPbufferARB
                        retrace_wglQueryPbufferARB(call);
                        return;
                    }
                    break;
                case 'R':
                    switch (name[4]) {
                    case 'e':
                        switch (name[5]) {
                        case 'a':
                            if (name[6] == 'l' && name[7] == 'i' && name[8] == 'z' && name[9] == 'e' && name[10] == 'L' && name[11] == 'a' && name[12] == 'y' && name[13] == 'e' && name[14] == 'r' && name[15] == 'P' && name[16] == 'a' && name[17] == 'l' && name[18] == 'e' && name[19] == 't' && name[20] == 't' && name[21] == 'e' && name[22] == '\0') {
                                // wglRealizeLayerPalette
                                retrace_wglRealizeLayerPalette(call);
                                return;
                            }
                            break;
                        case 'l':
                            switch (name[6]) {
                            case 'e':
                                switch (name[7]) {
                                case 'a':
                                    switch (name[8]) {
                                    case 's':
                                        switch (name[9]) {
                                        case 'e':
                                            switch (name[10]) {
                                            case 'P':
                                                if (name[11] == 'b' && name[12] == 'u' && name[13] == 'f' && name[14] == 'f' && name[15] == 'e' && name[16] == 'r' && name[17] == 'D' && name[18] == 'C' && name[19] == 'A' && name[20] == 'R' && name[21] == 'B' && name[22] == '\0') {
                                                    // wglReleasePbufferDCARB
                                                    retrace_wglReleasePbufferDCARB(call);
                                                    return;
                                                }
                                                break;
                                            case 'T':
                                                if (name[11] == 'e' && name[12] == 'x' && name[13] == 'I' && name[14] == 'm' && name[15] == 'a' && name[16] == 'g' && name[17] == 'e' && name[18] == 'A' && name[19] == 'R' && name[20] == 'B' && name[21] == '\0') {
                                                    // wglReleaseTexImageARB
                                                    retrace_wglReleaseTexImageARB(call);
                                                    return;
                                                }
                                                break;
                                            }
                                            break;
                                        }
                                        break;
                                    }
                                    break;
                                }
                                break;
                            }
                            break;
                        case 's':
                            if (name[6] == 't' && name[7] == 'o' && name[8] == 'r' && name[9] == 'e' && name[10] == 'B' && name[11] == 'u' && name[12] == 'f' && name[13] == 'f' && name[14] == 'e' && name[15] == 'r' && name[16] == 'R' && name[17] == 'e' && name[18] == 'g' && name[19] == 'i' && name[20] == 'o' && name[21] == 'n' && name[22] == 'A' && name[23] == 'R' && name[24] == 'B' && name[25] == '\0') {
                                // wglRestoreBufferRegionARB
                                retrace_wglRestoreBufferRegionARB(call);
                                return;
                            }
                            break;
                        }
                        break;
                    }
                    break;
                case 'S':
                    switch (name[4]) {
                    case 'a':
                        if (name[5] == 'v' && name[6] == 'e' && name[7] == 'B' && name[8] == 'u' && name[9] == 'f' && name[10] == 'f' && name[11] == 'e' && name[12] == 'r' && name[13] == 'R' && name[14] == 'e' && name[15] == 'g' && name[16] == 'i' && name[17] == 'o' && name[18] == 'n' && name[19] == 'A' && name[20] == 'R' && name[21] == 'B' && name[22] == '\0') {
                            // wglSaveBufferRegionARB
                            retrace_wglSaveBufferRegionARB(call);
                            return;
                        }
                        break;
                    case 'e':
                        switch (name[5]) {
                        case 't':
                            switch (name[6]) {
                            case 'L':
                                if (name[7] == 'a' && name[8] == 'y' && name[9] == 'e' && name[10] == 'r' && name[11] == 'P' && name[12] == 'a' && name[13] == 'l' && name[14] == 'e' && name[15] == 't' && name[16] == 't' && name[17] == 'e' && name[18] == 'E' && name[19] == 'n' && name[20] == 't' && name[21] == 'r' && name[22] == 'i' && name[23] == 'e' && name[24] == 's' && name[25] == '\0') {
                                    // wglSetLayerPaletteEntries
                                    retrace_wglSetLayerPaletteEntries(call);
                                    return;
                                }
                                break;
                            case 'P':
                                switch (name[7]) {
                                case 'b':
                                    if (name[8] == 'u' && name[9] == 'f' && name[10] == 'f' && name[11] == 'e' && name[12] == 'r' && name[13] == 'A' && name[14] == 't' && name[15] == 't' && name[16] == 'r' && name[17] == 'i' && name[18] == 'b' && name[19] == 'A' && name[20] == 'R' && name[21] == 'B' && name[22] == '\0') {
                                        // wglSetPbufferAttribARB
                                        retrace_wglSetPbufferAttribARB(call);
                                        return;
                                    }
                                    break;
                                case 'i':
                                    if (name[8] == 'x' && name[9] == 'e' && name[10] == 'l' && name[11] == 'F' && name[12] == 'o' && name[13] == 'r' && name[14] == 'm' && name[15] == 'a' && name[16] == 't' && name[17] == '\0') {
                                        // wglSetPixelFormat
                                        retrace_wglSetPixelFormat(call);
                                        return;
                                    }
                                    break;
                                }
                                break;
                            }
                            break;
                        }
                        break;
                    case 'h':
                        if (name[5] == 'a' && name[6] == 'r' && name[7] == 'e' && name[8] == 'L' && name[9] == 'i' && name[10] == 's' && name[11] == 't' && name[12] == 's' && name[13] == '\0') {
                            // wglShareLists
                            retrace_wglShareLists(call);
                            return;
                        }
                        break;
                    case 'w':
                        switch (name[5]) {
                        case 'a':
                            switch (name[6]) {
                            case 'p':
                                switch (name[7]) {
                                case 'B':
                                    if (name[8] == 'u' && name[9] == 'f' && name[10] == 'f' && name[11] == 'e' && name[12] == 'r' && name[13] == 's' && name[14] == '\0') {
                                        // wglSwapBuffers
                                        retrace_wglSwapBuffers(call);
                                        return;
                                    }
                                    break;
                                case 'I':
                                    if (name[8] == 'n' && name[9] == 't' && name[10] == 'e' && name[11] == 'r' && name[12] == 'v' && name[13] == 'a' && name[14] == 'l' && name[15] == 'E' && name[16] == 'X' && name[17] == 'T' && name[18] == '\0') {
                                        // wglSwapIntervalEXT
                                        retrace_wglSwapIntervalEXT(call);
                                        return;
                                    }
                                    break;
                                case 'L':
                                    if (name[8] == 'a' && name[9] == 'y' && name[10] == 'e' && name[11] == 'r' && name[12] == 'B' && name[13] == 'u' && name[14] == 'f' && name[15] == 'f' && name[16] == 'e' && name[17] == 'r' && name[18] == 's' && name[19] == '\0') {
                                        // wglSwapLayerBuffers
                                        retrace_wglSwapLayerBuffers(call);
                                        return;
                                    }
                                    break;
                                case 'M':
                                    if (name[8] == 'u' && name[9] == 'l' && name[10] == 't' && name[11] == 'i' && name[12] == 'p' && name[13] == 'l' && name[14] == 'e' && name[15] == 'B' && name[16] == 'u' && name[17] == 'f' && name[18] == 'f' && name[19] == 'e' && name[20] == 'r' && name[21] == 's' && name[22] == '\0') {
                                        // wglSwapMultipleBuffers
                                        retrace_wglSwapMultipleBuffers(call);
                                        return;
                                    }
                                    break;
                                }
                                break;
                            }
                            break;
                        }
                        break;
                    }
                    break;
                case 'U':
                    switch (name[4]) {
                    case 's':
                        switch (name[5]) {
                        case 'e':
                            switch (name[6]) {
                            case 'F':
                                switch (name[7]) {
                                case 'o':
                                    switch (name[8]) {
                                    case 'n':
                                        switch (name[9]) {
                                        case 't':
                                            switch (name[10]) {
                                            case 'B':
                                                switch (name[11]) {
                                                case 'i':
                                                    switch (name[12]) {
                                                    case 't':
                                                        switch (name[13]) {
                                                        case 'm':
                                                            switch (name[14]) {
                                                            case 'a':
                                                                switch (name[15]) {
                                                                case 'p':
                                                                    switch (name[16]) {
                                                                    case 's':
                                                                        switch (name[17]) {
                                                                        case 'A':
                                                                            if (name[18] == '\0') {
                                                                                // wglUseFontBitmapsA
                                                                                retrace_wglUseFontBitmapsA(call);
                                                                                return;
                                                                            }
                                                                            break;
                                                                        case 'W':
                                                                            if (name[18] == '\0') {
                                                                                // wglUseFontBitmapsW
                                                                                retrace_wglUseFontBitmapsW(call);
                                                                                return;
                                                                            }
                                                                            break;
                                                                        }
                                                                        break;
                                                                    }
                                                                    break;
                                                                }
                                                                break;
                                                            }
                                                            break;
                                                        }
                                                        break;
                                                    }
                                                    break;
                                                }
                                                break;
                                            case 'O':
                                                switch (name[11]) {
                                                case 'u':
                                                    switch (name[12]) {
                                                    case 't':
                                                        switch (name[13]) {
                                                        case 'l':
                                                            switch (name[14]) {
                                                            case 'i':
                                                                switch (name[15]) {
                                                                case 'n':
                                                                    switch (name[16]) {
                                                                    case 'e':
                                                                        switch (name[17]) {
                                                                        case 's':
                                                                            switch (name[18]) {
                                                                            case 'A':
                                                                                if (name[19] == '\0') {
                                                                                    // wglUseFontOutlinesA
                                                                                    retrace_wglUseFontOutlinesA(call);
                                                                                    return;
                                                                                }
                                                                                break;
                                                                            case 'W':
                                                                                if (name[19] == '\0') {
                                                                                    // wglUseFontOutlinesW
                                                                                    retrace_wglUseFontOutlinesW(call);
                                                                                    return;
                                                                                }
                                                                                break;
                                                                            }
                                                                            break;
                                                                        }
                                                                        break;
                                                                    }
                                                                    break;
                                                                }
                                                                break;
                                                            }
                                                            break;
                                                        }
                                                        break;
                                                    }
                                                    break;
                                                }
                                                break;
                                            }
                                            break;
                                        }
                                        break;
                                    }
                                    break;
                                }
                                break;
                            }
                            break;
                        }
                        break;
                    }
                    break;
                }
                break;
            }
            break;
        }
        break;
    }
    retrace::retrace_unknown(call);
}

