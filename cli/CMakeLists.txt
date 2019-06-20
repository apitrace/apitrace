# Expose the binary/install directories to source
#
# TODO: Use the same directory layout, for both build and install directories,
# so that binaries can find each other using just relative paths.
#
add_definitions(
    -DAPITRACE_PROGRAMS_INSTALL_DIR="${CMAKE_INSTALL_PREFIX}/bin"
    -DAPITRACE_SCRIPTS_INSTALL_DIR="${CMAKE_INSTALL_PREFIX}/${SCRIPTS_INSTALL_DIR}"
    -DAPITRACE_WRAPPERS_INSTALL_DIR="${CMAKE_INSTALL_PREFIX}/${WRAPPER_INSTALL_DIR}"
)
if (WIN32)
    add_definitions (-DAPITRACE_PYTHON_EXECUTABLE="python")
else ()
    add_definitions (-DAPITRACE_PYTHON_EXECUTABLE="${Python3_EXECUTABLE}")
endif ()

include_directories (
    ${CMAKE_SOURCE_DIR}/lib/highlight
    ${CMAKE_SOURCE_DIR}/thirdparty
)

add_executable (apitrace
    cli_main.cpp
    cli_diff.cpp
    cli_diff_state.cpp
    cli_diff_images.cpp
    cli_leaks.cpp
    cli_dump.cpp
    cli_dump_images.cpp
    cli_pager.cpp
    cli_pickle.cpp
    cli_repack.cpp
    cli_retrace.cpp
    cli_sed.cpp
    cli_trace.cpp
    cli_trim.cpp
    cli_resources.cpp
)

target_link_libraries (apitrace
    common
    brotli_dec brotli_enc brotli_common
    ${ZLIB_LIBRARIES}
    ${SNAPPY_LIBRARIES}
    ${GETOPT_LIBRARIES}
    ${CMAKE_DL_LIBS}
)

if (NOT CMAKE_CROSSCOMPILING)
    # On debug builds tell where the source is so that scripts can be found
    # http://www.cmake.org/cmake/help/v3.0/policy/CMP0043.html
    set_target_properties (apitrace PROPERTIES
        COMPILE_DEFINITIONS $<$<CONFIG:Debug>:APITRACE_SOURCE_DIR="${CMAKE_SOURCE_DIR}">
    )
endif ()

install (TARGETS apitrace RUNTIME DESTINATION bin)
install_pdb (apitrace RUNTIME DESTINATION bin)
