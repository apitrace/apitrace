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

First install Qt through [Homebrew](https://brew.sh/) like

    brew install qt5

Then do:

    cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_PREFIX_PATH=$(brew --prefix qt5)
    make -C build


# Android #

Android is no longer supported.  See https://git.io/vH2gW for more information.


# Windows #

## Microsoft Visual Studio ##

Additional requirements:

* CMake 3.7 or later

* Microsoft Visual Studio 2017 or later

* [Windows 10 SDK](https://dev.windows.com/en-us/downloads/windows-10-sdk)
  for D3D11.3 headers.

### CMake GUI ###

To build with Visual Studio first open a Command Prompt window (*not* Visual
Studio Command Prompt window), change into the Apitrace source, and invoke
CMake GUI as:

    cmake-gui -H. -Bbuild -DCMAKE_PREFIX_PATH=C:\Qt\QtX.Y.Z\X.Y\msvc2015

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

    cmake -H. -Bbuild -G "Visual Studio 15 2017" -DCMAKE_PREFIX_PATH=C:\Qt\QtX.Y.Z\X.Y\msvc2015

Note as off Qt version 5.9.1 there's no `msvc2017` directory, only `msvc2015`
and `msvc2017_64`, but `msvc2015` should work as MSVC 2017 is binary backwards
compatible with MSVC 2015.

After you've successfully configured, you can start the build by invoking CMake as:

    cmake --build build --config RelWithDebInfo

### Deployment ###

To run qapitrace, either ensure that `C:\Qt\QtX.Y.Z\X.Y\msvc????\bin` is in the system path, or use
[Qt's Windows deployment tool](http://doc.qt.io/qt-5/windows-deployment.html#the-windows-deployment-tool)
to copy all necessary DLLs, like:

    set Path=C:\Qt\QtX.Y.Z\X.Y\msvc2015\bin;%Path%
    windeployqt build\qapitrace.exe

### 64-bits ###

The steps to build 64-bits version are similar, but choosing _Visual Studio xx
Win64_ instead of _Visual Studio xx_, and using `C:\Qt\QtX.Y.Z\X.Y\msvc2017_64`
for Qt path.

### Windows XP ###

Windows XP is no longer supported.  If you need Windows XP support, your best
bet is to try an old release (search for windows-xp branch upstream.)

## MinGW ##

Additional requirements:

* [MinGW-w64](http://mingw-w64.sourceforge.net/) (tested with mingw-w64's gcc version 4.9)

* [DirectX headers](https://github.com/apitrace/dxsdk)

It's also possible to cross-compile Windows binaries from Linux with
[MinGW cross compilers](http://www.cmake.org/Wiki/CmakeMingw).

