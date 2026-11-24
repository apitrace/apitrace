# - try to find procps directories and libraries
#
# Once done this will define:
#
#  PROCPS_FOUND
#  PROCPS_INCLUDE_DIR
#  PROCPS_LIBRARY
#
# and the imported target:
#
#  procps::procps
#

include (FindPackageHandleStandardArgs)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_path (PROCPS_INCLUDE_DIR proc/readproc.h)
    find_library (PROCPS_LIBRARY NAMES proc procps)
    find_package_handle_standard_args (procps DEFAULT_MSG PROCPS_LIBRARY PROCPS_INCLUDE_DIR)
endif()

if (PROCPS_FOUND AND NOT TARGET procps::procps)
    add_library (procps::procps UNKNOWN IMPORTED)
    set_target_properties (procps::procps PROPERTIES
        IMPORTED_LOCATION "${PROCPS_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${PROCPS_INCLUDE_DIR}"
    )
endif ()
