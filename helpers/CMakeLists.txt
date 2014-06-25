##############################################################################
# API helpers


if (WIN32)
    add_convenience_library (d3dhelpers
        d3dshader.cpp
    )
endif ()
