Building from source
====================


Requirements common for all platforms:

* Python (requires version 2.6)

* CMake (tested with version 2.8)

Requirements to build the GUI (optional):

* Qt (tested with version 4.7)

* QJSON (tested with version 0.7.1)


Linux / Mac OS X
----------------

Build as:

    cmake -H. -Bbuild
    make -C build

You can also build the 32bit GL wrapper on 64bit distro with a multilib gcc by
doing:

    cmake -H. -Bbuild32 -DCMAKE_C_FLAGS=-m32 -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_EXE_LINKER_FLAGS=-m32
    make -C build32 glxtrace


Windows
-------

Additional requirements:

* Microsoft Visual Studio (tested with 2008 version) or MinGW (tested with gcc version 4.4)

* Microsoft DirectX SDK (tested with August 2007 release)

To build with Visual Studio first invoke CMake GUI as:

    cmake-gui -H. -B%cd%\build

and press the _Configure_ button.

It will try to detect most required/optional dependencies automatically.  When
not found automatically, you can manually specify the location of the
dependencies from the GUI.

If you are building with GUI support (i.e, with QT and QJSON), it should detect
the official QT sdk automatically, but you will need to build QJSON yourself
and also set the `QJSON_INCLUDE_DIR` and `QJSON_LIBRARIES` variables in the
generated `CMakeCache.txt` when building apitrace and repeat the above
sequence.

After you've succesfully configured, you can start the build by opening the
generated `build\apitrace.sln` solution file, or invoking `cmake` as:

    cmake --build build --config MinSizeRel

The steps to build 64bit version are similar, but choosing _Visual Studio 9
2008 Win64_ instead of _Visual Studio 9 2008_.

It's also possible to instruct `cmake` build Windows binaries on Linux with
[MinGW cross compilers](http://www.cmake.org/Wiki/CmakeMingw).


