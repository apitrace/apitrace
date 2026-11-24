add_convenience_library (crc32c EXCLUDE_FROM_ALL
    crc32c.c
)

target_include_directories (crc32c
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
