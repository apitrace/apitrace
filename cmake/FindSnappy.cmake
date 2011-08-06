# - Try to find Snappy
# Once done this will define
#  SNAPPY_FOUND - System has Snappy
#  SNAPPY_INCLUDE_DIRS - The Snappy include directories
#  SNAPPY_LIBRARIES - The libraries needed to use Snappy
#  SNAPPY_DEFINITIONS - Compiler switches required for using Snappy

find_package(PkgConfig)
pkg_check_modules(PC_SNAPPY QUIET snappy)
set(SNAPPY_DEFINITIONS ${PC_SNAPPY_CFLAGS_OTHER})

find_path(SNAPPY_INCLUDE_DIR snappy.h
          HINTS ${PC_SNAPPY_INCLUDEDIR} ${PC_SNAPPY_INCLUDE_DIRS}
          PATH_SUFFIXES snappy )

find_library(SNAPPY_LIBRARY NAMES snappy libsnappy
             HINTS ${PC_SNAPPY_LIBDIR} ${PC_SNAPPY_LIBRARY_DIRS} )

set(SNAPPY_LIBRARIES ${SNAPPY_LIBRARY} )
set(SNAPPY_INCLUDE_DIRS ${SNAPPY_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set SNAPPY_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(snappy  DEFAULT_MSG
                                  SNAPPY_LIBRARY SNAPPY_INCLUDE_DIR)

mark_as_advanced(SNAPPY_INCLUDE_DIR SNAPPY_LIBRARY )
