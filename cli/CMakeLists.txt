# Expose the binary/install directories to source
#
# TODO: Use the same directory layout, for both build and install directories,
# so that binaries can find each other using just relative paths.
#
set (APITRACE_DEFINITIONS
    APITRACE_PROGRAMS_INSTALL_DIR="${CMAKE_INSTALL_PREFIX}/bin"
    APITRACE_SCRIPTS_INSTALL_DIR="${CMAKE_INSTALL_PREFIX}/${SCRIPTS_INSTALL_DIR}"
    APITRACE_WRAPPERS_INSTALL_DIR="${CMAKE_INSTALL_PREFIX}/${WRAPPER_INSTALL_DIR}"
)
if (WIN32)
    list (APPEND APITRACE_DEFINITIONS APITRACE_PYTHON_EXECUTABLE="python")
else ()
    list (APPEND APITRACE_DEFINITIONS APITRACE_PYTHON_EXECUTABLE="${Python3_EXECUTABLE}")
endif ()

add_executable (apitrace
    cli_main.cpp
    cli_diff.cpp
    cli_diff_state.cpp
    cli_diff_images.cpp
    cli_leaks.cpp
    cli_dump.cpp
    cli_dump_images.cpp
    cli_gltrim.cpp
    cli_pager.cpp
    cli_pickle.cpp
    cli_repack.cpp
    cli_retrace.cpp
    cli_sed.cpp
    cli_trace.cpp
    cli_trim.cpp
    cli_info.cpp
    cli_resources.cpp
)

target_compile_definitions (apitrace PRIVATE ${APITRACE_DEFINITIONS})

target_include_directories (apitrace PRIVATE
    ${PROJECT_SOURCE_DIR}/thirdparty
    ${PROJECT_BINARY_DIR} # for version.h
)

target_link_libraries (apitrace PRIVATE
    common
    PkgConfig::BROTLIDEC
    PkgConfig::BROTLIENC
    getopt
    ${CMAKE_DL_LIBS}
)

add_dependencies(apitrace version)

if (NOT CMAKE_CROSSCOMPILING)
    # On debug builds tell where the source is so that scripts can be found
    target_compile_definitions (apitrace PRIVATE
        $<$<CONFIG:Debug>:APITRACE_SOURCE_DIR="${PROJECT_SOURCE_DIR}">
    )
endif ()

install (TARGETS apitrace)
install_pdb (apitrace TYPE BIN)
