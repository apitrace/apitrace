##############################################################################
# API retracers

# XXX: This is deliberately directory-scoped rather than PUBLIC on
# retrace_common: dispatch/glproc.hpp changes its contents with it, and
# frametrim/ (which links retrace_common, but is not a retracer) must keep
# seeing the non-RETRACE one.
add_compile_definitions (RETRACE)

add_custom_command (
    OUTPUT glretrace_gl.cpp
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/glretrace.py > ${CMAKE_CURRENT_BINARY_DIR}/glretrace_gl.cpp
    DEPENDS
        glretrace.py
        retrace.py
        ${CMAKE_SOURCE_DIR}/specs/glapi.py
        ${CMAKE_SOURCE_DIR}/specs/gltypes.py
        ${CMAKE_SOURCE_DIR}/specs/stdapi.py
)

add_custom_command (
    OUTPUT glstate_params.cpp
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/glstate_params.py > ${CMAKE_CURRENT_BINARY_DIR}/glstate_params.cpp
    DEPENDS
        glstate_params.py
        ${CMAKE_SOURCE_DIR}/specs/glparams.py
        ${CMAKE_SOURCE_DIR}/specs/gltypes.py
        ${CMAKE_SOURCE_DIR}/specs/stdapi.py
)

if (WIN32)
    set (ws_os ws_win32.cpp)
endif ()

add_library (retrace_common STATIC
    json.cpp
    process_name.hpp
    process_name.cpp
    retrace.cpp
    retrace_main.cpp
    retrace_stdc.cpp
    retrace_swizzle.cpp
    state_writer.cpp
    state_writer_json.cpp
    state_writer_ubjson.cpp
    ws.cpp
    ${ws_os}
)
target_include_directories (retrace_common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/lib/ubjson
    ${PROJECT_BINARY_DIR} # for version.h
)
target_link_libraries (retrace_common PUBLIC
    image
    common
    getopt
)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries (retrace_common PUBLIC rt)
endif ()
if (WIN32)
    target_link_libraries (retrace_common PUBLIC dxerr winmm psapi)
    if (HAVE_DETOURS)
        target_link_libraries (retrace_common PUBLIC detours)
    endif ()
endif ()
add_dependencies (retrace_common version)


add_library (glretrace_common STATIC
    glretrace.hpp
    glretrace_gl.cpp
    glretrace_cgl.cpp
    glretrace_glx.cpp
    glretrace_wgl.cpp
    glretrace_wgl_font_bitmaps.cpp
    glretrace_wgl_font_outlines.cpp
    glretrace_egl.cpp
    glretrace_main.cpp
    glretrace_ws.cpp
    glstate.cpp
    glstate_formats.cpp
    glstate_images.cpp
    glstate_params.cpp
    glstate_shaders.cpp
    glws.cpp
    metric_helper.cpp
    metric_writer.cpp
    metric_backend_amd_perfmon.cpp
    metric_backend_intel_perfquery.cpp
    metric_backend_opengl.cpp
)
target_link_libraries (glretrace_common PUBLIC
    retrace_common
    glhelpers
)
if (PROCPS_FOUND)
    target_link_libraries (glretrace_common PUBLIC procps::procps)
endif ()


if (WIN32)
    set (glws_os glws_wgl.cpp)
    target_link_libraries (glretrace_common PUBLIC gdi32)
else ()
    if (APPLE)
        set (glws_os glws_cocoa.mm)
    else ()
        set (glws_os glws_xlib.cpp glws_glx.cpp)
    endif ()
endif ()


if (WIN32 OR APPLE OR X11_FOUND)
    add_executable (glretrace
        ${glws_os}
        glproc_gl.cpp
    )

    target_link_libraries (glretrace PRIVATE
        retrace_common
        glretrace_common
        glhelpers
        glproc
    )

    if (WIN32)
    else ()
        if (APPLE)
            target_link_libraries (glretrace PRIVATE
                "-framework Cocoa"
                "-framework ApplicationServices" # CGS*
                #"-framework OpenGL" # CGL*
            )

            # Bundle Info.plist
            target_link_options (glretrace PRIVATE
                "SHELL:-sectcreate __TEXT __info_plist ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
            )
            set_target_properties (glretrace PROPERTIES
                LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
            )
        else ()
            target_link_libraries (glretrace PRIVATE X11::X11)
        endif ()

        target_link_libraries (glretrace PRIVATE
            # gdb doesn't like when pthreads is loaded through dlopen (which happens
            # when dlopen'ing libGL), so link pthreads to avoid this issue.  See also
            # http://stackoverflow.com/questions/2702628/gdb-cannot-find-new-threads-generic-error
            ${CMAKE_THREAD_LIBS_INIT}
            ${CMAKE_DL_LIBS}
        )
    endif ()

    install (TARGETS glretrace)
    install_pdb (glretrace TYPE BIN)
