include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/../d3dcommon
    ${DirectX_D3D8_INCLUDE_DIR}
)

link_libraries (
    ${DirectX_D3D8_LIBRARY}
)

set (api d3d8)

set (targets
    tri
    tri_pp
)

foreach (target ${targets})
    add_executable (${api}_${target} ${target}.cpp)
    set_target_properties (${api}_${target} PROPERTIES OUTPUT_NAME ${target})

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${target}.ref.txt)
        add_app_test(
            NAME ${api}_${target}
            TARGET ${api}_${target}
            REF ${target}.ref.txt
        )
    endif ()
endforeach (target)

