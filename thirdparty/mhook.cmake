remove_definitions (-DNOMINMAX)

if (NOT MSVC)
    target_compile_options (mhook PRIVATE
        -Wno-comment
        -Wno-enum-compare
        -Wno-pointer-to-int-cast
        -Wno-switch
        -Wno-unused-value
        -Wno-unused-variable
    )
endif ()
if (CMAKE_C_COMPILER_ID MATCHES Clang)
    target_compile_options (mhook PRIVATE
        -Wno-format-security
    )
endif ()


add_convenience_library (mhook
    mhook/disasm-lib/cpu.c
    mhook/disasm-lib/cpu.h
    mhook/disasm-lib/disasm.c
    mhook/disasm-lib/disasm.h
    mhook/disasm-lib/disasm_x86.c
    mhook/disasm-lib/disasm_x86.h
    mhook/disasm-lib/disasm_x86_tables.h
    mhook/disasm-lib/misc.h
    mhook/mhook-lib/mhook.c
    mhook/mhook-lib/mhook.h
)
target_compile_definitions (mhook PRIVATE
    UNICODE
    _UNICODE
    MINGW_HAS_SECURE_API=1

    # Ensure disasm errors are written to stderr.
    "printf(...)=fprintf(stderr,__VA_ARGS__)"
    # Silence mhook debug messages.
    ODPRINTF=__noop
)
target_include_directories (mhook INTERFACE mhook)
if (MINGW)
    # mhook uses MSVC format specifiers
    remove_definitions (-D__USE_MINGW_ANSI_STDIO=1)
    target_compile_definitions (mhook PRIVATE -D__USE_MINGW_ANSI_STDIO=0)
endif ()

add_executable (mhook-test
    mhook/mhook-test/mhook-test.cpp
)
target_link_libraries (mhook-test
    mhook
    ws2_32
)
if (MINGW)
    target_link_options (mhook-test PRIVATE "-municode")
endif()
add_test (NAME mhook-test COMMAND mhook-test)

if (CMAKE_CROSSCOMPILING)
    # It does not run reliably on old Wine versions
    set_tests_properties (mhook-test PROPERTIES DISABLED TRUE)
endif ()


install (
    FILES "mhook/COPYING"
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-mhook.txt
)
