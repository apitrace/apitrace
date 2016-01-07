This file lists the major user visible improvements.  For a full list of changes
and their authors see the git history.


# Version 7 #

* Native Android retrace with remote GUI

* OpenGL ES 3 support

* GUI ported to Qt5

* Thumbnails per draw call in GUI

* Rudimentary D3D7 retrace support


# Version 6 #

* OpenGL 4.5 support

* DXGI 1.3 support

* Support exporting buffers to disk from GUI


# Version 5 #

* OpenGL 4.4 support

* Stack backtraces on Linux

* Better tracing of enums in attribute arrays

* MD5 snapshots

* Better surface viewer


# Version 4 #

* Support tracing in Android, both native and Java applications

* Show frame thumbnails in the GUI

* Basic D3D 10.1, 11, and 11.1 trace support

* Basic D3D retrace support

* Multi-threaded trace/retrace support

* Several OpenGL ES state dump fixes

* GPU profiling

* Thumbnails in GUI

* Trim improvements


# Version 3 #

* Top-level `apitrace` command

* Trace and replay support for EGL, GLES1, and GLES2 APIs on Linux

* Ability to trim traces

* Basic CPU profiling when retracing

* Basic D3D10 trace support


# Version 2 #

* Flush/sync trace file only when there is an uncaught signal/exception,
  yielding a 5x speed up while tracing

* Employ snappy compression library instead of zlib, yielding a 2x speed up
  while tracing

* Implement and advertise `GL_GREMEDY_string_marker` and
  `GL_GREMEDY_frame_terminator` extensions

* Mac OS X support

* Support up-to OpenGL 4.2 calls

* Better GUI performance with very large traces, by loading frames from disk on
  demand


# Version 1 #

* Qt GUI, capable of visualizing the calls, the state, and editing the state


# Version 0 #

* D3D8 and D3D9 trace support

* OpenGL replay support
