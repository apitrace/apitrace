add_executable (apitrace
    cli_main.cpp
    cli_dump.cpp
)

install (TARGETS apitrace RUNTIME DESTINATION bin)
