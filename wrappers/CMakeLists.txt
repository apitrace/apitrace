##############################################################################
# API tracers


if (ENABLE_STATIC_LIBGCC)
    add_link_options (-static-libgcc)
endif ()
if (ENABLE_STATIC_LIBSTDCXX)
    # Unfortunately for a long time static libstdcxx didn't built with -fPIC
    # https://gcc.gnu.org/bugzilla/show_bug.cgi?id=28811
    include (CheckCXXSourceCompiles)
    set (CMAKE_REQUIRED_FLAGS ${CMAKE_SHARED_LIBRARY_C_FLAGS})
    set (CMAKE_REQUIRED_LIBRARIES -shared -static-libgcc -static-libstdc++)
    check_cxx_source_compiles (
        "
            #include <string>
            std::string s;
        "
        STATIC_LIBSTDCXX_PIC
    )
    unset (CMAKE_REQUIRED_LIBRARIES)
    unset (CMAKE_REQUIRED_FLAGS)
    if (STATIC_LIBSTDCXX_PIC)
        add_link_options (-static-libstdc++)
    endif ()
endif ()

function (target_linker_version_script target version_script)
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        # https://sourceware.org/binutils/docs/ld/VERSION.html
        target_link_options (${target} PRIVATE
            LINKER:--version-script=${version_script}
        )
        set_property (
            TARGET ${target}
            APPEND PROPERTY LINK_DEPENDS ${version_script}
        )
    endif ()
endfunction ()

if (NOT WIN32 AND NOT APPLE)
    # Prevent symbol relocations internal to our wrapper library to be
    # overwritten by the application.  And fail if there are missing
    # symbols.
    add_link_options (-Wl,-Bsymbolic -Wl,-Bsymbolic-functions)

    # Fail if there are missing symbols, except when using Address Sanitizer
    # since libasan symbols are supposed to be preloaded.
    if (NOT ENABLE_ASAN)
        add_link_options (-Wl,-z,defs)
    endif ()
endif ()

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Code shared across all APIs
add_convenience_library (trace
    memtrace.hpp
    memtrace.cpp
)
# The generated sources include headers from both the source and binary
# directories (what CMAKE_INCLUDE_CURRENT_DIR used to provide).
target_include_directories (trace PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/dispatch
)
target_link_libraries (trace PUBLIC
    common
    guids
    crc32c
)

# Code shared across all OpenGL variants
add_convenience_library (gltrace_common
    glcaps.cpp
    config.cpp
    gltrace_arrays.cpp
    gltrace_state.cpp
    glmemshadow.hpp
    glmemshadow.cpp
    gltrace_unpack_compressed.hpp
    gltrace_unpack_compressed.cpp
)
target_link_libraries (gltrace_common PUBLIC
    glhelpers
    trace
)

