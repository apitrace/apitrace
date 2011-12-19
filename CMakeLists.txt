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

set (ENABLE_EGL "AUTO" CACHE STRING "Enable EGL support.")


##############################################################################
# Find dependencies

include (FindPkgConfig)

set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set (CMAKE_USE_PYTHON_VERSION 2.7 2.6)

find_package (PythonInterp REQUIRED)
find_package (OpenGL REQUIRED)

if (ENABLE_GUI)
    if (NOT (ENABLE_GUI STREQUAL "AUTO"))
        set (REQUIRE_GUI REQUIRED)
    endif ()
    find_package (Qt4 4.7 COMPONENTS QtCore QtGui QtWebKit ${REQUIRE_GUI})
    find_package (QJSON ${REQUIRE_GUI})
endif ()

include_directories (${OPENGL_INCLUDE_DIR})

if (WIN32)
    find_package (DirectX)
elseif (APPLE)
else ()
    find_package (X11)

    if (X11_FOUND)
        include_directories (${X11_INCLUDE_DIR})
        add_definitions (-DHAVE_X11)
    endif ()

    if (ENABLE_EGL)
        if (NOT ENABLE_EGL STREQUAL "AUTO")
            set (REQUIRE_EGL REQUIRED)
        endif ()
        pkg_check_modules (EGL egl glesv1_cm glesv2 ${REQUIRE_EGL})
        if (EGL_FOUND)
            include_directories (${EGL_INCLUDE_DIR})
            add_definitions (-DHAVE_EGL)
        endif ()
    else ()
        set (EGL_FOUND 0)
    endif ()
endif ()


##############################################################################
# Set global build options

include (CheckCXXCompilerFlag)

if (WIN32)
    # MSVC & MinGW only define & use APIENTRY
    add_definitions (-DGLAPIENTRY=__stdcall)

    # http://msdn.microsoft.com/en-us/library/aa383745.aspx
    add_definitions (-D_WIN32_WINNT=0x0500 -DWINVER=0x0500)
else (WIN32)
    CHECK_CXX_COMPILER_FLAG("-fvisibility=hidden" CXX_COMPILER_FLAG_VISIBILITY)
    if (CXX_COMPILER_FLAG_VISIBILITY)
        add_definitions ("-fvisibility=hidden")
    endif (CXX_COMPILER_FLAG_VISIBILITY)
endif (WIN32)

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
        endif (${flag_var} MATCHES "/MD")
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
link_libraries (${ZLIB_LIBRARIES})

set (SNAPPY_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/snappy)
set (SNAPPY_LIBRARIES snappy_bundled)
add_subdirectory (thirdparty/snappy EXCLUDE_FROM_ALL)

include_directories (${SNAPPY_INCLUDE_DIRS})
link_libraries (${SNAPPY_LIBRARIES})

set (PNG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libpng)
set (PNG_DEFINITIONS "")
set (PNG_LIBRARIES png_bundled)

add_subdirectory (thirdparty/libpng EXCLUDE_FROM_ALL)
include_directories (${PNG_INCLUDE_DIR})
add_definitions (${PNG_DEFINITIONS})
link_libraries (${PNG_LIBRARIES})

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
    set (QJSON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/qjson)
    set (QJSON_LIBRARY_DIRS)
    set (QJSON_LIBRARIES qjson_bundled)
    set (QJSON_FOUND TRUE)
endif ()

# For glext headers
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)

# For EGL headers
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/khr)

##############################################################################
# Installation directories

if (WIN32 OR APPLE)
    # On Windows/MacOSX, applications are usually installed on a directory of
    # their own
    set (DOC_INSTALL_DIR doc)
else ()
    set (DOC_INSTALL_DIR share/doc/${CMAKE_PROJECT_NAME})
endif ()

set (LIB_INSTALL_DIR lib/apitrace)

if (APPLE)
    # MacOSX uses fat binaries, so no need to have per-architecture wrapper
    # directories
    set (LIB_ARCH_INSTALL_DIR ${LIB_INSTALL_DIR})
else ()
    set (LIB_ARCH_INSTALL_DIR ${LIB_INSTALL_DIR}/${CMAKE_SYSTEM_PROCESSOR})
