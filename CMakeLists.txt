cmake_minimum_required (VERSION 2.8)

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
    find_package (QJSON ${REQUIRE_GUI})
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
    endif ()
endif ()


##############################################################################
# Set global build options

include (CheckCXXCompilerFlag)

if (WIN32)
    # http://msdn.microsoft.com/en-us/library/aa383745.aspx
    add_definitions (-D_WIN32_WINNT=0x0500 -DWINVER=0x0500)
else (WIN32)
    CHECK_CXX_COMPILER_FLAG("-fvisibility=hidden" CXX_COMPILER_FLAG_VISIBILITY)
    if (CXX_COMPILER_FLAG_VISIBILITY)
        add_definitions ("-fvisibility=hidden")
    endif ()
endif ()

if (MSVC)
    # C99 includes for msvc
    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/msvc)

    # Enable math constants defines
    add_definitions (-D_USE_MATH_DEFINES)

    # No min/max macros
    add_definitions (-DNOMINMAX)

    # Adjust warnings
    add_definitions (-D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS)
    add_definitions (-D_SCL_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
    add_definitions (-W4)
    add_definitions (-wd4063) # not a valid value for switch of enum
    add_definitions (-wd4127) # conditional expression is constant
    add_definitions (-wd4244) # conversion from 'type1' to 'type2', possible loss of data
    add_definitions (-wd4505) # unreferenced local function has been removed
    add_definitions (-wd4800) # forcing value to bool 'true' or 'false' (performance warning)
    # XXX: it's safer to use ssize_t everywhere instead of disabling warning
    add_definitions (-wd4018) # signed/unsigned mismatch
    
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


# Put all executables into the same top level build directory, regardless of
# which subdirectory they are declared
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


##############################################################################
# Bundled dependencies
#
# We always use the bundled zlib, libpng, and snappy sources:
# - on Windows to make it easy to deploy the wrappers DLLs
# - on unices to prevent symbol collisions when tracing applications that link
# against other versions of these libraries

set (ZLIB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/zlib)
set (ZLIB_LIBRARIES z_bundled)
add_subdirectory (thirdparty/zlib EXCLUDE_FROM_ALL)

include_directories (${ZLIB_INCLUDE_DIRS})

set (SNAPPY_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/snappy)
set (SNAPPY_LIBRARIES snappy_bundled)
add_subdirectory (thirdparty/snappy EXCLUDE_FROM_ALL)

include_directories (${SNAPPY_INCLUDE_DIRS})

set (PNG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libpng)
set (PNG_DEFINITIONS "")
set (PNG_LIBRARIES png_bundled)

add_subdirectory (thirdparty/libpng EXCLUDE_FROM_ALL)
include_directories (${PNG_INCLUDE_DIR})
add_definitions (${PNG_DEFINITIONS})

if (MSVC)
    add_subdirectory (thirdparty/getopt EXCLUDE_FROM_ALL)
    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/getopt)
    set (GETOPT_LIBRARIES getopt_bundled)
endif ()

if (WIN32)
    add_subdirectory (thirdparty/less)
endif ()

# The Qt website provides binaries for Windows and MacOSX, and they are
# automatically found by cmake without any manual intervention.  The situation
# for QJSON is substantially different: there are no binaries for QJSON
# available, and there is no standard installation directory that is detected
# by cmake.
#
# By bundling the QJSON source, we make it much more easier to build the GUI on
# Windows and MacOSX.  But we only use the bundled sources when ENABLE_GUI is
# AUTO.
if (QT4_FOUND AND NOT QJSON_FOUND AND (ENABLE_GUI STREQUAL "AUTO"))
    add_subdirectory (thirdparty/qjson EXCLUDE_FROM_ALL)
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
# Installation directories

if (WIN32 OR APPLE)
    # On Windows/MacOSX, applications are usually installed on a directory of
    # their own
    set (DOC_INSTALL_DIR doc)
    set (LIB_INSTALL_DIR lib)
else ()
    set (DOC_INSTALL_DIR share/doc/${CMAKE_PROJECT_NAME})
    set (LIB_INSTALL_DIR lib/${CMAKE_PROJECT_NAME})
endif ()

set(SCRIPTS_INSTALL_DIR ${LIB_INSTALL_DIR}/scripts)
set(WRAPPER_INSTALL_DIR ${LIB_INSTALL_DIR}/wrappers)

# Expose the binary/install directories to source
#
# TODO: Use the same directory layout, for both build and install directories,
# so that binaries can find each other using just relative paths.
#
add_definitions(
    -DAPITRACE_BINARY_DIR="${CMAKE_BINARY_DIR}"
    -DAPITRACE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
    -DAPITRACE_WRAPPER_INSTALL_DIR="${CMAKE_INSTALL_PREFIX}/${WRAPPER_INSTALL_DIR}"
)


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
    common/trace_resource.cpp
    common/trace_tools_trace.cpp
    common/image.cpp
    common/image_bmp.cpp
    common/image_pnm.cpp
    common/image_png.cpp
    common/${os}
)

set_target_properties (common PROPERTIES
    COMPILE_DEFINITIONS APITRACE_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
    # Ensure it can be statically linked in shared libraries
    COMPILE_FLAGS "${CMAKE_SHARED_LIBRARY_CXX_FLAGS}"
)

if (ANDROID)
    target_link_libraries (common log)
endif ()


##############################################################################
# Sub-directories

add_subdirectory (dispatch)
add_subdirectory (wrappers)
add_subdirectory (retrace)


##############################################################################
# CLI

if (ENABLE_CLI)
    add_subdirectory(cli)
endif ()

##############################################################################
# Scripts (to support the CLI)

install (
    PROGRAMS
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts/tracediff.py
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts/jsondiff.py
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts/snapdiff.py
    DESTINATION ${SCRIPTS_INSTALL_DIR}
)

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
        LICENSE
        NEWS.markdown
        README.markdown
    DESTINATION ${DOC_INSTALL_DIR}
)

set (CPACK_PACKAGE_VERSION_MAJOR "3")
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
