About **apitrace**
==================

**apitrace** consists of a set of tools to:

* trace OpenGL, OpenGL ES, D3D9, D3D8, D3D7, and DDRAW APIs calls to a file;

* retrace OpenGL and OpenGL ES calls from a file;

* inspect OpenGL state at any call while retracing;

* visualize and edit trace files.


Basic usage
===========


Linux and Mac OS X
------------------

Run the application you want to trace as

    apitrace trace /path/to/application [args...]

and it will generate a trace named `application.trace` in the current
directory.  You can specify the written trace filename by setting the
`TRACE_FILE` environment variable before running.

View the trace with

    apitrace dump --color application.trace

Replay an OpenGL trace with

    glretrace application.trace

Pass the `-sb` option to use a single buffered visual.  Pass `--help` to
glretrace for more options.

Start the GUI as

    qapitrace application.trace


Windows
-------

* Copy `opengl32.dll`, `d3d8.dll`, or `d3d9.dll` from build/wrappers directory
  to the directory with the application you want to trace.

* Run the application.

* View the trace with

        \path\to\apitrace dump application.trace

* Replay the trace with

        \path\to\glretrace application.trace


Advanced command line usage
===========================


Call sets
---------

Several tools take `CALLSET` arguments, e.g:

    apitrace dump --calls CALLSET foo.trace
    glretrace -S CALLSET foo.trace

The call syntax is very flexible. Here are a few examples:

 * `4`             one call

 * `1,2,4,5`       set of calls

 * `"1 2 4 5"`     set of calls (commas are optional and can be replaced with whitespace)

 * `1-100/2`       calls 1, 3, 5, ...,  99

 * `1-1000/draw`   all draw calls between 1 and 1000

 * `1-1000/fbo`    all fbo changes between calls 1 and 1000

 * `frame`         all calls at end of frames

 * `@foo.txt`      read call numbers from `foo.txt`, using the same syntax as above



Tracing manually
----------------

### Linux ###

Run the application you want to trace as

     LD_PRELOAD=/path/to/apitrace/wrappers/glxtrace.so /path/to/application

and it will generate a trace named `application.trace` in the current
directory.  You can specify the written trace filename by setting the
`TRACE_FILE` environment variable before running.

The `LD_PRELOAD` mechanism should work with most applications.  There are some
applications, e.g., Unigine Heaven, which global function pointers with the
same name as GL entrypoints, living in a shared object that wasn't linked with
`-Bsymbolic` flag, so relocations to those globals function pointers get
overwritten with the address to our wrapper library, and the application will
segfault when trying to write to them.  For these applications it is possible
to trace by using `glxtrace.so` as an ordinary `libGL.so` and injecting into
`LD_LIBRARY_PATH`:

    ln -s glxtrace.so wrappers/libGL.so
    ln -s glxtrace.so wrappers/libGL.so.1
    ln -s glxtrace.so wrappers/libGL.so.1.2
    export LD_LIBRARY_PATH=/path/to/apitrace/wrappers:$LD_LIBRARY_PATH
    export TRACE_LIBGL=/path/to/real/libGL.so.1
    /path/to/application

See the `ld.so` man page for more information about `LD_PRELOAD` and
`LD_LIBRARY_PATH` environment flags.

To trace the application inside gdb, invoke gdb as:

    gdb --ex 'set exec-wrapper env LD_PRELOAD=/path/to/glxtrace.so' --args /path/to/application

### Mac OS X ###

Run the application you want to trace as

    DYLD_LIBRARY_PATH=/path/to/apitrace/wrappers /path/to/application

Note that although Mac OS X has an `LD_PRELOAD` equivalent,
`DYLD_INSERT_LIBRARIES`, it is mostly useless because it only works with
`DYLD_FORCE_FLAT_NAMESPACE=1` which breaks most applications.  See the `dyld` man
page for more details about these environment flags.


Emitting annotations to the trace from GL applications
------------------------------------------------------