endif ()

set(SCRIPTS_INSTALL_DIR ${LIB_INSTALL_DIR}/scripts)
set(WRAPPER_INSTALL_DIR ${LIB_ARCH_INSTALL_DIR}/wrappers)

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

add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/glproc.py > ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp
    DEPENDS glproc.py dispatch.py specs/wglapi.py specs/glxapi.py specs/cglapi.py specs/eglapi.py specs/glesapi.py specs/glapi.py specs/gltypes.py specs/stdapi.py
)

# Wrap glproc.hpp as a target to prevent the command from being executed
# multiple times simulatenously, when the targets that depend on it are built
# in parallel.
add_custom_target (glproc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp)

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

link_libraries (common)


##############################################################################
# API tracers

if (WIN32)
    if (MINGW)
        # Silence warnings about @nn suffix mismatch
        set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--enable-stdcall-fixup")
    endif (MINGW)

    # ddraw.dll
    if (DirectX_D3D_INCLUDE_DIR)
        include_directories (SYSTEM ${DirectX_D3D_INCLUDE_DIR})
        add_custom_command (
            OUTPUT ddrawtrace.cpp
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/ddrawtrace.py > ${CMAKE_CURRENT_BINARY_DIR}/ddrawtrace.cpp
            DEPENDS ddrawtrace.py trace.py specs/d3d.py specs/d3dtypes.py specs/d3dcaps.py specs/ddraw.py specs/winapi.py specs/stdapi.py
        )
        add_library (ddraw MODULE specs/ddraw.def ddrawtrace.cpp)
        set_target_properties (ddraw
            PROPERTIES PREFIX ""
            RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
            LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
        )
        install (TARGETS ddraw LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
    endif (DirectX_D3D_INCLUDE_DIR)

    # d3d8.dll
    if (DirectX_D3D8_INCLUDE_DIR AND DirectX_D3DX9_INCLUDE_DIR)
        include_directories (SYSTEM ${DirectX_D3D8_INCLUDE_DIR} ${DirectX_D3DX9_INCLUDE_DIR})
        add_custom_command (
            OUTPUT d3d8trace.cpp
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3d8trace.py > ${CMAKE_CURRENT_BINARY_DIR}/d3d8trace.cpp
            DEPENDS d3d8trace.py trace.py specs/d3d8.py specs/d3d8types.py specs/d3d8caps.py specs/winapi.py specs/stdapi.py
        )
        add_library (d3d8 MODULE specs/d3d8.def d3d8trace.cpp d3dshader.cpp)
        set_target_properties (d3d8
            PROPERTIES PREFIX ""
            RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
            LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
        )
        install (TARGETS d3d8 LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
    endif (DirectX_D3D8_INCLUDE_DIR AND DirectX_D3DX9_INCLUDE_DIR)

    # d3d9.dll
    if (DirectX_D3DX9_INCLUDE_DIR)
        include_directories (SYSTEM ${DirectX_D3DX9_INCLUDE_DIR})
        add_custom_command (
            OUTPUT d3d9trace.cpp
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3d9trace.py > ${CMAKE_CURRENT_BINARY_DIR}/d3d9trace.cpp
            DEPENDS d3d9trace.py trace.py specs/d3d9.py specs/d3d9types.py specs/d3d9caps.py specs/winapi.py specs/stdapi.py
        )
        add_library (d3d9 MODULE specs/d3d9.def d3d9trace.cpp d3dshader.cpp)
        set_target_properties (d3d9
            PROPERTIES PREFIX ""
            RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
            LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
        )
        install (TARGETS d3d9 LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
    endif (DirectX_D3DX9_INCLUDE_DIR)

    # d3d10.dll
    if (DirectX_D3D10_INCLUDE_DIR)
        include_directories (SYSTEM ${DirectX_D3D10_INCLUDE_DIR})
        add_custom_command (
            OUTPUT d3d10trace.cpp
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3d10trace.py > ${CMAKE_CURRENT_BINARY_DIR}/d3d10trace.cpp
            DEPENDS d3d10trace.py trace.py specs/d3d10misc.py specs/d3d10.py specs/dxgi.py specs/dxgitype.py specs/dxgiformat.py specs/winapi.py specs/stdapi.py
        )
        add_library (d3d10 MODULE specs/d3d10.def d3d10trace.cpp)
        set_target_properties (d3d10
            PROPERTIES PREFIX ""
            RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
            LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
        )
        install (TARGETS d3d10 LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
    endif (DirectX_D3D10_INCLUDE_DIR)

    # opengl32.dll
    add_custom_command (
        OUTPUT wgltrace.cpp
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/wgltrace.py > ${CMAKE_CURRENT_BINARY_DIR}/wgltrace.cpp
        DEPENDS wgltrace.py gltrace.py trace.py specs/wglapi.py specs/wglenum.py specs/glapi.py specs/glparams.py specs/gltypes.py specs/winapi.py specs/stdapi.py
    )
    add_library (wgltrace MODULE specs/opengl32.def
        wgltrace.cpp
        glcaps.cpp
        glproc_gl.cpp
    )
    add_dependencies (wgltrace glproc)
    set_target_properties (wgltrace PROPERTIES
        PREFIX ""
        OUTPUT_NAME opengl32
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
    )
    install (TARGETS wgltrace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})

elseif (APPLE)
    # OpenGL framework
    add_custom_command (
        OUTPUT cgltrace.cpp
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/cgltrace.py > ${CMAKE_CURRENT_BINARY_DIR}/cgltrace.cpp
        DEPENDS cgltrace.py gltrace.py trace.py specs/cglapi.py specs/glapi.py specs/glparams.py specs/gltypes.py specs/stdapi.py
    )

    add_library (cgltrace SHARED
        cgltrace.cpp
        glcaps.cpp
        glproc_gl.cpp
    )

    add_dependencies (cgltrace glproc)

    set_target_properties (cgltrace PROPERTIES
        # OpenGL framework name
        PREFIX "" OUTPUT_NAME "OpenGL" SUFFIX ""
        # Specificy the version and reexport GLU symbols
        LINK_FLAGS "-compatibility_version 1 -current_version 1.0.0 -Wl,-reexport_library,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGLU.dylib"
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
    )

    target_link_libraries (cgltrace dl)

    install (TARGETS cgltrace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
elseif (X11_FOUND)
    # libGL.so
    add_custom_command (
        OUTPUT glxtrace.cpp
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/glxtrace.py > ${CMAKE_CURRENT_BINARY_DIR}/glxtrace.cpp
        DEPENDS glxtrace.py gltrace.py trace.py specs/glxapi.py specs/glapi.py specs/glparams.py specs/gltypes.py specs/stdapi.py
    )

    add_library (glxtrace SHARED
        glxtrace.cpp
        glcaps.cpp
        glproc_gl.cpp
    )

    add_dependencies (glxtrace glproc)

    set_target_properties (glxtrace PROPERTIES
        # avoid the default "lib" prefix
        PREFIX ""
        # Prevent symbol relocations internal to our wrapper library to be
        # overwritten by the application.
        LINK_FLAGS "-Wl,-Bsymbolic -Wl,-Bsymbolic-functions"
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
    )

    target_link_libraries (glxtrace dl ${X11_X11_LIB})

    install (TARGETS glxtrace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
endif ()


if (EGL_FOUND)
    # libEGL.so/libGL.so
    add_custom_command (
        OUTPUT egltrace.cpp
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/egltrace.py > ${CMAKE_CURRENT_BINARY_DIR}/egltrace.cpp
        DEPENDS egltrace.py gltrace.py trace.py specs/eglapi.py specs/glapi.py specs/glparams.py specs/gltypes.py specs/stdapi.py
    )

    add_library (egltrace SHARED
        egltrace.cpp
        glcaps.cpp
        glproc_egl.cpp
    )

    add_dependencies (egltrace glproc)

    set_property (
        TARGET egltrace
        APPEND
        PROPERTY COMPILE_DEFINITIONS "TRACE_EGL"
    )

    set_target_properties (egltrace PROPERTIES
        # avoid the default "lib" prefix
        PREFIX ""
        LINK_FLAGS "-Wl,-Bsymbolic -Wl,-Bsymbolic-functions"
        # Prevent symbol relocations internal to our wrapper library to be
        # overwritten by the application.
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
    )

    target_link_libraries (egltrace dl)

    install (TARGETS egltrace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
endif ()

##############################################################################
# API retracers

add_custom_command (
    OUTPUT glretrace_gl.cpp
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/glretrace.py > ${CMAKE_CURRENT_BINARY_DIR}/glretrace_gl.cpp
    DEPENDS glretrace.py retrace.py specs/glapi.py specs/gltypes.py specs/stdapi.py
)

add_custom_command (
    OUTPUT glstate_params.cpp
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/glstate.py > ${CMAKE_CURRENT_BINARY_DIR}/glstate_params.cpp
    DEPENDS glstate.py specs/glparams.py specs/gltypes.py specs/stdapi.py
)

set (retrace_sources
    glretrace_gl.cpp
    glretrace_cgl.cpp
    glretrace_glx.cpp
    glretrace_wgl.cpp
    glretrace_egl.cpp
    glretrace_main.cpp
    glstate.cpp
    glstate_params.cpp
    retrace.cpp
    retrace_stdc.cpp
    glws.cpp
)

if (WIN32 OR APPLE OR X11_FOUND)
    add_executable (glretrace
        ${retrace_sources}
        ${glws_os}
        glproc_gl.cpp
    )

    add_dependencies (glretrace glproc)

    set_property (
        TARGET glretrace
        APPEND
        PROPERTY COMPILE_DEFINITIONS "RETRACE"
    )

    target_link_libraries (glretrace
        common
    )

    if (WIN32)
    else ()
        if (APPLE)
            target_link_libraries (glretrace
                "-framework Cocoa"
                "-framework ApplicationServices" # CGS*
                ${OPENGL_gl_LIBRARY} # CGL*
            )
        else ()
            target_link_libraries (glretrace ${X11_X11_LIB})
        endif ()

        target_link_libraries (glretrace
            # gdb doesn't like when pthreads is loaded through dlopen (which happens
            # when dlopen'ing libGL), so link pthreads to avoid this issue.  See also
            # http://stackoverflow.com/questions/2702628/gdb-cannot-find-new-threads-generic-error
            pthread
            dl
        )
    endif ()

    install (TARGETS glretrace RUNTIME DESTINATION bin) 
endif ()

if (EGL_FOUND AND X11_FOUND AND NOT WIN32 AND NOT APPLE)
    add_executable (eglretrace
        ${retrace_sources}
        glws_egl_xlib.cpp
        glproc_egl.cpp
    )

    add_dependencies (eglretrace glproc)

    set_property (
        TARGET eglretrace
        APPEND
        PROPERTY COMPILE_DEFINITIONS "RETRACE"
        PROPERTY COMPILE_DEFINITIONS "TRACE_EGL"
    )

    target_link_libraries (eglretrace
        common
        ${X11_X11_LIB}
        pthread
        dl
    )

    install (TARGETS eglretrace RUNTIME DESTINATION bin) 
endif ()

##############################################################################
# CLI

add_subdirectory(cli)

##############################################################################
# Scripts (to support the CLI)

install (
    PROGRAMS
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts/tracediff.sh
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
        TODO.markdown
    DESTINATION ${DOC_INSTALL_DIR}
)

set (CPACK_PACKAGE_VERSION_MAJOR "2")
set (CPACK_PACKAGE_VERSION_MINOR "0")

# Use current date in YYYYMMDD format as patch number 
execute_process (
    COMMAND ${PYTHON_EXECUTABLE} -c "import time, sys; sys.stdout.write(time.strftime('%Y%m%d'))"
    OUTPUT_VARIABLE CPACK_PACKAGE_VERSION_PATCH
)

# See http://www.vtk.org/Wiki/CMake:CPackPackageGenerators
if (WIN32)
    set (CPACK_GENERATOR "ZIP")
elseif (APPLE)
    set (CPACK_GENERATOR "DragNDrop")
else ()
    set (CPACK_GENERATOR "TBZ2")
endif ()

include(CPack)
