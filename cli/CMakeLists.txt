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

add_executable (apitrace
    cli_main.cpp
    cli_diff.cpp
    cli_diff_state.cpp
    cli_diff_images.cpp
    cli_dump.cpp
    cli_dump_images.cpp
    cli_pager.cpp
    cli_pickle.cpp
    cli_repack.cpp
    cli_retrace.cpp
    cli_trace.cpp
    cli_trim.cpp
    cli_resources.cpp
    trace_analyzer.cpp
)

target_link_libraries (apitrace
    common
    ${ZLIB_LIBRARIES}
    ${SNAPPY_LIBRARIES}
    ${GETOPT_LIBRARIES}
)

if (NOT CMAKE_CROSSCOMPILING)
    set_target_properties (apitrace PROPERTIES
        # On debug builds tell where the source is so that scripts can be found
        COMPILE_DEFINITIONS_DEBUG APITRACE_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
    )
endif ()

if (MSVC)
    # On MSVC builds tell which subdirectory the binaries with be (for each
    # configuration)
    set_target_properties (apitrace PROPERTIES
        COMPILE_DEFINITIONS_DEBUG APITRACE_CONFIGURATION_SUBDIR="Debug"
        COMPILE_DEFINITIONS_RELEASE APITRACE_CONFIGURATION_SUBDIR="Release"
        COMPILE_DEFINITIONS_MINSIZEREL APITRACE_CONFIGURATION_SUBDIR="MinSizeRel"
        COMPILE_DEFINITIONS_RELWITHDEBINFO APITRACE_CONFIGURATION_SUBDIR="RelWithDebInfo"
    )
endif ()


install (TARGETS apitrace RUNTIME DESTINATION bin)
