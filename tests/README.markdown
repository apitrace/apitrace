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

Some tests will crash on purpose.  On Mac OS X you can [avoid the application
quit unexpectedly](http://apple.stackexchange.com/a/105894) doing

    defaults write com.apple.CrashReporter DialogType none
