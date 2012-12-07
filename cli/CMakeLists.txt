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
)

target_link_libraries (apitrace
    common
    ${PNG_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${SNAPPY_LIBRARIES}
    ${GETOPT_LIBRARIES}
)

install (TARGETS apitrace RUNTIME DESTINATION bin)
