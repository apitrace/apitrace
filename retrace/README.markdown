The source for replaying retraces lives in this directory.

There are actually several discting layers in this directory which should be eventually be split out:

 - `*`retrace -- deserialization and interpretation of calls from a trace

 - `*`ws -- windowing system helpers and abstractions

 - `*`state -- dumping of state into JSON format
