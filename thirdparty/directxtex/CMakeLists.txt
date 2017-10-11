include (CheckIncludeFileCXX)

if (DirectX_D3D11_4_INCLUDE_FOUND)

    include_directories (BEFORE
        ${DirectX_D3D11_INCLUDE_DIR}
    )

    check_include_file_cxx (directxmath.h HAVE_DIRECTXMATH)
    if (NOT HAVE_DIRECTXMATH)
        include_directories (BEFORE
            ${CMAKE_CURRENT_SOURCE_DIR}/XNAMath
        )
        add_definitions (-DUSE_XNAMATH)
    endif ()

    if (MINGW)
        # Several MinGW headers are lacking.  Use our own.
        include_directories (BEFORE
            ${CMAKE_SOURCE_DIR}/thirdparty/mingw/wincodec
            ${CMAKE_SOURCE_DIR}/thirdparty/mingw/wrl
        )

        remove_definitions (-D_WIN32_WINNT=0x0602 -DWINVER=0x0602)
        add_definitions (
            #inhibit warnings
            -w

            # Force include this header with tweaks for mingw
            --include ${CMAKE_CURRENT_SOURCE_DIR}/mingw/tweaks.h

            -D_XM_NO_INTRINSICS_
            -DXMINLINE=inline
            -DXM_NO_ALIGNMENT

            -D_WIN32_WINNT=0x0501 -DWINVER=0x0501
        )
        if (CMAKE_SIZEOF_VOID_P EQUAL 8)
            add_definitions (-D_XM_X64_)
        else ()
            add_definitions (-D_XM_X86_)
        endif ()
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

    install (
        FILES MIT.txt
        DESTINATION ${DOC_INSTALL_DIR}
        RENAME LICENSE-directxtex.txt
    )
endif ()
