# - try to find procps directories and libraries
#
# Once done this will define:
#
#  procps_FOUND
#  procps_INCLUDE_DIR
#  procps_LIBRARY
#

include (FindPackageMessage)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_path (procps_INCLUDE_DIR proc/readproc.h)
    find_library (procps_LIBRARY NAMES proc procps)
    if (procps_INCLUDE_DIR AND procps_LIBRARY)
        set (procps_FOUND 1)
        mark_as_advanced (procps_FOUND)
        find_package_message (procps "Found procps" "[${procps_LIBRARY}][${procps_INCLUDE_DIR}]")
    else ()
        find_package_message (procps "procps not found" "[${procps_LIBRARY}][${procps_INCLUDE_DIR}]")
    endif ()
endif()

