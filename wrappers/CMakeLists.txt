##############################################################################
# API tracers


include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/helpers
    ${CMAKE_BINARY_DIR}/dispatch
    ${CMAKE_SOURCE_DIR}/dispatch
)


if (WIN32)
    if (MINGW)
        # Silence warnings about @nn suffix mismatch
        set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--enable-stdcall-fixup")
    endif ()

    # ddraw.dll
    if (DirectX_D3D_INCLUDE_DIR)
        include_directories (SYSTEM ${DirectX_D3D_INCLUDE_DIR})
        add_custom_command (
            OUTPUT ddrawtrace.cpp
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/ddrawtrace.py > ${CMAKE_CURRENT_BINARY_DIR}/ddrawtrace.cpp
            DEPENDS
                ddrawtrace.py
                dlltrace.py
                trace.py
                ${CMAKE_SOURCE_DIR}/dispatch/dispatch.py
                ${CMAKE_SOURCE_DIR}/specs/d3d.py
                ${CMAKE_SOURCE_DIR}/specs/d3dtypes.py
                ${CMAKE_SOURCE_DIR}/specs/d3dcaps.py
                ${CMAKE_SOURCE_DIR}/specs/ddraw.py
                ${CMAKE_SOURCE_DIR}/specs/winapi.py
                ${CMAKE_SOURCE_DIR}/specs/stdapi.py
        )
        add_library (ddrawtrace MODULE ddraw.def ddrawtrace.cpp)
        target_link_libraries (ddrawtrace
            common
            ${ZLIB_LIBRARIES}
            ${SNAPPY_LIBRARIES}
        )
        set_target_properties (ddrawtrace PROPERTIES
            PREFIX ""
            OUTPUT_NAME ddraw
        )
        install (TARGETS ddrawtrace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
    endif ()

    # d3d8.dll
    if (DirectX_D3D8_INCLUDE_DIR AND DirectX_D3DX9_INCLUDE_DIR)
        include_directories (SYSTEM ${DirectX_D3D8_INCLUDE_DIR} ${DirectX_D3DX9_INCLUDE_DIR})
        add_custom_command (
            OUTPUT d3d8trace.cpp
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3d8trace.py > ${CMAKE_CURRENT_BINARY_DIR}/d3d8trace.cpp
            DEPENDS
                d3d8trace.py
                dlltrace.py
                trace.py
                ${CMAKE_SOURCE_DIR}/dispatch/dispatch.py
                ${CMAKE_SOURCE_DIR}/specs/d3d8.py
                ${CMAKE_SOURCE_DIR}/specs/d3d8types.py
                ${CMAKE_SOURCE_DIR}/specs/d3d8caps.py
                ${CMAKE_SOURCE_DIR}/specs/winapi.py
                ${CMAKE_SOURCE_DIR}/specs/stdapi.py
        )
        add_library (d3d8trace MODULE d3d8.def d3d8trace.cpp d3d9shader.cpp)
        target_link_libraries (d3d8trace
            common
            ${ZLIB_LIBRARIES}
            ${SNAPPY_LIBRARIES}
        )
        set_target_properties (d3d8trace PROPERTIES
            PREFIX ""
            OUTPUT_NAME d3d8
        )
        install (TARGETS d3d8trace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
    endif ()

    # d3d9.dll
    if (DirectX_D3DX9_INCLUDE_DIR)
        include_directories (SYSTEM ${DirectX_D3DX9_INCLUDE_DIR})
        add_custom_command (
            OUTPUT d3d9trace.cpp
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3d9trace.py > ${CMAKE_CURRENT_BINARY_DIR}/d3d9trace.cpp
            DEPENDS
                d3d9trace.py
                dlltrace.py
                trace.py
                ${CMAKE_SOURCE_DIR}/dispatch/dispatch.py
                ${CMAKE_SOURCE_DIR}/specs/d3d9.py
                ${CMAKE_SOURCE_DIR}/specs/d3d9types.py
                ${CMAKE_SOURCE_DIR}/specs/d3d9caps.py
                ${CMAKE_SOURCE_DIR}/specs/winapi.py
                ${CMAKE_SOURCE_DIR}/specs/stdapi.py
        )
        add_library (d3d9trace MODULE d3d9.def d3d9trace.cpp d3d9shader.cpp)
        target_link_libraries (d3d9trace
            common
            ${ZLIB_LIBRARIES}
            ${SNAPPY_LIBRARIES}
        )
        set_target_properties (d3d9trace PROPERTIES
            PREFIX ""
            OUTPUT_NAME d3d9
        )
        install (TARGETS d3d9trace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
    endif ()

    # d3d10.dll
    if (DirectX_D3D10_INCLUDE_DIR)
        include_directories (SYSTEM ${DirectX_D3D10_INCLUDE_DIR})
        add_custom_command (
            OUTPUT d3d10trace.cpp
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3d10trace.py > ${CMAKE_CURRENT_BINARY_DIR}/d3d10trace.cpp
            DEPENDS
                d3d10trace.py
                dlltrace.py
                trace.py
                ${CMAKE_SOURCE_DIR}/dispatch/dispatch.py
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
        add_library (d3d10trace MODULE d3d10.def d3d10trace.cpp d3d10shader.cpp)
        target_link_libraries (d3d10trace
            common
            ${ZLIB_LIBRARIES}
            ${SNAPPY_LIBRARIES}
        )
        set_target_properties (d3d10trace PROPERTIES
            PREFIX ""
            OUTPUT_NAME d3d10
        )
        install (TARGETS d3d10trace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
    endif ()

    # d3d10_1.dll
    if (DirectX_D3D10_1_INCLUDE_DIR)
        include_directories (SYSTEM ${DirectX_D3D10_1_INCLUDE_DIR})
        add_custom_command (
            OUTPUT d3d10_1trace.cpp
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3d10_1trace.py > ${CMAKE_CURRENT_BINARY_DIR}/d3d10_1trace.cpp
            DEPENDS
                d3d10_1trace.py
                dlltrace.py
                trace.py
                ${CMAKE_SOURCE_DIR}/dispatch/dispatch.py
                ${CMAKE_SOURCE_DIR}/specs/d3d10_1.py
                ${CMAKE_SOURCE_DIR}/specs/d3d10.py
                ${CMAKE_SOURCE_DIR}/specs/d3d10sdklayers.py
                ${CMAKE_SOURCE_DIR}/specs/d3dcommon.py
                ${CMAKE_SOURCE_DIR}/specs/dxgi.py
                ${CMAKE_SOURCE_DIR}/specs/dxgitype.py
                ${CMAKE_SOURCE_DIR}/specs/dxgiformat.py
                ${CMAKE_SOURCE_DIR}/specs/winapi.py
                ${CMAKE_SOURCE_DIR}/specs/stdapi.py
        )
        add_library (d3d10_1trace MODULE d3d10_1.def d3d10_1trace.cpp)
        target_link_libraries (d3d10_1trace
            common
            ${ZLIB_LIBRARIES}
            ${SNAPPY_LIBRARIES}
        )
        set_target_properties (d3d10_1trace
            PROPERTIES PREFIX ""
            OUTPUT_NAME d3d10_1
        )
        install (TARGETS d3d10_1trace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
    endif ()

    # d3d11.dll
    if (DirectX_D3D11_INCLUDE_DIR)
        if (DirectX_D3D11_1_INCLUDE_DIR)
            set (HAVE_D3D11_1 1)
        else ()
            set (HAVE_D3D11_1 0)
        endif ()

        include_directories (SYSTEM ${DirectX_D3D11_INCLUDE_DIR})
        add_custom_command (
            OUTPUT d3d11trace.cpp
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3d11trace.py ${HAVE_D3D11_1} > ${CMAKE_CURRENT_BINARY_DIR}/d3d11trace.cpp
            DEPENDS
                d3d11trace.py
                dlltrace.py
                trace.py
                ${CMAKE_SOURCE_DIR}/specs/d3d11_1.py
                ${CMAKE_SOURCE_DIR}/specs/d3d11.py
                ${CMAKE_SOURCE_DIR}/specs/d3d11sdklayers.py
                ${CMAKE_SOURCE_DIR}/specs/d3dcommon.py
                ${CMAKE_SOURCE_DIR}/specs/dxgi.py
                ${CMAKE_SOURCE_DIR}/specs/dxgitype.py
                ${CMAKE_SOURCE_DIR}/specs/dxgiformat.py
                ${CMAKE_SOURCE_DIR}/specs/winapi.py
                ${CMAKE_SOURCE_DIR}/specs/stdapi.py
        )
        add_library (d3d11trace MODULE d3d11.def d3d11trace.cpp)
        target_link_libraries (d3d11trace
            common
            ${ZLIB_LIBRARIES}
            ${SNAPPY_LIBRARIES}
        )
        set_target_properties (d3d11trace
            PROPERTIES PREFIX ""
            OUTPUT_NAME d3d11
        )
        install (TARGETS d3d11trace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
    endif ()

    # opengl32.dll
    add_custom_command (
        OUTPUT wgltrace.cpp
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/wgltrace.py > ${CMAKE_CURRENT_BINARY_DIR}/wgltrace.cpp
        DEPENDS
                wgltrace.py
                gltrace.py
                trace.py
                ${CMAKE_SOURCE_DIR}/dispatch/dispatch.py
                ${CMAKE_SOURCE_DIR}/specs/wglapi.py
                ${CMAKE_SOURCE_DIR}/specs/wglenum.py
                ${CMAKE_SOURCE_DIR}/specs/glapi.py
                ${CMAKE_SOURCE_DIR}/specs/glparams.py
                ${CMAKE_SOURCE_DIR}/specs/gltypes.py
                ${CMAKE_SOURCE_DIR}/specs/winapi.py
                ${CMAKE_SOURCE_DIR}/specs/stdapi.py
    )
    add_library (wgltrace MODULE opengl32.def
        wgltrace.cpp
        glcaps.cpp
        gltrace_state.cpp
    )
    add_dependencies (wgltrace glproc)
    target_link_libraries (wgltrace
        glproc_gl
        common
        ${ZLIB_LIBRARIES}
        ${SNAPPY_LIBRARIES}
    )
    set_target_properties (wgltrace PROPERTIES
        PREFIX ""
        OUTPUT_NAME opengl32
    )
    install (TARGETS wgltrace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})

elseif (APPLE)
    # OpenGL framework
    add_custom_command (
        OUTPUT cgltrace.cpp
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/cgltrace.py > ${CMAKE_CURRENT_BINARY_DIR}/cgltrace.cpp
        DEPENDS
            cgltrace.py
            gltrace.py
            trace.py
            ${CMAKE_SOURCE_DIR}/specs/cglapi.py
            ${CMAKE_SOURCE_DIR}/specs/glapi.py
            ${CMAKE_SOURCE_DIR}/specs/glparams.py
            ${CMAKE_SOURCE_DIR}/specs/gltypes.py
            ${CMAKE_SOURCE_DIR}/specs/stdapi.py
    )

    add_library (cgltrace SHARED
        cgltrace.cpp
        glcaps.cpp
        gltrace_state.cpp
    )

    add_dependencies (cgltrace glproc)

    set_target_properties (cgltrace PROPERTIES
        # OpenGL framework name
        PREFIX "" OUTPUT_NAME "OpenGL" SUFFIX ""
        # Specificy the version and reexport GLU symbols
        LINK_FLAGS "-compatibility_version 1 -current_version 1.0.0 -Wl,-reexport_library,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGLU.dylib"
    )

    target_link_libraries (cgltrace
        glproc_gl
        common
        ${ZLIB_LIBRARIES}
        ${SNAPPY_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        dl
    )

    install (TARGETS cgltrace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
elseif (X11_FOUND)
    # libGL.so
    add_custom_command (
        OUTPUT glxtrace.cpp
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/glxtrace.py > ${CMAKE_CURRENT_BINARY_DIR}/glxtrace.cpp
        DEPENDS
            glxtrace.py
            gltrace.py
            trace.py
            ${CMAKE_SOURCE_DIR}/specs/glxapi.py
            ${CMAKE_SOURCE_DIR}/specs/glapi.py
            ${CMAKE_SOURCE_DIR}/specs/glparams.py
            ${CMAKE_SOURCE_DIR}/specs/gltypes.py
            ${CMAKE_SOURCE_DIR}/specs/stdapi.py
    )

    add_library (glxtrace SHARED
        glxtrace.cpp
        glcaps.cpp
        gltrace_state.cpp
    )

    add_dependencies (glxtrace glproc)

    set_target_properties (glxtrace PROPERTIES
        # avoid the default "lib" prefix
        PREFIX ""
        # Prevent symbol relocations internal to our wrapper library to be
        # overwritten by the application.
        LINK_FLAGS "-Wl,-Bsymbolic -Wl,-Bsymbolic-functions"
    )

    target_link_libraries (glxtrace
        glproc_gl
        common
        ${ZLIB_LIBRARIES}
        ${SNAPPY_LIBRARIES}
        ${X11_X11_LIB}
        ${CMAKE_THREAD_LIBS_INIT}
        dl
    )

    install (TARGETS glxtrace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
endif ()


if (ENABLE_EGL AND NOT WIN32 AND NOT APPLE)
    # libEGL.so/libGL.so
    add_custom_command (
        OUTPUT egltrace.cpp
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/egltrace.py > ${CMAKE_CURRENT_BINARY_DIR}/egltrace.cpp
        DEPENDS
            egltrace.py
            gltrace.py
            trace.py
            ${CMAKE_SOURCE_DIR}/specs/eglapi.py
            ${CMAKE_SOURCE_DIR}/specs/glesapi.py
            ${CMAKE_SOURCE_DIR}/specs/glapi.py
            ${CMAKE_SOURCE_DIR}/specs/glparams.py
            ${CMAKE_SOURCE_DIR}/specs/gltypes.py
            ${CMAKE_SOURCE_DIR}/specs/stdapi.py
    )

    add_library (egltrace SHARED
        egltrace.cpp
        glcaps.cpp
        gltrace_state.cpp
    )

    add_dependencies (egltrace glproc)

    set_target_properties (egltrace PROPERTIES
        # avoid the default "lib" prefix
        PREFIX ""
        # Prevent symbol relocations internal to our wrapper library to be
        # overwritten by the application.
        LINK_FLAGS "-Wl,-Bsymbolic -Wl,-Bsymbolic-functions"
    )

    target_link_libraries (egltrace
        glproc_egl
        common
        ${ZLIB_LIBRARIES}
        ${SNAPPY_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        dl
    )

    install (TARGETS egltrace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
endif ()
