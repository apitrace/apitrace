cmake_minimum_required (VERSION 2.6)

project (apitrace)

find_package (PythonInterp REQUIRED)
find_package (OpenGL REQUIRED)

find_package (ZLIB)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	# Nobody likes to include windows.h:
	# - Microsoft's GL/gl.h header depends on windows.h but doesn't include it;
	# - GLEW temporarily defines the necessary defines but undefines them later
	# - certain GLUT distributions don't include it;
	# - most of our programs are meant to be portable so don't include it.
	#
	# We could try to replicate the windows.h definitions required by
	# GL/gl.h, but the build time savings don't compensate the constant
	# headaches that brings, so instead we force windows.h to be included
	# on every file.
	if (MSVC)
		add_definitions (-FIwindows.h)
	else (MSVC)
		add_definitions (--include windows.h)
	endif (MSVC)

	# MSVC & MinGW only define & use APIENTRY
	add_definitions (-DGLAPIENTRY=__stdcall)

	link_libraries (winmm)
endif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

if (MSVC)
	# Enable math constants defines
	add_definitions (-D_USE_MATH_DEFINES)

	# Silence several MSVC pedantic warnings
	add_definitions (-D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS)
	add_definitions (-D_SCL_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
endif (MSVC)

# Use bundled ZLIB if system one can't be found
if (NOT ZLIB_FOUND)
	add_library (zlib STATIC
		zlib/adler32.c
		zlib/compress.c
		zlib/crc32.c
		zlib/gzio.c
		zlib/uncompr.c
		zlib/deflate.c
		zlib/trees.c
		zlib/zutil.c
		zlib/inflate.c
		zlib/infback.c
		zlib/inftrees.c
		zlib/inffast.c
	)

	include_directories (zlib)
	link_libraries (zlib)
else (NOT ZLIB_FOUND)
	include_directories (${ZLIB_INCLUDE_DIRS})
	link_libraries (${ZLIB_LIBRARIES})
endif (NOT ZLIB_FOUND)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

	# opengl32.dll
	add_custom_command (
		OUTPUT opengl32.cpp
		COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/opengl32.py > ${CMAKE_CURRENT_BINARY_DIR}/opengl32.cpp
		DEPENDS opengl32.py gl.py windows.py base.py
	)
	add_library (opengl32 SHARED opengl32.def opengl32.cpp log.cpp)
	set_target_properties (opengl32 PROPERTIES PREFIX "")

else ()

	# libGL.so
	add_custom_command (
		OUTPUT glx.cpp
		COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/glx.py > ${CMAKE_CURRENT_BINARY_DIR}/glx.cpp
		DEPENDS glx.py gl.py dl.py base.py
	)
	add_library (glxtrace SHARED glx.cpp log.cpp)
	set_target_properties (glxtrace PROPERTIES PREFIX "")
	target_link_libraries (glxtrace dl)
endif ()

