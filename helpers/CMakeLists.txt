##############################################################################
# API helpers


include_directories (
    ${CMAKE_BINARY_DIR}/dispatch
    ${CMAKE_SOURCE_DIR}/dispatch
)

add_convenience_library (glhelpers
    glfeatures.cpp
    glpack.cpp
    eglsize.cpp
)
add_dependencies (glhelpers glproc)

if (BUILD_TESTING)
    add_gtest (glsize_test glsize_test.cpp)
    add_dependencies (glsize_test glproc)
    target_link_libraries (glsize_test os)
endif ()


if (WIN32)
    add_convenience_library (d3dhelpers
        d3dshader.cpp
    )
endif ()
