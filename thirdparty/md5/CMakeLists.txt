add_library (md5 STATIC EXCLUDE_FROM_ALL
    md5.c
)

target_include_directories (md5
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