if (WIN32)
    if (MINGW AND CMAKE_SIZEOF_VOID_P EQUAL 4)
        # Silence warnings about @nn suffix mismatch
        add_link_options (-Wl,--enable-stdcall-fixup)
    endif ()

    # ddraw.dll
    if (DirectX_D3D_INCLUDE_FOUND)
        add_custom_command (
            OUTPUT ddrawtrace.cpp
            COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/ddrawtrace.py > ${CMAKE_CURRENT_BINARY_DIR}/ddrawtrace.cpp
            DEPENDS
                ddrawtrace.py
                dlltrace.py
                trace.py
                ${PROJECT_SOURCE_DIR}/dispatch/dispatch.py
                ${PROJECT_SOURCE_DIR}/specs/d3d.py
                ${PROJECT_SOURCE_DIR}/specs/d3dtypes.py
                ${PROJECT_SOURCE_DIR}/specs/d3dcaps.py
                ${PROJECT_SOURCE_DIR}/specs/ddraw.py
                ${PROJECT_SOURCE_DIR}/specs/winapi.py
                ${PROJECT_SOURCE_DIR}/specs/stdapi.py
        )
        add_library (ddrawtrace MODULE ddraw.def ddrawtrace.cpp)
        target_link_libraries (ddrawtrace PRIVATE
            d3dhelpers
            trace
        )
        target_include_directories (ddrawtrace SYSTEM BEFORE PRIVATE ${DirectX_D3D_INCLUDE_DIR})
        set_target_properties (ddrawtrace PROPERTIES
            PREFIX ""
            OUTPUT_NAME ddraw
        )
        install (TARGETS ddrawtrace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
        install_pdb (ddrawtrace DESTINATION ${WRAPPER_INSTALL_DIR})
    endif ()

    # d3d8.dll
    if (DirectX_D3D8_INCLUDE_FOUND AND DirectX_D3D9_INCLUDE_FOUND)
        add_custom_command (
            OUTPUT d3d8trace.cpp
            COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/d3d8trace.py > ${CMAKE_CURRENT_BINARY_DIR}/d3d8trace.cpp
            DEPENDS
                d3d8trace.py
                dlltrace.py
                trace.py
                ${PROJECT_SOURCE_DIR}/dispatch/dispatch.py
                ${PROJECT_SOURCE_DIR}/specs/d3d8.py
                ${PROJECT_SOURCE_DIR}/specs/d3d8types.py
                ${PROJECT_SOURCE_DIR}/specs/d3d8caps.py
                ${PROJECT_SOURCE_DIR}/specs/winapi.py
                ${PROJECT_SOURCE_DIR}/specs/stdapi.py
        )
        add_library (d3d8trace MODULE
            d3d8.def
            d3d8trace.cpp
            d3d9shader.cpp
        )
        target_link_libraries (d3d8trace PRIVATE
            d3dhelpers
            trace
        )
        target_include_directories (d3d8trace SYSTEM BEFORE PRIVATE ${DirectX_D3D9_INCLUDE_DIR} ${DirectX_D3D8_INCLUDE_DIR})
        set_target_properties (d3d8trace PROPERTIES
            PREFIX ""
            OUTPUT_NAME d3d8
        )
        install (TARGETS d3d8trace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
        install_pdb (d3d8trace DESTINATION ${WRAPPER_INSTALL_DIR})
    endif ()

    # d3d9.dll
    if (DirectX_D3D9_INCLUDE_FOUND)
        add_custom_command (
            OUTPUT d3d9trace.cpp
            COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/d3d9trace.py > ${CMAKE_CURRENT_BINARY_DIR}/d3d9trace.cpp
            DEPENDS
                d3d9trace.py
                dlltrace.py
                trace.py
                ${PROJECT_SOURCE_DIR}/dispatch/dispatch.py
                ${PROJECT_SOURCE_DIR}/specs/d3d9.py
                ${PROJECT_SOURCE_DIR}/specs/d3d9types.py
                ${PROJECT_SOURCE_DIR}/specs/d3d9caps.py
                ${PROJECT_SOURCE_DIR}/specs/dxva2.py
                ${PROJECT_SOURCE_DIR}/specs/winapi.py
                ${PROJECT_SOURCE_DIR}/specs/stdapi.py
        )
        add_library (d3d9trace MODULE
            d3d9.def
            d3d9trace.cpp
            d3d9shader.cpp
        )
        target_link_libraries (d3d9trace PRIVATE
            d3dhelpers
            trace
        )
        target_include_directories (d3d9trace SYSTEM BEFORE PRIVATE ${DirectX_D3D9_INCLUDE_DIR})
        set_target_properties (d3d9trace PROPERTIES
            PREFIX ""
            OUTPUT_NAME d3d9
        )
        install (TARGETS d3d9trace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
        install_pdb (d3d9trace DESTINATION ${WRAPPER_INSTALL_DIR})
    endif ()

    # dxgi.dll, d3d10.dll, d3d10_1.dll, d3d11.dll
    if (DirectX_D3D11_4_INCLUDE_FOUND)
        add_custom_command (
            OUTPUT dxgitrace.cpp
            COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/dxgitrace.py > ${CMAKE_CURRENT_BINARY_DIR}/dxgitrace.cpp
            DEPENDS
                dxgitrace.py
                dlltrace.py
                trace.py
                ${PROJECT_SOURCE_DIR}/dispatch/dispatch.py
                ${PROJECT_SOURCE_DIR}/specs/d3d12.py
                ${PROJECT_SOURCE_DIR}/specs/d3d12sdklayers.py
                ${PROJECT_SOURCE_DIR}/specs/d3d11.py
                ${PROJECT_SOURCE_DIR}/specs/dcomp.py
                ${PROJECT_SOURCE_DIR}/specs/d3d11sdklayers.py
                ${PROJECT_SOURCE_DIR}/specs/d3d10.py
                ${PROJECT_SOURCE_DIR}/specs/d3d10sdklayers.py
                ${PROJECT_SOURCE_DIR}/specs/dxgi.py
                ${PROJECT_SOURCE_DIR}/specs/d3d9.py
                ${PROJECT_SOURCE_DIR}/specs/winapi.py
                ${PROJECT_SOURCE_DIR}/specs/stdapi.py
        )
        add_library (dxgitrace SHARED
            dxgitrace.def
            dxgitrace.cpp
            d3dcommonshader.cpp
        )
        target_link_libraries (dxgitrace PRIVATE
            d3dhelpers
            trace
        )
        target_include_directories (dxgitrace SYSTEM BEFORE PRIVATE ${DirectX_D3D11_4_INCLUDE_DIR})
        if (HAVE_DETOURS)
            target_link_libraries (dxgitrace PRIVATE detours)
        endif ()
        set_target_properties (dxgitrace
            PROPERTIES PREFIX ""
        )
        install (TARGETS dxgitrace RUNTIME DESTINATION ${WRAPPER_INSTALL_DIR})
        install_pdb (dxgitrace DESTINATION ${WRAPPER_INSTALL_DIR})


        #
        # dxgi.dll, d3d10, and d3d11.dll are stubs that merely dispatch entry
        # points to dxgitrace.dll
        #

        # Whole archive linking is needed with Ubuntu 24.04's newer CMake and/or MinGW,
        # in order for the {dxgi,d3d10,d3d11}.dll's Xxx exports are resolved to
        # dxgitrace import library's Xxx@NN symbols.
        if (MINGW AND CMAKE_SIZEOF_VOID_P EQUAL 4 AND NOT CMAKE_VERSION VERSION_LESS "3.24")
            # Unfortunately $<LINK_LIBRARY:WHOLE_ARCHIVE,...> fails because
            # subsequent CMake versions ignore it with shared libraries
            set (DXGITRACE -Wl,--whole-archive dxgitrace -Wl,--no-whole-archive)
        else ()
            set (DXGITRACE dxgitrace)
        endif ()

        add_library (dxgistub MODULE
            dxgi.def
            stub.cpp
        )
        target_link_libraries (dxgistub PRIVATE
            ${DXGITRACE}
        )
        set_target_properties (dxgistub PROPERTIES
            PREFIX ""
            OUTPUT_NAME "dxgi"
        )
        install (TARGETS dxgistub LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})

        add_library (d3d10stub MODULE
            d3d10.def
            stub.cpp
        )
        target_link_libraries (d3d10stub PRIVATE
            ${DXGITRACE}
        )
        set_target_properties (d3d10stub PROPERTIES
            PREFIX ""
            OUTPUT_NAME "d3d10"
        )
        install (TARGETS d3d10stub LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})

        add_library (d3d10_1stub MODULE
            d3d10_1.def
            stub.cpp
        )
        target_link_libraries (d3d10_1stub PRIVATE
            ${DXGITRACE}
        )
        set_target_properties (d3d10_1stub PROPERTIES
            PREFIX ""
            OUTPUT_NAME "d3d10_1"
        )
        install (TARGETS d3d10_1stub LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})

        add_library (d3d11stub MODULE
            d3d11.def
            stub.cpp
        )
        target_link_libraries (d3d11stub PRIVATE
            ${DXGITRACE}
        )
        set_target_properties (d3d11stub PROPERTIES
            PREFIX ""
            OUTPUT_NAME "d3d11"
        )
        install (TARGETS d3d11stub LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})

        add_library (d3d12stub MODULE
            d3d12.def
            stub.cpp
        )
        target_link_libraries (d3d12stub PRIVATE
            ${DXGITRACE}
        )
        set_target_properties (d3d12stub PROPERTIES
            PREFIX ""
            OUTPUT_NAME "d3d12"
        )
        install (TARGETS d3d12stub LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
    endif ()

    # d2d1.dll, dwrite.dll
    if (DirectX_D2D1_1_INCLUDE_FOUND)
        add_custom_command (
            OUTPUT d2d1trace.cpp
            COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/d2d1trace.py > ${CMAKE_CURRENT_BINARY_DIR}/d2d1trace.cpp
            DEPENDS
                d2d1trace.py
                dlltrace.py
                trace.py
                ${PROJECT_SOURCE_DIR}/specs/d2d1.py
                ${PROJECT_SOURCE_DIR}/specs/dwrite.py
                ${PROJECT_SOURCE_DIR}/specs/dcommon.py
                ${PROJECT_SOURCE_DIR}/specs/dxgi.py
                ${PROJECT_SOURCE_DIR}/specs/winapi.py
                ${PROJECT_SOURCE_DIR}/specs/stdapi.py
        )
        add_library (d2d1trace MODULE d2d1trace.def d2d1trace.cpp)
        target_link_libraries (d2d1trace PRIVATE
            trace
        )
        target_include_directories (d2d1trace SYSTEM BEFORE PRIVATE ${DirectX_D2D1_1_INCLUDE_DIR})
        set_target_properties (d2d1trace
            PROPERTIES PREFIX ""
        )
        install (TARGETS d2d1trace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
        install_pdb (d2d1trace DESTINATION ${WRAPPER_INSTALL_DIR})
    endif ()

    # opengl32.dll
    add_custom_command (
        OUTPUT wgltrace.cpp
        COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/wgltrace.py > ${CMAKE_CURRENT_BINARY_DIR}/wgltrace.cpp
        DEPENDS
                wgltrace.py
                gltrace.py
                dlltrace.py
                trace.py
                ${PROJECT_SOURCE_DIR}/dispatch/dispatch.py
                ${PROJECT_SOURCE_DIR}/specs/wglapi.py
                ${PROJECT_SOURCE_DIR}/specs/wglenum.py
                ${PROJECT_SOURCE_DIR}/specs/glapi.py
                ${PROJECT_SOURCE_DIR}/specs/glparams.py
                ${PROJECT_SOURCE_DIR}/specs/gltypes.py
                ${PROJECT_SOURCE_DIR}/specs/winapi.py
                ${PROJECT_SOURCE_DIR}/specs/stdapi.py
    )
    add_library (wgltrace MODULE opengl32.def
        wgltrace.cpp
        glproc_gl.cpp
    )
    target_link_libraries (wgltrace PRIVATE
        gltrace_common
        glproc
        gdi32
    )
    set_target_properties (wgltrace PROPERTIES
        PREFIX ""
        OUTPUT_NAME opengl32
    )
    install (TARGETS wgltrace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
    install_pdb (wgltrace DESTINATION ${WRAPPER_INSTALL_DIR})

elseif (APPLE)
    # OpenGL framework
    add_custom_command (
        OUTPUT cgltrace.cpp
        COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/cgltrace.py > ${CMAKE_CURRENT_BINARY_DIR}/cgltrace.cpp
        DEPENDS
            cgltrace.py
            gltrace.py
            trace.py
            ${PROJECT_SOURCE_DIR}/dispatch/dispatch.py
            ${PROJECT_SOURCE_DIR}/specs/cglapi.py
            ${PROJECT_SOURCE_DIR}/specs/glapi.py
            ${PROJECT_SOURCE_DIR}/specs/glparams.py
            ${PROJECT_SOURCE_DIR}/specs/gltypes.py
            ${PROJECT_SOURCE_DIR}/specs/stdapi.py
    )

    add_library (cgltrace SHARED
        cgltrace.cpp
        glproc_gl.cpp
    )


    set_target_properties (cgltrace PROPERTIES
        # OpenGL framework name
        OUTPUT_NAME "OpenGL"
        # Mac framework
        FRAMEWORK true
    )

    # Specificy the version and reexport GLU symbols
    #
    # SHELL: keeps each option and its argument together, as otherwise
    # target_link_options would de-duplicate/reorder them independently.
    target_link_options (cgltrace PRIVATE
        "SHELL:-compatibility_version 1"
        "SHELL:-current_version 1.0.0"
        -Wl,-reexport_library,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGLU.dylib
    )

    target_link_libraries (cgltrace PRIVATE
        gltrace_common
        glproc
        ${CMAKE_THREAD_LIBS_INIT}
        ${CMAKE_DL_LIBS}
    )

    install (TARGETS cgltrace FRAMEWORK DESTINATION ${WRAPPER_INSTALL_DIR})
elseif (X11_FOUND)
    # libGL.so
    add_custom_command (
        OUTPUT glxtrace.cpp
        COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/glxtrace.py > ${CMAKE_CURRENT_BINARY_DIR}/glxtrace.cpp
        DEPENDS
            glxtrace.py
            gltrace.py
            trace.py
            ${PROJECT_SOURCE_DIR}/dispatch/dispatch.py
            ${PROJECT_SOURCE_DIR}/specs/glxapi.py
            ${PROJECT_SOURCE_DIR}/specs/glapi.py
            ${PROJECT_SOURCE_DIR}/specs/glparams.py
            ${PROJECT_SOURCE_DIR}/specs/gltypes.py
            ${PROJECT_SOURCE_DIR}/specs/stdapi.py
    )

    add_library (glxtrace SHARED
        glxtrace.cpp
        dlsym.cpp
        glproc_gl.cpp
    )


    set_target_properties (glxtrace PROPERTIES
        # avoid the default "lib" prefix
        PREFIX ""
    )

    target_link_libraries (glxtrace PRIVATE
        gltrace_common
        glproc
        ${CMAKE_THREAD_LIBS_INIT}
        ${CMAKE_DL_LIBS}
    )

    target_linker_version_script (glxtrace ${CMAKE_CURRENT_SOURCE_DIR}/glxtrace.version)

    install (TARGETS glxtrace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
endif ()


if (ENABLE_EGL AND NOT WIN32 AND NOT APPLE)
    # libEGL.so/libGL.so
    add_custom_command (
        OUTPUT egltrace.cpp
        COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/egltrace.py > ${CMAKE_CURRENT_BINARY_DIR}/egltrace.cpp
        DEPENDS
            egltrace.py
            gltrace.py
            trace.py
            ${PROJECT_SOURCE_DIR}/dispatch/dispatch.py
            ${PROJECT_SOURCE_DIR}/specs/eglapi.py
            ${PROJECT_SOURCE_DIR}/specs/eglenum.py
            ${PROJECT_SOURCE_DIR}/specs/glapi.py
            ${PROJECT_SOURCE_DIR}/specs/glparams.py
            ${PROJECT_SOURCE_DIR}/specs/gltypes.py
            ${PROJECT_SOURCE_DIR}/specs/stdapi.py
    )

    add_library (egltrace SHARED
        egltrace.cpp
        dlsym.cpp
        glproc_egl.cpp
    )


    set_target_properties (egltrace PROPERTIES
        # avoid the default "lib" prefix
        PREFIX ""
    )

    target_compile_definitions (egltrace PRIVATE EGLTRACE=1)

    target_link_libraries (egltrace PRIVATE
        gltrace_common
        glproc
        ${CMAKE_THREAD_LIBS_INIT}
        ${CMAKE_DL_LIBS}
    )

    target_linker_version_script (egltrace ${CMAKE_CURRENT_SOURCE_DIR}/egltrace.version)

    install (TARGETS egltrace LIBRARY DESTINATION ${WRAPPER_INSTALL_DIR})
endif ()
