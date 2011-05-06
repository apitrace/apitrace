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


typedef std::map<unsigned long, glws::Drawable *> DrawableMap;
typedef std::map<void *, glws::Context *> ContextMap;
static DrawableMap drawable_map;
static ContextMap context_map;


static glws::Drawable *
getDrawable(unsigned long drawable_id) {
    if (drawable_id == 0) {
        return NULL;
    }

    DrawableMap::const_iterator it;
    it = drawable_map.find(drawable_id);
    if (it == drawable_map.end()) {
        return (drawable_map[drawable_id] = ws->createDrawable(visual));
    }

    return it->second;
}

static void retrace_glXChooseVisual(Trace::Call &call) {
}

static void retrace_glXCreateContext(Trace::Call &call) {
    void * orig_context = call.ret->toPointer();
    glws::Context *context = ws->createContext(glretrace::visual);
    context_map[orig_context] = context;
}

static void retrace_glXDestroyContext(Trace::Call &call) {
}

static void retrace_glXMakeCurrent(Trace::Call &call) {
    if (drawable && context) {
        glFlush();
        if (!double_buffer) {
            frame_complete(call.no);
        }
    }

    glws::Drawable *new_drawable = getDrawable(static_cast<unsigned long>(call.arg(1)));
    glws::Context *new_context = context_map[call.arg(2).toPointer()];

    bool result = ws->makeCurrent(new_drawable, new_context);

    if (new_drawable && new_context && result) {
        drawable = new_drawable;
        context = new_context;
    } else {
        drawable = NULL;
        context = NULL;
    }
}

static void retrace_glXCopyContext(Trace::Call &call) {
}

static void retrace_glXSwapBuffers(Trace::Call &call) {
    frame_complete(call.no);
    if (double_buffer) {
        drawable->swapBuffers();
    } else {
        glFlush();
    }
}

static void retrace_glXCreateGLXPixmap(Trace::Call &call) {
}

static void retrace_glXDestroyGLXPixmap(Trace::Call &call) {
}

static void retrace_glXQueryExtension(Trace::Call &call) {
}

static void retrace_glXQueryVersion(Trace::Call &call) {
}

static void retrace_glXIsDirect(Trace::Call &call) {
}

static void retrace_glXGetConfig(Trace::Call &call) {
}

static void retrace_glXWaitGL(Trace::Call &call) {
}

static void retrace_glXWaitX(Trace::Call &call) {
}

static void retrace_glXUseXFont(Trace::Call &call) {
}

static void retrace_glXQueryExtensionsString(Trace::Call &call) {
}

static void retrace_glXQueryServerString(Trace::Call &call) {
}

static void retrace_glXGetClientString(Trace::Call &call) {
}

static void retrace_glXChooseFBConfig(Trace::Call &call) {
}

static void retrace_glXGetFBConfigAttrib(Trace::Call &call) {
}

static void retrace_glXGetFBConfigs(Trace::Call &call) {
}

static void retrace_glXGetVisualFromFBConfig(Trace::Call &call) {
}

static void retrace_glXCreateWindow(Trace::Call &call) {
}

static void retrace_glXDestroyWindow(Trace::Call &call) {
}

static void retrace_glXCreatePixmap(Trace::Call &call) {
}

static void retrace_glXDestroyPixmap(Trace::Call &call) {
}

static void retrace_glXCreatePbuffer(Trace::Call &call) {
}

static void retrace_glXDestroyPbuffer(Trace::Call &call) {
}

static void retrace_glXQueryDrawable(Trace::Call &call) {
}

static void retrace_glXCreateNewContext(Trace::Call &call) {
    retrace_glXCreateContext(call);
}

static void retrace_glXMakeContextCurrent(Trace::Call &call) {
    if (drawable && context) {
        glFlush();
        if (!double_buffer) {
            frame_complete(call.no);
        }
    }

    glws::Drawable *new_drawable = getDrawable(static_cast<unsigned long>(call.arg(1)));
    glws::Context *new_context = context_map[call.arg(3).toPointer()];

    bool result = ws->makeCurrent(new_drawable, new_context);

    if (new_drawable && new_context && result) {
        drawable = new_drawable;
        context = new_context;
    } else {
        drawable = NULL;
        context = NULL;
    }
}

