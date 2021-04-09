message (STATUS "Using CMake ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")
cmake_minimum_required (VERSION 3.8.0)


if (CMAKE_GENERATOR STREQUAL "Xcode")
    message (FATAL_ERROR "Xcode generator is not supported. Please build with \"Unix Makefiles\" or \"Ninja\" generators.")
endif ()

# http://www.cmake.org/cmake/help/v3.0/policy/CMP0042.html
if (POLICY CMP0042)
    cmake_policy (SET CMP0042 NEW)
endif()


project (apitrace)


##############################################################################
# Options

# We use a cached string variable instead of the standard (boolean) OPTION
# command so that we can default to auto-detecting optional depencies, while
# still providing a mechanism to force/disable these optional dependencies, as
# prescribed in http://www.gentoo.org/proj/en/qa/automagic.xml
set (ENABLE_GUI "AUTO" CACHE STRING "Enable Qt GUI.")

option (ENABLE_CLI "Enable command Line interface." ON)

option (ENABLE_X11 "Enable X11 support." ON)

option (ENABLE_EGL "Enable EGL support." ON)

option (ENABLE_WAFFLE "Enable WAFFLE support." OFF)

option (ENABLE_SSE42 "Enable SSE 4.2 intrinsics." OFF)

option (ENABLE_FRAME_POINTER "Disable frame pointer omission" ON)

option (ENABLE_ASAN "Enable Address Sanitizer" OFF)

option (BUILD_TESTING "Enable unit tests" ON)

option (ENABLE_TESTS "Enable additional tests" OFF)

if (ANDROID)
    message (FATAL_ERROR "Android is no longer supported (https://git.io/vH2gW)")
endif ()

# Proprietary Linux games often ship their own libraries (zlib, libstdc++,
# etc.) in order to ship a single set of binaries across multiple
# distributions.  Given that apitrace wrapper modules will be loaded into those
# processes, they must not depend on any shared object that could also be
# provided by such applications.  See also
# http://lists.freedesktop.org/archives/mesa-dev/2015-March/079121.html
if (NOT ANDROID)
    option (ENABLE_STATIC_SNAPPY "Statically link against snappy" ON)
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        option (ENABLE_STATIC_LIBGCC "Statically link LD_PRELOAD wrappers against libgcc" ON)
        option (ENABLE_STATIC_LIBSTDCXX "Statically link LD_PRELOAD wrappers against libstdc++" ON)
        if (NOT (ENABLE_STATIC_LIBGCC AND
                 ENABLE_STATIC_LIBSTDCXX AND
                 ENABLE_STATIC_SNAPPY))
            # XXX: Should probably throw a run-time too.
            message (WARNING
                "LD_PRELOAD wrappers not statically linked against all "
                "dependencies, therefore will fail to work with many "
                "third-party applications built on different Linux "
                "distributions and that ship their own shared-object "
                "dependencies."
            )
        endif ()
        option (ENABLE_STATIC_EXE "Statically link executables" OFF)
    endif ()
endif ()


##############################################################################
# Find dependencies

include (CheckCXXSourceCompiles)

if (MSVC)
    if (${MSVC_VERSION} LESS 1920)
        message (FATAL_ERROR "Visual Studio 2019 or later required")
    endif ()
endif ()

# Check for compiler TLS support.  We don't use compiler TLS support on Windows
# because, even if the compiler supports it, Windows XP does not support TLS on
# DLLs.
if (NOT WIN32)
    check_cxx_source_compiles ("__thread int i; int main() { return 0; }" HAVE_COMPILER_TLS)
    if (NOT HAVE_COMPILER_TLS)
        message (FATAL_ERROR "C++ compiler does not support __thread keyword.")
    endif ()
endif ()

