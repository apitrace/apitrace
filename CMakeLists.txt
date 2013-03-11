cmake_minimum_required (VERSION 2.8)


# Use clang on MacOSX. gcc doesn't support __thread key, and Apple has
# abandoned it for clang.  This must be done before the project is defined.
# But DONT force clang if we are cross-compiling to Android.
if (APPLE AND NOT ANDROID_NDK)
    set (CMAKE_C_COMPILER "clang")
    set (CMAKE_CXX_COMPILER "clang++")
endif ()


project (apitrace)


##############################################################################
# Options

# On Mac OS X build fat binaries with i386 and x86_64 architectures by default.
if (APPLE AND NOT CMAKE_OSX_ARCHITECTURES)
    set (CMAKE_OSX_ARCHITECTURES "i386;x86_64" CACHE STRING "Build architectures for OSX" FORCE)
endif ()

# We use a cached string variable instead of the standard (boolean) OPTION
# command so that we can default to auto-detecting optional depencies, while
# still providing a mechanism to force/disable these optional dependencies, as
# prescribed in http://www.gentoo.org/proj/en/qa/automagic.xml
set (ENABLE_GUI "AUTO" CACHE STRING "Enable Qt GUI.")

set (ENABLE_CLI true CACHE BOOL "Enable command Line interface.")

set (ENABLE_EGL true CACHE BOOL "Enable EGL support.")


##############################################################################
# Find dependencies

# Check for compiler TLS support.  We don't use compiler TLS support on Windows
# because, even if the compiler supports it, Windows XP does not support TLS on
# DLLs.
if (NOT WIN32)
    include (CheckCXXSourceCompiles)
    check_cxx_source_compiles("__thread int i; int main() { return 0; }" HAVE_COMPILER_TLS)
    if (HAVE_COMPILER_TLS)
        add_definitions (-DHAVE_COMPILER_TLS=__thread)
    else ()
        message (WARNING "C++ compiler does not support __thread keyword.")
    endif ()
endif ()

set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set (CMAKE_USE_PYTHON_VERSION 2.7 2.6)

if (ANDROID)
    set (ENABLE_GUI false)
else ()
    macro (find_host_package)
        find_package (${ARGN})
    endmacro()
endif ()

find_host_package (PythonInterp REQUIRED)
find_package (Threads)

if (ENABLE_GUI)
    if (NOT (ENABLE_GUI STREQUAL "AUTO"))
        set (REQUIRE_GUI REQUIRED)
    endif ()
    find_package (Qt4 4.7 COMPONENTS QtCore QtGui QtWebKit ${REQUIRE_GUI})
endif ()

if (WIN32)
    find_package (DirectX)
    set (ENABLE_EGL false)
elseif (APPLE)
    set (ENABLE_EGL false)
else ()
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


##############################################################################
# Set global build options

include (CheckCXXCompilerFlag)

if (WIN32)
    # http://msdn.microsoft.com/en-us/library/aa383745.aspx
    add_definitions (-D_WIN32_WINNT=0x0601 -DWINVER=0x0601)
else (WIN32)
    CHECK_CXX_COMPILER_FLAG("-fvisibility=hidden" CXX_COMPILER_FLAG_VISIBILITY)
    if (CXX_COMPILER_FLAG_VISIBILITY)
        add_definitions ("-fvisibility=hidden")
    endif ()
endif ()

