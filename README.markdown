About **apitrace**
==================

**apitrace** consists of a set of tools to:

* trace OpenGL, D3D9, D3D8, D3D7, and DDRAW APIs calls to a file;

* retrace OpenGL calls from a file;

* visualize trace files, and inspect state.


Building from source
====================


Requirements common for all platforms:

* Python (requires version 2.6)

* CMake (tested with version 2.8)

Requirements to build the GUI (optional):

* Qt (tested with version 4.7)

* QJSON (tested with version 0.7.1)


Linux / Mac OS X
----------------

Build as:

    cmake -H. -Bbuild
    make -C build

You can also build the 32bit GL wrapper on 64bit distro with a multilib gcc by
doing:

    cmake -H. -Bbuild32 -DCMAKE_C_FLAGS=-m32 -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_EXE_LINKER_FLAGS=-m32
    make -C build32 glxtrace


Windows
-------

Additional requirements:

* Microsoft Visual Studio (tested with 2008 version) or MinGW (tested with gcc version 4.4)

* Microsoft DirectX SDK (tested with August 2007 release)

To build with Visual Studio first invoke CMake GUI as:

    cmake-gui -H. -B%cd%\build

and press the _Configure_ button.

It will try to detect most required/optional dependencies automatically.  When
not found automatically, you can manually specify the location of the
dependencies from the GUI.

If you are building with GUI support (i.e, with QT and QJSON), it should detect
the official QT sdk automatically, but you will need to build QJSON yourself
and also set the `QJSON_INCLUDE_DIR` and `QJSON_LIBRARIES` variables in the
generated `CMakeCache.txt` when building apitrace and repeat the above
sequence.

After you've succesfully configured, you can start the build by opening the
generated `build\apitrace.sln` solution file, or invoking `cmake` as:

    cmake --build build --config MinSizeRel

The steps to build 64bit version are similar, but choosing _Visual Studio 9
2008 Win64_ instead of _Visual Studio 9 2008_.

It's also possible to instruct `cmake` build Windows binaries on Linux with
[MinGW cross compilers](http://www.cmake.org/Wiki/CmakeMingw).


Usage
=====


Linux
-----

Run the application you want to trace as

     LD_PRELOAD=/path/to/glxtrace.so /path/to/application

and it will generate a trace named `application.trace` in the current
directory.  You can specify the written trace filename by setting the
`TRACE_FILE` environment variable before running.

View the trace with

    /path/to/tracedump application.trace | less -R

Replay the trace with

    /path/to/glretrace application.trace

Pass the `-sb` option to use a single buffered visual.  Pass `--help` to
glretrace for more options.

Start the GUI as

    /path/to/qapitrace application.trace


The `LD_PRELOAD` mechanism should work with most applications.  There are some
applications, e.g., Unigine Heaven, which global function pointers with the
same name as GL entrypoints, living in a shared object that wasn't linked with
`-Bsymbolic` flag, so relocations to those globals function pointers get
overwritten with the address to our wrapper library, and the application will
segfault when trying to write to them.  For these applications it is possible
to trace by using `glxtrace.so` as an ordinary `libGL.so` and injecting into
`LD_LIBRARY_PATH`:

    ln -s glxtrace.so libGL.so
    ln -s glxtrace.so libGL.so.1
    ln -s glxtrace.so libGL.so.1.2
    export LD_LIBRARY_PATH=/path/to/directory/where/glxtrace/is:$LD_LIBRARY_PATH
    export TRACE_LIBGL=/path/to/real/libGL.so.1
    /path/to/application

See the 'ld.so' man page for more information about `LD_PRELOAD` and
`LD_LIBRARY_PATH` environment flags.


Mac OS X
--------

Usage on Mac OS X is similar to Linux above, except for the tracing procedure,
which is instead:

    DYLD_LIBRARY_PATH=/path/to/apitrace/wrappers /path/to/application

Note that although Mac OS X has an `LD_PRELOAD` equivalent,
`DYLD_INSERT_LIBRARIES`, it is mostly useless because it only works with
`DYLD_FORCE_FLAT_NAMESPACE=1` which breaks most applications.  See the `dyld` man
page for more details about these environment flags.


Windows
-------

* Copy `opengl32.dll`, `d3d8.dll`, or `d3d9.dll` from build/wrappers directory
  to the directory with the application you want to trace.

* Run the application.

* View the trace with

        /path/to/tracedump application.trace

* Replay the trace with

        /path/to/glretrace application.trace


Links
=====

About **apitrace**:

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