list (APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include (ConvenienceLibrary)
include (InstallPDB)

if (ANDROID)
    set (ENABLE_GUI false)
else ()
    macro (find_host_package)
        find_package (${ARGN})
    endmacro()
endif ()

if (CMAKE_VERSION VERSION_LESS 3.12)
    find_host_package (PythonInterp 3 REQUIRED)
    if (NOT PYTHON_VERSION_MAJOR EQUAL 3)
        message (FATAL_ERROR "Python 3.x required and requested, but Python ${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR} found.")
    endif ()
    set (Python3_EXECUTABLE "${PYTHON_EXECUTABLE}")
else ()
    find_host_package (Python3 REQUIRED)
endif ()

find_package (Threads)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ENABLE_STATIC_EXE)
    find_package (procps)
    if (PROCPS_FOUND)
        add_definitions (-DHAVE_READPROC_H)
    endif ()
endif ()

if (ENABLE_GUI)
    if (NOT (ENABLE_GUI STREQUAL "AUTO"))
        set (REQUIRE_GUI REQUIRED)
    endif ()
    if (POLICY CMP0020)
        cmake_policy (SET CMP0020 NEW)
    endif()
    find_package (Qt5Widgets ${REQUIRE_GUI})
    find_package (Qt5Network ${REQUIRE_GUI})
endif ()

if (WIN32)
    # http://msdn.microsoft.com/en-us/library/aa383745.aspx
    # Windows 8
    add_definitions (-D_WIN32_WINNT=0x0602 -DWINVER=0x0602)

    find_package (DirectX)

    if (MSVC)
        # Log Windows SDK version
        unset (WINDOWS_SDK_VERSION)
        if (DEFINED CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
            set (WINDOWS_SDK_VERSION ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION})
        elseif (DEFINED ENV{WindowsSDKLibVersion})
            string (REGEX REPLACE "\\\\$" "" WINDOWS_SDK_VERSION "$ENV{WindowsSDKLibVersion}")
        endif ()
        if (DEFINED WINDOWS_SDK_VERSION)
            message (STATUS "Using Windows SDK ${WINDOWS_SDK_VERSION}")
        endif ()

        # https://developer.microsoft.com/en-us/windows/projects/campaigns/windows-dev-essentials-how-windows-10-sdks-ship
        set (REQUIRED_WINDOWS_SDK 10.0.18362)
        if (NOT DirectX_D3D11_4_INCLUDE_FOUND)
            if (CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
                message (SEND_ERROR
                    "Windows ${REQUIRED_WINDOWS_SDK} SDK required, but ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION} found.\n"
                    "Install it then select it by configuring with -DCMAKE_SYSTEM_VERSION=${REQUIRED_WINDOWS_SDK} option.")

            else ()
                message (SEND_ERROR
                    "Windows ${REQUIRED_WINDOWS_SDK} SDK required.\n"
                    "Install it then select it by invoking `vcvarsall.bat <arch> ${REQUIRED_WINDOWS_SDK}`"
                )
            endif ()
        endif ()
    endif ()

    set (ENABLE_EGL false)
elseif (APPLE)
    set (ENABLE_EGL false)
    add_definitions (-DGL_SILENCE_DEPRECATION)
elseif (ENABLE_X11)
    find_package (X11)

    if (X11_FOUND)
        include_directories (${X11_INCLUDE_DIR})
        add_definitions (-DHAVE_X11)
    else ()
        # Print a clear message when X11 is not found
        include (FindPackageMessage)
        find_package_message (X11 "Could not find X11" "")
    endif ()
endif ()

if (ENABLE_EGL AND ENABLE_WAFFLE)
    # Use Waffle for eglretrace
    find_package (Waffle REQUIRED)
endif ()

if (ENABLE_EGL AND NOT ENABLE_X11)
    add_definitions (-DEGL_NO_X11)
endif ()


##############################################################################
# Set global build options

set (CMAKE_C_STANDARD 99)
set (CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_STANDARD 17)
set (CXX_STANDARD_REQUIRED ON)

include (CheckCXXCompilerFlag)
include (CheckIncludeFileCXX)

macro (add_compiler_flags)
    string (REPLACE ";" " " _FLAGS "${ARGV}")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_FLAGS}")
endmacro ()

