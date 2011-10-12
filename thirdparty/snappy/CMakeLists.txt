add_definitions (-DHAVE_CONFIG_H)

# Adjust warnings
if (CMAKE_COMPILER_IS_GNUCXX)
    add_definitions (-Wno-unused-function)
endif ()

add_library (snappy_bundled STATIC
    snappy.cc
    snappy-sinksource.cc
    snappy-stubs-internal.cc
    snappy-c.cc
)

set_target_properties (snappy_bundled PROPERTIES
    COMPILE_FLAGS "${CMAKE_SHARED_LIBRARY_CXX_FLAGS}"
)
