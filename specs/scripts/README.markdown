This directory contains several helper scripts that facilitate the generation
of the API descriptions from specs and/or header files.

The specs/headers are not expressive enough, which is why we can't just code
generate everything from them directly.  However the scripts in this directory
usually get 90% of the work done automatically.


OpenGL
======

For OpenGL the typical procedure is to run

    make -B

which will generate several python scripts with prototypes and defines from the
Khronos `.spec` files:
    
* glapi.py

* glparams.py 

* glxapi.py 

* wglapi.py

* wglenum.py

and then manually crossport new functions / enums to the identically named
files in the parent dir via a side-by-side diff tool, such as gvimdiff.


OpenGL ES
=========

Khronos doesn't provide `.spec` files for OpenGL ES.  But the `gltxt.py` script
can extract and convert prototypes for the `.txt` extension specifications:

    $ ./gltxt.py http://www.khronos.org/registry/gles/extensions/OES/OES_mapbuffer.txt
        # GL_OES_mapbuffer
        GlFunction(Void, "glGetBufferPointervOES", [(GLenum, "target"), (GLenum, "pname"), (OpaquePointer(OpaquePointer(Void)), "params")], sideeffects=False),
        GlFunction(OpaquePointer(Void), "glMapBufferOES", [(GLenum, "target"), (GLenum, "access")]),
        GlFunction(GLboolean, "glUnmapBufferOES", [(GLenum, "target")]),


Generic
=======

When the domain specific scripts don't work the fallback solution is `cdecl.py`, which can parse most C declarations:

    $ echo 'void *memcpy(void *dest, const void *src, size_t n);' | ./cdecl.py 
        Function(OpaquePointer(Void), "memcpy", [(OpaquePointer(Void), "dest"), (OpaquePointer(Const(Void)), "src"), (size_t, "n")]),


