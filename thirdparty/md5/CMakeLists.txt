add_library (md5_bundled STATIC EXCLUDE_FROM_ALL 
    md5.c
)

set_target_properties (md5_bundled PROPERTIES
    COMPILE_FLAGS "${CMAKE_SHARED_LIBRARY_C_FLAGS}"
)
