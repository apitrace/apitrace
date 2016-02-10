# Overview #

Although focus was and still is on graphical APIs, apitrace has a
generic infrastructure to trace any kind of API:

 * the API's types and calls are specified in Python files in specs
   sub-directory;

   * there is a type hierarchy in `specs/stdapi.py`, capable of representing
     most types in C language, and additional semantic metadata

 * Python scripts generate C++ code to trace and serialize calls parameters to
   a file, and vice-versa.

   * The _Visitor_ design pattern is used to navigate over the types.

   * The _Template_ design pattern is used to enable any step of code
     generation to be overriden by derived classes, allowing to handle
     cases that need special treatment without sacrifycing code reuse.

apitrace's architecture is composed of several layers/components.  There are
too many to show in a single graph, so only those relevant for OpenGL API are
illustrated below:

                                   specs
                                     ^
                                     |
                                   glproc
                                   ^ ^ ^
                                   | | |
                            /------/ | \--\
                            |        |    |
                         helpers     |    |
                          ^ ^ ^      |    |
                          | | |      |    |
                 /--------/ | \----\ |    |
                 |          |      | |    |
                 |          |      | |    |
               trace     retrace glstate glws
                 ^          ^       ^     ^
                 |          |       |     |
                 |          \-----\ | /---/
                 |                | | |
                 |                | | |
              gltrace           glretrace
               ^ ^ ^                ^
               | | |                |
        /------/ | \-----\          |
        |        |       |          |
        |        |       |          |
    glxtrace wgltrace cgltrace  qapitrace

Here is a quick synopsis of what the layers do:

 * `specs` -- specifies the types, functions, and interfaces of the API,
   expressed as a hierarchy of Python objects

 * `dispatch` -- runtime dispatch of calls to DLLs (open the DLL, get the symbol
   address, and call it passing all arguments as-is)

   * `glproc` -- specialization of the dispatch generation for GL APIs

 * `helpers` -- helper functions to determine sizes of arrays, blobs, etc.  It
   often needs to dispatch calls to give the answers.

 * `trace` -- generate C++ code for tracing an API based on its spec

   * `gltrace` -- specialization of the tracing generation for GL API, with extra
     code to generate

   * `glxtrace`,` wgltrace`, `cgltrace` -- specialization of the tracing code for the
     GLX, WGL, and CGL APIs.

 * `retrace` -- generate C++ code to interpret calls from trace, based on the
   API's spec

   * `glretrace` -- specialization of the retrace code for the GL API

 * `glstate` -- code to dump OpenGL state to a JSON file

 * `glws` -- abstraction of the window system specific APIs (GXL, WGL, CGL), to
   enable cross-platform portability of `glretrace`

 * `qapitrace` -- the GUI; it reads traces directly, and gets JSON state by
   invoking `glretrace`

The architecture for Direct3D APIs is similar, with the exception that, because
it's not cross platform, there are less specialized variations.


# Coding Style #

These are guidelines for new code.  Admittedly some of the existing code hasn't
been updated to follow these conventions yet.

Whitespace (all languages):

 * indentation is 4 spaces

 * never use tabs as indents, except on Makefiles

 * otherwise tab equals to 8 spaces

 * separate classes with two empty lines

Naming convention:

 * `camelCase` for functions/methods

 * `UpperCase` for structures/classes

 * `lowercase` for namespaces/modules

 * `UPPER_CASE` for #defines

 * single underscore prefix for variables/functions in automatically generated
   code

C++:

 * enclose single statement `if` clauses in `{` `}`, specially for
   automatically generated code

 * `} else {`

 * use `inline` keyword for functions/methods which are called with high-frequency

CMake:

 * `lower_case` commands

 * space between `(` and precedent name


And when in doubt, be consistent with the existing code.


# Commit policy #

Feature development:

* Existing features in master branch should not degrade at any time, for any
  platform.  Unless they are seldom used or redundant, and there is agreement.

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
  tools, including `glretrace`, must handle old traces without regressions.

* No backwards compatibility guarantees for derived data (ASCII dumps, state,
  images, etc).

* There should be no gratuitous changes to command line tool interfaces, but no
  guarantees are given.


# Regression testing #

There is a regression test suite under development in
https://github.com/apitrace/apitrace-tests .


# Further reading #

* [Writing ELF Shared Library Wrappers](https://github.com/amonakov/on-wrapping/blob/master/interposers-discussion.asciidoc)
* [Intercepting and Instrumenting COM Applications](https://www.usenix.org/legacy/events/coots99/full_papers/hunt/hunt_html)
* [Proxy-Dll: Intercept Calls to DirectX](http://www.mikoweb.eu/index.php?node=21)


# How to's #

## How to support a new OpenGL extension ##

All OpenGL (and OpenGL ES) function prototypes live in `specs/glapi.py`.  This
file is semi-automatically derived from Khronos XML description of OpenGL /
OpenGL ES.  To refresh do

    $ make -C specs/scripts/ glapi.py
    $ meld specs/glapi.py specs/scripts/glapi.py

and then port over new prototypes.  See also `specs/scripts/README.markdown`.

The upstream XML description is not rich enough to describe all semantic
details that apitrace needs, therefore one needs to manually tweak the
specifications:

* Fix-up the types of array, blob, and pointer arguments.

  * For `glGet*` you can use `"_gl_param_size(pname)"` for automatically determining the number of parameters written, e.g.

            GlFunction(Void, "glGetIntegerv", [(GLenum, "pname"), Out(Array(GLint, "_gl_param_size(pname)"), "params")], sideeffects=False),

* Add the `sideeffects=False` keyword argument where appropriate, so that those
  calls can be merely ignored by `glretrace`.

* Replace generically type `GLuint` object IDs with typed ones, (e.g., replace
  `(GLuint, "texture")` into `(GLtexture, "texture")`, so that `glretrace` can
  swizzle the objects IDs, when replaying on a different OpenGL implementation.


## Dump more OpenGL state ##

TBD.


## Dump more D3D10/D3D11 parameter state ##

In short, to dump another piece of state (e.g., `D3DXX_FOO_STATE`) you need to:

* declare a

        void dumpStateObject(JSONWriter &, const D3DXX_FOO_STATE &);

  in `retrace/dxgistate_so.hpp`

* add 

        d3dxx.D3DXX_FOO_STATE,

  to the bottom of `retrace/dxgistate_so.py` so that C++ code to dump that structure is generated.

* add a new `dumpFooState` function to `retrace/d3d10state.cpp` or `retrace/d3d11state.cpp`, similar to the existing `dumpBlendState` function, which gets the state object, and then calls `dumpStateObjectDesc` template and the generated `dumpStateObject` functions to do the grunt work.

* update `dumpParameters` to call `dumpFooState`

