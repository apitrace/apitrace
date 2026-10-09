// SPDX-License-Identifier: MIT

/*
 * EGL helpers shared by the EGL-based glws backends.
 */

#pragma once

#include <stdint.h>

#include "glproc.hpp"
#include "glws.hpp"

#include <EGL/eglext.h>


namespace glws {


class EglImage : public Image
{
public:
    EGLDisplay display;
    EGLImageKHR image;

    EglImage(EGLDisplay dpy, EGLImageKHR img) :
        display(dpy),
        image(img)
    {}

    ~EglImage() {
        eglDestroyImageKHR(display, image);
    }

    void
    bind(int target) override {
        glEGLImageTargetTexture2DOES(target, image);
    }
};


/*
 * Create an image from a GL_TEXTURE_2D texture of an EGL context, or return
 * NULL if EGL_KHR_gl_texture_2D_image is not supported.
 */
inline Image *
createEglImage(EGLDisplay display, EGLContext context, unsigned texture)
{
    const char *extensions = eglQueryString(display, EGL_EXTENSIONS);
    if (!extensions ||
        !checkExtension("EGL_KHR_gl_texture_2D_image", extensions)) {
        return NULL;
    }

    EGLImageKHR image = eglCreateImageKHR(display, context, EGL_GL_TEXTURE_2D_KHR,
                                          (EGLClientBuffer)(uintptr_t)texture, NULL);
    if (image == EGL_NO_IMAGE_KHR) {
        return NULL;
    }
    return new EglImage(display, image);
}


} /* namespace glws */
