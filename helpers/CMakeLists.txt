##############################################################################
# API helpers


if (WIN32)
    add_library (d3dhelpers STATIC
        d3dshader.cpp
    )
    set_target_properties (d3dhelpers PROPERTIES
        # Ensure it can be statically linked in shared libraries
        COMPILE_FLAGS "${CMAKE_SHARED_LIBRARY_CXX_FLAGS}"
    )
endif ()
