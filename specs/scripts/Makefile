
all: \
	download \
	glapi.py \
	glparams.py \
	glxapi.py \
	wglapi.py \
	eglapi.py \
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
	wget -nv -N https://cvs.khronos.org/svn/repos/ogl/trunk/doc/registry/public/api/$@

%.spec:
	wget -nv -N https://www.opengl.org/registry/oldspecs/$@

%.tm:
	wget -nv -N https://www.opengl.org/registry/oldspecs/$@

glapi.py: xml2api.py gl.xml
	python3 $^ > $@

glxapi.py: xml2api.py glx.xml
	python3 $^ > $@

wglapi.py: xml2api.py wgl.xml
	python3 $^ > $@

eglapi.py: xml2api.py egl.xml
	python3 $^ > $@

glparams.py: xml2glparams.py gl.xml
	python3 $^ > $@

glxenum.py: xml2enum.py glx.xml
	python3 $^ > $@

wglenum.py: xml2enum.py wgl.xml
	python3 $^ > $@

eglenum.py: xml2enum.py egl.xml
	python3 $^ > $@

clean:
	rm -f \
        glapi.py glxapi.py wglapi.py \
        glparams.py wglenum.py

.PRECIOUS: %.xml %.spec %.tm

.PHONY: download clean