macro (add_linker_flags)
    string (REPLACE ";" " " _FLAGS "${ARGV}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${_FLAGS}")
    set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${_FLAGS}")
    set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${_FLAGS}")
endmacro ()

add_definitions (
    -D__STDC_LIMIT_MACROS
    -D__STDC_FORMAT_MACROS
)

if (WIN32)
else (WIN32)
    CHECK_CXX_COMPILER_FLAG("-fvisibility=hidden" CXX_COMPILER_FLAG_VISIBILITY)
    if (CXX_COMPILER_FLAG_VISIBILITY)
        add_compiler_flags (-fvisibility=hidden)
    endif ()
endif ()

if (MSVC)
    # No RTTI required
    string (REGEX REPLACE "/GR *" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")

    # Disable C++ exceptions
    add_definitions (-D_HAS_EXCEPTIONS=0)
    string (REGEX REPLACE "/EHsc *" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHs-c-")

    # Enable math constants defines
    add_definitions (-D_USE_MATH_DEFINES)

    # No min/max macros
    add_definitions (-DNOMINMAX)

    # Adjust warnings
    add_definitions (-D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS)
    add_definitions (-D_SCL_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
    add_compiler_flags (-W3)
    # XXX: it's safer to use ssize_t everywhere instead of disabling warning
    add_compiler_flags (-wd4018) # signed/unsigned mismatch
    add_compiler_flags (-wd4063) # not a valid value for switch of enum
    add_compiler_flags (-wd4100) # unreferenced formal parameter
    add_compiler_flags (-wd4127) # conditional expression is constant
    add_compiler_flags (-wd4244) # conversion from 'type1' to 'type2', possible loss of data
    add_compiler_flags (-wd4267) # conversion from 'type1' to 'type2', possible loss of data
    add_compiler_flags (-wd4505) # unreferenced local function has been removed
    add_compiler_flags (-wd4512) # assignment operator could not be generated
    add_compiler_flags (-wd4577) # 'noexcept' used with no exception handling mode specified
    add_compiler_flags (-wd4800) # forcing value to bool 'true' or 'false' (performance warning)
    
    # Use static runtime
    # http://www.cmake.org/Wiki/CMake_FAQ#How_can_I_build_my_MSVC_application_with_a_static_runtime.3F
    foreach (flag_var
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
    )
        if (${flag_var} MATCHES "/MD")
            string (REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif ()
    endforeach (flag_var)
else ()
    # Adjust warnings
    add_compiler_flags (-Wall)
    # XXX: it's safer to use ssize_t everywhere instead of disabling warning
    add_compiler_flags (-Wno-sign-compare) # comparison between signed and unsigned integer expressions

    # Disable strict aliasing assumptions.  We generate a lot of C++ code, and
    # it's not always easy to guarantee or spot when strict aliasing
    # assumptions are violated.  Above all, the benefit is not worth the risk.
    add_compiler_flags (-fno-strict-aliasing)

    # No RTTI required
    # XXX: there's a dynamic_cast in Android
    if (NOT ANDROID)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
    endif ()

    # Disable C++ exceptions
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")

    # Enable Address Sanitizer
    if (ENABLE_ASAN)
        add_compiler_flags (-fsanitize=address)
        add_linker_flags (-fsanitize=address)
    endif ()

    # Enable SSE2 intrinsics on x86
    if (MINGW AND CMAKE_SIZEOF_VOID_P EQUAL 4)
        add_compiler_flags (-msse2 -mfpmath=sse)

        # And tell GCC to assume 4 bytes alignment, many Linux/Windows
        # applications only guarantee that, but not on systems where ABI
        # clearly states otherwise.
        #
        # See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=38496
        if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
            add_compiler_flags (-mstackrealign)
        else ()
            add_compiler_flags (-mincoming-stack-boundary=2)
        endif ()
    endif ()

    if (ENABLE_SSE42)
        if (MINGW)
            add_compiler_flags (-msse4.2)
        endif ()
        add_definitions (-DENABLE_SSE42)
    endif ()

    # Be nice to Eclipse
    add_compiler_flags (-fmessage-length=0)
endif ()

if (APPLE)
    check_cxx_source_compiles ("#include <AvailabilityMacros.h>\n#if MAC_OS_X_VERSION_MAX_ALLOWED >= 101200\nint main() { return 0; }\n#endif" HAVE_MACOSX_10_12_SDK)
    if (NOT HAVE_MACOSX_10_12_SDK)
        message (FATAL_ERROR "macOS 10.12 SDK or newer (i.e. Xcode 8.0 or newer) required")
    endif ()
endif ()

if (MINGW)
    # Avoid depending on MinGW runtime DLLs
    add_linker_flags (-static-libgcc -static-libstdc++)
elseif (ENABLE_STATIC_EXE)
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif ()

if (ENABLE_FRAME_POINTER)
    # disable frame pointer omission
    if (MSVC)
        add_compiler_flags (/Oy-)
    else ()
        add_compiler_flags (-fno-omit-frame-pointer)
    endif ()
endif ()

if (WIN32)
    # Increase number of sections in obj files
    if (MSVC)
        add_compiler_flags (-bigobj)
    else ()
        add_compiler_flags (-Wa,-mbig-obj)
    endif ()

    # Enable Data Execution Prevention and Address Space Layout Randomization
    if (MSVC)
        add_linker_flags (/NXCOMPAT /DYNAMICBASE)
    else ()
        add_linker_flags (-Wl,--nxcompat)

        # `--dynamicbase` causes ld segmentation fault with Ubuntu 20.04 MinGW
        # cross compilation toolchain, unless it's paired with
        # `--export-all-symbols`
        set (CMAKE_REQUIRED_LINK_OPTIONS "-Wl,--dynamicbase")
        check_cxx_source_compiles("int main() { return 0; }" HAVE_DYNAMICBASE)
        unset (CMAKE_REQUIRED_LINK_OPTIONS)
        if (HAVE_DYNAMICBASE)
            add_linker_flags (-Wl,--dynamicbase)
        endif ()
    endif ()

    # Use more than 2GB virtual memory address space for 32-bits processes
    # where available (3GB on 32-bits Windows with 4GT, 4GB on 64-bits Windows)
    if (CMAKE_SIZEOF_VOID_P EQUAL 4)
        if (MSVC)
            add_linker_flags (/LARGEADDRESSAWARE)
        else ()
            add_linker_flags (-Wl,--large-address-aware)
        endif ()
    endif ()
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # For RTLD_DEFAULT and RTLD_NEXT
    add_definitions (-D_GNU_SOURCE)
endif ()

include (TestBigEndian)
test_big_endian (HAVE_BIGENDIAN)
if (HAVE_BIGENDIAN)
    add_definitions (-DHAVE_BIGENDIAN)
endif ()

# Force certain components to always be built as release
macro (force_release_build)
    if (MSVC)
        # TODO: Needs special care due since runtime must match
    else ()
        set (CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_RELWITHDEBINFO})
        set (CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
    endif ()
endmacro ()

# Put all executables into the same top level build directory, regardless of
# which subdirectory they are declared
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (BUILD_TESTING)
    enable_testing ()
endif ()
if (CMAKE_CROSSCOMPILING)
    add_custom_target (check)
elseif (DEFINED CMAKE_BUILD_TYPE)
    # Single configuration
    add_custom_target (check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)
else ()
    # Multiple configuration
    add_custom_target (check COMMAND ${CMAKE_CTEST_COMMAND} -C "$<CONFIG>" --output-on-failure)
endif ()


##############################################################################
# Installation directories

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Debian multiarch support
    execute_process(COMMAND dpkg-architecture -qDEB_HOST_MULTIARCH
        OUTPUT_VARIABLE ARCH_SUBDIR
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

if (WIN32 OR APPLE)
    # On Windows/MacOSX, applications are usually installed on a directory of
    # their own
    set (DOC_DEFAULT_INSTALL_DIR doc)
    set (LIB_INSTALL_DIR lib)
    set (LIB_ARCH_INSTALL_DIR lib)
else ()
    set (DOC_DEFAULT_INSTALL_DIR share/doc/${CMAKE_PROJECT_NAME})
    set (LIB_INSTALL_DIR lib${LIB_SUFFIX}/${CMAKE_PROJECT_NAME})
    if (ARCH_SUBDIR)
        set (LIB_ARCH_INSTALL_DIR lib/${ARCH_SUBDIR}/${CMAKE_PROJECT_NAME})
    else ()
        set (LIB_ARCH_INSTALL_DIR lib${LIB_SUFFIX}/${CMAKE_PROJECT_NAME})
    endif ()
endif ()

# Allow customization of the doc installation dir (Slackware uses different
# location)
set (DOC_INSTALL_DIR "${DOC_DEFAULT_INSTALL_DIR}" CACHE STRING "Documentation installation directory")

set (SCRIPTS_INSTALL_DIR ${LIB_INSTALL_DIR}/scripts)
set (WRAPPER_INSTALL_DIR ${LIB_ARCH_INSTALL_DIR}/wrappers)


##############################################################################
# Bundled dependencies
#
# We prefer to bundle and statically link against many dependencies:
# - on Windows to make it easy to deploy the wrappers DLLs
# - on unices to prevent symbol collisions when tracing applications that link
# against other versions of these libraries

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/compat)

if (NOT ENABLE_STATIC_SNAPPY)
    find_package (SNAPPY)
endif ()
if (ENABLE_STATIC_SNAPPY OR NOT SNAPPY_FOUND)
    message (STATUS "Using bundled SNAPPY")
    set (SNAPPY_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/snappy)
    set (SNAPPY_LIBRARIES snappy_bundled)
    add_subdirectory (thirdparty/snappy)
endif ()
include_directories (${SNAPPY_INCLUDE_DIRS})

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/brotli/c/include)
add_subdirectory (thirdparty/brotli)

if (NOT WIN32 AND NOT ENABLE_STATIC_EXE)
    # zlib 1.2.4-1.2.5 made it impossible to read the last block of incomplete
    # gzip traces (e.g., apitrace-tests/traces/zlib-no-eof.trace).
    find_package (ZLIB 1.2.6)
endif ()
if (NOT ZLIB_FOUND)
    message (STATUS "Using bundled ZLIB")
    set (ZLIB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/zlib)
    set (ZLIB_LIBRARIES z_bundled)
    add_subdirectory (thirdparty/zlib)
endif ()
include_directories (${ZLIB_INCLUDE_DIRS})

# FindPNG.cmake will search ZLIB internally (without requiring any particular
# version), adding its include dirs and libraries, and overwriting ZLIB_FOUND.
# So if the system's ZLIB was did not meet the our requirements, then there's
# no safe way to use the system's PNG library.
if (NOT WIN32 AND NOT APPLE AND NOT ENABLE_STATIC_EXE AND ZLIB_FOUND)
    find_package (PNG)
endif ()
if (NOT PNG_FOUND)
    message (STATUS "Using bundled PNG")
    set (PNG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libpng)
    set (PNG_DEFINITIONS "")
    set (PNG_LIBRARIES png_bundled)
    add_subdirectory (thirdparty/libpng)
endif ()

if (MSVC)
    add_subdirectory (thirdparty/getopt)
    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/getopt)
    set (GETOPT_LIBRARIES getopt_bundled)
endif ()

if (WIN32)
    add_subdirectory (thirdparty/dxerr)
    add_subdirectory (thirdparty/directxtex)
    add_subdirectory (thirdparty/devcon)
    add_subdirectory (thirdparty/mhook)
endif ()

add_subdirectory (thirdparty/crc32c)

if (CMAKE_EXECUTABLE_FORMAT STREQUAL "ELF")
    add_subdirectory (thirdparty/libbacktrace)
    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libbacktrace)
    include_directories (${CMAKE_CURRENT_BINARY_DIR}/thirdparty/libbacktrace)
    set (LIBBACKTRACE_LIBRARIES ${CMAKE_DL_LIBS} backtrace)
    add_definitions (-DHAVE_BACKTRACE=1)
endif ()

add_subdirectory (thirdparty/md5)
set (MD5_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/md5)
set (MD5_LIBRARIES md5_bundled)

# We use bundled headers for all Khronos APIs, to guarantee support for both
# OpenGL and OpenGL ES at build time, because the OpenGL and OpenGL ES 1 APIs
# are so intertwined that conditional compilation extremely difficult. This
# also avoids missing/inconsistent declarations in system headers.
include_directories (BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/khronos)

# We use non-standard C++ flags, so we can't just use GTest's CMakeLists.txt
add_definitions (-DGTEST_HAS_TR1_TUPLE=0)
include_directories (BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/gtest/include)
add_library (gtest ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/gtest/src/gtest-all.cc)
set_property (TARGET gtest APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/gtest)
target_link_libraries (gtest ${CMAKE_THREAD_LIBS_INIT})

# Convenience macro for adding unit tests
macro (add_gtest)
    add_executable (${ARGV})
    target_link_libraries (${ARGV0} gtest)
    add_dependencies (check ${ARGV0})
    add_test (NAME ${ARGV0} COMMAND $<TARGET_FILE:${ARGV0}>)
endmacro ()


##############################################################################
# Common libraries / utilities

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/trace
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/os
    ${CMAKE_CURRENT_SOURCE_DIR}/compat
)

