##############################################################################
# Dispatch

add_custom_command (
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/glproc.cpp
    COMMAND Python3::Interpreter
        ${CMAKE_CURRENT_SOURCE_DIR}/glproc.py
        ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/glproc.cpp
    MAIN_DEPENDENCY
        glproc.py
    DEPENDS
        dispatch.py
        ${PROJECT_SOURCE_DIR}/specs/wglapi.py
        ${PROJECT_SOURCE_DIR}/specs/glxapi.py
        ${PROJECT_SOURCE_DIR}/specs/cglapi.py
        ${PROJECT_SOURCE_DIR}/specs/eglapi.py
        ${PROJECT_SOURCE_DIR}/specs/glapi.py
        ${PROJECT_SOURCE_DIR}/specs/gltypes.py
        ${PROJECT_SOURCE_DIR}/specs/stdapi.py
)


# Wrap glproc.{hpp,cpp} as a target to prevent the command from being executed
# multiple times simultanously, when the targets that depend on it are built in
# parallel.
add_convenience_library (glproc EXCLUDE_FROM_ALL
    ${CMAKE_CURRENT_BINARY_DIR}/glproc.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/glproc.cpp
)

# Both the (hand written) headers in the source directory and the generated
# glproc.hpp in the binary directory.
target_include_directories (glproc
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries (glproc PRIVATE os)

# Headers are only needed for their include directory; deliberately not using
# X11::X11 here, as that would add a dependency on libX11.so to the tracers,
# which are LD_PRELOAD'ed onto arbitrary applications.
if (X11_FOUND)
    target_include_directories (glproc PUBLIC ${X11_INCLUDE_DIR})
endif ()
