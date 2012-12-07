include (CheckIncludeFileCXX)

if (MSVC AND DirectX_D3D11_INCLUDE_DIR)

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

    add_library (directxtex STATIC
        DirectXTex/BC4BC5.cpp
        DirectXTex/BC6HBC7.cpp
        DirectXTex/BC.cpp
        DirectXTex/DirectXTexCompress.cpp
        DirectXTex/DirectXTexConvert.cpp
        DirectXTex/DirectXTexD3D11.cpp
        DirectXTex/DirectXTexDDS.cpp
        DirectXTex/DirectXTexFlipRotate.cpp
        DirectXTex/DirectXTexImage.cpp
        DirectXTex/DirectXTexMipmaps.cpp
        DirectXTex/DirectXTexMisc.cpp
        DirectXTex/DirectXTexNormalMaps.cpp
        DirectXTex/DirectXTexResize.cpp
        DirectXTex/DirectXTexTGA.cpp
        DirectXTex/DirectXTexUtil.cpp
        DirectXTex/DirectXTexWIC.cpp
    )

endif ()
