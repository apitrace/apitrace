# Find SNAPPY - A fast compressor/decompressor
#
# This module defines
#  SNAPPY_FOUND - whether the snappy library was found
#  SNAPPY_LIBRARIES - the snappy library
#  SNAPPY_INCLUDE_DIR - the include path of the snappy library
#

find_path (SNAPPY_INCLUDE_DIR NAMES snappy.h)
find_library (SNAPPY_LIBRARIES NAMES snappy)

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (SNAPPY DEFAULT_MSG SNAPPY_LIBRARIES SNAPPY_INCLUDE_DIR)
