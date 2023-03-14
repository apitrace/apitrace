add_library (getopt STATIC EXCLUDE_FROM_ALL
    getopt_long.c
)

target_include_directories (getopt
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

install (
    FILES LICENSE
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-getopt.txt
)
