Reporting bugs
==============

Before reporting, please skim through the known issues below.

Report any issues on [github](https://github.com/apitrace/apitrace/issues),
always including the following information:

* operating system name and version

* OpenGL/D3D driver name and version


Known issues
============

These are issues that the developers are already aware of, but for which there
is no immediate plan to address them, because either:

* they stem from architectural limitations that are difficult to overcome
  and/or time-consuming;

* they are corners cases that are thought to be of very little practical use;

* merely lack of time/opportunity.

That said, feel free to file an issue and/or send an email to the mailing list
if:

* you want discuss the rationale, propose your ideas on how to address it, or
  volunteer to work on it;

* a known issue is important for you and you would like to see it addressed
  sooner rather than later.


Tracing
-------

* Fake calls may be emitted in the trace, in order to provide complete
  information for retracing.  The typical case is OpenGL vertex arrays in user
  memory (as opposed to VBOs): where glXxxPointer calls will be deferred,
  glInterleavedArrays will be decomposed, etc.

  This should not affect the rendered output, but it may use different paths in
  the OpenGL driver, exercising different paths and with different performance
  characteristics.

  There is no way to distinguish the fake calls from those actually
  made by the application yet.

* Huge traces will be generated for applications that make extensive use of
  immediate mode drawing (ie., glBegin/glEnd), use vertex/element arrays in
  user memory, or generate a lot of dynamic vertex data per frame.  This is
  because large quantities of (often redundant) data will be recorded over and
  over; and although the traces are compressed, the compression algorithms used
  aim a good performance/compression balance, hence fail to identify and remove
  the redundancy at this scale.

  However, this should not be a problem for modern OpenGL applications that
  make an efficient use of VBOs and vertex shaders.

* On MacOSX, the internal OpenGL calls done by GLU are not traced yet.


Retracing
---------

* Replaying can take substantially more CPU due to the overhead of reading the
  trace file from disk.

  However, the overhead should not be significant for modern 3D applications
  that do efficient use of VBOs and vertex shaders.  The communication between
  CPU to GPU can easily be a bottleneck on the powerful discrete GPUs of
  nowadays, and trace overhead tend to be propotional so it is not a
  coincidence that applications that are optimized for modern GPUs will also be
  traced and replayed efficiently.

* glretrace needs to infer window sizes from glViewport calls, because calls
  that create/resize windows happen outside of OpenGL and are not traced.
  Therefore window will be missing if the application relies on the default
  viewport instead of explicitly invoking glViewport; or it will to too big if
  the window is shrunk.  Most apps call glViewport before rendering.

* OS specific OpenGL APIs (WGL, GLX, CGL, etc), are not retraced literally, but
  instead partially emulated.  This is by design, to allow traces to be
  retraced on any OS, as the focus is on the OS-independent parts of OpenGL API.

* There is no guarantee that the same visual that was used on tracing will be
  used for retracing OpenGL.  Actually currently, glretrace will always choose
  a standard 32bit RGBA, 24bit depth, 8bit alpha, double buffer visual.  Unless
  overridden on command line.

* OpenGL context sharing is not fully respected -- all contexts are assumed to
  share state.  This is by far the most common case though.


GUI
---

* Not all types of arguments can be edited.



Proprietary/confidential applications
=====================================

Issues should be preferably filed on github to facilitate collaborative
development and for future reference.

Access to applications source code is not required -- binaries are sufficient.

If the bug happens with a proprietary application, and you don't want to
publicly release the application and/or any data collected from it, then
alternatively you can provide the necessary application and/or data via e-mail,
to *jose dot r dot fonseca at gmail dot com*.

If it is not technically/legally feasible for you to provide application and/or
data at all, then you must be either:

* develop and provide a test application, stripped-down of all
  proprietary/confidential data, but that can reproduce the issue;

* be willing/able to do the investigation of the issue, namely to identify the
  root cause of the issue (e.g., which OpenGL call is not properly handled and
  why), using all necessary tools (such as debuggers).

Failure to do so will render the apitrace authors powerless to address the
issue.


Attachments
===========

github issue tracker doesn't support attachments.

Please attach long logs to https://gist.github.com/ and paste the URL into the
issue description.

For big attachments, such as traces, please upload them temporarily to a web
server you control, or use a file upload service such as http://dropbox.com/
and paste the URL into the issue description.

Trace files are only slightly compressed (for performance reasons).  You can
further reduce their size when attaching/uploading by compressing with
[XZ](http://tukaani.org/xz/) or [7-Zip](http://www.7-zip.org/).


Bugs on tracing
===============

For bugs that happen while tracing (e.g., crashes while tracing the
application, or incorrect traces) please:

* provide information on how to obtain the application;

* describe how you were using it when the issue happened.


Bugs on retracing/GUI
=====================

For bugs on retracing (e.g. crashes when retracing the application,
incorrect inconsistent rendering, or viewing with the GUI) please:

* provide the trace file;

* describe the results you got, and what results you were expecting.


Obtaining stack back-traces
===========================


Linux/MacOSX
------------

Please rebuild apitrace with debugging information, by passing
`-DCMAKE_BUILD_TYPE=Debug` to cmake, or editing its value in `CMakeCache.txt`
and rebuilding.

To obtain a stack back-trace, run the application with gdb from a terminal:

    $ gdb --args application arg1 arg2 ...
    (gdb) run
    ...
    (gdb) bt

On Linux, to trace an application inside gdb the `LD_PRELOAD` environment
variable should be set from within gdb like:

    gdb --ex 'set exec-wrapper env LD_PRELOAD=/path/to/glxtrace.so' --args application arg1 ...

See also more detailed and Distro specific instructions:

* http://wiki.debian.org/HowToGetABacktrace

* https://wiki.ubuntu.com/Backtrace

* http://fedoraproject.org/wiki/StackTraces

* http://live.gnome.org/GettingTraces


Windows
-------

See:

* https://developer.mozilla.org/en/how_to_get_a_stacktrace_with_windbg
