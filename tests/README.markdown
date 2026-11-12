# About #

This is an automated test suite for apitrace.

# Requirements #

Same as apitrace requirements.

# Usage #

To run the test suite do on Unices:

    cmake -DAPITRACE_SOURCE_DIR=/path/to/apitrace/tree -DAPITRACE_EXECUTABLE=/path/to/apitrace/build/apitrace -H. -Bbuild
    export CTEST_OUTPUT_ON_FAILURE=1
    make -C build all test

You can run multiple tests in parallel by specifying `CTEST_PARALLEL_LEVEL` environment variable.

Or on Windows:

    cmake -G "Visual Studio XX YYYY" -H. -Bbuild
    set CTEST_OUTPUT_ON_FAILURE=1
    cmake --build build --target ALL_BUILD
    cmake --build build --target RUN_TESTS

A detailed log will be written to `Testing/Temporary/LastTest.log`.


# Tips #

Framebuffer PNG references can select an attachment with a suffix, for example
`example.ref.draw.GL_COLOR_ATTACHMENT1.png`. The `draw` label selects the call
in `example.ref.txt`, and the suffix selects the image in the framebuffer dump.
References without an attachment suffix keep selecting the default color image.

Texture readback references use `example.ref.dump.textures.json`, where `dump`
is a call label in the `.ref.txt` file. Each entry selects a texture key and
checks width, height, depth, image type, and decoded pixels. `absent: true`
asserts that a failed image was omitted; an optional `warning` checks stderr.
Float images are compared as native float32 PNM/PX data, without PNG conversion.
Rows follow the existing vertically flipped, stacked image representation.

`apps/egl/gles2/texture_readback_refs.py` regenerates these references from CPU
fixtures, independently of retrace output. Run the `es20`, `es30`, and `es31`
groups with matching actual GLES versions to cover both probe and query paths.
Debug EGL builds also register `texture_readback_fault_*`; these require GDB
with Python support and inject errors inside the real helper, including a
second-slice failure and a missing depth-readback capability.

Some tests will crash on purpose.  On Mac OS X you can [avoid the application
quit unexpectedly](http://apple.stackexchange.com/a/105894) doing

    defaults write com.apple.CrashReporter DialogType none
