
all: \
	download \
	glapi.py \
	glparams.py \
	glxapi.py \
	wglapi.py \
	wglenum.py \
	eglenum.py

download: \
	gl.xml \
	wgl.xml \
	egl.xml \
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


%.xml:
	wget -N https://cvs.khronos.org/svn/repos/ogl/trunk/doc/registry/public/api/$@

%.spec:
	wget -N http://www.opengl.org/registry/oldspecs/$@

%.tm:
	wget -N http://www.opengl.org/registry/oldspecs/$@

glapi.py: glspec.py gl.tm gl.spec
	python glspec.py gl gl.tm gl.spec > $@

glxapi.py: glspec.py glx.tm glx.spec glxext.spec
	python glspec.py glX glx.tm glx.spec glxext.spec > $@

wglapi.py: glspec.py wgl.tm wgl.spec wglext.spec
	python glspec.py wgl wgl.tm wgl.spec wglext.spec > $@

glparams.py: xml2glparams.py gl.xml
	python $^ > $@

wglenum.py: xml2enum.py wgl.xml
	python $^ > $@

eglenum.py: xml2enum.py egl.xml
	python $^ > $@

clean:
	rm -f \
        glapi.py glxapi.py wglapi.py \
        glparams.py wglenum.py

.PRECIOUS: %.xml %.spec %.tm

.PHONY: download clean
