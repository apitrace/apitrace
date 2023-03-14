add_library (md5 STATIC EXCLUDE_FROM_ALL
    md5.c
)

target_include_directories (md5
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
if (CMAKE_C_COMPILER_ID MATCHES Clang)
    target_compile_options (md5 PRIVATE -Wno-deprecated-non-prototype)
endif ()
