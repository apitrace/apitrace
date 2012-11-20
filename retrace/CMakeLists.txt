##############################################################################
# API retracers

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/helpers
    ${CMAKE_BINARY_DIR}/dispatch
    ${CMAKE_SOURCE_DIR}/dispatch
)

add_definitions (-DRETRACE)

add_custom_command (
    OUTPUT glretrace_gl.cpp
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/glretrace.py > ${CMAKE_CURRENT_BINARY_DIR}/glretrace_gl.cpp
    DEPENDS
                glretrace.py
                retrace.py
                ${CMAKE_SOURCE_DIR}/specs/glapi.py
                ${CMAKE_SOURCE_DIR}/specs/gltypes.py
                ${CMAKE_SOURCE_DIR}/specs/stdapi.py
)

add_custom_command (
    OUTPUT glstate_params.cpp
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/glstate_params.py > ${CMAKE_CURRENT_BINARY_DIR}/glstate_params.cpp
    DEPENDS
                glstate_params.py
                ${CMAKE_SOURCE_DIR}/specs/glparams.py
                ${CMAKE_SOURCE_DIR}/specs/gltypes.py
                ${CMAKE_SOURCE_DIR}/specs/stdapi.py
)

add_library (retrace_common
    retrace.cpp
    retrace_main.cpp
    retrace_stdc.cpp
    retrace_swizzle.cpp
)

