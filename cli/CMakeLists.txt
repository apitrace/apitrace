add_executable (apitrace
  apitrace.cpp
  apitrace_dump.cpp)

install (TARGETS apitrace RUNTIME DESTINATION bin)
