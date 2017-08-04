include_directories (
    ${CMAKE_SOURCE_DIR}/lib/guids
    ${CMAKE_SOURCE_DIR}/lib/highlight
    ${CMAKE_SOURCE_DIR}/thirdparty
)

add_convenience_library (common
    trace_callset.cpp
    trace_dump.cpp
    trace_fast_callset.cpp
    trace_file.cpp
    trace_file_read.cpp
    trace_file_zlib.cpp
    trace_file_brotli.cpp
    trace_file_snappy.cpp
    trace_format.hpp
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
)

target_link_libraries (common
    guids
    highlight
    os
    brotli_dec brotli_common
)

add_gtest (trace_parser_flags_test trace_parser_flags_test.cpp)
target_link_libraries (trace_parser_flags_test common)
