include_directories (
    ${CMAKE_SOURCE_DIR}/guids
)

if (WIN32)
    set (os os_win32.cpp)
else ()
    set (os os_posix.cpp)
endif ()

add_convenience_library (common
    trace_callset.cpp
    trace_dump.cpp
    trace_fast_callset.cpp
    trace_file.cpp
    trace_file_read.cpp
    trace_file_write.cpp
    trace_file_zlib.cpp
    trace_file_snappy.cpp
    trace_model.cpp
    trace_parser.cpp
    trace_parser_flags.cpp
    trace_writer.cpp
    trace_writer_local.cpp
    trace_writer_model.cpp
    trace_loader.cpp
    trace_profiler.cpp
    trace_option.cpp
    ${os}
    os_backtrace.cpp
    os_crtdbg.cpp
    highlight.cpp
)

target_link_libraries (common
    guids
    ${LIBBACKTRACE_LIBRARIES}
)
if (WIN32)
    target_link_libraries (common
        shell32
    )
endif ()
if (ANDROID)
    target_link_libraries (common
        log
    )
endif ()
