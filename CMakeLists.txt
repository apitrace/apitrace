cmake_minimum_required (VERSION 2.8)

project (apitrace)


##############################################################################
# Find dependencies

set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set (CMAKE_USE_PYTHON_VERSION 2.7 2.6)

find_package (PythonInterp REQUIRED)
find_package (OpenGL REQUIRED)
find_package (Qt4 4.7 COMPONENTS QtCore QtGui QtWebKit)
find_package (QJSON)

if (NOT WIN32)
    # Always use the bundled zlib and libpng sources on Windows to make it easy
    # to deploy the wrappers DLLs
    find_package (ZLIB)
    find_package (PNG)
endif (NOT WIN32)

if (NOT WIN32)
    find_package (X11 REQUIRED)

    # On Mac OS X, GLX is provided as a separate OpenGL implementation, different
    # from the standard OpenGL framework which provides support for native Mac OS X
    # applications.
    if (APPLE)
        find_path (X11_GL_INCLUDE_PATH GL/glx.h ${X11_INC_SEARCH_PATH})
        if (NOT X11_GL_INCLUDE_PATH)
            message (SEND_ERROR "Could not find GL/glx.h")
        endif (NOT X11_GL_INCLUDE_PATH)
        set (X11_INCLUDE_DIR ${X11_INCLUDE_DIR} ${X11_GL_INCLUDE_PATH})

        find_library (X11_GL_LIB GL ${X11_LIB_SEARCH_PATH})
        if (NOT X11_GL_LIB)
            message (SEND_ERROR "Could not find libGL.dylib")
        endif (NOT X11_GL_LIB)
    else ()
        set (X11_INCLUDE_DIR ${X11_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
        set (X11_GL_LIB ${OPENGL_gl_LIBRARY})
    endif ()

    include_directories (${X11_INCLUDE_DIR})
endif (NOT WIN32)

if (WIN32)
    find_package (DirectX)
endif (WIN32)


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
    include_directories (msvc)

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


# Put all executables into the same top level build directory, regardless of
# which subdirectory they are declared
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories (${CMAKE_CURRENT_SOURCE_DIR})


##############################################################################
# Bundled dependencies

# Use bundled ZLIB if system one can't be found
if (NOT ZLIB_FOUND)
    set (ZLIB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/zlib)
    set (ZLIB_LIBRARIES zlib)
    add_subdirectory (zlib EXCLUDE_FROM_ALL)
endif (NOT ZLIB_FOUND)

include_directories (${ZLIB_INCLUDE_DIRS})
link_libraries (${ZLIB_LIBRARIES})

# Use bundled LIBPNG if system one can't be found
if (PNG_FOUND)
else (PNG_FOUND)
    set (PNG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libpng)
    set (PNG_DEFINITIONS "")
    set (PNG_LIBRARIES png)
    add_subdirectory (libpng EXCLUDE_FROM_ALL)
endif (PNG_FOUND)
include_directories (${PNG_INCLUDE_DIR})
add_definitions (${PNG_DEFINITIONS})
link_libraries (${PNG_LIBRARIES})


##############################################################################
# Common libraries / utilities

add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/glproc.py > ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp
    DEPENDS glproc.py dispatch.py wglapi.py glxapi.py cglapi.py glapi.py gltypes.py stdapi.py
)

if (WIN32)
    set (os os_win32.cpp)
    set (glws glws_wgl.cpp)
else (WIN32)
    set (os os_posix.cpp)
    set (glws glws_glx.cpp)
endif (WIN32)

add_library (trace trace_model.cpp trace_parser.cpp trace_writer.cpp trace_model_writer.cpp ${os})

add_executable (tracedump tracedump.cpp)
target_link_libraries (tracedump trace)
install (TARGETS tracedump RUNTIME DESTINATION bin) 


##############################################################################
# API tracers

if (WIN32)
    # ddraw.dll
    if (DirectX_D3D_INCLUDE_DIR)
        include_directories (SYSTEM ${DirectX_D3D_INCLUDE_DIR})
        add_custom_command (
            OUTPUT ddraw.cpp
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3d.py > ${CMAKE_CURRENT_BINARY_DIR}/ddraw.cpp
            DEPENDS d3d.py d3dtypes.py d3dcaps.py ddraw.py trace.py winapi.py stdapi.py
        )
        add_library (ddraw MODULE ddraw.def ddraw.cpp trace_writer.cpp os_win32.cpp)
        set_target_properties (ddraw
            PROPERTIES PREFIX ""
            RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
            LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
        )
        install (TARGETS ddraw LIBRARY DESTINATION wrappers)
    endif (DirectX_D3D_INCLUDE_DIR)

    # d3d8.dll
    if (DirectX_D3D8_INCLUDE_DIR AND DirectX_D3DX9_INCLUDE_DIR)
        include_directories (SYSTEM ${DirectX_D3D8_INCLUDE_DIR} ${DirectX_D3DX9_INCLUDE_DIR})
        add_custom_command (
            OUTPUT d3d8.cpp
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3d8.py > ${CMAKE_CURRENT_BINARY_DIR}/d3d8.cpp
            DEPENDS d3d8.py trace.py d3d8types.py d3d8caps.py winapi.py stdapi.py
        )
        add_library (d3d8 MODULE d3d8.def d3d8.cpp d3dshader.cpp trace_writer.cpp os_win32.cpp)
        set_target_properties (d3d8
            PROPERTIES PREFIX ""
            RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
            LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
        )
        install (TARGETS d3d8 LIBRARY DESTINATION wrappers)
    endif (DirectX_D3D8_INCLUDE_DIR AND DirectX_D3DX9_INCLUDE_DIR)

    # d3d9.dll
    if (DirectX_D3DX9_INCLUDE_DIR)
        include_directories (SYSTEM ${DirectX_D3DX9_INCLUDE_DIR})
        add_custom_command (
            OUTPUT d3d9.cpp
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3d9.py > ${CMAKE_CURRENT_BINARY_DIR}/d3d9.cpp
            DEPENDS d3d9.py trace.py d3d9types.py d3d9caps.py winapi.py stdapi.py
        )
        add_library (d3d9 MODULE d3d9.def d3d9.cpp d3dshader.cpp trace_writer.cpp os_win32.cpp)
        set_target_properties (d3d9
            PROPERTIES PREFIX ""
            RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
            LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
        )
        install (TARGETS d3d9 LIBRARY DESTINATION wrappers)
    endif (DirectX_D3DX9_INCLUDE_DIR)

    # d3d10.dll
    #if (DirectX_D3D10_INCLUDE_DIR)
    #    include_directories (SYSTEM ${DirectX_D3D10_INCLUDE_DIR})
    #    add_custom_command (
    #        OUTPUT d3d10.cpp
    #        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3d10misc.py > ${CMAKE_CURRENT_BINARY_DIR}/d3d10.cpp
    #        DEPENDS d3d10misc.py winapi.py stdapi.py
    #    )
    #    add_library (d3d10 MODULE d3d10.def d3d10.cpp trace_writer.cpp os_win32.cpp)
    #    set_target_properties (d3d10 PROPERTIES PREFIX "")
    #    install (TARGETS d3d10 LIBRARY DESTINATION wrappers)
    #endif (DirectX_D3D10_INCLUDE_DIR)

    # opengl32.dll
    add_custom_command (
        OUTPUT wgltrace.cpp
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/wgltrace.py > ${CMAKE_CURRENT_BINARY_DIR}/wgltrace.cpp
        DEPENDS wgltrace.py gltrace.py trace.py wglapi.py wglenum.py glapi.py glparams.py gltypes.py winapi.py stdapi.py
    )
    add_library (wgltrace MODULE opengl32.def wgltrace.cpp trace_writer.cpp os_win32.cpp ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp)
    set_target_properties (wgltrace PROPERTIES
        PREFIX ""
        OUTPUT_NAME opengl32
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
    )
    if (MINGW)
        set_target_properties(wgltrace PROPERTIES LINK_FLAGS "-Wl,--enable-stdcall-fixup ${CMAKE_CURRENT_SOURCE_DIR}/opengl32.def")
    endif (MINGW)
    install (TARGETS wgltrace LIBRARY DESTINATION wrappers)

elseif (APPLE)
    # OpenGL framework
    add_custom_command (
        OUTPUT cgltrace.cpp
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/cgltrace.py > ${CMAKE_CURRENT_BINARY_DIR}/cgltrace.cpp
        DEPENDS cgltrace.py gltrace.py trace.py cglapi.py glapi.py glparams.py gltypes.py stdapi.py
    )

    add_library (cgltrace SHARED cgltrace.cpp trace_writer.cpp os_posix.cpp ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp)

    set_target_properties (cgltrace PROPERTIES
        # OpenGL framework name
        PREFIX "" OUTPUT_NAME "OpenGL" SUFFIX ""
        # Specificy the version and reexport GLU symbols
        LINK_FLAGS "-compatibility_version 1 -current_version 1.0.0 -Wl,-reexport_library,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGLU.dylib"
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/wrappers
    )

    target_link_libraries (cgltrace dl)

    install (TARGETS cgltrace LIBRARY DESTINATION wrappers)
else ()
    # libGL.so
    add_custom_command (
        OUTPUT glxtrace.cpp
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/glxtrace.py > ${CMAKE_CURRENT_BINARY_DIR}/glxtrace.cpp
        DEPENDS glxtrace.py gltrace.py trace.py glxapi.py glapi.py glparams.py gltypes.py stdapi.py
    )

    add_library (glxtrace SHARED
        ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp
        glxtrace.cpp
        glsnapshot.cpp
        trace_writer.cpp
        image.cpp
        os_posix.cpp
    )

    set_target_properties (glxtrace PROPERTIES
        # avoid the default "lib" prefix
        PREFIX ""
    )

    # Prevent symbol relocations internal to our wrapper library to be
    # overwritten by the application.
    set_target_properties (glxtrace PROPERTIES
        LINK_FLAGS "-Wl,-Bsymbolic -Wl,-Bsymbolic-functions"
    )

    target_link_libraries (glxtrace dl ${X11_X11_LIB})
    
    install (TARGETS glxtrace LIBRARY DESTINATION lib)
endif ()


##############################################################################
# API retracers

add_custom_command (
    OUTPUT glretrace_gl.cpp
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/glretrace.py > ${CMAKE_CURRENT_BINARY_DIR}/glretrace_gl.cpp
    DEPENDS glretrace.py retrace.py codegen.py glapi.py gltypes.py stdapi.py
)

add_custom_command (
    OUTPUT glstate_params.cpp
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/glstate.py > ${CMAKE_CURRENT_BINARY_DIR}/glstate_params.cpp
    DEPENDS glstate.py glparams.py gltypes.py stdapi.py
)

include_directories (
    ${CMAKE_CURRENT_BINARY_DIR}
    ${OPENGL_INCLUDE_PATH}
)

add_executable (glretrace
    glretrace_gl.cpp
    glretrace_cgl.cpp
    glretrace_glx.cpp
    glretrace_wgl.cpp
    glretrace_main.cpp
    glstate.cpp
    glstate_params.cpp
    retrace.cpp
    ${glws}
    image.cpp 
    ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp
)

set_property (
    TARGET glretrace
    APPEND
    PROPERTY COMPILE_DEFINITIONS "RETRACE"
)

target_link_libraries (glretrace
    trace
)

if (WIN32)
    target_link_libraries (glretrace ${OPENGL_gl_LIBRARY})
elseif (APPLE)
    # XXX: We use GLX on MacOSX, which is in a separate library.
    target_link_libraries (glretrace
        ${X11_GL_LIB}
        ${X11_X11_LIB}
        "-framework ApplicationServices" # CGS*
        ${OPENGL_gl_LIBRARY} # CGL*
    )
else ()
    target_link_libraries (glretrace ${OPENGL_gl_LIBRARY} ${X11_X11_LIB})
endif ()

install (TARGETS glretrace RUNTIME DESTINATION bin) 


##############################################################################
# GUI

if (QT4_FOUND AND QJSON_FOUND)
    add_subdirectory(gui)
endif (QT4_FOUND AND QJSON_FOUND)


##############################################################################
# Packaging

install (FILES LICENSE README TODO DESTINATION doc)

set (CPACK_PACKAGE_VERSION_MAJOR "1")
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
