
all: download glapi.py glxapi.py wglapi.py

download: \
	enum.spec \
	enumext.spec \
	gl.spec \
	gl.tm \
	glxenum.spec \
	glxenumext.spec \
	glx.spec \
	glxext.spec \
	glx.tm \
	wglenum.spec \
	wglenumext.spec \
	wgl.spec \
	wglext.spec \
	wgl.tm

%.spec %.tm:
	wget -N http://www.opengl.org/registry/api/$@

glapi.py: spec.py gl.tm gl.spec
	python spec.py gl gl.tm gl.spec > $@

glxapi.py: spec.py glx.tm glx.spec glxext.spec
	python spec.py glX glx.tm glx.spec glxext.spec > $@

wglapi.py: spec.py wgl.tm wgl.spec wglext.spec
	python spec.py wgl wgl.tm wgl.spec wglext.spec > $@

.PRECIOUS: %.spec %.tm
