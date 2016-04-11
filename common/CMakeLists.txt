include_directories (
    ${CMAKE_SOURCE_DIR}/guids
    ${CMAKE_SOURCE_DIR}/thirdparty
)

if (WIN32)
    set (os os_win32.cpp)
elseif (APPLE)
    set (os os_posix.cpp os_osx.mm)
else ()
    set (os os_posix.cpp)
endif ()

add_convenience_library (common
    trace_callset.cpp
    trace_dump.cpp
    trace_fast_callset.cpp
    trace_file.cpp
    trace_file_read.cpp
    trace_file_zlib.cpp
    trace_file_brotli.cpp
    trace_file_snappy.cpp
    trace_model.cpp
    trace_parser.cpp
    trace_parser_flags.cpp
    trace_parser_loop.cpp
    trace_writer.cpp
    trace_writer_local.cpp
    trace_writer_model.cpp
    trace_profiler.cpp
    trace_option.cpp
    trace_ostream_snappy.cpp
    trace_ostream_zlib.cpp
    ${os}
    os_backtrace.cpp
    os_crtdbg.cpp
    highlight.cpp
)

target_link_libraries (common
    guids
    ${LIBBACKTRACE_LIBRARIES}
    brotli_dec_bundled
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

if (APPLE)
    target_link_libraries (common
        "-framework Foundation"
    )
endif ()

add_gtest (os_thread_test os_thread_test.cpp)
target_link_libraries (os_thread_test common)

add_gtest (trace_parser_flags_test trace_parser_flags_test.cpp)
target_link_libraries (trace_parser_flags_test common)
