add_executable (apitrace
    cli_main.cpp
    cli_diff.cpp
    cli_diff_state.cpp
    cli_diff_images.cpp
    cli_dump.cpp
    cli_pager.cpp
    cli_pickle.cpp
    cli_repack.cpp
    cli_trace.cpp
)

install (TARGETS apitrace RUNTIME DESTINATION bin)