You can emit string and frame annotations through the
[`GL_GREMEDY_string_marker`](http://www.opengl.org/registry/specs/GREMEDY/string_marker.txt)
and
[`GL_GREMEDY_frame_terminator`](http://www.opengl.org/registry/specs/GREMEDY/frame_terminator.txt)
GL extensions.

**apitrace** will advertise and intercept these GL extensions independently of
the GL implementation.  So all you have to do is to use these extensions when
available.

For example, if you use [GLEW](http://glew.sourceforge.net/) to dynamically
detect and use GL extensions, you could easily accomplish this by doing:

    void foo() {
    
      if (GLEW_GREMEDY_string_marker) {
        glStringMarkerGREMEDY(0, __FUNCTION__ ": enter");
      }
      
      ...
      
      if (GLEW_GREMEDY_string_marker) {
        glStringMarkerGREMEDY(0, __FUNCTION__ ": leave");
      }
      
    }

This has the added advantage of working equally well with gDEBugger.


Dump GL state at a particular call
----------------------------------

You can get a dump of the bound GL state at call 12345 by doing:

    glretrace -D 12345 application.trace > 12345.json

This is precisely the mechanism the GUI obtains its own state.

You can compare two state dumps by doing:

    apitrace diff-state 12345.json 67890.json


Comparing two traces side by side
---------------------------------

    apitrace diff trace1.trace trace2.trace

This works only on Unices, and it will truncate the traces due to performance
limitations.


Recording a video with FFmpeg
-----------------------------

You can make a video of the output by doing

    glretrace -s - application.trace \
    | ffmpeg -r 30 -f image2pipe -vcodec ppm -i pipe: -vcodec mpeg4 -y output.mp4


Advanced usage for OpenGL implementors
======================================

There are several advanced usage examples meant for OpenGL implementors.


Regression testing
------------------

These are the steps to create a regression test-suite around **apitrace**:

* obtain a trace

* obtain reference snapshots, by doing:

        mkdir /path/to/snapshots/
        glretrace -s /path/to/reference/snapshots/ application.trace

  on reference system.

* prune the snapshots which are not interesting

* to do a regression test, do:

        glretrace -c /path/to/reference/snapshots/ application.trace

  Alternatively, for a HTML summary, use `apitrace diff-images`:

        glretrace -s /path/to/current/snapshots/ application.trace
        apitrace diff-images --output summary.html /path/to/reference/snapshots/ /path/to/current/snapshots/


Automated git-bisection
-----------------------

With tracecheck.py it is possible to automate git bisect and pinpoint the
commit responsible for a regression.

Below is an example of using tracecheck.py to bisect a regression in the
Mesa-based Intel 965 driver.  But the procedure could be applied to any GL
driver hosted on a git repository.

First, create a build script, named build-script.sh, containing:

    #!/bin/sh
    set -e
    export PATH=/usr/lib/ccache:$PATH
    export CFLAGS='-g'
    export CXXFLAGS='-g'
    ./autogen.sh --disable-egl --disable-gallium --disable-glut --disable-glu --disable-glw --with-dri-drivers=i965
    make clean
    make "$@"

It is important that builds are both robust, and efficient.  Due to broken
dependency discovery in Mesa's makefile system, it was necessary invoke `make
clean` in every iteration step.  `ccache` should be installed to avoid
recompiling unchanged source files.

Then do:

    cd /path/to/mesa
    export LIBGL_DEBUG=verbose
    export LD_LIBRARY_PATH=$PWD/lib
    export LIBGL_DRIVERS_DIR=$PWD/lib
    git bisect start \
        6491e9593d5cbc5644eb02593a2f562447efdcbb 71acbb54f49089b03d3498b6f88c1681d3f649ac \
        -- src/mesa/drivers/dri/intel src/mesa/drivers/dri/i965/
    git bisect run /path/to/tracecheck.py \
        --precision-threshold 8.0 \
        --build /path/to/build-script.sh \
        --gl-renderer '.*Mesa.*Intel.*' \
        --retrace=/path/to/glretrace \
        -c /path/to/reference/snapshots/ \
        topogun-1.06-orc-84k.trace

The trace-check.py script will skip automatically when there are build
failures.

The `--gl-renderer` option will also cause a commit to be skipped if the
`GL_RENDERER` is unexpected (e.g., when a software renderer or another GL
driver is unintentionally loaded due to missing symbol in the DRI driver, or
another runtime fault).


Side by side retracing
----------------------

In order to determine which draw call a regression first manifests one could
generate snapshots for every draw call, using the `-S` option.  That is, however,
very inefficient for big traces with many draw calls.

A faster approach is to run both the bad and a good GL driver side-by-side.
The latter can be either a previously known good build of the GL driver, or a
reference software renderer.

This can be achieved with retracediff.py script, which invokes glretrace with
different environments, allowing to choose the desired GL driver by
manipulating variables such as `LD_LIBRARY_PATH` or `LIBGL_DRIVERS_DIR`.

For example:

    ./scripts/retracediff.py \
        --ref-env LD_LIBRARY_PATH=/path/to/reference/GL/implementation \
        -r ./glretrace \
        --diff-prefix=/path/to/output/diffs \
        application.trace



Links
=====

About **apitrace**:

* [Official mailing list](http://lists.freedesktop.org/mailman/listinfo/apitrace)

* [Zack Rusin's blog introducing the GUI](http://zrusin.blogspot.com/2011/04/apitrace.html)

* [Jose's Fonseca blog introducing the tool](http://jrfonseca.blogspot.com/2008/07/tracing-d3d-applications.html)


Direct3D
--------

Open-source:

* [Proxy DLL](http://www.mikoweb.eu/index.php?node=21)

  * [Intercept Calls to DirectX with a Proxy DLL](http://www.codeguru.com/cpp/g-m/directx/directx8/article.php/c11453/)

* [Direct3D 9 API Interceptor](http://graphics.stanford.edu/~mdfisher/D3D9Interceptor.html)

Closed-source:

* [Microsoft PIX](http://msdn.microsoft.com/en-us/library/ee417062.aspx)

  * [D3DSpy](http://doc.51windows.net/Directx9_SDK/?url=/directx9_sdk/graphics/programmingguide/TutorialsAndSamplesAndToolsAndTips/Tools/D3DSpy.htm): the predecessor of PIX

* [AMD GPU PerfStudio](http://developer.amd.com/gpu/PerfStudio/pages/APITraceWindow.aspx)


OpenGL
------

Open-source:

* [BuGLe](http://www.opengl.org/sdk/tools/BuGLe/)

* [GLIntercept](http://code.google.com/p/glintercept/)

* [tracy](https://gitorious.org/tracy): OpenGL ES and OpenVG trace, retrace, and state inspection

Closed-source:

* [gDEBugger](http://www.gremedy.com/products.php)

* [glslDevil](http://cumbia.informatik.uni-stuttgart.de/glsldevil/index.html)

* [AMD GPU PerfStudio](http://developer.amd.com/gpu/PerfStudio/pages/APITraceWindow.aspx)