endif ()

if (ENABLE_EGL AND X11_FOUND AND NOT WIN32 AND NOT APPLE AND NOT ENABLE_WAFFLE)
    add_executable (eglretrace
        glws_xlib.cpp
        glws_egl_xlib.cpp
        glproc_egl.cpp
    )

    target_link_libraries (eglretrace PRIVATE
        retrace_common
        glretrace_common
        glhelpers
        glproc
        X11::X11
        Threads::Threads
        ${CMAKE_DL_LIBS}
    )
    install (TARGETS eglretrace)
endif ()

if (ENABLE_EGL AND ENABLE_WAFFLE)
    add_executable (eglretrace
        glws_waffle.cpp
        glproc_egl.cpp
    )

    target_compile_definitions (eglretrace PRIVATE HAVE_WAFFLE)

    target_link_libraries (eglretrace PRIVATE
        retrace_common
        glretrace_common
        glhelpers
        glproc
        PkgConfig::WAFFLE
        X11::X11
        Threads::Threads
        ${CMAKE_DL_LIBS}
    )
    install (TARGETS eglretrace)
endif ()

if (WIN32)
    if (DirectX_D3D_INCLUDE_FOUND)
        list (APPEND D3DRETRACE_INCLUDE_DIRS ${DirectX_D3D_INCLUDE_DIR})
        add_custom_command (
            OUTPUT d3dretrace_ddraw.cpp
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/ddrawretrace.py > ${CMAKE_CURRENT_BINARY_DIR}/d3dretrace_ddraw.cpp
            DEPENDS
                ddrawretrace.py
                dllretrace.py
                retrace.py
                ${CMAKE_SOURCE_DIR}/dispatch/dispatch.py
                ${CMAKE_SOURCE_DIR}/specs/ddraw.py
                ${CMAKE_SOURCE_DIR}/specs/d3d.py
                ${CMAKE_SOURCE_DIR}/specs/d3dtypes.py
                ${CMAKE_SOURCE_DIR}/specs/d3dcaps.py
                ${CMAKE_SOURCE_DIR}/specs/winapi.py
                ${CMAKE_SOURCE_DIR}/specs/stdapi.py
        )
        set (D3DSTATE_SOURCES ${D3DSTATE_SOURCES}
            d3d7state.cpp
            d3d7state_images.cpp
        )
    else ()
        message (FATAL_ERROR "d3d.h is required.")
    endif ()

    if (DirectX_D3D8_INCLUDE_FOUND)
        list (APPEND D3DRETRACE_INCLUDE_DIRS ${DirectX_D3D8_INCLUDE_DIR})
        set (HAVE_D3D8 1)
        set (D3DSTATE_SOURCES ${D3DSTATE_SOURCES}
            d3d8state.cpp
            d3d8state_images.cpp
        )
    else ()
        set (HAVE_D3D8 0)
    endif ()
    add_custom_command (
        OUTPUT d3dretrace_d3d8.cpp
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3d9retrace.py d3d8 ${HAVE_D3D8} > ${CMAKE_CURRENT_BINARY_DIR}/d3dretrace_d3d8.cpp
        DEPENDS
            d3d9retrace.py
            dllretrace.py
            retrace.py
            ${CMAKE_SOURCE_DIR}/dispatch/dispatch.py
            ${CMAKE_SOURCE_DIR}/specs/d3d8.py
            ${CMAKE_SOURCE_DIR}/specs/d3d8types.py
            ${CMAKE_SOURCE_DIR}/specs/d3d8caps.py
            ${CMAKE_SOURCE_DIR}/specs/winapi.py
            ${CMAKE_SOURCE_DIR}/specs/stdapi.py
    )

    if (DirectX_D3D9_INCLUDE_FOUND)
        list (APPEND D3DRETRACE_INCLUDE_DIRS ${DirectX_D3D9_INCLUDE_DIR})
        set (HAVE_D3D9 1)
    else ()
        set (HAVE_D3D9 0)
    endif ()
    add_custom_command (
        OUTPUT d3dretrace_d3d9.cpp
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3d9retrace.py d3d9 ${HAVE_D3D9} > ${CMAKE_CURRENT_BINARY_DIR}/d3dretrace_d3d9.cpp
        DEPENDS
            d3d9retrace.py
            dllretrace.py
            retrace.py
            ${CMAKE_SOURCE_DIR}/dispatch/dispatch.py
            ${CMAKE_SOURCE_DIR}/specs/d3d9.py
            ${CMAKE_SOURCE_DIR}/specs/d3d9types.py
            ${CMAKE_SOURCE_DIR}/specs/d3d9caps.py
            ${CMAKE_SOURCE_DIR}/specs/dxva2.py
            ${CMAKE_SOURCE_DIR}/specs/winapi.py
            ${CMAKE_SOURCE_DIR}/specs/stdapi.py
    )

    if (DirectX_D3D11_4_INCLUDE_FOUND)
        list (APPEND D3DRETRACE_INCLUDE_DIRS ${DirectX_D3D11_4_INCLUDE_DIR})
        set (D3DSTATE_SOURCES ${D3DSTATE_SOURCES}
            dxgistate.cpp
            dxgistate_so.cpp
            d3d10state.cpp
            d3d10state_images.cpp
            d3d11state.cpp
            d3d11state_images.cpp
            d3d12state_images.cpp
            d3dretrace_dxgi.cpp
            d3dretrace_dxgi_misc.cpp
            d3dretrace_dxgi_d3d11on12.cpp
        )
        add_custom_command (
            OUTPUT dxgistate_so.cpp
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/dxgistate_so.py > ${CMAKE_CURRENT_BINARY_DIR}/dxgistate_so.cpp
            DEPENDS
                ${CMAKE_CURRENT_SOURCE_DIR}/dxgistate_so.py
                ${CMAKE_SOURCE_DIR}/specs/d3d11.py
                ${CMAKE_SOURCE_DIR}/specs/dcomp.py
                ${CMAKE_SOURCE_DIR}/specs/d3d11sdklayers.py
                ${CMAKE_SOURCE_DIR}/specs/d3d10.py
                ${CMAKE_SOURCE_DIR}/specs/d3d10sdklayers.py
                ${CMAKE_SOURCE_DIR}/specs/dxgi.py
                ${CMAKE_SOURCE_DIR}/specs/winapi.py
                ${CMAKE_SOURCE_DIR}/specs/stdapi.py
        )
        add_custom_command (
            OUTPUT d3dretrace_dxgi.cpp
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/dxgiretrace.py > ${CMAKE_CURRENT_BINARY_DIR}/d3dretrace_dxgi.cpp
            DEPENDS
                dxgiretrace.py
                dllretrace.py
                retrace.py
                ${CMAKE_SOURCE_DIR}/dispatch/dispatch.py
                ${CMAKE_SOURCE_DIR}/specs/d3d11.py
                ${CMAKE_SOURCE_DIR}/specs/dcomp.py
                ${CMAKE_SOURCE_DIR}/specs/d3d11sdklayers.py
                ${CMAKE_SOURCE_DIR}/specs/d3d10.py
                ${CMAKE_SOURCE_DIR}/specs/d3d10sdklayers.py
                ${CMAKE_SOURCE_DIR}/specs/dxgi.py
                ${CMAKE_SOURCE_DIR}/specs/winapi.py
                ${CMAKE_SOURCE_DIR}/specs/stdapi.py
        )
    endif ()

    add_executable (d3dretrace
        d3dretrace_main.cpp
        d3dretrace_ddraw.cpp
        d3dretrace_d3d8.cpp
        d3dretrace_d3d9.cpp
        d3dretrace_dxgi_priv.cpp
        d3dretrace_ws.cpp
        d3dretrace_d3d9on12.cpp
        d3d9state.cpp
        d3d9state_formats.cpp
        d3d9state_images.cpp
        ${D3DSTATE_SOURCES}
    )
    target_include_directories (d3dretrace SYSTEM BEFORE PRIVATE ${D3DRETRACE_INCLUDE_DIRS})
    target_link_libraries (d3dretrace PRIVATE
        retrace_common
        d3dhelpers
        ole32
    )
    if (DirectX_D3D11_4_INCLUDE_FOUND)
        target_compile_definitions (d3dretrace PRIVATE HAVE_DXGI)
        target_link_libraries (d3dretrace PRIVATE directxtex)
    endif ()

    install (TARGETS d3dretrace)
    install_pdb (d3dretrace TYPE BIN)
endif ()
