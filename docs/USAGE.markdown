# Basic usage #

Run the application you want to trace as

    apitrace trace --api [gl|egl|d3d7|d3d8|d3d9|dxgi] /path/to/application [args...]

and it will generate a trace named `application.trace` in the current
directory.  You can specify the written trace filename by passing the
`--output` command line option. The default API is gl if none is specified.

Problems while tracing (e.g, if the application uses calls/parameters
unsupported by apitrace) will be reported via stderr output on Unices.  On
Windows you'll need to run
[DebugView](http://technet.microsoft.com/en-us/sysinternals/bb896647) to view
these messages.

If you cannot obtain a trace, check the [application specific instructions on
the wiki](https://github.com/apitrace/apitrace/wiki/Applications), or the
[manual tracing instructions below](#tracing-manually).

View the trace with

    apitrace dump application.trace

Replay an OpenGL trace with

    apitrace replay application.trace

Pass the `--sb` option to use a single buffered visual.  Pass `--help` to
`apitrace replay` for more options.

If you run into problems [check if it is a known issue and file an issue if
not](BUGS.markdown).


# Basic GUI usage #

Start the GUI as

    qapitrace application.trace

You can also tell the GUI to go directly to a specific call

    qapitrace application.trace 12345

Press `Ctrl-T` to see per-frame thumbnails.  And while inspecting frame calls,
press again `Ctrl-T` to see per-draw call thumbnails.


# Backtrace Capturing #

apitrace now has the ability to capture the call stack to an OpenGL call on
certain OSes (only Android and Linux at the moment).  This can be helpful in
determing which piece of code made that glDrawArrays call.

To use the feature you need to set an environment variable with the list of GL
call prefixes you wish to capture stack traces to.

    export APITRACE_BACKTRACE="glDraw* glUniform*"

The backtrace data will show up in qapitrace in the bottom section as a new tab.


# Advanced command line usage #


## Call sets ##

Several tools take `CALLSET` arguments, e.g:

    apitrace dump --calls=CALLSET foo.trace
    apitrace dump-images --calls=CALLSET foo.trace
    apitrace trim --calls=CALLSET1 --calls=CALLSET2 foo.trace

The call syntax is very flexible. Here are a few examples:

 * `4`             one call

 * `0,2,4,5`       set of calls

 * `"0 2 4 5"`     set of calls (commas are optional and can be replaced with whitespace)

 * `0-100/2`       calls 1, 3, 5, ...,  99

 * `0-1000/draw`   all draw calls between 0 and 1000

 * `0-1000/fbo`    all fbo changes between calls 0 and 1000

 * `frame`         all calls at end of frames

 * `@foo.txt`      read call numbers from `foo.txt`, using the same syntax as above



## Tracing manually ##

### Linux ###

On 64 bits systems, you'll need to determine whether the application is 64 bits
or 32 bits.  This can be done by doing

    file /path/to/application

But beware of wrapper shell scripts -- what matters is the architecture of the
main process.

Run the GLX application you want to trace as

    LD_PRELOAD=/path/to/apitrace/wrappers/glxtrace.so /path/to/application

and it will generate a trace named `application.trace` in the current
directory.  You can specify the written trace filename by setting the
`TRACE_FILE` environment variable before running.

For EGL applications you will need to use `egltrace.so` instead of
`glxtrace.so`.

The `LD_PRELOAD` mechanism should work with the majority of applications.  There
are some applications (e.g., Unigine Heaven, Android GPU emulator, etc.), that
have global function pointers with the same name as OpenGL entrypoints, living in a
shared object that wasn't linked with `-Bsymbolic` flag, so relocations to
those global function pointers get overwritten with the address to our wrapper
library, and the application will segfault when trying to write to them.  For
these applications it is possible to trace by using `glxtrace.so` as an
ordinary `libGL.so` and injecting it via `LD_LIBRARY_PATH`:

    ln -s glxtrace.so wrappers/libGL.so
    ln -s glxtrace.so wrappers/libGL.so.1
    ln -s glxtrace.so wrappers/libGL.so.1.2
    export LD_LIBRARY_PATH=/path/to/apitrace/wrappers:$LD_LIBRARY_PATH
    export TRACE_LIBGL=/path/to/real/libGL.so.1
    /path/to/application

If you are an application developer, you can avoid this either by linking with
`-Bsymbolic` flag, or by using some unique prefix for your function pointers.

See the `ld.so` man page for more information about `LD_PRELOAD` and
`LD_LIBRARY_PATH` environment flags.

### Android ###

To trace standalone native OpenGL ES applications, use
`LD_PRELOAD=/path/to/egltrace.so /path/to/application` as described in the
previous section.  To trace Java applications, refer to Android.markdown.

### Mac OS X ###

Run the application you want to trace as

    DYLD_FRAMEWORK_PATH=/path/to/apitrace/wrappers /path/to/application

Note that although Mac OS X has an `LD_PRELOAD` equivalent,
`DYLD_INSERT_LIBRARIES`, it is mostly useless because it only works with
`DYLD_FORCE_FLAT_NAMESPACE=1` which breaks most applications.  See the `dyld` man
page for more details about these environment flags.

### Windows ###

When tracing third-party applications, you can identify the target
application's main executable, either by:

* right clicking on the application's icon in the _Start Menu_, choose
  _Properties_, and see the _Target_ field;

* or by starting the application, run Windows Task Manager (taskmgr.exe), right
  click on the application name in the _Applications_ tab, choose _Go To Process_,
  note the highlighted _Image Name_, and search it on `C:\Program Files` or
  `C:\Program Files (x86)`.

On 64 bits Windows, you'll need to determine ether the application is a 64 bits
or 32 bits. 32 bits applications will have a `*32` suffix in the _Image Name_
column of the _Processes_ tab of _Windows Task Manager_ window.

You also need to know which graphics API is being used.  If you are unsure, the
simplest way to determine what API an application uses is to:

* download and run [Process Explorer](http://technet.microsoft.com/en-us/sysinternals/bb896653.aspx)

* search and select the application's process in _Process Explorer_

* list the DLLs by pressing `Ctrl + D`

* sort DLLs alphabetically, and look for the DLLs such as `opengl32.dll`,
  `d3d9.dll`, etc.

Copy the appropriate `opengl32.dll`, `d3d8.dll`, or `d3d9.dll` from the
wrappers directory to the directory with the application you want to trace.
Then run the application as usual.

You can specify the written trace filename by setting the `TRACE_FILE`
environment variable before running.

For D3D10 and higher you really must use `apitrace trace -a DXGI ...`. This is
because D3D10-11 API span many DLLs which depend on each other, and once a DLL
with a given name is loaded Windows will reuse it for LoadLibrary calls of the
same name, causing internal calls to be traced erroneously. `apitrace trace`
solves this issue by injecting a DLL `dxgitrace.dll` and patching all modules
to hook only the APIs of interest.


## Emitting annotations to the trace ##

### OpenGL annotations ###

From within OpenGL applications you can embed annotations in the trace file
through the following extensions:

* [`GL_KHR_debug`](http://www.opengl.org/registry/specs/KHR/debug.txt)

* [`GL_ARB_debug_output`](http://www.opengl.org/registry/specs/ARB/debug_output.txt)

* [`GL_EXT_debug_marker`](http://www.khronos.org/registry/gles/extensions/EXT/EXT_debug_marker.txt)

* [`GL_EXT_debug_label`](http://www.opengl.org/registry/specs/EXT/EXT_debug_label.txt)

* [`GL_AMD_debug_output`](http://www.opengl.org/registry/specs/AMD/debug_output.txt)

* [`GL_GREMEDY_string_marker`](http://www.opengl.org/registry/specs/GREMEDY/string_marker.txt)

* [`GL_GREMEDY_frame_terminator`](http://www.opengl.org/registry/specs/GREMEDY/frame_terminator.txt)

**apitrace** will advertise and intercept these OpenGL extensions regardless
of whether the OpenGL implementation supports them or not.  So all you have
to do is to use these extensions when available, and you can be sure they
will be available when tracing inside **apitrace**.

For example, if you use [GLEW](http://glew.sourceforge.net/) to dynamically
detect and use OpenGL extensions, you could easily accomplish this by doing:

    void foo() {
    
      if (GLEW_KHR_debug) {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, __FUNCTION__);
      }
      
      ...
      
      if (GLEW_KHR_debug) {
        glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_OTHER,
                             0, GL_DEBUG_SEVERITY_MEDIUM, -1, "bla bla");
      }
      
      ...
      
      if (GLEW_KHR_debug) {
        glPopDebugGroup();
      }
    
    }

This has the added advantage of working equally well with other OpenGL debugging tools.

Also, provided that the OpenGL implementation supports `GL_KHR_debug`, labels
defined via glObjectLabel() , and the labels of several objects (textures,
framebuffers, samplers, etc. ) will appear in the GUI state dumps, in the
parameters tab.


For OpenGL ES applications you can embed annotations in the trace file through the
[`GL_KHR_debug`](http://www.khronos.org/registry/gles/extensions/KHR/debug.txt) or 
[`GL_EXT_debug_marker`](http://www.khronos.org/registry/gles/extensions/EXT/EXT_debug_marker.txt)
extensions.


### Direct3D annotations ###

For Direct3D applications you can follow the standard procedure for
[adding user defined events to Visual Studio Graphics Debugger / PIX](http://msdn.microsoft.com/en-us/library/vstudio/hh873200.aspx):

- `D3DPERF_BeginEvent`, `D3DPERF_EndEvent`, and `D3DPERF_SetMarker` for D3D9,
  D3D10, and D3D11.0 applications.

- `ID3DUserDefinedAnnotation::BeginEvent`,
  `ID3DUserDefinedAnnotation::EndEvent`, and
  `ID3DUserDefinedAnnotation::SetMarker` for D3D11.1 applications.

And for [naming objects](http://blogs.msdn.com/b/chuckw/archive/2010/04/15/object-naming.aspx)
which support `SetPrivateData` method:

    pObject->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(szName), szName);

Note that [programmatic capture interfaces](https://msdn.microsoft.com/en-us/library/hh780905.aspx)
are currently _not_ supported.

See also:

* <http://seanmiddleditch.com/direct3d-11-debug-api-tricks/>

* <http://blogs.msdn.com/b/chuckw/archive/2012/11/30/direct3d-sdk-debug-layer-tricks.aspx>


## Mask OpenGL features ##

It's now possible to mask some of OpenGL features while tracing via a configuration file:

 * `$XDG_CONFIG_HOME/apitrace/gltrace.conf` or `$HOME/.config/apitrace/gltrace.conf` on Linux

 * `$HOME/Library/Preferences/apitrace/gltrace.conf` on MacOS X

 * `%LOCALAPPDATA%\apitrace\gltrace.conf` on Windows

Here's an example `gltrace.conf` config file showing some variables:

    # comment line
    GL_VERSION = "2.0"
    GL_VENDOR = "Acme, Inc."
    GL_EXTENSIONS = "GL_EXT_texture_swizzle GL_ARB_multitexture"
    GL_RENDERER = "Acme rasterizer"
    GL_SHADING_LANGUAGE_VERSION = "1.30"
    GL_MAX_TEXTURE_SIZE = 1024

This basically overrides the respective `glGetString()` and `glGetIntegerv()`
parameters.

String values are contained inside `""` pairs and may span multiple lines.
Integer values are given without quotes.

## Identify OpenGL object leaks ##

You can identify OpenGL object leaks by running:

    apitrace leaks application.trace

This will print leaked object list and its generated call numbers.

apitrace provides very basic leak tracking: it tracks all textures/
framebuffers/renderbuffers/buffers name generate and delete call.  If a object is not
deleted until context destruction, it's treated as 'leaked'.  This logic doesn't
consider multi-context in multi-thread situation, so may report incorrect
results in such scenarios.

To use this fomr the GUI, go to  menu -> Trace -> LeakTrace

## Dump OpenGL state at a particular call ##

You can get a dump of the bound OpenGL state at call 12345 by doing:

    apitrace replay -D 12345 application.trace > 12345.json

This is precisely the mechanism the GUI uses to obtain its own state.

You can compare two state dumps by doing:

    apitrace diff-state 12345.json 67890.json


## Comparing two traces side by side ##

    apitrace diff trace1.trace trace2.trace

This works only on Unices, and it will truncate the traces due to performance
limitations.


## Recording a video with FFmpeg/Libav ##

You can make a video of the output with FFmpeg by doing

    apitrace dump-images -o - application.trace \
    | ffmpeg -r 30 -f image2pipe -vcodec ppm -i pipe: -vcodec mpeg4 -y output.mp4

or Libav (which replaces FFmpeg on recent Debian/Ubuntu distros) doing

    apitrace dump-images -o - application.trace \
    | avconv -r 30 -f image2pipe -vcodec ppm -i - -vcodec mpeg4 -y output.mp4


## Recording a video with gstreamer ##

You can make a video of the output with gstreamer by doing

    glretrace --snapshot-format=RGB -s - smokinguns.trace | gst-launch-0.10 fdsrc blocksize=409600 ! queue \
    ! videoparse format=rgb width=1920 height=1080 ! queue ! ffmpegcolorspace ! queue \
    ! vaapiupload direct-rendering=0 ! queue ! vaapiencodeh264 ! filesink location=xxx.264


## Trimming a trace ##

You can truncate a trace by doing:

    apitrace trim --calls 0-12345 -o trimed.trace application.trace

If you need precise control over which calls to trim you can specify the
individual call numbers in a plain text file, as described in the 'Call sets'
section above.

There is also experimental support for automatically trimming the calls
necessary for a given frame or call:

    apitrace trim-auto --calls=12345 -o trimed.trace application.trace
    apitrace trim-auto --frames=12345 -o trimed.trace application.trace


## Profiling a trace ##

You can perform gpu and cpu profiling with the command line options:

 * `--pgpu` record gpu times for frames and draw calls.

 * `--pcpu` record cpu times for frames and draw calls.

 * `--ppd` record pixels drawn for each draw call.

The results from these can then be read by hand or analyzed with a script.

`scripts/profileshader.py` will read the profile results and format them into a
table which displays profiling results per shader.

For example, to record all profiling data and utilise the per shader script:

    apitrace replay --pgpu --pcpu --ppd foo.trace | ./scripts/profileshader.py


# Advanced usage for OpenGL implementers #

There are several advanced usage examples meant for OpenGL implementors.


## Regression testing ##

These are the steps to create a regression test-suite around **apitrace**:

* obtain a trace

* obtain reference snapshots, by doing on a reference system:

        mkdir /path/to/reference/snapshots/
        apitrace dump-images -o /path/to/reference/snapshots/ application.trace

* prune the snapshots which are not interesting

* to do a regression test, use `apitrace diff-images`:

        apitrace dump-images -o /path/to/test/snapshots/ application.trace
        apitrace diff-images --output summary.html /path/to/reference/snapshots/ /path/to/test/snapshots/


## Automated git-bisection ##

With tracecheck.py it is possible to automate git bisect and pinpoint the
commit responsible for a regression.

Below is an example of using tracecheck.py to bisect a regression in the
Mesa-based Intel 965 driver.  But the procedure could be applied to any OpenGL
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
dependency discovery in Mesa's makefile system, it was necessary to invoke `make
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
`GL_RENDERER` is unexpected (e.g., when a software renderer or another OpenGL
driver is unintentionally loaded due to a missing symbol in the DRI driver, or
another runtime fault).


## Side by side retracing ##

In order to determine which draw call a regression first manifests one could
generate snapshots for every draw call, using the `-S` option.  That is, however,
very inefficient for big traces with many draw calls.

A faster approach is to run both the bad and a good OpenGL driver side-by-side.
The latter can be either a previously known good build of the OpenGL driver, or a
reference software renderer.

This can be achieved with retracediff.py script, which invokes glretrace with
different environments, allowing to choose the desired OpenGL driver by
manipulating variables such as `LD_LIBRARY_PATH`, `LIBGL_DRIVERS_DIR`, or
`TRACE_LIBGL`.

For example, on Linux:

    ./scripts/retracediff.py \
        --ref-env LD_LIBRARY_PATH=/path/to/reference/OpenGL/implementation \
        --retrace /path/to/glretrace \
        --diff-prefix=/path/to/output/diffs \
        application.trace

Or on Windows:

    python scripts\retracediff.py --retrace \path\to\glretrace.exe --ref-env TRACE_LIBGL=\path\to\reference\opengl32.dll application.trace


# Advanced GUI usage #

qapitrace has rudimentary support for replaying traces on a remote
target device. This can be useful, for example, when developing for an
embedded system. The primary GUI will run on the local host, while any
replays will be performed on the target device.

In order to target a remote device, use the command-line:

    qapitrace --remote-target <HOST> <trace-file>

In order for this to work, the following must be available in the
system configuration:

1. It must be possible for the current user to initiate an ssh session
   that has access to the target's window system. The command to be
   exectuted by qapitrace will be:

        ssh <HOST> glretrace

   For example, if the target device is using the X window system, one
   can test whether an ssh session has access to the target X server
   with:

        ssh <HOST> xdpyinfo

   If this command fails with something like "cannot open display"
   then the user will have to configure the target to set the DISPLAY
   environment variable, (for example, setting DISPLAY=:0 in the
   .bashrc file on the target or similar).

   Also, note that if the ssh session requires a custom username, then
   this must be configured on the host side so that ssh can be
   initiated without a username.

   For example, if you normally connect with `ssh user@192.168.0.2`
   you could configure ~/.ssh/config on the host with a block such as:

        Host target
          HostName 192.168.0.2
          User user

   And after this you should be able to connect with `ssh target` so
   that you can also use `qapitrace --remote-target target`.

2. The target host must have a functional glretrace binary available

3. The target host must have access to `trace-file` at the same path
   in the filesystem as the `trace-file` path on the host system being
   passed to the qapitrace command line.
