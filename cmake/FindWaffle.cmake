# - try to find Waffle include dirs and library

if (ANDROID)
    set (Android_root_path "$ENV{OUT}")
	if (Android_root_path)
		set (Waffle_INC_SEARCH_PATH "$ENV{OUT}/obj/include/waffle-1"
			"$ENV{OUT}/obj/lib"
			"$ENV{ANDROID_BUILD_TOP}/external/waffle"
		)

		set (Waffle_LIB_SEARCH_PATH "$ENV{OUT}/obj/lib")
	endif ()

	find_path (Waffle_INCLUDE_DIR waffle.h
		PATHS ${Waffle_INC_SEARCH_PATH}
		DOC "The directory where waffle.h resides"
		NO_CMAKE_FIND_ROOT_PATH NO_DEFAULT_PATH
		)

	find_library (Waffle_LIBRARY waffle-1
		PATHS ${Waffle_LIB_SEARCH_PATH}
		DOC "The directory where libwaffle-1 resides"
		NO_CMAKE_FIND_ROOT_PATH NO_DEFAULT_PATH
		)

	if (Waffle_INCLUDE_DIR AND Waffle_LIBRARY)
		set (Waffle_FOUND 1)
	endif (Waffle_INCLUDE_DIR AND Waffle_LIBRARY)

	mark_as_advanced (
		Waffle_INCLUDE_DIR
		Waffle_LIBRARY
	)
endif ()

mark_as_advanced (
	Waffle_FOUND
)
