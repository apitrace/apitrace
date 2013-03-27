The dispatch layer objecting is to get public or private symbols from DLLs /
shared objects and dispatch calls to them.

It used both by the tracing wrappers (to dispatch the intercepted calls to
their true counterparts) and when replaying traces (to dispatch the calls
recorded on the file)

All code is generated from dispatch.py Python script, which is then used in
several places.
