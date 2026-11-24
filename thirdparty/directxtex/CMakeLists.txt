include (CheckIncludeFileCXX)

if (DirectX_D3D11_4_INCLUDE_FOUND)

    if (MINGW)
        set (MINGW_GUID mingw/guid.cpp)
    endif ()

    add_library (directxtex STATIC
        DirectXTex/BC4BC5.cpp
        DirectXTex/BC6HBC7.cpp
        DirectXTex/BC.cpp
        DirectXTex/DirectXTexCompress.cpp
        DirectXTex/DirectXTexConvert.cpp
        #DirectXTex/DirectXTexD3D11.cpp
        #DirectXTex/DirectXTexDDS.cpp
        #DirectXTex/DirectXTexFlipRotate.cpp
        DirectXTex/DirectXTexImage.cpp
        DirectXTex/DirectXTexMipmaps.cpp
        DirectXTex/DirectXTexMisc.cpp
        #DirectXTex/DirectXTexNormalMaps.cpp
        #DirectXTex/DirectXTexResize.cpp
        DirectXTex/DirectXTexTGA.cpp
        DirectXTex/DirectXTexUtil.cpp
        DirectXTex/DirectXTexWIC.cpp
        ${MINGW_GUID}
    )

    target_link_libraries (directxtex PRIVATE compat)

    target_include_directories (directxtex SYSTEM BEFORE PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/DirectXTex
    )
    target_include_directories (directxtex BEFORE PRIVATE
        ${DirectX_D3D11_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/thirdparty/directxmath/Inc
    )

    if (MINGW)
        # Several MinGW headers are lacking.  Use our own.
        target_include_directories (directxtex BEFORE PRIVATE
            ${PROJECT_SOURCE_DIR}/thirdparty/mingw/wincodec
        )

        # The WIN32_WINNT/WINVER set for the whole project must be dropped, as
        # target_compile_definitions can only add definitions, not remove
        # them, and the code below conflicts with it.
        remove_definitions (-D_WIN32_WINNT=0x0602 -DWINVER=0x0602)
        target_compile_options (directxtex PRIVATE
            # inhibit warnings
            -w

            # Force include this header with tweaks for mingw
            "SHELL:--include ${CMAKE_CURRENT_SOURCE_DIR}/mingw/tweaks.h"
        )
        target_compile_definitions (directxtex PRIVATE
            _XM_NO_INTRINSICS_
            _XM_NO_XMVECTOR_OVERLOADS_
            XMINLINE=inline
            XM_NO_ALIGNMENT

            _WIN32_WINNT=0x0501
            WINVER=0x0501
        )
        if (CMAKE_SIZEOF_VOID_P EQUAL 8)
            target_compile_definitions (directxtex PRIVATE _XM_X64_)
        else ()
            target_compile_definitions (directxtex PRIVATE _XM_X86_)
        endif ()
    else ()
        # MSVC uses intrinsics by default, but it seems to expose bugs for some
        # formats.
        target_compile_definitions (directxtex PRIVATE _XM_NO_INTRINSICS_)
    endif ()

    set_target_properties (directxtex PROPERTIES C_STANDARD 99 CXX_STANDARD 11)

    install (
        FILES MIT.txt
        DESTINATION ${DOC_INSTALL_DIR}
        RENAME LICENSE-directxtex.txt
    )
endif ()
