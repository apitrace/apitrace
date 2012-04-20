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
    glretrace_gl.cpp
    glretrace_cgl.cpp
    glretrace_glx.cpp
    glretrace_wgl.cpp
    glretrace_egl.cpp
    glretrace_main.cpp
    glstate.cpp
    glstate_images.cpp
    glstate_params.cpp
    glstate_shaders.cpp
    retrace.cpp
    retrace_stdc.cpp
    glws.cpp
)

add_dependencies (retrace_common glproc)

if (WIN32 OR APPLE OR X11_FOUND)
    add_executable (glretrace
        ${glws_os}
    )

    add_dependencies (glretrace glproc)

    target_link_libraries (glretrace
        retrace_common
        glproc_gl
        common
        ${PNG_LIBRARIES}
        ${ZLIB_LIBRARIES}
        ${SNAPPY_LIBRARIES}
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
        glproc_egl
        common
        ${PNG_LIBRARIES}
        ${ZLIB_LIBRARIES}
        ${SNAPPY_LIBRARIES}
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
    add_custom_command (
        OUTPUT d3dretrace_d3d9.cpp
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3dretrace.py > ${CMAKE_CURRENT_BINARY_DIR}/d3dretrace_d3d9.cpp
        DEPENDS
                d3dretrace.py
                dllretrace.py
                retrace.py
                ${CMAKE_SOURCE_DIR}/dispatch/dispatch.py
                ${CMAKE_SOURCE_DIR}/specs/d3d9.py
                ${CMAKE_SOURCE_DIR}/specs/d3d9types.py
                ${CMAKE_SOURCE_DIR}/specs/d3d9caps.py
                ${CMAKE_SOURCE_DIR}/specs/winapi.py
                ${CMAKE_SOURCE_DIR}/specs/stdapi.py
    )

    include_directories (SYSTEM ${DirectX_D3DX9_INCLUDE_DIR})
    add_executable (d3dretrace
        retrace.cpp
        retrace_stdc.cpp
        d3dretrace_main.cpp
        d3dretrace_d3d9.cpp
    )
    target_link_libraries (d3dretrace
        common
        ${ZLIB_LIBRARIES}
        ${SNAPPY_LIBRARIES}
    )
    
    install (TARGETS d3dretrace RUNTIME DESTINATION bin) 
endif ()

