##############################################################################
# API retracers

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/helpers
    ${CMAKE_BINARY_DIR}/dispatch
    ${CMAKE_SOURCE_DIR}/dispatch
    ${CMAKE_SOURCE_DIR}/image
    ${CMAKE_SOURCE_DIR}/thirdparty/dxerr
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

if (WIN32)
    set (ws_os ws_win32.cpp)
endif ()

add_library (retrace_common STATIC
    retrace.cpp
    retrace_main.cpp
    retrace_stdc.cpp
    retrace_swizzle.cpp
    json.cpp
    state_writer.cpp
    state_writer_json.cpp
    state_writer_ubjson.cpp
    ws.cpp
    ${ws_os}
)
target_link_libraries (retrace_common
    image
    common
    ${ZLIB_LIBRARIES}
    ${SNAPPY_LIBRARIES}
    ${GETOPT_LIBRARIES}
)
if (NOT ANDROID AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries (retrace_common rt)
endif ()
if (WIN32)
    target_link_libraries (retrace_common dxerr winmm)
endif ()


add_library (glretrace_common STATIC
    glretrace_gl.cpp
    glretrace_cgl.cpp
    glretrace_glx.cpp
    glretrace_wgl.cpp
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
add_dependencies (glretrace_common glproc)
target_link_libraries (glretrace_common
    retrace_common
)
if (PROCPS_FOUND)
    target_link_libraries (glretrace_common ${PROCPS_LIBRARY})
endif ()


if (WIN32)
    set (glws_os glws_wgl.cpp)
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
    )

    add_dependencies (glretrace glproc)

    target_link_libraries (glretrace
        retrace_common
        glretrace_common
        glhelpers
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
            ${CMAKE_DL_LIBS}
        )
    endif ()

    install (TARGETS glretrace RUNTIME DESTINATION bin) 
    install_pdb (glretrace DESTINATION bin)
endif ()

if (ENABLE_EGL AND X11_FOUND AND NOT WIN32 AND NOT APPLE AND NOT ENABLE_WAFFLE)
    add_executable (eglretrace
        glws_xlib.cpp
        glws_egl_xlib.cpp
    )

    add_dependencies (eglretrace glproc)

    target_link_libraries (eglretrace
        retrace_common
        glretrace_common
        glhelpers
        glproc_egl
        ${X11_X11_LIB}
        ${CMAKE_THREAD_LIBS_INIT}
        ${CMAKE_DL_LIBS}
    )
    install (TARGETS eglretrace RUNTIME DESTINATION bin) 
endif ()

if (ENABLE_EGL)
    if ((ANDROID OR ENABLE_WAFFLE) AND Waffle_FOUND)
        add_executable (eglretrace
            glws_waffle.cpp
        )

        add_dependencies (eglretrace glproc)
        include_directories (${Waffle_INCLUDE_DIR})

        target_link_libraries (eglretrace
            retrace_common
            glretrace_common
            glhelpers
            glproc_egl
            ${Waffle_LIBRARY}
            ${X11_X11_LIB}
            ${CMAKE_THREAD_LIBS_INIT}
            ${CMAKE_DL_LIBS}
        )
        install (TARGETS eglretrace RUNTIME DESTINATION bin)
    elseif (ANDROID AND (DEFINED ANDROID_SDK))
        add_library(retrace SHARED glws_egl_android.cpp)

        target_link_libraries (retrace
            retrace_common
            glretrace_common
            glhelpers
            glproc_egl
            android
        )
        set (ANDROID_BUILD_DIR "${CMAKE_BINARY_DIR}/android-build")

        configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/android/local.properties.in" "${CMAKE_BINARY_DIR}/local.properties")

        add_custom_target (copyAndroidStuff
                COMMAND ${CMAKE_COMMAND} -E remove_directory "${ANDROID_BUILD_DIR}"
                COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/android" "${ANDROID_BUILD_DIR}"
                COMMAND ${CMAKE_COMMAND} -E make_directory "${ANDROID_BUILD_DIR}/libs/${ANDROID_NDK_ABI_NAME}"

                # FIXME According to android.toolchain.cmake decumentation the
                # android libs should be putted to
                # ${LIBRARY_OUTPUT_PATH_ROOT}/libs/${ANDROID_NDK_ABI_NAME} but
                # android.toolchain.cmake sees LIBRARY_OUTPUT_PATH_ROOT as
                # CMAKE_SOURCE_DIR and this CMakeLists.txt file sees it as
                # ANDROID_BUILD_DIR ;-( so, I must use
                # ${CMAKE_SOURCE_DIR}/libs/${ANDROID_NDK_ABI_NAME} insted of
                # ${LIBRARY_OUTPUT_PATH_ROOT}/libs/${ANDROID_NDK_ABI_NAME} ...

                COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/libs/${ANDROID_NDK_ABI_NAME}/libgnustl_shared.so" "${ANDROID_BUILD_DIR}/libs/${ANDROID_NDK_ABI_NAME}/"
                COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/libs/${ANDROID_NDK_ABI_NAME}/libretrace.so" "${ANDROID_BUILD_DIR}/libs/${ANDROID_NDK_ABI_NAME}/"
                DEPENDS retrace
        )

        add_custom_target (retraceAPK
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_BINARY_DIR}/local.properties" "${ANDROID_BUILD_DIR}/local.properties"
                COMMAND "${CMAKE_SOURCE_DIR}/thirdparty/gradlew/gradlew" "assembleDebug"
                DEPENDS copyAndroidStuff
                DEPENDS "${CMAKE_BINARY_DIR}/local.properties"
                WORKING_DIRECTORY "${ANDROID_BUILD_DIR}")

        add_custom_target (installRetraceAPK
                COMMAND "${CMAKE_SOURCE_DIR}/thirdparty/gradlew/gradlew" "installDebug"
                DEPENDS retraceAPK
                WORKING_DIRECTORY "${ANDROID_BUILD_DIR}")
    endif ()
