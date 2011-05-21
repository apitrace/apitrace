include(CheckTypeSize)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckCSourceCompiles)

check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(stdint.h    HAVE_STDINT_H)
check_include_file(stddef.h    HAVE_STDDEF_H)

set(CMAKE_REQUIRED_DEFINITIONS -D_LARGEFILE64_SOURCE=1)
check_type_size(off64_t OFF64_T)
if(HAVE_OFF64_T)
   add_definitions(-D_LARGEFILE64_SOURCE=1)
endif()
set(CMAKE_REQUIRED_DEFINITIONS)

check_function_exists(fseeko HAVE_FSEEKO)
if(NOT HAVE_FSEEKO)
    add_definitions(-DNO_FSEEKO)
endif()

check_include_file(unistd.h Z_HAVE_UNISTD_H)

add_definitions(-DNO_VIZ)

if (NOT WIN32)
    add_definitions(-fPIC)
endif (NOT WIN32)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library (zlib STATIC
    adler32.c
    compress.c
    crc32.c
    gzclose.c
    gzlib.c
    gzread.c
    gzwrite.c
    uncompr.c
    deflate.c
    trees.c
    zutil.c
    inflate.c
    infback.c
    inftrees.c
    inffast.c
)
