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
	wget -nv -O $@ "https://www.opengl.org/registry/api/$@"

GL/glext.h: GL/glext.patch FORCE
	wget -nv -O $@ "https://www.opengl.org/registry/api/$@"
	patch $@ $<

EGL/egl.h EGL/eglext.h KHR/khrplatform.h: FORCE
	wget -nv -O $@ "https://www.khronos.org/registry/egl/api/$@"

EGL/eglplatform.h: EGL/eglplatform.patch FORCE
	wget -nv -O $@ "https://www.khronos.org/registry/egl/api/$@"
	patch $@ $<

GLES/gl.h GLES/glplatform.h: FORCE
	wget -nv -O $@ "https://www.khronos.org/registry/gles/api/$@"

GLES/glext.h: FORCE
	wget -nv -O $@ "https://www.khronos.org/registry/gles/api/$@"
	sed -i.bak -e 's/__glext_h_/__gles_glext_h_/g' $@

GLES2/gl2.h GLES2/gl2ext.h GLES2/gl2platform.h: FORCE
	wget -nv -O $@ "https://www.khronos.org/registry/gles/api/$@"

FORCE:

.PHONY: update FORCE