if (MSVC)
    # C99 includes for MSVC
    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/msinttypes)

    # Enable math constants defines
    add_definitions (-D_USE_MATH_DEFINES)

    # No min/max macros
    add_definitions (-DNOMINMAX)

    # Adjust warnings
    add_definitions (-D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS)
    add_definitions (-D_SCL_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
    add_definitions (-W4)
    # XXX: it's safer to use ssize_t everywhere instead of disabling warning
    add_definitions (-wd4018) # signed/unsigned mismatch
    add_definitions (-wd4063) # not a valid value for switch of enum
    add_definitions (-wd4100) # unreferenced formal parameter
    add_definitions (-wd4127) # conditional expression is constant
    add_definitions (-wd4244) # conversion from 'type1' to 'type2', possible loss of data
    add_definitions (-wd4505) # unreferenced local function has been removed
    add_definitions (-wd4512) # assignment operator could not be generated
    add_definitions (-wd4800) # forcing value to bool 'true' or 'false' (performance warning)
    
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
    add_definitions (-Wall)
    # XXX: it's safer to use ssize_t everywhere instead of disabling warning
    add_definitions (-Wno-sign-compare) # comparison between signed and unsigned integer expressions

    # Use GDB extensions if available
    if (CMAKE_COMPILER_IS_GNUC)
        set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -ggdb -O0")
        set (CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -ggdb")
    endif ()
    if (CMAKE_COMPILER_IS_GNUCXX)
        set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb -O0")
        set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -ggdb")
    endif ()

    # Be nice to Eclipse
    add_definitions (-fmessage-length=0)
endif ()

if (MINGW)
    # Avoid depending on MinGW runtime DLLs
    check_cxx_compiler_flag (-static-libgcc HAVE_STATIC_LIBGCC_FLAG)
    if (HAVE_STATIC_LIBGCC_FLAG)
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc")
        set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc")
        set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -static-libgcc")
    endif ()
    check_cxx_compiler_flag (-static-libstdc++ HAVE_STATIC_LIBSTDCXX_FLAG)
    if (HAVE_STATIC_LIBSTDCXX_FLAG)
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++")
        set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libstdc++")
        set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -static-libstdc++")
    endif ()
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # For RTLD_DEFAULT and RTLD_NEXT
    add_definitions (-D_GNU_SOURCE)
endif ()


# Put all executables into the same top level build directory, regardless of
# which subdirectory they are declared
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


##############################################################################
# Installation directories

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Debian multiarch support
    execute_process(COMMAND dpkg-architecture -qDEB_HOST_MULTIARCH
        OUTPUT_VARIABLE ARCH_SUBDIR
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    CHECK_INCLUDE_FILES(proc/readproc.h READPROC_H_FOUND)
    if (READPROC_H_FOUND)
        add_definitions (-DHAVE_READPROC_H)
        find_library (proc_LIBRARY NAMES proc procps)
    endif ()
endif()

if (WIN32 OR APPLE)
    # On Windows/MacOSX, applications are usually installed on a directory of
    # their own
    set (DOC_INSTALL_DIR doc)
    set (LIB_INSTALL_DIR lib)
    set (LIB_ARCH_INSTALL_DIR lib)
else ()
    set (DOC_INSTALL_DIR share/doc/${CMAKE_PROJECT_NAME})
    set (LIB_INSTALL_DIR lib/${CMAKE_PROJECT_NAME})
    if (ARCH_SUBDIR)
        set (LIB_ARCH_INSTALL_DIR lib/${ARCH_SUBDIR}/${CMAKE_PROJECT_NAME})
    else ()
        set (LIB_ARCH_INSTALL_DIR lib/${CMAKE_PROJECT_NAME})
    endif ()
endif ()

set (SCRIPTS_INSTALL_DIR ${LIB_INSTALL_DIR}/scripts)
set (WRAPPER_INSTALL_DIR ${LIB_ARCH_INSTALL_DIR}/wrappers)


##############################################################################
# Bundled dependencies
#
# We always use the bundled zlib, libpng, and snappy sources:
# - on Windows to make it easy to deploy the wrappers DLLs
# - on unices to prevent symbol collisions when tracing applications that link
# against other versions of these libraries

set (ZLIB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/zlib)
set (ZLIB_LIBRARIES z_bundled)
add_subdirectory (thirdparty/zlib)

include_directories (${ZLIB_INCLUDE_DIRS})

set (SNAPPY_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/snappy)
set (SNAPPY_LIBRARIES snappy_bundled)
add_subdirectory (thirdparty/snappy)

include_directories (${SNAPPY_INCLUDE_DIRS})

set (PNG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libpng)
set (PNG_DEFINITIONS "")
set (PNG_LIBRARIES png_bundled)

add_subdirectory (thirdparty/libpng)

if (MSVC)
    add_subdirectory (thirdparty/getopt)
    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/getopt)
    set (GETOPT_LIBRARIES getopt_bundled)
endif ()

if (WIN32)
    add_subdirectory (thirdparty/less)
    add_subdirectory (thirdparty/directxtex)
endif ()

if (CMAKE_EXECUTABLE_FORMAT STREQUAL "ELF")
    add_subdirectory (thirdparty/libbacktrace)
    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libbacktrace)
    set (LIBBACKTRACE_LIBRARIES backtrace)
endif ()

# Always use bundled QJSon.
# - The packaged versions QJson are very old, and do not support NaN/Infinity.
# - To make it easier to build the GUI on Windows and MacOSX, as there are no
# binaries at all.
if (QT4_FOUND)
    add_definitions (-DQJSON_EXPORT=)
    add_subdirectory (thirdparty/qjson)
    set (QJSON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)
    set (QJSON_LIBRARY_DIRS)
    set (QJSON_LIBRARIES qjson_bundled)
    set (QJSON_FOUND TRUE)
endif ()

# We use bundled headers for all Khronos APIs, to guarantee support for both
# OpenGL and OpenGL ES at build time, because the OpenGL and OpenGL ES 1 APIs
# are so intertwined that conditional compilation extremely difficult. This
# also avoids missing/inconsistent declarations in system headers.
include_directories (BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/khronos)


##############################################################################
# Common libraries / utilities

include_directories (
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)

if (WIN32)
    set (os os_win32.cpp)
    set (glws_os glws_wgl.cpp)
else ()
    set (os os_posix.cpp)
    if (APPLE)
        set (glws_os glws_cocoa.mm)
    else ()
        set (glws_os glws_glx.cpp)
    endif ()
endif ()

add_library (common STATIC
    common/trace_callset.cpp
    common/trace_dump.cpp
    common/trace_fast_callset.cpp
    common/trace_file.cpp
    common/trace_file_read.cpp
    common/trace_file_write.cpp
    common/trace_file_zlib.cpp
    common/trace_file_snappy.cpp
    common/trace_model.cpp
    common/trace_parser.cpp
    common/trace_parser_flags.cpp
    common/trace_writer.cpp
    common/trace_writer_local.cpp
    common/trace_writer_model.cpp
    common/trace_loader.cpp
    common/trace_profiler.cpp
    common/trace_option.cpp
    common/${os}
    common/trace_backtrace.cpp
)

set_target_properties (common PROPERTIES
    # Ensure it can be statically linked in shared libraries
    COMPILE_FLAGS "${CMAKE_SHARED_LIBRARY_CXX_FLAGS}"
)

if (ANDROID)
    target_link_libraries (common
        log
    )
endif ()


##############################################################################
# Sub-directories

add_subdirectory (dispatch)
add_subdirectory (helpers)
add_subdirectory (wrappers)
add_subdirectory (image)
add_subdirectory (retrace)


##############################################################################
# CLI

if (ENABLE_CLI)
    if (WIN32)
        add_subdirectory (inject)
    endif ()
    add_subdirectory (cli)
endif ()

##############################################################################
# Scripts (to support the CLI)

install (
    PROGRAMS
        scripts/highlight.py
        scripts/jsondiff.py
        scripts/profileshader.py
        scripts/retracediff.py
        scripts/snapdiff.py
        scripts/tracecheck.py
        scripts/tracediff.py
        scripts/unpickle.py
    DESTINATION ${SCRIPTS_INSTALL_DIR}
)
if (WIN32)
    install (
        PROGRAMS scripts/convert.py
        DESTINATION ${SCRIPTS_INSTALL_DIR}
    )
endif ()

##############################################################################
# GUI

if (ENABLE_GUI AND QT4_FOUND AND QJSON_FOUND)
    add_subdirectory(gui)
endif ()


##############################################################################
# Packaging

install (
    FILES
        BUGS.markdown
        NEWS.markdown
        README.markdown
    DESTINATION ${DOC_INSTALL_DIR}
)
install (
    FILES LICENSE
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE.txt
)
if (MSVC)
    install (
        FILES thirdparty/msinttypes/LICENSE
        DESTINATION ${DOC_INSTALL_DIR}
        RENAME LICENSE-msinttypes.txt
    )
endif ()

set (CPACK_PACKAGE_VERSION_MAJOR "4")
set (CPACK_PACKAGE_VERSION_MINOR "0")

# Use current date in YYYYMMDD format as patch number 
execute_process (
    COMMAND ${PYTHON_EXECUTABLE} -c "import time, sys; sys.stdout.write(time.strftime('%Y%m%d'))"
    OUTPUT_VARIABLE CPACK_PACKAGE_VERSION_PATCH
)

# cpack mistakenly detects Mingw-w64 as win32
if (MINGW)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set (CPACK_SYSTEM_NAME win64)
    endif ()
endif ()

# See http://www.vtk.org/Wiki/CMake:CPackPackageGenerators
if (WIN32)
    set (CPACK_GENERATOR "ZIP")
elseif (APPLE)
    set (CPACK_GENERATOR "DragNDrop")
else ()
    set (CPACK_GENERATOR "TBZ2")
endif ()

include(CPack)
