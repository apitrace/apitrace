if (DirectX_D3D11_1_INCLUDE_FOUND)

    include_directories (BEFORE SYSTEM
        ${DirectX_D3D11_1_INCLUDE_DIR}
    )

    add_library (dxerr STATIC
        dxerr.cpp
    )

    install (
        FILES license.txt
        DESTINATION ${DOC_INSTALL_DIR}
        RENAME LICENSE-dxerr.txt
    )
endif ()
