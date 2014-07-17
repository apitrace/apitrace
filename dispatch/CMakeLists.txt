##############################################################################
# Dispatch

include_directories (
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_command (
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/glproc.cpp
    COMMAND ${PYTHON_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/glproc.py
        ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/glproc.cpp
    DEPENDS
        glproc.py
        dispatch.py
        ${CMAKE_SOURCE_DIR}/specs/wglapi.py
        ${CMAKE_SOURCE_DIR}/specs/glxapi.py
        ${CMAKE_SOURCE_DIR}/specs/cglapi.py
        ${CMAKE_SOURCE_DIR}/specs/eglapi.py
        ${CMAKE_SOURCE_DIR}/specs/glesapi.py
        ${CMAKE_SOURCE_DIR}/specs/glapi.py
        ${CMAKE_SOURCE_DIR}/specs/gltypes.py
        ${CMAKE_SOURCE_DIR}/specs/stdapi.py
)


# Wrap glproc.hpp as a target to prevent the command from being executed
# multiple times simulatenously, when the targets that depend on it are built
# in parallel.
add_custom_target (glproc DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/glproc.cpp
)


add_convenience_library (glproc_gl EXCLUDE_FROM_ALL
    glproc_gl.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/glproc.cpp
)

add_dependencies (glproc_gl glproc)

if (ENABLE_EGL)
    add_convenience_library (glproc_egl EXCLUDE_FROM_ALL
        glproc_egl.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/glproc.cpp
    )

    add_dependencies (glproc_egl glproc)
endif ()

