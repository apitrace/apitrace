Reporting bugs
==============

Please report any issues on
[github](https://github.com/apitrace/apitrace/issues).

Always include the following information:

* operating system name and version

* OpenGL/D3D driver name and version


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

For big attachments, such as traces, please upload it temporarily to a web
server you control, or use a file upload service such as
http://www.megaupload.com/ or http://dropbox.com/ and paste the URL into the
issue description.

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
`-DCMAKE_BUILD_TYPE=Debug` to cmake.

To obtain a stack back-trace, run the application with gdb from a terminal:

    $ gdb --args application arg1 arg2 ...
    (gdb) run
    ...
    (gdb) bt


Windows
-------

WRITEME


