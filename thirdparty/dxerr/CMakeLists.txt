add_definitions (-DHAVE_DDRAW)
add_definitions (-DHAVE_D3D9)

if (DirectX_D3D11_4_INCLUDE_FOUND)
    include_directories (BEFORE SYSTEM ${DirectX_D3D11_4_INCLUDE_DIR})
    add_definitions (-DHAVE_D3D10_1)
    add_definitions (-DHAVE_D3D11_1)
endif ()

add_library (dxerr STATIC
    dxerr.cpp
)

install (
    FILES license.txt
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-dxerr.txt
)
