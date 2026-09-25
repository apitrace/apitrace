add_definitions (-DGLFW_INCLUDE_GL)

include_directories(
    ${GLFW_INCLUDE_DIR}
    ${EGL_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_PATH}
)

set (api egl_gl)

set (targets
    tri
    tri_glsl
    core
)

foreach (target ${targets})
    add_executable (${api}_${target} ${target}.c)
    set_target_properties (${api}_${target} PROPERTIES OUTPUT_NAME ${target})
    target_link_libraries (${api}_${target} glad ${GLFW_LIBRARY} ${OPENGL_gl_LIBRARY} ${EGL_LIBRARIES})

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${target}.ref.txt)
        add_app_test(
            NAME ${api}_${target}
            TARGET ${api}_${target}
            REF ${target}.ref.txt
        )
    endif ()
endforeach (target)
