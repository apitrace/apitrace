
all: \
	download \
	glapi.py \
	glparams.py \
	glxapi.py \
	wglapi.py \
	glxenum.py \
	wglenum.py \
	eglenum.py

download: \
	gl.xml \
	glx.xml \
	wgl.xml \
	egl.xml \
	gl.spec \
	gl.tm \
	glxenum.spec \
	glx.spec \
	glxext.spec \
	glx.tm \
	wglenum.spec \
	wgl.spec \
	wglext.spec \
	wgl.tm


%.xml:
	wget -N https://cvs.khronos.org/svn/repos/ogl/trunk/doc/registry/public/api/$@

%.spec:
	wget -N http://www.opengl.org/registry/oldspecs/$@

%.tm:
	wget -N http://www.opengl.org/registry/oldspecs/$@

glapi.py: spec2api.py gl.tm gl.spec
	python spec2api.py gl gl.tm gl.spec > $@

glxapi.py: spec2api.py glx.tm glx.spec glxext.spec
	python spec2api.py glX glx.tm glx.spec glxext.spec > $@

wglapi.py: spec2api.py wgl.tm wgl.spec wglext.spec
	python spec2api.py wgl wgl.tm wgl.spec wglext.spec > $@

glparams.py: xml2glparams.py gl.xml
	python $^ > $@

glxenum.py: xml2enum.py glx.xml
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