endif ()

if (WIN32)
    if (DirectX_D3D_INCLUDE_FOUND)
        include_directories (BEFORE SYSTEM ${DirectX_D3D_INCLUDE_DIR})
        add_custom_command (
            OUTPUT d3dretrace_ddraw.cpp
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/ddrawretrace.py > ${CMAKE_CURRENT_BINARY_DIR}/d3dretrace_ddraw.cpp
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
        include_directories (BEFORE SYSTEM ${DirectX_D3D8_INCLUDE_DIR})
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
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3d9retrace.py d3d8 ${HAVE_D3D8} > ${CMAKE_CURRENT_BINARY_DIR}/d3dretrace_d3d8.cpp
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
        include_directories (BEFORE SYSTEM ${DirectX_D3D9_INCLUDE_DIR})
        set (HAVE_D3D9 1)
    else ()
        set (HAVE_D3D9 0)
    endif ()
    add_custom_command (
        OUTPUT d3dretrace_d3d9.cpp
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/d3d9retrace.py d3d9 ${HAVE_D3D9} > ${CMAKE_CURRENT_BINARY_DIR}/d3dretrace_d3d9.cpp
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

    if (DirectX_D3D11_3_INCLUDE_FOUND)
        add_definitions (-DHAVE_DXGI)
        include_directories (BEFORE SYSTEM
            ${DirectX_D3D11_3_INCLUDE_DIR}
            ${CMAKE_SOURCE_DIR}/thirdparty/directxtex/DirectXTex
        )
        set (D3DSTATE_SOURCES ${D3DSTATE_SOURCES}
            dxgistate.cpp
            dxgistate_so.cpp
            d3d10state.cpp
            d3d10state_images.cpp
            d3d11state.cpp
            d3d11state_images.cpp
            d3dretrace_dxgi.cpp
            d3dretrace_dxgi_misc.cpp
        )
        add_custom_command (
            OUTPUT dxgistate_so.cpp
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/dxgistate_so.py > ${CMAKE_CURRENT_BINARY_DIR}/dxgistate_so.cpp
            DEPENDS
                ${CMAKE_CURRENT_SOURCE_DIR}/dxgistate_so.py
                ${CMAKE_SOURCE_DIR}/specs/d3d11.py
                ${CMAKE_SOURCE_DIR}/specs/d3d11sdklayers.py
                ${CMAKE_SOURCE_DIR}/specs/d3d10.py
                ${CMAKE_SOURCE_DIR}/specs/d3d10sdklayers.py
                ${CMAKE_SOURCE_DIR}/specs/dxgi.py
                ${CMAKE_SOURCE_DIR}/specs/winapi.py
                ${CMAKE_SOURCE_DIR}/specs/stdapi.py
        )
        add_custom_command (
            OUTPUT d3dretrace_dxgi.cpp
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/dxgiretrace.py > ${CMAKE_CURRENT_BINARY_DIR}/d3dretrace_dxgi.cpp
            DEPENDS
                dxgiretrace.py
                dllretrace.py
                retrace.py
                ${CMAKE_SOURCE_DIR}/dispatch/dispatch.py
                ${CMAKE_SOURCE_DIR}/specs/d3d11.py
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
        d3d9state.cpp
        d3d9state_formats.cpp
        d3d9state_images.cpp
        ${D3DSTATE_SOURCES}
    )
    target_link_libraries (d3dretrace
        retrace_common
        d3dhelpers
    )
    if (DirectX_D3D11_3_INCLUDE_FOUND)
        target_link_libraries (d3dretrace directxtex)
    endif ()

    install (TARGETS d3dretrace RUNTIME DESTINATION bin)
    install_pdb (d3dretrace DESTINATION bin)
endif ()
