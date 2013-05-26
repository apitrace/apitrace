include (CheckIncludeFileCXX)

if (DirectX_D3D11_INCLUDE_DIR)

    include_directories (BEFORE
        ${DirectX_D3D11_INCLUDE_DIR}
    )

    CHECK_INCLUDE_FILE_CXX (directxmath.h HAVE_DIRECTXMATH)
    if (NOT HAVE_DIRECTXMATH)
        include_directories (BEFORE
            ${CMAKE_CURRENT_SOURCE_DIR}/XNAMath
        )
        add_definitions (-DUSE_XNAMATH)
    endif ()

    if (MINGW)
        include_directories (BEFORE
            ${CMAKE_CURRENT_SOURCE_DIR}/mingw
            ${CMAKE_SOURCE_DIR}/dispatch # for compat.h
        )
        add_definitions (
            -std=c++0x # static_assert, nullptr unique_ptr
            -w #inhibit warnings

            # workaround http://sourceforge.net/tracker/index.php?func=detail&aid=3067012&group_id=202880&atid=983354
            --include malloc.h

            -D_XM_NO_INTRINSICS_
            -DXMINLINE=inline
            -DXM_NO_ALIGNMENT
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

endif ()
