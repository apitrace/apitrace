Things To Do
============

(in no particular order)


Tracing
-------

* Allow clamping to a GL version or a number of extensions.

* Put (de)compression in a separate thread.

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


CLI
---

* Add retrace     Replay all the calls in a trace
* Add trim        Trim a trace by including only the specified calls/frames
* Add dump-state  Output the OpenGL state in JSON format
* Add dump-images Create image files for each frame/drawing operation of a trace

* Add some common command-line options:

  Most commands acting on a trace accept the following common options:

   --calls=[RANGE]       Operate only on calls with index numbers within
			 the specified range.

   --frames=[RANGE]      Operate only on frames with index numbers within
			 the specified range.

   --functions=[REGEXP]  Operate only on function calls which have a
			 name that matches the given regular expression.

   A [RANGE] can be any of the following:

     Example    Description        
     -------    -----------        
     4          A single number specifying a single call or frame.

     10-20      Two numbers separated by '-' (FIRST-LAST) specifying a
		range of consecutive calls/frames from FIRST to LAST. If
		either of FIRST or LAST is omitted, the first or last
		call/frame index in the trace will be used.

     1-100/5    Either of the above range specifications followed by '/'
		and a number (INTERVAL). This specifies the inclusion of
		only each INTERVAL call/frame within the range. For
		example, 1-100/5 species frame 1, frame 5, frame 10,
		... up to frame 100.

     4,10-20/2  A comma separated list of any of the above range
		specifications, specifying the union of the ranges.

* Add some retrace-specific options (-b, -db, -sb, -v)

* Add some trime-specific options (--every={draw,framebuffer,frame})

* Accept a filename for --calls or --functions (to use the same calls
  or functions as present in a trace dump).

* Add an "apitrace bisect" for trimming down a trace based on user
  input at each stage.

* Add an "apitrace git-bisect-helper" for calling from git-bisect.


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
