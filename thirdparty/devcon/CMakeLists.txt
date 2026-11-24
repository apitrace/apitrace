add_library (devcon STATIC
    devcon.cpp
)
target_include_directories (devcon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (devcon PUBLIC
    setupapi
)

install (
    FILES "license.rtf"
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-devcon.rtf
)
