Overview
=========

Although focus has and still is on graphical APIs, apitrace has an
infrastructure to trace generic APIs:

 * The APIs types and calls are specified in Python files in spec

   * there is a type hierarchy in specs\stdapi.py , capable of representing
     most types in C language, and additional semantic metadata

 * Python scripts generate C code to trace and serialize calls to disk, and
   vice-versa.

   * Visitor software design pattern is used to navigate over the types.

   * Template design pattern is use so that any step of code generation can be
     overriden by derived classes, allowing to easily handle cases that need
     special treatment without sacrifycing code reuse.

There are several main layers in apitrace.  Too many to show in a single graph,
so below only those relevant for GL are shown:

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

And here is a quick synopsis of what the layers do:

 * specs -- specification of the API, expressed in a Python class hierarchy

 * dispatch -- runtime dispatch of calls to DLLs (open the DLL, get the symbol
   address, and call it passing all arguments as-is)

 * helpers -- helper functions to determine sizes of array/blob/etc, and other.
   It often needs to dispatch calls to give the answers.

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

These are guidelines for new code.  Some of existing hasn't been updated to
these conventions yet.

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
  platform.  (Unless it is not widely used and there is agreement.)

* It's fine to add new features for only some platforms.

* Non-trivial changes should be staged in a branch, to enable peer-review and
  regression testing.  Branch should be deleted once code has been merged.

* Releases are tagged commits from master.  There are no stable branches.


Backwards compatibility:

* Backwards binary compatibility with old traces must be always maintained: all
  tools, including glretrace, must handle old traces without regressions.

* No backwards compatibility guarantees for derived data (ASCII dumps, state,
  images, etc).

* There should be no gratuitous change to command line tool interfaces, but no
  guarantees are given.



Regression testing
==================

There is a regression test suite under development in
https://github.com/apitrace/apitrace-tests .