static void retrace_glXGetCurrentReadDrawable(Trace::Call &call) {
}

static void retrace_glXQueryContext(Trace::Call &call) {
}

static void retrace_glXSelectEvent(Trace::Call &call) {
}

static void retrace_glXGetSelectedEvent(Trace::Call &call) {
}

static void retrace_glXGetProcAddressARB(Trace::Call &call) {
}

static void retrace_glXGetProcAddress(Trace::Call &call) {
}

void glretrace::retrace_call_glx(Trace::Call &call) {
    const char *name = call.name().c_str();

    switch (name[3]) {
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
                            case 'F':
                                if (name[10] == 'B' && name[11] == 'C' && name[12] == 'o' && name[13] == 'n' && name[14] == 'f' && name[15] == 'i' && name[16] == 'g' && name[17] == '\0') {
                                    // glXChooseFBConfig
                                    retrace_glXChooseFBConfig(call);
                                    return;
                                }
                                break;
                            case 'V':
                                if (name[10] == 'i' && name[11] == 's' && name[12] == 'u' && name[13] == 'a' && name[14] == 'l' && name[15] == '\0') {
                                    // glXChooseVisual
                                    retrace_glXChooseVisual(call);
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
        case 'o':
            if (name[5] == 'p' && name[6] == 'y' && name[7] == 'C' && name[8] == 'o' && name[9] == 'n' && name[10] == 't' && name[11] == 'e' && name[12] == 'x' && name[13] == 't' && name[14] == '\0') {
                // glXCopyContext
                retrace_glXCopyContext(call);
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
                            case 'C':
                                if (name[10] == 'o' && name[11] == 'n' && name[12] == 't' && name[13] == 'e' && name[14] == 'x' && name[15] == 't' && name[16] == '\0') {
                                    // glXCreateContext
                                    retrace_glXCreateContext(call);
                                    return;
                                }
                                break;
                            case 'G':
                                if (name[10] == 'L' && name[11] == 'X' && name[12] == 'P' && name[13] == 'i' && name[14] == 'x' && name[15] == 'm' && name[16] == 'a' && name[17] == 'p' && name[18] == '\0') {
                                    // glXCreateGLXPixmap
                                    retrace_glXCreateGLXPixmap(call);
                                    return;
                                }
                                break;
                            case 'N':
                                if (name[10] == 'e' && name[11] == 'w' && name[12] == 'C' && name[13] == 'o' && name[14] == 'n' && name[15] == 't' && name[16] == 'e' && name[17] == 'x' && name[18] == 't' && name[19] == '\0') {
                                    // glXCreateNewContext
                                    retrace_glXCreateNewContext(call);
                                    return;
                                }
                                break;
                            case 'P':
                                switch (name[10]) {
                                case 'b':
                                    if (name[11] == 'u' && name[12] == 'f' && name[13] == 'f' && name[14] == 'e' && name[15] == 'r' && name[16] == '\0') {
                                        // glXCreatePbuffer
                                        retrace_glXCreatePbuffer(call);
                                        return;
                                    }
                                    break;
                                case 'i':
                                    if (name[11] == 'x' && name[12] == 'm' && name[13] == 'a' && name[14] == 'p' && name[15] == '\0') {
                                        // glXCreatePixmap
                                        retrace_glXCreatePixmap(call);
                                        return;
                                    }
                                    break;
                                }
                                break;
                            case 'W':
                                if (name[10] == 'i' && name[11] == 'n' && name[12] == 'd' && name[13] == 'o' && name[14] == 'w' && name[15] == '\0') {
                                    // glXCreateWindow
                                    retrace_glXCreateWindow(call);
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
            case 's':
                switch (name[6]) {
                case 't':
                    switch (name[7]) {
                    case 'r':
                        switch (name[8]) {
                        case 'o':
                            switch (name[9]) {
                            case 'y':
                                switch (name[10]) {
                                case 'C':
                                    if (name[11] == 'o' && name[12] == 'n' && name[13] == 't' && name[14] == 'e' && name[15] == 'x' && name[16] == 't' && name[17] == '\0') {
                                        // glXDestroyContext
                                        retrace_glXDestroyContext(call);
                                        return;
                                    }
                                    break;
                                case 'G':
                                    if (name[11] == 'L' && name[12] == 'X' && name[13] == 'P' && name[14] == 'i' && name[15] == 'x' && name[16] == 'm' && name[17] == 'a' && name[18] == 'p' && name[19] == '\0') {
                                        // glXDestroyGLXPixmap
                                        retrace_glXDestroyGLXPixmap(call);
                                        return;
                                    }
                                    break;
                                case 'P':
                                    switch (name[11]) {
                                    case 'b':
                                        if (name[12] == 'u' && name[13] == 'f' && name[14] == 'f' && name[15] == 'e' && name[16] == 'r' && name[17] == '\0') {
                                            // glXDestroyPbuffer
                                            retrace_glXDestroyPbuffer(call);
                                            return;
                                        }
                                        break;
                                    case 'i':
                                        if (name[12] == 'x' && name[13] == 'm' && name[14] == 'a' && name[15] == 'p' && name[16] == '\0') {
                                            // glXDestroyPixmap
                                            retrace_glXDestroyPixmap(call);
                                            return;
                                        }
                                        break;
                                    }
                                    break;
                                case 'W':
                                    if (name[11] == 'i' && name[12] == 'n' && name[13] == 'd' && name[14] == 'o' && name[15] == 'w' && name[16] == '\0') {
                                        // glXDestroyWindow
                                        retrace_glXDestroyWindow(call);
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
    case 'G':
        switch (name[4]) {
        case 'e':
            switch (name[5]) {
            case 't':
                switch (name[6]) {
                case 'C':
                    switch (name[7]) {
                    case 'l':
                        if (name[8] == 'i' && name[9] == 'e' && name[10] == 'n' && name[11] == 't' && name[12] == 'S' && name[13] == 't' && name[14] == 'r' && name[15] == 'i' && name[16] == 'n' && name[17] == 'g' && name[18] == '\0') {
                            // glXGetClientString
                            retrace_glXGetClientString(call);
                            return;
                        }
                        break;
                    case 'o':
                        if (name[8] == 'n' && name[9] == 'f' && name[10] == 'i' && name[11] == 'g' && name[12] == '\0') {
                            // glXGetConfig
                            retrace_glXGetConfig(call);
                            return;
                        }
                        break;
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
                                                    // glXGetCurrentContext
                                                    return;
                                                }
                                                break;
                                            case 'D':
                                                switch (name[14]) {
                                                case 'i':
                                                    if (name[15] == 's' && name[16] == 'p' && name[17] == 'l' && name[18] == 'a' && name[19] == 'y' && name[20] == '\0') {
                                                        // glXGetCurrentDisplay
                                                        return;
                                                    }
                                                    break;
                                                case 'r':
                                                    if (name[15] == 'a' && name[16] == 'w' && name[17] == 'a' && name[18] == 'b' && name[19] == 'l' && name[20] == 'e' && name[21] == '\0') {
                                                        // glXGetCurrentDrawable
                                                        return;
                                                    }
                                                    break;
                                                }
                                                break;
                                            case 'R':
                                                if (name[14] == 'e' && name[15] == 'a' && name[16] == 'd' && name[17] == 'D' && name[18] == 'r' && name[19] == 'a' && name[20] == 'w' && name[21] == 'a' && name[22] == 'b' && name[23] == 'l' && name[24] == 'e' && name[25] == '\0') {
                                                    // glXGetCurrentReadDrawable
                                                    retrace_glXGetCurrentReadDrawable(call);
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
                case 'F':
                    switch (name[7]) {
                    case 'B':
                        switch (name[8]) {
                        case 'C':
                            switch (name[9]) {
                            case 'o':
                                switch (name[10]) {
                                case 'n':
                                    switch (name[11]) {
                                    case 'f':
                                        switch (name[12]) {
                                        case 'i':
                                            switch (name[13]) {
                                            case 'g':
                                                switch (name[14]) {
                                                case 'A':
                                                    if (name[15] == 't' && name[16] == 't' && name[17] == 'r' && name[18] == 'i' && name[19] == 'b' && name[20] == '\0') {
                                                        // glXGetFBConfigAttrib
                                                        retrace_glXGetFBConfigAttrib(call);
                                                        return;
                                                    }
                                                    break;
                                                case 's':
                                                    if (name[15] == '\0') {
                                                        // glXGetFBConfigs
                                                        retrace_glXGetFBConfigs(call);
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
                case 'P':
                    switch (name[7]) {
                    case 'r':
                        switch (name[8]) {
                        case 'o':
                            switch (name[9]) {
                            case 'c':
                                switch (name[10]) {
                                case 'A':
                                    switch (name[11]) {
                                    case 'd':
                                        switch (name[12]) {
                                        case 'd':
                                            switch (name[13]) {
                                            case 'r':
                                                switch (name[14]) {
                                                case 'e':
                                                    switch (name[15]) {
                                                    case 's':
                                                        switch (name[16]) {
                                                        case 's':
                                                            switch (name[17]) {
                                                            case '\0':
                                                                // glXGetProcAddress
                                                                retrace_glXGetProcAddress(call);
                                                                return;
                                                                break;
                                                            case 'A':
                                                                if (name[18] == 'R' && name[19] == 'B' && name[20] == '\0') {
                                                                    // glXGetProcAddressARB
                                                                    retrace_glXGetProcAddressARB(call);
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
                case 'S':
                    if (name[7] == 'e' && name[8] == 'l' && name[9] == 'e' && name[10] == 'c' && name[11] == 't' && name[12] == 'e' && name[13] == 'd' && name[14] == 'E' && name[15] == 'v' && name[16] == 'e' && name[17] == 'n' && name[18] == 't' && name[19] == '\0') {
                        // glXGetSelectedEvent
                        retrace_glXGetSelectedEvent(call);
                        return;
                    }
                    break;
                case 'V':
                    if (name[7] == 'i' && name[8] == 's' && name[9] == 'u' && name[10] == 'a' && name[11] == 'l' && name[12] == 'F' && name[13] == 'r' && name[14] == 'o' && name[15] == 'm' && name[16] == 'F' && name[17] == 'B' && name[18] == 'C' && name[19] == 'o' && name[20] == 'n' && name[21] == 'f' && name[22] == 'i' && name[23] == 'g' && name[24] == '\0') {
                        // glXGetVisualFromFBConfig
                        retrace_glXGetVisualFromFBConfig(call);
                        return;
                    }
                    break;
                }
                break;
            }
            break;
        }
        break;
    case 'I':
        if (name[4] == 's' && name[5] == 'D' && name[6] == 'i' && name[7] == 'r' && name[8] == 'e' && name[9] == 'c' && name[10] == 't' && name[11] == '\0') {
            // glXIsDirect
            retrace_glXIsDirect(call);
            return;
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
                            if (name[9] == 'n' && name[10] == 't' && name[11] == 'e' && name[12] == 'x' && name[13] == 't' && name[14] == 'C' && name[15] == 'u' && name[16] == 'r' && name[17] == 'r' && name[18] == 'e' && name[19] == 'n' && name[20] == 't' && name[21] == '\0') {
                                // glXMakeContextCurrent
                                retrace_glXMakeContextCurrent(call);
                                return;
                            }
                            break;
                        case 'u':
                            if (name[9] == 'r' && name[10] == 'r' && name[11] == 'e' && name[12] == 'n' && name[13] == 't' && name[14] == '\0') {
                                // glXMakeCurrent
                                retrace_glXMakeCurrent(call);
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
        switch (name[4]) {
        case 'u':
            switch (name[5]) {
            case 'e':
                switch (name[6]) {
                case 'r':
                    switch (name[7]) {
                    case 'y':
                        switch (name[8]) {
                        case 'C':
                            if (name[9] == 'o' && name[10] == 'n' && name[11] == 't' && name[12] == 'e' && name[13] == 'x' && name[14] == 't' && name[15] == '\0') {
                                // glXQueryContext
                                retrace_glXQueryContext(call);
                                return;
                            }
                            break;
                        case 'D':
                            if (name[9] == 'r' && name[10] == 'a' && name[11] == 'w' && name[12] == 'a' && name[13] == 'b' && name[14] == 'l' && name[15] == 'e' && name[16] == '\0') {
                                // glXQueryDrawable
                                retrace_glXQueryDrawable(call);
                                return;
                            }
                            break;
                        case 'E':
                            switch (name[9]) {
                            case 'x':
                                switch (name[10]) {
                                case 't':
                                    switch (name[11]) {
                                    case 'e':
                                        switch (name[12]) {
                                        case 'n':
                                            switch (name[13]) {
                                            case 's':
                                                switch (name[14]) {
                                                case 'i':
                                                    switch (name[15]) {
                                                    case 'o':
                                                        switch (name[16]) {
                                                        case 'n':
                                                            switch (name[17]) {
                                                            case '\0':
                                                                // glXQueryExtension
                                                                retrace_glXQueryExtension(call);
                                                                return;
                                                                break;
                                                            case 's':
                                                                if (name[18] == 'S' && name[19] == 't' && name[20] == 'r' && name[21] == 'i' && name[22] == 'n' && name[23] == 'g' && name[24] == '\0') {
                                                                    // glXQueryExtensionsString
                                                                    retrace_glXQueryExtensionsString(call);
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
                        case 'S':
                            if (name[9] == 'e' && name[10] == 'r' && name[11] == 'v' && name[12] == 'e' && name[13] == 'r' && name[14] == 'S' && name[15] == 't' && name[16] == 'r' && name[17] == 'i' && name[18] == 'n' && name[19] == 'g' && name[20] == '\0') {
                                // glXQueryServerString
                                retrace_glXQueryServerString(call);
                                return;
                            }
                            break;
                        case 'V':
                            if (name[9] == 'e' && name[10] == 'r' && name[11] == 's' && name[12] == 'i' && name[13] == 'o' && name[14] == 'n' && name[15] == '\0') {
                                // glXQueryVersion
                                retrace_glXQueryVersion(call);
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
    case 'S':
        switch (name[4]) {
        case 'e':
            if (name[5] == 'l' && name[6] == 'e' && name[7] == 'c' && name[8] == 't' && name[9] == 'E' && name[10] == 'v' && name[11] == 'e' && name[12] == 'n' && name[13] == 't' && name[14] == '\0') {
                // glXSelectEvent
                retrace_glXSelectEvent(call);
                return;
            }
            break;
        case 'w':
            if (name[5] == 'a' && name[6] == 'p' && name[7] == 'B' && name[8] == 'u' && name[9] == 'f' && name[10] == 'f' && name[11] == 'e' && name[12] == 'r' && name[13] == 's' && name[14] == '\0') {
                // glXSwapBuffers
                retrace_glXSwapBuffers(call);
                return;
            }
            break;
        }
        break;
    case 'U':
        if (name[4] == 's' && name[5] == 'e' && name[6] == 'X' && name[7] == 'F' && name[8] == 'o' && name[9] == 'n' && name[10] == 't' && name[11] == '\0') {
            // glXUseXFont
            retrace_glXUseXFont(call);
            return;
        }
        break;
    case 'W':
        switch (name[4]) {
        case 'a':
            switch (name[5]) {
            case 'i':
                switch (name[6]) {
                case 't':
                    switch (name[7]) {
                    case 'G':
                        if (name[8] == 'L' && name[9] == '\0') {
                            // glXWaitGL
                            retrace_glXWaitGL(call);
                            return;
                        }
                        break;
                    case 'X':
                        if (name[8] == '\0') {
                            // glXWaitX
                            retrace_glXWaitX(call);
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
    retrace::retrace_unknown(call);
}

