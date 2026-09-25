include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/../d3dcommon
)

link_libraries (
    d3d9
)

set (api d3d9)

set (targets
    freelibrary
    fvf
    map_cubemap
    map_readonly
    subprocess
    tri
    tri_pp
    tex_sysmem
)

foreach (target ${targets})
    add_executable (${api}_${target} ${target}.cpp)
    set_target_properties (${api}_${target} PROPERTIES OUTPUT_NAME ${target})
endforeach (target)

add_app_tests ()

# XXX: Doesn't work yet
set_tests_properties (app_d3d9_subprocess_createprocessasuserw PROPERTIES WILL_FAIL TRUE)
