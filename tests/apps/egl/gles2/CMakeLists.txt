include_directories(
    ${GLFW_INCLUDE_DIR}
)

set (api egl_gles2)

set (targets
    debug_marker
    tri_glsl
    map_buffer
)

foreach (target ${targets})
    add_executable (${api}_${target} ${target}.cpp)
    set_target_properties (${api}_${target} PROPERTIES OUTPUT_NAME ${target})
    target_link_libraries (${api}_${target}
        glad
        ${GLFW_LIBRARY}
    )
endforeach (target)

add_library (rtld_local_module MODULE rtld_local_module.cpp)
target_link_libraries (rtld_local_module ${GLFW_LIBRARY} ${GLESV2_LIBRARIES} ${EGL_LIBRARIES})
set_target_properties (rtld_local_module PROPERTIES OUTPUT_NAME rtld_local)
add_executable (${api}_rtld_local rtld_local.cpp)
set_target_properties (${api}_rtld_local PROPERTIES OUTPUT_NAME rtld_local)
target_link_libraries (${api}_rtld_local ${CMAKE_DL_LIBS})

add_app_tests ()
