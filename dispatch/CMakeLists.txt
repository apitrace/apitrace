##############################################################################
# Dispatch

set (CMAKE_INCLUDE_CURRENT_DIR ON)

add_custom_command (
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/glproc.cpp
    COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/glproc.py
        ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/glproc.cpp
    MAIN_DEPENDENCY
        glproc.py
    DEPENDS
        dispatch.py
        ${CMAKE_SOURCE_DIR}/specs/wglapi.py
        ${CMAKE_SOURCE_DIR}/specs/glxapi.py
        ${CMAKE_SOURCE_DIR}/specs/cglapi.py
        ${CMAKE_SOURCE_DIR}/specs/eglapi.py
        ${CMAKE_SOURCE_DIR}/specs/glapi.py
        ${CMAKE_SOURCE_DIR}/specs/gltypes.py
        ${CMAKE_SOURCE_DIR}/specs/stdapi.py
)


# Wrap glproc.{hpp,cpp} as a target to prevent the command from being executed
# multiple times simultanously, when the targets that depend on it are built in
# parallel.
add_convenience_library (glproc EXCLUDE_FROM_ALL
    ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/glproc.cpp
)

add_convenience_library (glproc_gl EXCLUDE_FROM_ALL
    glproc_gl.cpp
)
add_dependencies (glproc_gl glproc)
target_link_libraries (glproc_gl glproc)

if (ENABLE_EGL)
    add_convenience_library (glproc_egl EXCLUDE_FROM_ALL
        glproc_egl.cpp
    )
    add_dependencies (glproc_egl glproc)
    target_link_libraries (glproc_egl glproc)
endif ()
