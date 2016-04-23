[![Build Status](https://travis-ci.org/apitrace/apitrace.svg?branch=master)](https://travis-ci.org/apitrace/apitrace)
[![Build status](https://ci.appveyor.com/api/projects/status/5da6kauyfvclv6y0/branch/master?svg=true)](https://ci.appveyor.com/project/jrfonseca/apitrace/branch/master)


# Requirements #

Requirements common for all platforms:

* C++ compiler

* Python version 2.7

  * Python Image Library

* CMake version 2.8.11 or higher (tested with version 2.8.12.2)

Optional dependencies:

* zlib 1.2.6 or higher

* libpng

The GUI also dependends on:

* Qt version 5.2.1 or higher (tested with version 5.4.0 and 5.3.0; use the
  [6.1 release](https://github.com/apitrace/apitrace/releases/tag/6.1), if you
  must build with Qt4)

Qt will be required if `-DENABLE_GUI=TRUE` is passed to CMake, and never used
if `-DENABLE_GUI=FALSE` is passed instead.  The implicit default is
`-DENABLE_GUI=AUTO`, which will build the GUI if Qt is available.

If you have Qt in a non-standard directory, you'll need to set
[`-DCMAKE_PREFIX_PATH`](http://doc.qt.io/qt-5/cmake-manual.html).


The code also depends on snappy libraries, but the bundled sources are always
used regardless of system availability, to make the wrapper shared-objects/DLL
self contained, and to prevent symbol collisions when tracing.


# Linux #

Optional dependencies:

* Xlib headers

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
        -H. -Bbuild32 \
        -DCMAKE_C_FLAGS=-m32 \
        -DCMAKE_CXX_FLAGS=-m32 \
        -DCMAKE_SYSTEM_LIBRARY_PATH=/usr/lib32 \
        -DENABLE_GUI=FALSE
    make -C build32 glxtrace

The `/usr/lib32` refers to the path where the 32-bits shared objects are may
differ depending on the actual Linux distribution.


# Mac OS X #

Build as:

    cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_PREFIX_PATH=~/QtX.Y.Z/X.Y/clang_64
    make -C build


# Android #

Additional requirements:

* [Android CMake](https://github.com/taka-no-me/android-cmake)
* [Android NDK](http://developer.android.com/sdk/ndk/index.html)
* [Android SDK](http://developer.android.com/sdk/index.html#Other). **Make sure you have Android 4.4.2 (API 19) platform SDK installed and Android build tools "21.1.2". API 19 is needed only to build the APK, but it will still run on lower API versions (with works starting with API 12).**

Build as:

    wget -N https://raw.githubusercontent.com/taka-no-me/android-cmake/master/android.toolchain.cmake
    
    cmake \
        -H. -Bbuild \
        -DCMAKE_TOOLCHAIN_FILE=$PWD/android.toolchain.cmake \
        -DANDROID_NDK=/path/to/your/ndk \
        -DANDROID_API_LEVEL=9 \
        -DANDROID_STL=gnustl_shared \
        [-DANDROID_SDK=/path/to/your/sdk]

    make -C build
    [make -C build retraceAPK] will build Android retrace APK.
    [make -C build installRetraceAPK] will build & install Android retrace APK on the connected device.

You can also choose a particular ABI by passing `ANDROID_ABI` variable to
cmake, e.g., `-DANDROID_ABI=x86`.  Currently, when targeting AArch64 you [must
build with GCC](https://github.com/apitrace/apitrace/issues/312), by invoking
CMake as:

    cmake \
        -H. -Bbuild \
        -DCMAKE_TOOLCHAIN_FILE=$PWD/android.toolchain.cmake \
        -DANDROID_API_LEVEL=9 \
        -DANDROID_TOOLCHAIN_NAME=aarch64-linux-android-4.9 \
        -DANDROID_ABI=arm64-v8a

** You'll need Android SDK only to build the retrace APK. **


# FirefoxOS #

Put Apitrace source tree into `B2GROOT/external/apitrace/` and the `Android.mk`
file (`B2GROOT/external/apitrace/Android.mk`) will do the needful to compile
and install apitrace appropriately into the system image as part of FirefoxOS
build process. It expects a linaro-type of Android NDK to be present in
`../../prebuilt/ndk/android-ndk-r7` (ie `B2GROOT/prebuilt/ndk/android-ndk-r7`).


# Windows #

## Microsoft Visual Studio ##

Additional requirements:

* CMake 3.4 or later

* Microsoft Visual Studio 2013 or newer (instructions presume 2015)

* [Windows 10 SDK](https://dev.windows.com/en-us/downloads/windows-10-sdk)
  for D3D11.3 headers.

* Microsoft DirectX SDK is now part of Microsoft Visual Studio (from version
  2012), but D3D8 headers are not included, so if you want D3D8 support you'll
  need to donwload and install the
  [August 2007 release of DirectX SDK](https://www.microsoft.com/en-gb/download/details.aspx?id=13287)

### CMake GUI ###

To build with Visual Studio first open a Command Prompt window (*not* Visual
Studio Command Prompt window), change into the Apitrace source, and invoke
CMake GUI as:

    cmake-gui -H. -Bbuild -DCMAKE_SYSTEM_VERSION=10.0 -DCMAKE_PREFIX_PATH=C:\Qt\QtX.Y.Z\X.Y\msvc2015

and press the _Configure_ button.

It will try to detect most required/optional dependencies automatically.  When
not found automatically, you can manually specify the location of the
dependencies from the CMake GUI.

If the source/build/compiler/tools are spread across multiple drives, you might
need to [use absolute paths](https://github.com/apitrace/apitrace/issues/352).

After you've successfully configured, you can start the build by opening the
generated `build\apitrace.sln` solution file

### CMake CLI ###

Another option is to use the commandline to configure the project. First of all find out which
generators are available on your system:

    cmake --help

At the end of the output, choose a generator and start configuring the project:

    cmake -H. -Bbuild -G "Visual Studio 14 2015" -DCMAKE_SYSTEM_VERSION=10.0 -DCMAKE_PREFIX_PATH=C:\Qt\QtX.Y.Z\X.Y\msvc2015

After you've successfully configured, you can start the build by invoking CMake as:

    cmake --build build --config RelWithDebInfo

### Deployment ###

To run qapitrace, either ensure that `C:\Qt\QtX.Y.Z\X.Y\msvc2015\bin` is in the system path, or use
[Qt's Windows deployment tool](http://doc.qt.io/qt-5/windows-deployment.html#the-windows-deployment-tool)
to copy all necessary DLLs, like:

    set Path=C:\Qt\QtX.Y.Z\X.Y\msvc2015\bin;%Path%
    windeployqt build\qapitrace.exe

### 64-bits ###

The steps to build 64-bits version are similar, but choosing _Visual Studio xx
Win64_ instead of _Visual Studio xx_, and using `C:\Qt\QtX.Y.Z\X.Y\msvc2015_64`
for Qt path.

### Windows XP ###

By default, binaries generated by recent builds of Visual Studio will not work
on Windows XP.  If you want to obtain binaries that work on Windows XP then
pass the `-T v140_xp -DCMAKE_SYSTEM_VERSION=5.1` options to CMake when building
with Visual Studio 2015.

## MinGW ##

Additional requirements:

* [MinGW-w64](http://mingw-w64.sourceforge.net/) (tested with mingw-w64's gcc version 4.9)

* [DirectX headers](https://github.com/apitrace/dxsdk)

It's also possible to cross-compile Windows binaries from Linux with
[MinGW cross compilers](http://www.cmake.org/Wiki/CmakeMingw).

