remove_definitions (-DNOMINMAX)

if (MINGW)
    # mhook uses MSVC format specifiers
    remove_definitions (-D__USE_MINGW_ANSI_STDIO=1)
    add_definitions (-D__USE_MINGW_ANSI_STDIO=0)
endif ()

add_definitions (
    -DUNICODE -D_UNICODE
    -DMINGW_HAS_SECURE_API=1
)

if (NOT MSVC)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-comment")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-enum-compare")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-pointer-to-int-cast")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-switch")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-value")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-variable")
endif ()
if (CMAKE_C_COMPILER_ID MATCHES Clang)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-format-security")
endif ()


add_convenience_library (mhook
    disasm-lib/cpu.c
    disasm-lib/disasm.c
    disasm-lib/disasm_x86.c
    disasm-lib/misc.c
    mhook-lib/mhook.cpp
)


add_executable (mhook-test
    mhook-test.cpp
)
target_link_libraries (mhook-test
    mhook
    ws2_32
)
add_test (NAME mhook-test COMMAND mhook-test)

if (CMAKE_CROSSCOMPILING)
    # It does not run reliably on old Wine versions
    set_tests_properties (mhook-test PROPERTIES DISABLED TRUE)
endif ()


install (
    FILES "COPYING"
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-mhook.txt
)
