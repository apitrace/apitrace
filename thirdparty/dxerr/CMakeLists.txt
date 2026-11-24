add_library (dxerr STATIC
    dxerr.cpp
)

target_include_directories (dxerr PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (dxerr PRIVATE compat)

target_compile_definitions (dxerr PRIVATE
    HAVE_DDRAW
    HAVE_D3D9
)

if (DirectX_D3D11_4_INCLUDE_FOUND)
    target_include_directories (dxerr SYSTEM BEFORE PRIVATE ${DirectX_D3D11_4_INCLUDE_DIR})
    target_compile_definitions (dxerr PRIVATE
        HAVE_D3D10_1
        HAVE_D3D11_1
    )
endif ()

install (
    FILES license.txt
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-dxerr.txt
)
