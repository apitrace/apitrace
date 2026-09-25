add_definitions (-DGLFW_INCLUDE_GL)

include_directories (
    ${OPENGL_INCLUDE_PATH}
    ${GLFW_INCLUDE_DIR}
)

link_libraries (
    glad
    ${GLFW_LIBRARY}
    ${OPENGL_glu_LIBRARY}
    ${OPENGL_gl_LIBRARY}
)

set (api gl)

set (targets
    child
    config
    coherent_memory_write
    compiled_vertex_array
    coherent_memory_read
    debug
    default
    dlopen
    tri
    tri_glsl
    tri_glsl_core
    tri_glsl_es2
    gremedy
    varray
    map_buffer
    map_coherent
    msaa4
    program_binary
    exception
    window_resize
)

if (CMAKE_SYSTEM_NAME MATCHES Linux)
    set (targets ${targets} fork glvnd)
endif ()

if (WIN32)
    set (targets ${targets} wglfont)
endif ()

foreach (target ${targets})
    add_executable (${api}_${target} ${target}.cpp)
    target_link_libraries (${api}_${target} ${CMAKE_DL_LIBS})
    set_target_properties (${api}_${target} PROPERTIES OUTPUT_NAME ${target})
endforeach (target)


add_app_tests ()

if (WIN32 OR APPLE)
    set_tests_properties (app_gl_msaa4 PROPERTIES WILL_FAIL TRUE)
endif ()
