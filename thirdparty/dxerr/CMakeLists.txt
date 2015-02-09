macro (HaveDirectXInclude NAME)
    if (DirectX_${NAME}_INCLUDE_FOUND)
        include_directories (BEFORE SYSTEM
            ${DirectX_${NAME}_INCLUDE_DIR}
        )
        add_definitions (-DHAVE_${NAME})
    endif ()
endmacro ()

havedirectxinclude (DDRAW)
havedirectxinclude (D3D9)
havedirectxinclude (D3D10_1)
havedirectxinclude (D3D11_1)

add_library (dxerr STATIC
    dxerr.cpp
)

install (
    FILES license.txt
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-dxerr.txt
)
