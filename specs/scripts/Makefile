
all: \
	download \
	glapi.py \
	glparams.py \
	glxapi.py \
	wglapi.py \
	wglenum.py \
	eglenum.py

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
	wgl.tm \
	eglenum.spec

%.spec:
	wget -N http://www.opengl.org/registry/api/$@

%.tm:
	wget -N http://www.opengl.org/registry/api/$@

eglenum.spec:
	wget -N http://www.khronos.org/registry/egl/api/$@

glapi.py: glspec.py gl.tm gl.spec
	python glspec.py gl gl.tm gl.spec > $@

glxapi.py: glspec.py glx.tm glx.spec glxext.spec
	python glspec.py glX glx.tm glx.spec glxext.spec > $@

wglapi.py: glspec.py wgl.tm wgl.spec wglext.spec
	python glspec.py wgl wgl.tm wgl.spec wglext.spec > $@

glparams.py: glparams.sed enum.spec sort.sh
	sed -n -f glparams.sed enum.spec | ./sort.sh > $@

wglenum.py: wglenum.sh wglenumext.spec
	./wglenum.sh wglenumext.spec > $@

eglenum.py: eglenum.sh eglenum.spec
	./eglenum.sh eglenum.spec > $@

clean:
	rm -f \
        glapi.py glxapi.py wglapi.py \
        glparams.py wglenum.py

.PRECIOUS: %.spec %.tm

.PHONY: download clean