add_subdirectory (lib)
add_subdirectory (dispatch)
add_subdirectory (helpers)
add_subdirectory (wrappers)
add_subdirectory (retrace)
add_subdirectory (frametrim)


##############################################################################
# CLI

if (ENABLE_CLI)
    if (WIN32)
        add_subdirectory (inject)
    endif ()
    add_subdirectory (cli)
    add_subdirectory (scripts)
endif ()


##############################################################################
# GUI

if (ENABLE_GUI AND Qt5Widgets_FOUND AND Qt5Network_FOUND)
  add_subdirectory (gui)
endif ()


##############################################################################
# Additional tests

if (ENABLE_TESTS)
    if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
        message (EMIT_ERROR
                 "tests/CMakeLists.txt is missing, please do\n"
                 "  git clone https://github.com/apitrace/apitrace-tests tests")
    else ()
        add_subdirectory (tests)
    endif ()
endif ()


##############################################################################
# Packaging

install (
    FILES
        README.markdown
        docs/BUGS.markdown
        docs/NEWS.markdown
        docs/USAGE.markdown
    DESTINATION ${DOC_INSTALL_DIR}
)
install (
    FILES LICENSE
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE.txt
)

set (CPACK_PACKAGE_VERSION "latest")
# https://www.appveyor.com/docs/environment-variables/
if (($ENV{APPVEYOR}) AND ($ENV{APPVEYOR_REPO_TAG}))
    set (CPACK_PACKAGE_VERSION "$ENV{APPVEYOR_REPO_TAG_NAME}")
endif ()
# https://docs.github.com/en/actions/reference/environment-variables#default-environment-variables
if ("$ENV{GITHUB_ACTIONS}" STREQUAL "true" AND "$ENV{GITHUB_REF}" MATCHES [[^refs/tags/([^/]*)$]])
    set (CPACK_PACKAGE_VERSION "${CMAKE_MATCH_1}")
endif ()

# cpack mistakenly detects Mingw-w64 as win32
if (MINGW)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set (CPACK_SYSTEM_NAME win64)
    endif ()
endif ()

# See http://www.vtk.org/Wiki/CMake:CPackPackageGenerators
if (WIN32)
    if (CMAKE_VERSION VERSION_LESS 3.1)
        set (CPACK_GENERATOR "ZIP")
    else ()
        set (CPACK_GENERATOR "7Z")
    endif ()
elseif (APPLE)
    set (CPACK_GENERATOR "DragNDrop")
else ()
    set (CPACK_GENERATOR "TBZ2")
endif ()

include(CPack)