target_link_libraries (retrace_common
    common
    ${PNG_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${SNAPPY_LIBRARIES}
    ${GETOPT_LIBRARIES}
)

add_library (glretrace_common
    glretrace_gl.cpp
    glretrace_cgl.cpp
    glretrace_glx.cpp
    glretrace_wgl.cpp
    glretrace_egl.cpp
    glretrace_main.cpp
    glretrace_ws.cpp
    glstate.cpp
    glstate_images.cpp
    glstate_params.cpp
    glstate_shaders.cpp
    glws.cpp
)

add_dependencies (glretrace_common glproc)


if (WIN32 OR APPLE OR X11_FOUND)
    add_executable (glretrace
        ${glws_os}
    )

    add_dependencies (glretrace glproc)

    target_link_libraries (glretrace
        retrace_common
        glretrace_common
        glproc_gl
    )

    if (WIN32)
    else ()
        if (APPLE)
            target_link_libraries (glretrace
                "-framework Cocoa"
                "-framework ApplicationServices" # CGS*
                #"-framework OpenGL" # CGL*
            )
        else ()
            target_link_libraries (glretrace ${X11_X11_LIB})
        endif ()

        target_link_libraries (glretrace
            # gdb doesn't like when pthreads is loaded through dlopen (which happens
            # when dlopen'ing libGL), so link pthreads to avoid this issue.  See also
            # http://stackoverflow.com/questions/2702628/gdb-cannot-find-new-threads-generic-error
            ${CMAKE_THREAD_LIBS_INIT}
            dl
        )

        if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
            target_link_libraries (glretrace rt)
        endif ()

    endif ()

    install (TARGETS glretrace RUNTIME DESTINATION bin) 
endif ()

if (ENABLE_EGL AND X11_FOUND AND NOT WIN32 AND NOT APPLE)
    add_executable (eglretrace
        glws_egl_xlib.cpp
    )

    add_dependencies (eglretrace glproc)

    target_link_libraries (eglretrace
        retrace_common
        glretrace_common
        glproc_egl
        ${X11_X11_LIB}
        ${CMAKE_THREAD_LIBS_INIT}
        dl
    )

    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        target_link_libraries (eglretrace rt)
    endif ()

    install (TARGETS eglretrace RUNTIME DESTINATION bin) 
endif ()

if (WIN32 AND DirectX_D3DX9_INCLUDE_DIR)
    include_directories (SYSTEM ${DirectX_D3DX9_INCLUDE_DIR})
    add_custom_command (
        OUTPUT d3dretrace_d3d9.cpp
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3d9retrace.py > ${CMAKE_CURRENT_BINARY_DIR}/d3dretrace_d3d9.cpp
        DEPENDS
                d3d9retrace.py
                dllretrace.py
                retrace.py
                ${CMAKE_SOURCE_DIR}/dispatch/dispatch.py
                ${CMAKE_SOURCE_DIR}/specs/d3d9.py
                ${CMAKE_SOURCE_DIR}/specs/d3d9types.py
                ${CMAKE_SOURCE_DIR}/specs/d3d9caps.py
                ${CMAKE_SOURCE_DIR}/specs/winapi.py
                ${CMAKE_SOURCE_DIR}/specs/stdapi.py
    )

    if (DirectX_D3D10_INCLUDE_DIR)
        include_directories (SYSTEM ${DirectX_D3D10_INCLUDE_DIR})
        set (D3D10_MODULES d3d10)
        if (DirectX_D3D10_1_INCLUDE_DIR)
            include_directories (SYSTEM ${DirectX_D3D10_1_INCLUDE_DIR})
            set (D3D10_MODULES ${D3D10_MODULES} d3d10_1)
        endif ()
        set (D3DSTATE_SOURCES ${D3DSTATE_SOURCES}
            d3d10state.cpp
            d3d10state_images.cpp
        )
    endif ()

    if (DirectX_D3D11_INCLUDE_DIR)
        include_directories (SYSTEM ${DirectX_D3D11_INCLUDE_DIR})
        set (D3D10_MODULES ${D3D10_MODULES} d3d11)
        if (DirectX_D3D11_1_INCLUDE_DIR)
            include_directories (SYSTEM ${DirectX_D3D11_1_INCLUDE_DIR})
            set (D3D10_MODULES ${D3D10_MODULES} d3d11_1)
        endif ()
        set (D3DSTATE_SOURCES ${D3DSTATE_SOURCES}
            d3d11state.cpp
            d3d11state_images.cpp
        )
    endif ()

    add_custom_command (
        OUTPUT d3dretrace_d3d10.cpp
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3dcommonretrace.py ${D3D10_MODULES} > ${CMAKE_CURRENT_BINARY_DIR}/d3dretrace_d3d10.cpp
        DEPENDS
                d3dcommonretrace.py
                dllretrace.py
                retrace.py
                ${CMAKE_SOURCE_DIR}/dispatch/dispatch.py
                ${CMAKE_SOURCE_DIR}/specs/d3d11_1.py
                ${CMAKE_SOURCE_DIR}/specs/d3d11.py
                ${CMAKE_SOURCE_DIR}/specs/d3d11sdklayers.py
                ${CMAKE_SOURCE_DIR}/specs/d3d10_1.py
                ${CMAKE_SOURCE_DIR}/specs/d3d10.py
                ${CMAKE_SOURCE_DIR}/specs/d3d10misc.py
                ${CMAKE_SOURCE_DIR}/specs/d3d10sdklayers.py
                ${CMAKE_SOURCE_DIR}/specs/d3d10shader.py
                ${CMAKE_SOURCE_DIR}/specs/d3d10effect.py
                ${CMAKE_SOURCE_DIR}/specs/d3dcommon.py
                ${CMAKE_SOURCE_DIR}/specs/dxgi.py
                ${CMAKE_SOURCE_DIR}/specs/dxgitype.py
                ${CMAKE_SOURCE_DIR}/specs/dxgiformat.py
                ${CMAKE_SOURCE_DIR}/specs/winapi.py
                ${CMAKE_SOURCE_DIR}/specs/stdapi.py
    )

    add_executable (d3dretrace
        d3dretrace_main.cpp
        d3dretrace_d3d9.cpp
        d3dretrace_d3d10.cpp
        d3dretrace_ws.cpp
        d3d9state.cpp
        d3d9state_images.cpp
        ${D3DSTATE_SOURCES}
    )
    target_link_libraries (d3dretrace
        retrace_common
    )

    install (TARGETS d3dretrace RUNTIME DESTINATION bin)
endif ()
