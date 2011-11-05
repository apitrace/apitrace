add_executable (apitrace
    cli_main.cpp
    cli_diff.cpp
    cli_diff_state.cpp
    cli_dump.cpp
    cli_trace.cpp
)

install (TARGETS apitrace RUNTIME DESTINATION bin)
