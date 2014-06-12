##############################################################################
# Dispatch

include_directories (
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/glproc.py > ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp
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
add_custom_target (glproc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp)


add_library (glproc_gl STATIC EXCLUDE_FROM_ALL
    glproc_gl.cpp
)

add_dependencies (glproc_gl glproc)

set_target_properties (glproc_gl PROPERTIES
    # Ensure it can be statically linked in shared libraries
    COMPILE_FLAGS "${CMAKE_SHARED_LIBRARY_CXX_FLAGS}"
)

if (ENABLE_EGL)
    add_library (glproc_egl STATIC EXCLUDE_FROM_ALL
        glproc_egl.cpp
    )

    add_dependencies (glproc_egl glproc)

    set_target_properties (glproc_egl PROPERTIES
        # Ensure it can be statically linked in shared libraries
        COMPILE_FLAGS "${CMAKE_SHARED_LIBRARY_CXX_FLAGS}"
    )
endif ()

