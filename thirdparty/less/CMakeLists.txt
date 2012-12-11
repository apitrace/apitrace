add_definitions (-DNDEBUG)
if (WIN32)
    add_definitions (-DWIN32 -D_CONSOLE)
    configure_file (defines.wn ${CMAKE_CURRENT_BINARY_DIR}/defines.h COPYONLY)
else ()
    # XXX unsupported
endif ()

if (MSVC)
    add_definitions (-wd4131) # uses old-style declarator
endif ()

include_directories (${CMAKE_CURRENT_BINARY_DIR})

add_executable (less
	main.c screen.c brac.c ch.c charset.c cmdbuf.c
	command.c cvt.c decode.c edit.c filename.c forwback.c
	help.c ifile.c input.c jump.c line.c linenum.c
	lsystem.c mark.c optfunc.c option.c opttbl.c os.c
	output.c pattern.c position.c prompt.c search.c signal.c
	tags.c ttyin.c version.c regexp.c
)

add_executable (lesskey lesskey version)
add_executable (lessecho lessecho version)

install (
    TARGETS less lesskey lessecho
    RUNTIME DESTINATION bin
)

install (
    FILES LICENSE
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-less.txt
)
