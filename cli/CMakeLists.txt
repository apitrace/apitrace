add_executable (apitrace
    cli_main.cpp
    cli_dump.cpp
    cli_trace.cpp
)

install (TARGETS apitrace RUNTIME DESTINATION bin)
