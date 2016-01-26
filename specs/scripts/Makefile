
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
	python $^ > $@

glxapi.py: xml2api.py glx.xml
	python $^ > $@

wglapi.py: xml2api.py wgl.xml
	python $^ > $@

eglapi.py: xml2api.py egl.xml
	python $^ > $@

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
