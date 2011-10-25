Things To Do
============

(in no particular order)


Tracing
-------

* Allow clamping to a GL version or a number of extensions.

* Trace multiple threads:

  * `GetCurrentThreadId()`

  * `pthread_self()`

* Put zlib (de)compression in a separate thread.

* Trace TSCs

* Trace window sizes somehow

* Allow to distinguish between the calls really done by the program, vs the
  fakes one necessary to retrace correctly.

* Start tracing on demand (e.g., key-press, or by frame no), emitting calls
  that recreate all current state.

* Add option to include call stack frames in the trace.

* Trace the internal GL calls done by GLU on Mac OS X & Windows.


Retracing
---------

* Use visuals that best match those used in the trace; specially auto
  detect single/double buffer visuals.

* Respect multiple context sharing of the traces.

* Support multiple threads

* Leverage `ARB_debug_output` where available.

* D3D support.


GUI
---

* Timeline view.

* Visualize meshes in draw commands.

* Breakpointing and step-by-step debugging.


Other
-----

* Side-by-side trace diffing; either as a separate tool on or the GUI.

* Ability to extract just a single frame from a trace, and all previous calls
  that contributed to it:

  * via a state tracker (i.e., knowledge of how calls affect the state);

  * or by leveragine retrace, dumping the calls to emit all state at beginning
    of the frame.


See also:

* open issues on [github](https://github.com/apitrace/apitrace/issues)

* _Known issues_ section in BUGS.markdown

* FIXME, TODO, and XXX comments on the source code.
