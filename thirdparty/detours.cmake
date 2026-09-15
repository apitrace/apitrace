# Microsoft Detours -- inline API hooking

# Only the core, native-arch sources are needed for in-process
# DetourAttach()/DetourDetach().  The disolXXX.cpp files build offline
# cross-architecture disassemblers used by Detours' binary rewriting tools,
# which apitrace doesn't use.
add_convenience_library (detours STATIC EXCLUDE_FROM_ALL
    detours/src/creatwth.cpp
    detours/src/detours.cpp
    detours/src/disasm.cpp
    detours/src/image.cpp
    detours/src/modules.cpp
)

target_include_directories (detours PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/detours/src
)

if (NOT MSVC)
    target_compile_options (detours PRIVATE
        -Wno-unused-parameter
        -Wno-unused-variable
        -Wno-unknown-pragmas
    )
endif ()

target_optimize (detours)

install (
    FILES "detours/LICENSE.md"
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-detours.txt
)
