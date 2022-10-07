[![Build Status](https://github.com/apitrace/apitrace/actions/workflows/build.yml/badge.svg?branch=master)](https://github.com/apitrace/apitrace/actions/workflows/build.yml)


# Requirements #

Requirements common for all platforms:

* C++ 17 compiler

* Python version 3.6 or newer

  * Python Image Library

* CMake version 3.8 or newer

Optional dependencies:

* zlib 1.2.6 or higher

* libpng

The GUI also dependends on:

* Qt version 5.15

Qt will be required if `-DENABLE_GUI=TRUE` is passed to CMake, and never used
if `-DENABLE_GUI=FALSE` is passed instead.  The implicit default is
`-DENABLE_GUI=AUTO`, which will build the GUI if Qt is available.

If you have Qt in a non-standard directory, you'll need to set
[`-DCMAKE_PREFIX_PATH`](https://doc.qt.io/qt-5/cmake-manual.html).


The code also depends on snappy libraries, but the bundled sources are always
used regardless of system availability, to make the wrapper shared-objects/DLL
self contained, and to prevent symbol collisions when tracing.


# Linux #

Optional dependencies:

* Xlib headers

* libprocps (procps development libraries)

* libdwarf

Build as:

    cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=RelWithDebInfo
    make -C build

Other possible values for `CMAKE_BUILD_TYPE` `Debug`, `Release`,
`RelWithDebInfo`, and `MinSizeRel`.

You can also build the 32-bits GL wrapper on a 64-bits distribution, provided
you have a multilib gcc and 32-bits X11 libraries, by doing:

    cmake \
        -S. -Bbuild32 \
        -DCMAKE_C_FLAGS=-m32 \
        -DCMAKE_CXX_FLAGS=-m32 \
        -DCMAKE_SYSTEM_LIBRARY_PATH=/usr/lib32 \
        -DENABLE_GUI=FALSE
    make -C build32 glxtrace

The `/usr/lib32` refers to the path where the 32-bits shared objects are may
differ depending on the actual Linux distribution.


# Mac OS X #

First install Qt through [Homebrew](https://brew.sh/) like

    brew install qt5

Then do:

    cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_PREFIX_PATH=$(brew --prefix qt5)
    make -C build


# Android #

Android is no longer supported.  See https://git.io/vH2gW for more information.


# Windows #

## Microsoft Visual Studio ##

Additional requirements:

* CMake 3.7 or later

* Microsoft Visual Studio 2019 or later

* [Windows 10 SDK](https://dev.windows.com/en-us/downloads/windows-10-sdk)
  for D3D11.3 headers.

### CMake GUI ###

To build with Visual Studio first open a Command Prompt window (*not* Visual
Studio Command Prompt window), change into the Apitrace source, and invoke
CMake GUI as:

    cmake-gui -S. -Bbuild -DCMAKE_PREFIX_PATH=C:\Qt\QtX.Y.Z\X.Y\msvc2019

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

    cmake -S. -Bbuild -G "Visual Studio 17 2022" -A Win32 -DCMAKE_PREFIX_PATH=C:\Qt\QtX.Y.Z\X.Y\msvc2019

After you've successfully configured, you can start the build by invoking CMake as:

    cmake --build build --config RelWithDebInfo

### Deployment ###

To run qapitrace, either ensure that `C:\Qt\QtX.Y.Z\X.Y\msvc????\bin` is in the system path, or use
[Qt's Windows deployment tool](https://doc.qt.io/qt-5/windows-deployment.html#the-windows-deployment-tool)
to copy all necessary DLLs, like:

    set Path=C:\Qt\QtX.Y.Z\X.Y\msvc2019\bin;%Path%
    windeployqt build\qapitrace.exe

### 64-bits ###

The steps to build 64-bits version are similar, but choosing `-A x64` instead
of `-A Win32`, and using `C:\Qt\QtX.Y.Z\X.Y\msvc2019_64` for Qt path.

### Windows XP ###

Windows XP is no longer supported.  If you need Windows XP support, your best
bet is to user an [older version of the code](https://github.com/apitrace/apitrace/tree/windows-xp).

## MinGW ##

Additional requirements:

* [MinGW-w64](https://www.mingw-w64.org/) (tested with mingw-w64's gcc version 4.9)

* [DirectX headers](https://github.com/apitrace/dxsdk)

It's also possible to cross-compile Windows binaries from Linux with
[MinGW cross compilers](https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/cross_compiling/Mingw).

