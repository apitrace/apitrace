include_directories(
    ${GLFW_INCLUDE_DIR}
    ${EGL_INCLUDE_DIRS}
)

set (api egl_gles2)

set (targets
    debug_marker
    eglimage
    tri_glsl
    map_buffer
    msaa_fbo
    texture_readback
)

foreach (target ${targets})
    add_executable (${api}_${target} ${target}.cpp)
    set_target_properties (${api}_${target} PROPERTIES OUTPUT_NAME ${target})
    target_link_libraries (${api}_${target}
        glad
        ${GLFW_LIBRARY}
    )
endforeach (target)

# eglimage calls eglQueryString directly
target_link_libraries (${api}_eglimage ${EGL_LIBRARIES})

add_library (rtld_local_module MODULE rtld_local_module.cpp)
target_link_libraries (rtld_local_module ${GLFW_LIBRARY} ${GLESV2_LIBRARIES} ${EGL_LIBRARIES})
set_target_properties (rtld_local_module PROPERTIES OUTPUT_NAME rtld_local)
add_executable (${api}_rtld_local rtld_local.cpp)
set_target_properties (${api}_rtld_local PROPERTIES OUTPUT_NAME rtld_local)
target_link_libraries (${api}_rtld_local ${CMAKE_DL_LIBS})

add_app_tests ()

get_property (app_tests DIRECTORY PROPERTY TESTS)
foreach (test ${app_tests})
    if (test MATCHES "texture_readback")
        set_tests_properties (${test} PROPERTIES TIMEOUT 60)
    endif ()
endforeach ()

if (TARGET eglretrace AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    foreach (layer 0 1 -1)
        add_test (NAME texture_readback_fault_${layer}
            COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/texture_fault.py
                --apitrace ${APITRACE_EXECUTABLE}
                --retrace $<TARGET_FILE:eglretrace>
                --app $<TARGET_FILE:${api}_texture_readback>
                --layer ${layer})
        set_tests_properties (texture_readback_fault_${layer} PROPERTIES
            SKIP_RETURN_CODE 125 TIMEOUT 60)
    endforeach ()
endif ()
