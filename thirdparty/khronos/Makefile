update: \
	GL/glext.h \
	GL/glxext.h \
	GL/wglext.h \
	EGL/egl.h \
	EGL/eglext.h \
	EGL/eglplatform.h \
	KHR/khrplatform.h \
	GLES/gl.h \
	GLES/glext.h \
	GLES/glplatform.h \
	GLES2/gl2.h \
	GLES2/gl2ext.h \
	GLES2/gl2platform.h

GL/wglext.h GL/glxext.h: FORCE
	wget -O $@ "http://www.opengl.org/registry/api/$(@F)"

GL/glext.h: GL/glext.patch FORCE
	wget -O $@ "http://www.opengl.org/registry/api/$(@F)"
	patch $@ $<

EGL/egl.h EGL/eglext.h KHR/khrplatform.h: FORCE
	wget -O $@ "http://www.khronos.org/registry/egl/api/$@"

EGL/eglplatform.h: EGL/eglplatform.patch FORCE
	wget -O $@ "http://www.khronos.org/registry/egl/api/$@"
	patch $@ $<

GLES/gl.h GLES/glext.h GLES/glplatform.h: FORCE
	wget -O $@ "http://www.khronos.org/registry/gles/api/1.1/$(@F)"

GLES2/gl2.h GLES2/gl2ext.h GLES2/gl2platform.h: FORCE
	wget -O $@ "http://www.khronos.org/registry/gles/api/2.0/$(@F)"

.PHONY: update

FORCE:
