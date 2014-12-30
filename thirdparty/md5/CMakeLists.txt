include (CheckCCompilerFlag)

check_c_compiler_flag ("-fno-strict-aliasing" C_COMPILER_FLAG_NO_STRICT_ALIASING)
if (C_COMPILER_FLAG_NO_STRICT_ALIASING)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing")
endif ()

add_library (md5_bundled STATIC EXCLUDE_FROM_ALL
    md5.c
)
