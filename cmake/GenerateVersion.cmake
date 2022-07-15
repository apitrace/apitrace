# https://www.appveyor.com/docs/environment-variables/
if (($ENV{APPVEYOR}) AND ($ENV{APPVEYOR_REPO_TAG}))
    set (APITRACE_VERSION "$ENV{APPVEYOR_REPO_TAG_NAME}")
# https://docs.github.com/en/actions/reference/environment-variables#default-environment-variables
elseif ("$ENV{GITHUB_ACTIONS}" STREQUAL "true" AND "$ENV{GITHUB_REF}" MATCHES [[^refs/tags/([^/]*)$]])
    set (APITRACE_VERSION "${CMAKE_MATCH_1}")
# git
elseif (GIT_EXECUTABLE)
  get_filename_component(SRC_DIR ${SRC} DIRECTORY)
  # Generate a git-describe version string from Git repository tags
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --dirty --match "*"
    WORKING_DIRECTORY ${SRC_DIR}
    OUTPUT_VARIABLE GIT_DESCRIBE_VERSION
    RESULT_VARIABLE GIT_DESCRIBE_ERROR_CODE
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  if (NOT GIT_DESCRIBE_ERROR_CODE)
    set(APITRACE_VERSION ${GIT_DESCRIBE_VERSION})
  endif ()
endif ()

if (NOT DEFINED APITRACE_VERSION)
  set(APITRACE_VERSION 0.0-unknown)
  message(WARNING "Failed to determine APITRACE_VERSION. Using default version \"${APITRACE_VERSION}\".")
endif ()

# propagate version into header and cpack
configure_file(${SRC} ${DST} @ONLY)

set (CPACK_PACKAGE_VERSION "${APITRACE_VERSION}")
