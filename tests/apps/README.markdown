These directories contain many drawing-api-specific test applications.

Within each directory's CMakeLists.txt file are you will see
definitions of tests such as the following:

    add_app_test (
        NAME "gl_default_sb"
        TARGET gl_tri
        ARGS -sb
        REF default_sb.ref.txt
    )

This block specifies a test with NAME "gl_default_sb" that involves
executing the TARGET program "gl_tri" with ARGS of "-sb". The REF
script contains a reference trance that should result from tracing the
given program, along with specifications for additional checks to be
peformed.

The actual execution of the test (and parsing of the REF script) is
performed by the python program in ../app_driver.py. This driver
program runs the application with the given arguments, performs a
trace of the application, checks the trace against the reference
script, and then run a "replay" of the application (replaying the
commands in the trace).

In addition to a dump of the expected trace content, the reference
file can contain directives to cause the driver program to perform
additional checking. Some of the available directives are:

  #image: Dump the current framebuffer image and compare against the
          given file.

  #state: Dump the current state in JSON format and compare against
          the given file.
