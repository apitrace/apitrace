update: glext.h glxext.h wglext.h

%.h: FORCE
	wget -N "http://www.opengl.org/registry/api/$@"

glext.h: glext.sed FORCE
	wget -O - "http://www.opengl.org/registry/api/$@" | sed -f $*.sed > $@

.PHONY: update

FORCE:
