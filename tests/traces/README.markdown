This directory tests various operations of the apitrace command-line
interface (cli) operating on existings.

All files with a .script extension specify a simple script for
testing. The test driver (in ../tool_driver.py) will parse the script
file line-by-line, interpreting each line as arguments to the apitrace
cli interface and invoking the cli with the given arguments. This
continues until the first line of the script beginning with the "dump"
command.

After a "dump" command in the script, the remainder of the script file
provides the expected output of the given dump command. The tool
driver will report a test failure if the actual dump output differs
from that given in the.

Here are descriptions of some of the trace files contained here which
are used by the test scripts:

*   zlib-no-eof.trace:  is a short, zlib compressed trace, with an unexpected end of
    file because the application terminated abnormally (which is actual very
    normal).

    Depending on how zlib handles the unexpected end of file, it may fail to read
    any data from it.

*   incomplete-call.trace:  trace with an incomplete call, with missing arguments

*   glxsimple.trace: trace from a simple program showing drawing with
    glClear, with GLSL shader, and with texture. See ../cli/src