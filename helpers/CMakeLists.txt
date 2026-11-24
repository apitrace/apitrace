##############################################################################
# API helpers


add_convenience_library (glhelpers
    glfeatures.cpp
    glpack.cpp
    eglsize.cpp
)
target_include_directories (glhelpers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (glhelpers PUBLIC glproc os)

if (BUILD_TESTING)
    add_gtest (glsize_test glsize_test.cpp)
    target_include_directories (glsize_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries (glsize_test PRIVATE os glproc)
endif ()


if (WIN32)
    add_convenience_library (d3dhelpers
        d3dshader.cpp
    )
    target_include_directories (d3dhelpers PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/dispatch
    )
    target_link_libraries (d3dhelpers PUBLIC os)
endif ()
