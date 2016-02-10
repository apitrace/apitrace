add_library (devcon STATIC
    devcon.cpp
)
target_link_libraries (devcon
    setupapi
)

install (
    FILES "license.rtf"
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-devcon.rtf
)
