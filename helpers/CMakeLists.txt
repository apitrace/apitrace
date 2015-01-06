##############################################################################
# API helpers


include_directories (
    ${CMAKE_BINARY_DIR}/dispatch
    ${CMAKE_SOURCE_DIR}/dispatch
)

add_convenience_library (glhelpers
    glprofile.cpp
    eglsize.cpp
)
add_dependencies (glhelpers glproc)


if (WIN32)
    add_convenience_library (d3dhelpers
        d3dshader.cpp
    )
endif ()
