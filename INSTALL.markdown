Building from source
====================


Requirements
------------

Requirements common for all platforms:

* Python version 2.6 or 2.7

  * Python Image Library

* CMake version 2.8 or higher (tested with version 2.8)


The GUI also dependends on:

* Qt version 4.7 or higher (tested with version 4.8)

* QJSON version 0.5 or higher (tested with version 0.7.1, which is bundled)

Qt and QJSON will be required if `-DENABLE_GUI=TRUE` is passed to CMake, and
never used if `-DENABLE_GUI=FALSE` is passed instead.  The implicit default is
`-DENABLE_GUI=AUTO`, which will build the GUI if Qt is available, using the
bundled QJSON if it is not found on the system.


The code also depends on zlib, libpng, and snappy libraries, but the bundled
sources are always used regardless of system availability, to make the wrapper
shared-objects/DLL self contained, and to prevent symbol collisions when
tracing.


Linux / Mac OS X
----------------

Additional optional dependencies for Linux:

* libprocps (procps development libraries)

* libdwarf

Build as:

    cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=RelWithDebInfo
    make -C build

Other possible values for `CMAKE_BUILD_TYPE` `Debug`, `Release`,
`RelWithDebInfo`, and `MinSizeRel`.

You can also build the 32-bits GL wrapper on a 64-bits distribution, provided
you have a multilib gcc and 32-bits X11 libraries, by doing:

    cmake \
        -DCMAKE_C_FLAGS=-m32 \
        -DCMAKE_CXX_FLAGS=-m32 \
        -DCMAKE_EXE_LINKER_FLAGS=-m32 \
        -DCMAKE_SYSTEM_LIBRARY_PATH=/usr/lib32 \
        -DENABLE_GUI=FALSE \
        -H. -Bbuild32
    make -C build32 glxtrace

The `/usr/lib32` refers to the path where the 32-bits shared objects are may
differ depending on the actual Linux distribution.


Android
-------

Additional requirements:

* [Android NDK](http://developer.android.com/sdk/ndk/index.html)

Build as:

    export ANDROID_NDK=/path/to/your/ndk
    cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain/android.toolchain.cmake -DANDROID_API_LEVEL=9 -H. -Bbuild
    make -C build

You can also choose a particular ABI by passing `ANDROID_ABI` variable to
cmake, e.g., `-DANDROID_ABI=x86`.

FirefoxOS
---------

Put Apitrace source tree into `B2GROOT/external/apitrace/` and the `Android.mk`
file (`B2GROOT/external/apitrace/Android.mk`) will do the needful to compile
and install apitrace appropriately into the system image as part of FirefoxOS
build process. It expects a linaro-type of Android NDK to be present in
`../../prebuilt/ndk/android-ndk-r7` (ie `B2GROOT/prebuilt/ndk/android-ndk-r7`).


Windows
-------

Additional requirements:

* For Direct3D 11.1 support:

  * [Microsoft Visual Studio 11 Ultimate Beta](http://www.microsoft.com/download/en/details.aspx?id=27543)

* Other:

  * Microsoft Visual Studio (tested with 2010 version) or MinGW (tested with
    mingw-w64's gcc version 4.6.2)

  * [Microsoft DirectX SDK](http://msdn.microsoft.com/en-us/directx/aa937781):

    * for D3D 10, 10.1, and 11 support the [June 2010 release](http://www.microsoft.com/en-us/download/details.aspx?id=6812) is
      recommended.

    * for D3D7, D3D8 support the [August 2007 release](http://www.microsoft.com/downloads/details.aspx?familyid=529F03BE-1339-48C4-BD5A-8506E5ACF571)
      or earlier is required, as later releases do not include the necessary
      headers.

To build with Visual Studio first invoke CMake GUI as:

    cmake-gui -H%cd% -B%cd%\build

and press the _Configure_ button.

It will try to detect most required/optional dependencies automatically.  When
not found automatically, you can manually specify the location of the
dependencies from the CMake GUI.

Qt on Windows doesn't ship with 64-bit binaries, you may want to add
`-DENABLE_GUI=FALSE` to the above cmake command line for Windows 64-bits
builds.

After you've successfully configured, you can start the build by opening the
generated `build\apitrace.sln` solution file, or invoking CMake as:

    cmake --build build --config MinSizeRel

The steps to build 64bit version are similar, but choosing _Visual Studio 10
Win64_ instead of _Visual Studio 10_.

It's also possible to instruct CMake build Windows binaries on Linux with
[MinGW cross compilers](http://www.cmake.org/Wiki/CmakeMingw).


