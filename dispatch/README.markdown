The dispatch layer objective is to resolve the addresses of public and private
symbols from DLLs / shared objects and dispatch calls to them.

It used both by the tracing wrappers (to dispatch the intercepted calls to
their true counterparts) and when replaying traces (to dispatch the calls
recorded on the file).

Most of the code is generated from dispatch.py script, which is then derived
for particular APIs.
