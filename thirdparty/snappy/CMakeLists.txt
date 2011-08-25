include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library (snappy_bundled STATIC
    snappy.cc
    snappy-sinksource.cc
    snappy-stubs-internal.cc
    snappy-c.cc
)

set_target_properties (snappy_bundled PROPERTIES
    COMPILE_FLAGS "${CMAKE_SHARED_LIBRARY_CXX_FLAGS}"
)
