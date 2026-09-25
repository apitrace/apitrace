include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/../d3dcommon
)

link_libraries (
    ddraw
)

set (api d3d7)

set (targets
    tri
    ddraw
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

