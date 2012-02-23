Building from source
====================


Requirements
------------

Requirements common for all platforms:

* Python version 2.6 or 2.7

  * Python Image Library

* CMake version 2.8 or higher (tested with version 2.8)


The GUI also dependends on:

* Qt version 4.7

* QJSON version 0.5 or higher (tested with version 0.7.1, which is bundled)

Qt and QJSON will be required if `-DENABLE_GUI=TRUE` is passed to `cmake`, and
never used if `-DENABLED_GUI=FALSE` is passed instead.  The implicit default is
`-DENABLE_GUI=AUTO`, which will build the GUI if Qt is available, using the
bundled QJSON if it is not found on the system.


The code also depends on zlib, libpng, and snappy libraries, but the bundled
sources are always used regardless of system availability, to make the wrapper
shared-objects/DLL self contained, and to prevent symbol collisions when
tracing.


Linux / Mac OS X
----------------

Build as:

    cmake -H. -Bbuild
    make -C build

You can also build the 32bit GL wrapper on 64bit distro with a multilib gcc by
doing:

    cmake -H. -Bbuild32 -DCMAKE_C_FLAGS=-m32 -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_EXE_LINKER_FLAGS=-m32 -DENABLE_GUI=FALSE
    make -C build32 glxtrace

Android
-------

Additional requirements:

* [Android NDK](http://developer.android.com/sdk/ndk/index.html)

Build as:

    export ANDROID_NDK=/path/to/your/ndk
    cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain/android.toolchain.cmake -DANDROID_API_LEVEL=9 -H. -Bbuild
    make -C build

This will generate a libapitrace.so in your `libs/armeabi-v7a/` directory that can be used.

Windows
-------

Additional requirements:

* Microsoft Visual Studio (tested with 2008 version) or MinGW (tested with gcc version 4.4)

* Microsoft DirectX SDK:

  * for D3D10 support the [latest](http://msdn.microsoft.com/en-us/directx/default.aspx) is
    recommended.

  * for DDRAW, D3D7, D3D8 support the [August 2007 release](http://www.microsoft.com/downloads/details.aspx?familyid=529F03BE-1339-48C4-BD5A-8506E5ACF571)
    or earlier is required, as later releases do not include the necessary
    headers.

To build with Visual Studio first invoke CMake GUI as:

    cmake-gui -H. -B%cd%\build

and press the _Configure_ button.

It will try to detect most required/optional dependencies automatically.  When
not found automatically, you can manually specify the location of the
dependencies from the CMake GUI.

After you've successfully configured, you can start the build by opening the
generated `build\apitrace.sln` solution file, or invoking `cmake` as:

    cmake --build build --config MinSizeRel

The steps to build 64bit version are similar, but choosing _Visual Studio 9
2008 Win64_ instead of _Visual Studio 9 2008_.

It's also possible to instruct `cmake` build Windows binaries on Linux with
[MinGW cross compilers](http://www.cmake.org/Wiki/CmakeMingw).


