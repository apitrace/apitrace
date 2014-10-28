Overview
=========

Although focus was and still is on graphical APIs, apitrace has a
generic infrastructure to trace any kind of API:

 * the APIs types and calls are specified in Python files in specs
   sub-directory;

   * there is a type hierarchy in specs/stdapi.py, capable of representing
     most types in C language, and additional semantic metadata

 * Python scripts generate C++ code to trace and serialize calls parameters to
   a file, and vice-versa.

   * Visitor software design pattern is used to navigate over the types.

   * Template design pattern is use so that any step of code generation can be
     overriden by derived classes, allowing to easily handle cases that need
     special treatment without sacrifycing code reuse.

apitrace's architecture is composed of several layers.  Too many to show in a
single graph, so only those relevant for OpenGL API are shown below:

                        specs
                          ^
                          |
                       dispatch  <-------------- glws
                          ^                       ^
                          |                      /
                       helpers  <--- glstate    /
                         ^  ^          ^       /
                        /    \         |      /
                       /      \        |     /
                   trace      retrace  |    /
                    ^             ^    |   /
                   /               \   |  /
               gltrace            glretrace
               /  |  \
              /   |   \
             /    |    \
            /     |     \
           /      |      \
     glxtrace  wgltrace  cgltrace

Here is a quick synopsis of what the layers do:

 * specs -- specification of the API, expressed in a Python class hierarchy

 * dispatch -- runtime dispatch of calls to DLLs (open the DLL, get the symbol
   address, and call it passing all arguments as-is)

 * helpers -- helper functions to determine sizes of arrays, blobs, etc.  It
   often needs to dispatch calls to give the answers.

 * trace -- generate C++ code for tracing an API based on its spec

   * gltrace -- specialization of the tracing generation for GL API, with extra
     code to generate

   * glxtrace, wgltrace, cgltrace -- specialization of the tracing code for the
     GLX, WGL, and CGL APIs.

 * retrace -- generate C++ code to interpret calls from trace, based on the
   API's spec

   * glretrace -- specialization of the retrace code for the GL API

 * glstate -- code to dump OpenGL state to a JSON file

 * glws -- abstraction of the window system specific APIs (GXL, WGL, CGL), to
   enable cross-platform portability of glretrace


Coding Style
============

These are guidelines for new code.  Admittedly some of the existing code hasn't
been updated to follow these conventions yet.

Whitespace (all languages):

 * indentation is 4 spaces

 * never use tabs as indents

 * otherwise tab equals to 8 spaces

 * separate classes with two empty lines

Naming convention:

 * camelCase for functions/methods

 * UpperCase for structures/classes

 * lowercase for namespaces/modules

 * `UPPER_CASE` for #defines

 * single underscore prefix for variables/functions in automatically generated
   code

C++:

 * enclose single statement `if` clauses in { }, specially for automatically
   generated code

 * } else {

 * use inlines for functions/methods which are called with high-frequency

CMake:

 * `lower_case` commands

 * space between ( and precedent name


When in doubt, be consistent with the existing code.


Commit policy
=============

Feature development:

* Existing features in master branch should not degrade at any time, for any
  platform.  (Unless they are seldom used or redundant and there is agreement.)

  * In particular, new features / changes must not introduce any sort of
    instability when tracing.

    While application developers and driver developers may be able to
    workaround quirks in apitrace, we want to be able to obtain traces from
    non-technical end-users with minimal intervention.

    This implies that tracing should not make any non-standard assumptions, and
    care must be taken to ensure the tracing code is robust against invalid
    parameters, multiple threads, etc.

* It's fine to add new features for only some platforms or APIs.

* Non-trivial changes should be staged in a branch, to allow review and
  regression testing.  Feature branches should be deleted once they have been
  merged.

* Releases are tagged commits from master.  There are no stable branches.


Backwards compatibility:

* Backwards binary compatibility with old traces must be always maintained: all
  tools, including glretrace, must handle old traces without regressions.

* No backwards compatibility guarantees for derived data (ASCII dumps, state,
  images, etc).

* There should be no gratuitous changes to command line tool interfaces, but no
  guarantees are given.



Regression testing
==================

There is a regression test suite under development in
https://github.com/apitrace/apitrace-tests .

