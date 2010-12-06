cmake_minimum_required (VERSION 2.8)

project (apitrace)

set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Set default built type
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Debug
        CACHE
        STRING "Choose the build type, options are: None, Debug, Release, RelWithDebInfo, or MinSizeRel."
        FORCE)
endif (NOT CMAKE_BUILD_TYPE)

set (CMAKE_USE_PYTHON_VERSION 2.6)

find_package (PythonInterp REQUIRED)
find_package (OpenGL REQUIRED)

if (NOT WIN32)
    # Always use the bundled zlib and libpng sources on Windows to make it easy
    # to deploy the wrappers DLLs
    find_package (ZLIB)
    find_package (PNG)
else (NOT WIN32)
    find_package (DirectX)
endif (NOT WIN32)

find_package (GLUT)

if (WIN32)
    # MSVC & MinGW only define & use APIENTRY
    add_definitions (-DGLAPIENTRY=__stdcall)
endif (WIN32)

if (MSVC)
    # Enable math constants defines
    add_definitions (-D_USE_MATH_DEFINES)

    # Adjust warnings
    add_definitions (-D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS)
    add_definitions (-D_SCL_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
    add_definitions (-W4)
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

# Use bundled ZLIB if system one can't be found
if (ZLIB_FOUND)
    include_directories (${ZLIB_INCLUDE_DIRS})
    link_libraries (${ZLIB_LIBRARIES})
else (ZLIB_FOUND)
    add_library (zlib STATIC
        zlib/adler32.c
        zlib/compress.c
        zlib/crc32.c
        zlib/gzio.c
        zlib/uncompr.c
        zlib/deflate.c
        zlib/trees.c
        zlib/zutil.c
        zlib/inflate.c
        zlib/infback.c
        zlib/inftrees.c
        zlib/inffast.c
    )
    include_directories (zlib)
    link_libraries (zlib)
endif (ZLIB_FOUND)

# Use bundled LIBPNG if system one can't be found
if (PNG_FOUND)
    include_directories (${PNG_INCLUDE_DIR})
    add_definitions (${PNG_DEFINITIONS})
    link_libraries (${PNG_LIBRARIES})
else (PNG_FOUND)
    add_library (png STATIC
        libpng/png.c
        libpng/pngerror.c
        libpng/pngget.c
        libpng/pngmem.c
        libpng/pngpread.c
        libpng/pngread.c
        libpng/pngrio.c
        libpng/pngrtran.c
        libpng/pngrutil.c
        libpng/pngset.c
        libpng/pngtrans.c
        libpng/pngwio.c
        libpng/pngwrite.c
        libpng/pngwtran.c
        libpng/pngwutil.c
    )
    include_directories (libpng)
    link_libraries (png)
endif (PNG_FOUND)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32)
    # Put wrappers in a separate directory
    set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/wrappers)

    # d3d8.dll
    #if (DirectX_D3D8_FOUND)
    #    include_directories (${DirectX_D3D8_INCLUDE_DIR})
    #    add_custom_command (
    #        OUTPUT d3d8.cpp
    #        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3d8.py > ${CMAKE_CURRENT_BINARY_DIR}/d3d8.cpp
    #        DEPENDS d3d8.py d3d8types.py d3d8caps.py winapi.py stdapi.py
    #    )
    #    add_library (d3d8 SHARED d3d8.def d3d8.cpp trace_write.cpp os_win32.cpp)
    #    set_target_properties (d3d8 PROPERTIES PREFIX "")
    #endif (DirectX_D3D8_FOUND)

    # d3d9.dll
    if (DirectX_D3DX9_FOUND)
        include_directories (${DirectX_D3DX9_INCLUDE_DIR})
        add_custom_command (
            OUTPUT d3d9.cpp
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3d9.py > ${CMAKE_CURRENT_BINARY_DIR}/d3d9.cpp
            DEPENDS d3d9.py trace.py d3d9types.py d3d9caps.py d3dshader.py winapi.py stdapi.py
        )
        add_library (d3d9 SHARED d3d9.def d3d9.cpp trace_write.cpp os_win32.cpp)
        set_target_properties (d3d9 PROPERTIES PREFIX "")
    endif (DirectX_D3DX9_FOUND)

    # d3d10.dll
    #if (DirectX_D3D10_FOUND)
    #    include_directories (${DirectX_D3D10_INCLUDE_DIR})
    #    add_custom_command (
    #        OUTPUT d3d10.cpp
    #        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3d10misc.py > ${CMAKE_CURRENT_BINARY_DIR}/d3d10.cpp
    #        DEPENDS d3d10misc.py winapi.py stdapi.py
    #    )
    #    add_library (d3d10 SHARED d3d10.def d3d10.cpp trace_write.cpp os_win32.cpp)
    #    set_target_properties (d3d10 PROPERTIES PREFIX "")
    #endif (DirectX_D3D10_FOUND)

    # opengl32.dll
    add_custom_command (
        OUTPUT wgltrace.cpp
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/wgltrace.py > ${CMAKE_CURRENT_BINARY_DIR}/wgltrace.cpp
        DEPENDS wgltrace.py trace.py wglapi.py glapi.py glenum.py winapi.py stdapi.py
    )
    add_library (opengl SHARED opengl32.def wgltrace.cpp trace_write.cpp os_win32.cpp)
    set_target_properties (opengl PROPERTIES
        PREFIX ""
        OUTPUT_NAME opengl32)
    if (MINGW)
        SET_TARGET_PROPERTIES(opengl PROPERTIES LINK_FLAGS "-Wl,--enable-stdcall-fixup ${CMAKE_CURRENT_SOURCE_DIR}/opengl32.def")
    endif (MINGW)

else ()

    # libGL.so
    add_custom_command (
        OUTPUT glxtrace.cpp
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/glxtrace.py > ${CMAKE_CURRENT_BINARY_DIR}/glxtrace.cpp
        DEPENDS glxtrace.py trace.py glxapi.py glapi.py glenum.py stdapi.py
    )

    add_library (glxtrace SHARED glxtrace.cpp trace_write.cpp os_posix.cpp)
    set_target_properties (glxtrace PROPERTIES PREFIX "")
    target_link_libraries (glxtrace dl)
endif ()

add_executable (dump dump.cpp trace_model.cpp)

if (WIN32)
    set (os os_win32.cpp)
else (WIN32)
    set (os os_posix.cpp)
endif (WIN32)

if (GLUT_INCLUDE_DIR)
    add_custom_command (
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/glproc.py > ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp
        DEPENDS glproc.py wglapi.py glxapi.py glapi.py glenum.py stdapi.py
    )

    add_custom_command (
        OUTPUT glretrace.cpp
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/glretrace.py > ${CMAKE_CURRENT_BINARY_DIR}/glretrace.cpp
        DEPENDS glretrace.py retrace.py codegen.py glapi.py glenum.py stdapi.py
    )

    include_directories (
        ${CMAKE_CURRENT_BINARY_DIR}
        ${OPENGL_INCLUDE_PATH}
        ${GLUT_INCLUDE_DIR}
    )

    add_executable (glretrace glretrace.cpp trace_model.cpp image.cpp ${os} ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp)

    target_link_libraries (glretrace
        ${OPENGL_gl_LIBRARY}
        ${OPENGL_glu_LIBRARY}
        ${GLUT_glut_LIBRARY}
    )
endif (GLUT_INCLUDE_DIR)


